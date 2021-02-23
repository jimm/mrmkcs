#include "catch.hpp"
#include "../src/consts.h"
#include "../src/song.h"

#define CATCH_CATEGORY "[song]"

TEST_CASE("char to song number conversion", CATCH_CATEGORY) {
  REQUIRE(char_to_seq_number('0') == 0);
  REQUIRE(char_to_seq_number('9') == 9);
  REQUIRE(char_to_seq_number('A') == 10);
  REQUIRE(char_to_seq_number('Z') == 35);
  REQUIRE(char_to_seq_number('a') == 10);
  REQUIRE(char_to_seq_number('z') == 35);
}
