#include "include/Helpers/Simd.hpp"
#define BOOST_TEST_MODULE IntegersTest
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <cstdint>

#include "include/Algos/Integer.hpp"
#include "include/Helpers/Assembly.hpp"

BOOST_AUTO_TEST_CASE(test_all_integegral_v)
{
  Bin2Chars::Helpers::Assembly::pin_thread_to_cpu(3);

  char buff[32];
  Bin2Chars::Helpers::Simd::x86_64::WriteCharsToPtrFowardReturnLength<uint32_t>(&buff[0], 1'234'567'891);
}
