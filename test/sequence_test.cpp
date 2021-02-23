#include "catch.hpp"
#include "test_helper.h"
#include "../src/sequence.h"

#define CATCH_CATEGORY "[sequence]"

TEST_CASE("finding index of note off", CATCH_CATEGORY) {
  MrMKCS *mrm = create_test_data();
  int i;

  Sequence &seq = mrm->song()->sequence(char_to_seq_number('A'));
  REQUIRE(seq.index_of_note_off_for_note_on(0) == 4);
  REQUIRE(seq.index_of_note_off_for_note_on(1) == 5);
  REQUIRE(seq.index_of_note_off_for_note_on(2) == 6);
  REQUIRE(seq.index_of_note_off_for_note_on(3) == -1); // controller message

  // Find matching note on with velocity 0
  seq = mrm->song()->sequence(char_to_seq_number('B'));
  REQUIRE(seq.index_of_note_off_for_note_on(0) == 1);

  delete mrm;
}
