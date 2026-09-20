#!/usr/bin/env python3

from __future__ import annotations

import argparse
import html
import re
from collections import defaultdict
from pathlib import Path

from generate_benchmark_table import (
    Benchmark,
    detailed_group_anchor,
    discover_benchmarks,
    group_benchmarks,
    load_json,
    notation_display,
    platform_sort_key,
    ratio,
    return_type_display,
    validate_duplicates,
)


START_MARKER = "<!-- BENCHMARK_SUMMARY:START -->"
END_MARKER = "<!-- BENCHMARK_SUMMARY:END -->"

DEFAULT_METRIC = "core"
DEFAULT_STATISTIC = "median"


PRECISION_BUCKETS = (
    (0, 10, "0–10"),
    (11, 30, "11–30"),
    (31, 100, "31–100"),
    (101, 300, "101–300"),
    (301, 500, "301–500"),
)


IMPLEMENTATION_DISPLAY_NAMES = {
    "STD_LIB": "Standard library",
    "RYU": "Ryu",
}


def precision_value(
    label: str,
) -> int | None:
    match = re.fullmatch(
        r"precision\s+(\d+)",
        label,
        flags=re.IGNORECASE,
    )

    if match is None:
        return None

    return int(match.group(1))


def precision_bucket(
    value: int,
) -> str:
    for low, high, label in PRECISION_BUCKETS:
        if low <= value <= high:
            return label

    return "501+"


def average(
    values: list[float],
) -> float | None:
    if not values:
        return None

    return sum(values) / len(values)


def implementation_display_name(
    name: str,
) -> str:
    return IMPLEMENTATION_DISPLAY_NAMES.get(
        name,
        name.replace("_", " ").title(),
    )


def implementation_sort_key(
    name: str,
) -> tuple[int, str]:
    return (
        {
            "STD_LIB": 0,
            "RYU": 1,
        }.get(name, 100),
        name,
    )


def comparison_implementations(
    benchmarks: list[Benchmark],
) -> list[str]:
    """
    Discover all implementations that can be compared against BIN2CHARS.

    BIN2CHARS is the reference implementation and is intentionally
    excluded from the displayed comparison set.

    This keeps the summary fully data-driven.
    """

    implementations: set[str] = set()

    for benchmark in benchmarks:
        implementations.update(benchmark.implementations.keys())

    implementations.discard("BIN2CHARS")

    return sorted(
        implementations,
        key=implementation_sort_key,
    )


def relative_summary(
    benchmarks: list[Benchmark],
    *,
    implementation: str,
    metric: str,
    statistic: str,
) -> float | None:
    """
    Average the relative baseline/Bin2Chars ratios for a group.
    """

    values: list[float] = []

    for benchmark in benchmarks:
        baseline = benchmark.implementations.get(implementation)

        bin2chars = benchmark.implementations.get("BIN2CHARS")

        value = ratio(
            baseline,
            bin2chars,
            metric=metric,
            statistic=statistic,
        )

        if value is not None:
            values.append(value)

    return average(values)


def format_ratio(
    value: float | None,
) -> str:
    if value is None:
        return "—"

    return f"{value:.3f}×"


def relative_link(
    value: float | None,
    *,
    benchmark_anchor: str,
) -> str:
    if value is None:
        return "—"

    return f"[{format_ratio(value)}](benchmark/README.md#{benchmark_anchor})"


def platform_metadata(
    benchmarks: list[Benchmark],
) -> dict[str, object]:
    """
    Get representative CPU/build metadata for a platform.

    Stable hardware fields are shown in the root README.
    Frequency is intentionally omitted because benchmark JSONs
    can legitimately record different observed frequencies.
    """

    documents: list[dict] = []

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

    result: dict[str, object] = {
        "model name": first_cpu.get("model name"),
        "cache size": first_cpu.get("cache size"),
        "cache alignment": first_cpu.get("cache alignment"),
        "microcode": first_cpu.get("microcode"),
    }

    if len(trials) == 1:
        result["trials"] = next(iter(trials))
    elif trials:
        result["trials"] = ", ".join(
            str(value)
            for value in sorted(
                trials,
                key=str,
            )
        )

    if len(batch_sizes) == 1:
        result["batch size"] = next(iter(batch_sizes))
    elif batch_sizes:
        result["batch size"] = ", ".join(
            str(value)
            for value in sorted(
                batch_sizes,
                key=str,
            )
        )

    return {key: value for key, value in result.items() if value is not None}


def render_cpu_info(
    metadata: dict[str, object],
) -> list[str]:
    if not metadata:
        return [
            "| Property | Value |",
            "| :--- | :--- |",
            "| — | CPU information unavailable |",
        ]

    rows = [
        "| Property | Value |",
        "| :--- | :--- |",
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
        rows.append(f"| {display_names.get(key, key)} | `{html.escape(str(value))}` |")

    return rows


def render_integer_group(
    group: list[Benchmark],
    *,
    metric: str,
    statistic: str,
) -> list[str]:
    """
    Compact integer summary.

    Return types are intentionally NOT collapsible.
    """

    implementations = comparison_implementations(group)

    if not implementations:
        return [
            "| Baseline | Relative performance |",
            "| :--- | ---: |",
            "| — | — |",
        ]

    sections = [
        "| Baseline | Relative performance |",
        "| :--- | ---: |",
    ]

    anchor = detailed_group_anchor(
        group[0].platform,
        "integer",
        None,
        group[0].return_type,
    )

    for implementation in implementations:
        value = relative_summary(
            group,
            implementation=implementation,
            metric=metric,
            statistic=statistic,
        )

        relative = relative_link(
            value,
            benchmark_anchor=anchor,
        )

        if value is None:
            displayed_value = "—"
        else:
            displayed_value = (
                f"{relative} {implementation_display_name(implementation)}"
            )

        sections.append(
            f"| {implementation_display_name(implementation)} | {displayed_value} |"
        )

    return sections


def render_floating_group(
    group: list[Benchmark],
    *,
    metric: str,
    statistic: str,
) -> list[str]:
    """
    Compact floating-point summary.

    Return types are intentionally NOT collapsible.

    Each precision bucket can contain multiple implementations:

        1.241× Standard library
        1.000× Ryu
    """

    implementations = comparison_implementations(group)

    value_types = sorted(
        {
            benchmark.value_type
            for benchmark in group
            if benchmark.value_type is not None
        },
        key=lambda value_type: {
            "float": 0,
            "double": 1,
        }.get(
            value_type,
            100,
        ),
    )

    buckets = (
        "0–10",
        "11–30",
        "31–100",
        "101–300",
        "301–500",
        "501+",
    )

    sections = [
        "| Type | " + " | ".join(buckets) + " |",
        "| :--- | " + " | ".join(["---:"] * len(buckets)) + " |",
    ]

    anchor = detailed_group_anchor(
        group[0].platform,
        "floating",
        group[0].notation,
        group[0].return_type,
    )

    for value_type in value_types:
        type_benchmarks = [
            benchmark for benchmark in group if benchmark.value_type == value_type
        ]

        bucketed: dict[
            str,
            list[Benchmark],
        ] = defaultdict(list)

        for benchmark in type_benchmarks:
            precision = precision_value(benchmark.label)

            if precision is None:
                continue

            bucketed[precision_bucket(precision)].append(benchmark)

        cells = [f"`{value_type}`"]

        for bucket in buckets:
            bucket_benchmarks = bucketed.get(
                bucket,
                [],
            )

            pieces: list[str] = []

            for implementation in implementations:
                value = relative_summary(
                    bucket_benchmarks,
                    implementation=implementation,
                    metric=metric,
                    statistic=statistic,
                )

                if value is None:
                    continue

                pieces.append(
                    (
                        f"{
                            relative_link(
                                value,
                                benchmark_anchor=anchor,
                            )
                        } "
                        f"{implementation_display_name(implementation)}"
                    )
                )

            cells.append("<br>".join(pieces) if pieces else "—")

        sections.append("| " + " | ".join(cells) + " |")

    if not value_types:
        sections.append("| — | No floating-point benchmark data recorded |")

    return sections


def render_platform_summary(
    platform: str,
    benchmarks: list[Benchmark],
    *,
    metric: str,
    statistic: str,
) -> list[str]:
    """
    Render one complete compact platform summary.

    The hierarchy intentionally uses nested <dl>/<dd> elements
    instead of <blockquote>:

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

    Return types remain intentionally non-collapsible.
    """

    groups = group_benchmarks(benchmarks)

    platform_keys = [key for key in groups if key[0] == platform]

    metadata = platform_metadata(benchmarks)

    cpu_name = metadata.get(
        "model name",
        "CPU information unavailable",
    )

    sections = [
        "<details>",
        (
            "<summary>"
            f"<strong>{html.escape(platform)}</strong>"
            f" — {html.escape(str(cpu_name))}"
            "</summary>"
        ),
        "",
        # Platform level indentation.
        "<dl>",
        "<dd>",
        "",
        # CPU information is at the same hierarchy level
        # as Integer / Floating point.
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
        key=lambda category: (
            0 if category == "integer" else 1,
            category,
        ),
    )

    for category in categories:
        category_keys = [key for key in platform_keys if key[1] == category]

        category_display = "Integer" if category == "integer" else "Floating point"

        # Category level indentation.
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
                0
                if notation is None
                else (
                    1
                    if notation == "decimal"
                    else (2 if notation == "exponential" else 100)
                ),
                ("" if notation is None else str(notation)),
            ),
        )

        for notation in notations:
            notation_keys = [key for key in category_keys if key[2] == notation]

            if notation is not None:
                # Notation level indentation.
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
                    else (1 if return_type == "std_string" else 100),
                    return_type,
                ),
            )

            # Return types are deliberately NOT collapsible.
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

                return_type_heading = html.escape(return_type_display(return_type))

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
                        f"#### {return_type_heading}",
                        "",
                    ]
                )

                if category == "integer":
                    sections.extend(
                        render_integer_group(
                            group,
                            metric=metric,
                            statistic=statistic,
                        )
                    )
                else:
                    sections.extend(
                        render_floating_group(
                            group,
                            metric=metric,
                            statistic=statistic,
                        )
                    )

                sections.extend(
                    [
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
            "[View complete benchmark data →](benchmark/README.md)",
            "",
            "</details>",
            "",
        ]
    )

    return sections


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
        description=(
            "Generate the compact root README benchmark "
            "summary from benchmark/results/**/*.json"
        )
    )

    parser.add_argument(
        "--results",
        type=Path,
        default=Path("benchmark/results"),
    )

    parser.add_argument(
        "--readme",
        type=Path,
        default=Path("README.md"),
    )

    parser.add_argument(
        "--metric",
        default=DEFAULT_METRIC,
        choices=(
            "tsc",
            "core",
            "ref",
        ),
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
    )

    args = parser.parse_args()

    benchmarks = discover_benchmarks(args.results)

    validate_duplicates(benchmarks)

    by_platform: dict[
        str,
        list[Benchmark],
    ] = defaultdict(list)

    for benchmark in benchmarks:
        by_platform[benchmark.platform].append(benchmark)

    sections = [
        "## Performance",
        "",
        (
            "Benchmark summaries are generated directly from the "
            "benchmark JSON artifacts. Relative performance uses "
            f"the **{args.statistic} {args.metric}** measurement."
        ),
        "",
    ]

    platforms = sorted(
        by_platform,
        key=platform_sort_key,
    )

    for index, platform in enumerate(platforms):
        if index:
            sections.extend(
                [
                    "",
                    "---",
                    "",
                ]
            )

        sections.extend(
            render_platform_summary(
                platform,
                by_platform[platform],
                metric=args.metric,
                statistic=args.statistic,
            )
        )

    generated = "\n".join(sections).rstrip()

    readme = args.readme.read_text(encoding="utf-8")

    updated = replace_marked_section(
        readme,
        generated,
    )

    args.readme.write_text(
        updated,
        encoding="utf-8",
    )

    print(f"Generated compact benchmark summary for {len(platforms)} platform(s).")

    print(f"Updated: {args.readme}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
