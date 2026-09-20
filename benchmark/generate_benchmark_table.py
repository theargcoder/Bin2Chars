#!/usr/bin/env python3

from __future__ import annotations

import argparse
import html
import json
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Any


START_MARKER = "<!-- BENCHMARK_RESULTS:START -->"
END_MARKER = "<!-- BENCHMARK_RESULTS:END -->"

DEFAULT_METRIC = "core"
DEFAULT_STATISTIC = "median"

VALID_RETURN_TYPES = {"buffered", "std_string"}
VALID_FLOAT_FORMATS = {"decimal", "exponential"}


@dataclass(frozen=True)
class Benchmark:
    platform: str
    category: str
    notation: str | None
    return_type: str
    value_type: str | None
    label: str
    implementations: dict[str, dict[str, dict[str, float]]]
    source: Path
    trials: int | None


def load_json(path: Path) -> dict[str, Any]:
    with path.open("r", encoding="utf-8") as file:
        document = json.load(file)

    if not isinstance(document, dict):
        raise ValueError(f"{path}: root JSON value must be an object")

    return document


def numeric(value: Any) -> float:
    if not isinstance(value, (int, float)):
        raise TypeError(f"Expected numeric value, got {type(value).__name__}")

    return float(value)


def integer_or_none(value: Any) -> int | None:
    if value is None:
        return None

    if isinstance(value, bool) or not isinstance(value, int):
        raise TypeError(f"Expected integer or null, got {type(value).__name__}")

    return value


def extract_measurements(
    implementation: dict[str, Any],
) -> dict[str, dict[str, float]]:
    result: dict[str, dict[str, float]] = {}

    for metric_name, metric_data in implementation.items():
        if not isinstance(metric_data, dict):
            continue

        statistics: dict[str, float] = {}

        for statistic in (
            "mean",
            "median",
            "min",
            "p95",
            "p99",
        ):
            if statistic in metric_data:
                statistics[statistic] = numeric(metric_data[statistic])

        if statistics:
            result[metric_name] = statistics

    return result


def parse_path_identity(
    path: Path,
    results_root: Path,
) -> tuple[
    str,
    str,
    str | None,
    str,
    str | None,
]:
    """
    Convert the directory hierarchy into the benchmark identity.

    Integer:
        <platform>/integers/<return_type>/all_integers.json

    Floating:
        <platform>/floats/<notation>/<return_type>/<float|double>.json

    The first directory under results_root is intentionally treated as
    the complete platform/build identity.

    Examples:
        Linux-x86_64-AVX2
        Linux-x86_64-AVX512
        Macos-ARM64-None
        Windows-x86_64-AVX2
        etc.

    No platform names are hardcoded.
    """

    try:
        relative = path.relative_to(results_root)
    except ValueError as exc:
        raise ValueError(f"{path} is not inside results root {results_root}") from exc

    parts = relative.parts

    if len(parts) < 4:
        raise ValueError(f"{path}: unexpected benchmark path:\n  {relative}")

    platform = parts[0]
    category = parts[1]

    if category == "integers":
        if len(parts) != 4:
            raise ValueError(
                f"{path}: integer benchmark must have the form:\n"
                f"  <platform>/integers/<return_type>/all_integers.json"
            )

        return_type = parts[2]
        filename = parts[3]

        if return_type not in VALID_RETURN_TYPES:
            raise ValueError(f"{path}: unsupported integer return type {return_type!r}")

        if filename != "all_integers.json":
            raise ValueError(
                f"{path}: integer benchmark filename must be 'all_integers.json'"
            )

        return (
            platform,
            "integer",
            None,
            return_type,
            None,
        )

    if category == "floats":
        if len(parts) != 5:
            raise ValueError(
                f"{path}: floating benchmark must have the form:\n"
                f"  <platform>/floats/<notation>/<return_type>/"
                f"<float|double>.json"
            )

        notation = parts[2]
        return_type = parts[3]
        filename = parts[4]

        if notation not in VALID_FLOAT_FORMATS:
            raise ValueError(f"{path}: unsupported floating notation {notation!r}")

        if return_type not in VALID_RETURN_TYPES:
            raise ValueError(
                f"{path}: unsupported floating return type {return_type!r}"
            )

        value_type = Path(filename).stem

        if value_type not in {"float", "double"}:
            raise ValueError(
                f"{path}: floating benchmark filename must be "
                "'float.json' or 'double.json'"
            )

        return (
            platform,
            "floating",
            notation,
            return_type,
            value_type,
        )

    raise ValueError(f"{path}: unsupported benchmark category {category!r}")


def validate_metadata(
    path: Path,
    document: dict[str, Any],
    *,
    notation: str | None,
    return_type: str,
) -> None:
    """
    Path hierarchy is authoritative, but the JSON metadata should agree
    with it whenever those metadata fields are present.
    """

    json_return_type = document.get("type")

    if json_return_type is not None and json_return_type != return_type:
        raise ValueError(
            f"{path}: JSON 'type'={json_return_type!r} disagrees "
            f"with directory return type={return_type!r}"
        )

    if notation is not None:
        json_format = document.get("format")

        if json_format is not None and json_format != notation:
            raise ValueError(
                f"{path}: JSON 'format'={json_format!r} disagrees "
                f"with directory notation={notation!r}"
            )


def parse_benchmark_file(
    path: Path,
    results_root: Path,
) -> list[Benchmark]:
    document = load_json(path)

    (
        platform,
        category,
        notation,
        return_type,
        value_type,
    ) = parse_path_identity(
        path,
        results_root,
    )

    validate_metadata(
        path,
        document,
        notation=notation,
        return_type=return_type,
    )

    trials = integer_or_none(document.get("trials"))

    yields = document.get("yields")

    if not isinstance(yields, list):
        raise ValueError(f"{path}: expected 'yields' to be an array")

    benchmarks: list[Benchmark] = []

    for item in yields:
        if not isinstance(item, dict):
            raise ValueError(f"{path}: every yields entry must be an object")

        for label, implementations_raw in item.items():
            if not isinstance(implementations_raw, dict):
                raise ValueError(
                    f"{path}: benchmark {label!r} must contain "
                    "an object of implementations"
                )

            implementations: dict[
                str,
                dict[str, dict[str, float]],
            ] = {}

            for (
                implementation_name,
                implementation_data,
            ) in implementations_raw.items():
                if not isinstance(
                    implementation_data,
                    dict,
                ):
                    continue

                measurements = extract_measurements(implementation_data)

                if measurements:
                    implementations[implementation_name] = measurements

            if not implementations:
                continue

            benchmarks.append(
                Benchmark(
                    platform=platform,
                    category=category,
                    notation=notation,
                    return_type=return_type,
                    value_type=value_type,
                    label=label,
                    implementations=implementations,
                    source=path,
                    trials=trials,
                )
            )

    return benchmarks


def discover_benchmarks(
    results_root: Path,
) -> list[Benchmark]:
    benchmarks: list[Benchmark] = []

    for path in sorted(results_root.rglob("*.json")):
        benchmarks.extend(
            parse_benchmark_file(
                path,
                results_root,
            )
        )

    return benchmarks


def format_cycles(
    value: float,
) -> str:
    if value.is_integer():
        return f"{int(value)}"

    return f"{value:.3f}".rstrip("0").rstrip(".")


def return_type_display(
    return_type: str,
) -> str:
    return {
        "buffered": "Buffered",
        "std_string": "`std::string`",
    }.get(
        return_type,
        return_type,
    )


def notation_display(
    notation: str,
) -> str:
    return {
        "decimal": "Decimal notation",
        "exponential": "Exponential notation",
    }.get(
        notation,
        notation,
    )


def implementation_display(
    name: str,
) -> str:
    return {
        "BIN2CHARS": "Bin2Chars",
        "STD_LIB": "Standard library",
        "RYU": "Ryu",
    }.get(
        name,
        name,
    )


def ratio(
    baseline: dict[str, dict[str, float]] | None,
    candidate: dict[str, dict[str, float]] | None,
    *,
    metric: str,
    statistic: str,
) -> float | None:
    if baseline is None or candidate is None:
        return None

    try:
        baseline_value = baseline[metric][statistic]

        candidate_value = candidate[metric][statistic]
    except KeyError:
        return None

    if candidate_value == 0:
        return None

    return baseline_value / candidate_value


def implementation_detail(
    measurements: dict[str, dict[str, float]],
) -> str:
    sections: list[str] = []

    metric_names = {
        "tsc": "TSC",
        "core": "Core",
        "ref": "Reference",
    }

    for metric in (
        "tsc",
        "core",
        "ref",
    ):
        statistics = measurements.get(metric)

        if statistics is None:
            continue

        lines = [f"**{metric_names.get(metric, metric)}**"]

        for statistic in (
            "mean",
            "median",
            "min",
            "p95",
            "p99",
        ):
            value = statistics.get(statistic)

            if value is None:
                continue

            lines.append(f"{statistic}: {format_cycles(value)}")

        sections.append("<br>".join(lines))

    return "<br><br>".join(sections) or "No measurements"


def details_cell(
    measurements: dict[str, dict[str, float]] | None,
    *,
    metric: str,
    statistic: str,
) -> str:
    if measurements is None:
        return "<details><summary>N/A</summary><br>No benchmark data</details>"

    value = measurements.get(
        metric,
        {},
    ).get(statistic)

    headline = "N/A" if value is None else f"{format_cycles(value)} cycles"

    details = implementation_detail(measurements)

    return f"<details><summary>{headline}</summary><br>{details}</details>"


def relative_cell(
    implementations: dict[
        str,
        dict[str, dict[str, float]],
    ],
    *,
    metric: str,
    statistic: str,
) -> str:
    """
    Render relative performance with the same presentation style
    as the compact summary:

        1.688× Standard library
        1.628× Ryu

    The expanded section still contains every available metric.
    """

    bin2chars = implementations.get("BIN2CHARS")

    if bin2chars is None:
        return (
            "<details>"
            "<summary>N/A</summary>"
            "<br>Bin2Chars data is unavailable."
            "</details>"
        )

    baselines = [
        ("STD_LIB", "Standard library"),
        ("RYU", "Ryu"),
    ]

    available: list[
        tuple[
            str,
            str,
            dict[str, dict[str, float]],
        ]
    ] = []

    for name, display in baselines:
        baseline = implementations.get(name)

        if baseline is not None:
            available.append(
                (
                    name,
                    display,
                    baseline,
                )
            )

    if not available:
        return (
            "<details>"
            "<summary>N/A</summary>"
            "<br>No comparison baseline is available."
            "</details>"
        )

    headline_parts: list[str] = []
    detail_parts: list[str] = []

    for _, display, baseline in available:
        headline_ratio = ratio(
            baseline,
            bin2chars,
            metric=metric,
            statistic=statistic,
        )

        if headline_ratio is None:
            headline_parts.append(f"{display}: N/A")
        else:
            headline_parts.append(f"{headline_ratio:.3f}× {display}")

        metric_ratios: list[str] = []

        for comparison_metric in (
            "tsc",
            "core",
            "ref",
        ):
            comparison_ratio = ratio(
                baseline,
                bin2chars,
                metric=comparison_metric,
                statistic=statistic,
            )

            if comparison_ratio is None:
                continue

            metric_label = {
                "tsc": "TSC",
                "core": "Core",
                "ref": "Reference",
            }[comparison_metric]

            metric_ratios.append(f"{metric_label}: {comparison_ratio:.3f}×")

        detail_parts.append(f"**vs {display}**<br>" + "<br>".join(metric_ratios))

    headline = " · ".join(headline_parts)

    details = "<br><br>".join(detail_parts)

    formula = (
        f"**Formula:** baseline {metric} {statistic} ÷ Bin2Chars {metric} {statistic}"
    )

    return (
        "<details>"
        f"<summary>{headline}</summary>"
        f"<br>{details}"
        f"<br><br>{formula}"
        "</details>"
    )


def integer_row_sort_key(
    benchmark: Benchmark,
) -> tuple[int, int, int | str]:
    match = re.fullmatch(
        r"(u?)int(\d+)_t",
        benchmark.label,
    )

    if match is None:
        return (
            1,
            1_000_000,
            benchmark.label,
        )

    unsigned = 1 if match.group(1) else 0

    width = int(match.group(2))

    return (
        0,
        width,
        unsigned,
    )


def precision_sort_key(
    benchmark: Benchmark,
) -> tuple[int, str]:
    match = re.fullmatch(
        r"precision\s+(\d+)",
        benchmark.label,
        flags=re.IGNORECASE,
    )

    if match is None:
        return (
            1_000_000,
            benchmark.label,
        )

    return (
        int(match.group(1)),
        benchmark.label,
    )


def benchmark_sort_key(
    benchmark: Benchmark,
) -> tuple[Any, ...]:
    if benchmark.category == "integer":
        return integer_row_sort_key(benchmark)

    value_type_order = {
        "float": 0,
        "double": 1,
    }

    return (
        value_type_order.get(
            benchmark.value_type or "",
            100,
        ),
        *precision_sort_key(benchmark),
    )


def display_type(
    benchmark: Benchmark,
) -> str:
    if benchmark.category == "integer":
        return f"`{benchmark.label}`"

    value_type = benchmark.value_type or "unknown"

    return f"`{value_type}` — `{benchmark.label}`"


def implementation_names(
    benchmarks: list[Benchmark],
) -> list[str]:
    names: set[str] = set()

    for benchmark in benchmarks:
        names.update(benchmark.implementations.keys())

    preferred_order = [
        "BIN2CHARS",
        "STD_LIB",
        "RYU",
    ]

    result = [name for name in preferred_order if name in names]

    result.extend(
        sorted(
            names.difference(preferred_order),
            key=str.upper,
        )
    )

    return result


def slug_component(
    value: str,
) -> str:
    value = value.casefold()

    value = re.sub(
        r"[^a-z0-9]+",
        "-",
        value,
    )

    return value.strip("-")


def detailed_group_anchor(
    platform: str,
    category: str,
    notation: str | None,
    return_type: str,
) -> str:
    """
    Stable fragment ID shared by the detailed benchmark README
    and the compact root README.
    """

    parts = [
        "benchmark",
        platform,
        category,
    ]

    if notation is not None:
        parts.append(notation)

    parts.append(return_type)

    return "-".join(slug_component(part) for part in parts if part)


def render_table(
    benchmarks: list[Benchmark],
    *,
    metric: str,
    statistic: str,
) -> str:
    """
    Render one benchmark table.

    Columns:

        Type
        Bin2Chars
        Standard library
        Ryu
        Relative performance
    """

    names = implementation_names(benchmarks)

    columns = [
        "Type",
    ]

    if "BIN2CHARS" in names:
        columns.append("Bin2Chars")

    if "STD_LIB" in names:
        columns.append("Standard library")

    if "RYU" in names:
        columns.append("Ryu")

    if "BIN2CHARS" in names and ("STD_LIB" in names or "RYU" in names):
        columns.append("Relative performance")

    alignment = [
        ":---",
        *(":---:" for _ in range(len(columns) - 1)),
    ]

    rows = [
        "| " + " | ".join(columns) + " |",
        "| " + " | ".join(alignment) + " |",
    ]

    for benchmark in sorted(
        benchmarks,
        key=benchmark_sort_key,
    ):
        cells = [display_type(benchmark)]

        if "BIN2CHARS" in names:
            cells.append(
                details_cell(
                    benchmark.implementations.get("BIN2CHARS"),
                    metric=metric,
                    statistic=statistic,
                )
            )

        if "STD_LIB" in names:
            cells.append(
                details_cell(
                    benchmark.implementations.get("STD_LIB"),
                    metric=metric,
                    statistic=statistic,
                )
            )

        if "RYU" in names:
            cells.append(
                details_cell(
                    benchmark.implementations.get("RYU"),
                    metric=metric,
                    statistic=statistic,
                )
            )

        if "BIN2CHARS" in names and ("STD_LIB" in names or "RYU" in names):
            cells.append(
                relative_cell(
                    benchmark.implementations,
                    metric=metric,
                    statistic=statistic,
                )
            )

        rows.append("| " + " | ".join(cells) + " |")

    return "\n".join(rows)


def platform_metadata(
    benchmarks: list[Benchmark],
) -> dict[str, object]:
    """
    Collect representative CPU/build metadata for one platform.

    Hardware/build fields are derived directly from the benchmark JSON
    artifacts. Frequency is intentionally omitted because the recorded
    benchmark frequencies may legitimately differ between JSON files.
    """

    documents: list[dict[str, Any]] = []

    seen_sources: set[Path] = set()

    for benchmark in benchmarks:
        if benchmark.source in seen_sources:
            continue

        seen_sources.add(benchmark.source)

        documents.append(load_json(benchmark.source))

    if not documents:
        return {}

    cpu_infos = [
        document.get("cpu info")
        for document in documents
        if isinstance(
            document.get("cpu info"),
            dict,
        )
    ]

    first_cpu = cpu_infos[0] if cpu_infos else {}

    trials = {
        document.get("trials")
        for document in documents
        if document.get("trials") is not None
    }

    batch_sizes = {
        document.get("batch size")
        for document in documents
        if document.get("batch size") is not None
    }

    metadata: dict[
        str,
        object,
    ] = {
        "model name": first_cpu.get("model name"),
        "cache size": first_cpu.get("cache size"),
        "cache alignment": first_cpu.get("cache alignment"),
        "microcode": first_cpu.get("microcode"),
    }

    if len(trials) == 1:
        metadata["trials"] = next(iter(trials))
    elif trials:
        metadata["trials"] = ", ".join(
            str(value)
            for value in sorted(
                trials,
                key=str,
            )
        )

    if len(batch_sizes) == 1:
        metadata["batch size"] = next(iter(batch_sizes))
    elif batch_sizes:
        metadata["batch size"] = ", ".join(
            str(value)
            for value in sorted(
                batch_sizes,
                key=str,
            )
        )

    return {key: value for key, value in metadata.items() if value is not None}


def render_cpu_info(
    metadata: dict[str, object],
) -> list[str]:
    """
    Render the CPU metadata table used inside each platform section.
    """

    if not metadata:
        return [
            "| Property | Value |",
            "| :--- | :---: |",
            "| — | CPU information unavailable |",
        ]

    rows = [
        "| Property | Value |",
        "| :--- | :---: |",
    ]

    display_names = {
        "model name": "CPU",
        "cache size": "Cache",
        "cache alignment": "Cache alignment",
        "microcode": "Microcode",
        "trials": "Trials",
        "batch size": "Batch size",
    }

    for key, value in metadata.items():
        property_name = display_names.get(
            key,
            key,
        )

        rows.append(f"| {property_name} | `{html.escape(str(value))}` |")

    return rows


def validate_duplicates(
    benchmarks: list[Benchmark],
) -> None:
    """
    A duplicate is defined by the FULL benchmark identity.

    This means:
      - buffered != std_string
      - decimal != exponential
      - float != double
      - different platforms != each other
      - precision 10 != precision 20
    """

    seen: dict[
        tuple[
            str,
            str,
            str | None,
            str,
            str | None,
            str,
        ],
        Benchmark,
    ] = {}

    for benchmark in benchmarks:
        key = (
            benchmark.platform,
            benchmark.category,
            benchmark.notation,
            benchmark.return_type,
            benchmark.value_type,
            benchmark.label,
        )

        previous = seen.get(key)

        if previous is None:
            seen[key] = benchmark
            continue

        raise ValueError(
            "Duplicate benchmark identity detected:\n"
            f"  platform:    {benchmark.platform}\n"
            f"  category:    {benchmark.category}\n"
            f"  notation:    {benchmark.notation}\n"
            f"  return type: {benchmark.return_type}\n"
            f"  value type:  {benchmark.value_type}\n"
            f"  label:       {benchmark.label}\n"
            f"  first:       {previous.source}\n"
            f"  second:      {benchmark.source}"
        )


def group_benchmarks(
    benchmarks: list[Benchmark],
) -> dict[
    tuple[
        str,
        str,
        str | None,
        str,
    ],
    list[Benchmark],
]:
    groups: dict[
        tuple[
            str,
            str,
            str | None,
            str,
        ],
        list[Benchmark],
    ] = {}

    for benchmark in benchmarks:
        key = (
            benchmark.platform,
            benchmark.category,
            benchmark.notation,
            benchmark.return_type,
        )

        groups.setdefault(
            key,
            [],
        ).append(benchmark)

    return groups


def platform_sort_key(
    platform: str,
) -> tuple[str, str]:
    """
    Case-insensitive stable ordering for an arbitrary number of
    platform/build directories.
    """

    return (
        platform.casefold(),
        platform,
    )


def group_sort_key(
    key: tuple[
        str,
        str,
        str | None,
        str,
    ],
) -> tuple[Any, ...]:
    (
        platform,
        category,
        notation,
        return_type,
    ) = key

    category_order = {
        "integer": 0,
        "floating": 1,
    }

    notation_order = {
        None: 0,
        "decimal": 0,
        "exponential": 1,
    }

    return (
        platform_sort_key(platform),
        category_order.get(
            category,
            100,
        ),
        notation_order.get(
            notation,
            100,
        ),
        (0 if return_type == "buffered" else 1),
    )


def render_platform(
    platform: str,
    benchmarks: list[Benchmark],
    groups: dict[
        tuple[
            str,
            str,
            str | None,
            str,
        ],
        list[Benchmark],
    ],
    *,
    metric: str,
    statistic: str,
) -> list[str]:
    """
    Render one complete platform section.

    The hierarchy uses nested <dl>/<dd> elements instead of
    <blockquote>, preserving indentation while avoiding the
    gray GitHub blockquote styling.

    Resulting hierarchy:

        Platform
          CPU information
          Integer
            Buffered
            std::string
          Floating point
            Decimal notation
              Buffered
              std::string
            Exponential notation
              Buffered
              std::string
    """

    platform_keys = [key for key in groups if key[0] == platform]

    platform_keys.sort(key=group_sort_key)

    metadata = platform_metadata(benchmarks)

    cpu_name = metadata.get(
        "model name",
        "CPU information unavailable",
    )

    sections: list[str] = [
        "<details>",
        (
            "<summary>"
            f"<strong>{html.escape(platform)}</strong>"
            f" — {html.escape(str(cpu_name))}"
            "</summary>"
        ),
        "",
        # Platform-level indentation.
        "<dl>",
        "<dd>",
        "",
        # CPU information sits directly under the platform.
        "<details>",
        "<summary><strong>CPU information</strong></summary>",
        "",
        *render_cpu_info(metadata),
        "",
        "</details>",
        "",
    ]

    categories = sorted(
        {key[1] for key in platform_keys},
        key=lambda category: 0 if category == "integer" else 1,
    )

    for category in categories:
        category_keys = [key for key in platform_keys if key[1] == category]

        category_display = "Integer" if category == "integer" else "Floating point"

        # Category-level indentation.
        sections.extend(
            [
                "<details>",
                (f"<summary><strong>{category_display}</strong></summary>"),
                "",
                "<dl>",
                "<dd>",
                "",
            ]
        )

        notations = sorted(
            {key[2] for key in category_keys},
            key=lambda notation: (
                0 if notation is None else (1 if notation == "decimal" else 2)
            ),
        )

        for notation in notations:
            notation_keys = [key for key in category_keys if key[2] == notation]

            if notation is not None:
                # Notation-level indentation.
                sections.extend(
                    [
                        "<details>",
                        (
                            "<summary><strong>"
                            f"{html.escape(notation_display(notation))}"
                            "</strong></summary>"
                        ),
                        "",
                        "<dl>",
                        "<dd>",
                        "",
                    ]
                )

            return_types = sorted(
                {key[3] for key in notation_keys},
                key=lambda return_type: (
                    0
                    if return_type == "buffered"
                    else (1 if return_type == "std_string" else 100)
                ),
            )

            for return_type in return_types:
                group = groups[
                    (
                        platform,
                        category,
                        notation,
                        return_type,
                    )
                ]

                anchor = detailed_group_anchor(
                    platform,
                    category,
                    notation,
                    return_type,
                )

                return_type_heading = return_type_display(return_type)

                sections.extend(
                    [
                        (
                            f'<a id="{
                                html.escape(
                                    anchor,
                                    quote=True,
                                )
                            }"></a>'
                        ),
                        "<details>",
                        (f"<summary><strong>{return_type_heading}</strong></summary>"),
                        "",
                        render_table(
                            group,
                            metric=metric,
                            statistic=statistic,
                        ),
                        "",
                        "</details>",
                        "",
                    ]
                )

            if notation is not None:
                sections.extend(
                    [
                        "</dd>",
                        "</dl>",
                        "",
                        "</details>",
                        "",
                    ]
                )

        sections.extend(
            [
                "</dd>",
                "</dl>",
                "",
                "</details>",
                "",
            ]
        )

    sections.extend(
        [
            "</dd>",
            "</dl>",
            "",
            "</details>",
            "",
        ]
    )

    return sections


def render_results(
    benchmarks: list[Benchmark],
    *,
    metric: str,
    statistic: str,
) -> str:
    if not benchmarks:
        return "## Benchmark Results\n\nNo benchmark JSON files were found."

    validate_duplicates(benchmarks)

    groups = group_benchmarks(benchmarks)

    platforms = sorted(
        {benchmark.platform for benchmark in benchmarks},
        key=platform_sort_key,
    )

    sections = [
        "## Benchmark Results",
        "",
        (
            "Headline values use the "
            f"**{statistic} {metric} cycles** "
            "recorded in the benchmark JSON files."
        ),
        "",
        (
            "Expand any benchmark cell to inspect "
            "the complete recorded mean, median, "
            "minimum, p95, and p99 measurements."
        ),
        "",
        (
            "**Relative performance** is calculated "
            "directly from the recorded measurements "
            "as `baseline / Bin2Chars`."
        ),
        "",
    ]

    for index, platform in enumerate(platforms):
        if index > 0:
            sections.extend(
                [
                    "",
                    "---",
                    "",
                ]
            )

        platform_benchmarks = [
            benchmark for benchmark in benchmarks if benchmark.platform == platform
        ]

        sections.extend(
            render_platform(
                platform,
                platform_benchmarks,
                groups,
                metric=metric,
                statistic=statistic,
            )
        )

    return "\n".join(sections).rstrip()


def replace_marked_section(
    readme: str,
    generated: str,
) -> str:
    pattern = re.compile(
        rf"{re.escape(START_MARKER)}.*?"
        rf"{re.escape(END_MARKER)}",
        flags=re.DOTALL,
    )

    replacement = f"{START_MARKER}\n{generated}\n{END_MARKER}"

    if pattern.search(readme):
        return pattern.sub(
            replacement,
            readme,
            count=1,
        )

    if readme and not readme.endswith("\n\n"):
        readme = readme.rstrip() + "\n\n"

    return readme + replacement + "\n"


def main() -> int:
    parser = argparse.ArgumentParser(
        description=("Generate benchmark Markdown from benchmark/results/**/*.json")
    )

    parser.add_argument(
        "--results",
        type=Path,
        default=Path("benchmark/results"),
        help=("Benchmark results root directory"),
    )

    parser.add_argument(
        "--readme",
        type=Path,
        default=Path("benchmark/README.md"),
        help=("README file to update"),
    )

    parser.add_argument(
        "--metric",
        default=DEFAULT_METRIC,
        choices=(
            "tsc",
            "core",
            "ref",
        ),
        help=("Headline timing metric"),
    )

    parser.add_argument(
        "--statistic",
        default=DEFAULT_STATISTIC,
        choices=(
            "mean",
            "median",
            "min",
            "p95",
            "p99",
        ),
        help=("Headline statistic"),
    )

    args = parser.parse_args()

    benchmarks = discover_benchmarks(args.results)

    generated = render_results(
        benchmarks,
        metric=args.metric,
        statistic=args.statistic,
    )

    readme = args.readme.read_text(encoding="utf-8")

    updated = replace_marked_section(
        readme,
        generated,
    )

    args.readme.write_text(
        updated,
        encoding="utf-8",
    )

    platform_count = len({benchmark.platform for benchmark in benchmarks})

    print(
        "Generated "
        f"{len(benchmarks)} benchmark entries "
        f"across {platform_count} "
        "result directories."
    )

    print(f"Updated: {args.readme}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
