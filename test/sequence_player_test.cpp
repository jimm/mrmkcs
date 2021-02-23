#include "catch.hpp"
#include "test_helper.h"
#include "../src/sequence_player.h"

#define CATCH_CATEGORY "[sequence_player]"

TEST_CASE("one tick sends multiple events", CATCH_CATEGORY) {
  MrMKCS *mrm = create_test_data();
  Output *out1 = mrm->outputs()[0];
  Output *out2 = mrm->outputs()[1];

  // sanity checks
  REQUIRE(out1->name == "out 1 port");
  REQUIRE(out2->name == "out 2 port");

  REQUIRE(out1->num_io_messages == 0);
  REQUIRE(out2->num_io_messages == 0);

  mrm->play_sequence(char_to_seq_number('P'));

  // nothing sent yet because clock ticks not sent
  REQUIRE(out1->num_io_messages == 0);
  REQUIRE(out2->num_io_messages == 0);

  clock_ticks(1);

  REQUIRE(out1->num_io_messages == 2);
  REQUIRE(out1->io_messages[0] == CLOCK_MESSAGE);
  REQUIRE(out1->io_messages[1] == Pm_Message(PROGRAM_CHANGE + 0, 42, 0));

  REQUIRE(out2->num_io_messages == 2);
  REQUIRE(out2->io_messages[0] == CLOCK_MESSAGE);
  REQUIRE(out2->io_messages[1] == Pm_Message(PROGRAM_CHANGE + 1, 99, 0));

  delete mrm;
}

TEST_CASE("waits proper number of ticks", CATCH_CATEGORY) {
  MrMKCS *mrm = create_test_data();
  Clock &clock = mrm->clock();
  Output *out1 = mrm->outputs()[0];

  mrm->play_sequence(char_to_seq_number('A'));
  clock_ticks(1);

  REQUIRE(out1->num_io_messages == 5);
  REQUIRE(out1->io_messages[0] == CLOCK_MESSAGE);
  REQUIRE(out1->io_messages[1] == Pm_Message(NOTE_ON, 64, 127));
  REQUIRE(out1->io_messages[2] == Pm_Message(NOTE_ON, 68, 99));
  REQUIRE(out1->io_messages[3] == Pm_Message(NOTE_ON, 71, 127));
  REQUIRE(out1->io_messages[4] == Pm_Message(CONTROLLER, CC_SUSTAIN, 127));

  out1->clear_io_messages();
  clock_ticks(47);
  REQUIRE(clock.time() == 48);
  REQUIRE(out1->num_io_messages == 47);
  for (int i = 0; i < 47; ++i)
    REQUIRE(out1->io_messages[i] == CLOCK_MESSAGE);

  out1->clear_io_messages();
  clock_ticks(1);
  REQUIRE(clock.time() == 49);
  REQUIRE(out1->num_io_messages == 4);
  REQUIRE(out1->io_messages[0] == CLOCK_MESSAGE);
  REQUIRE(out1->io_messages[1] == Pm_Message(NOTE_OFF, 64, 127));
  REQUIRE(out1->io_messages[2] == Pm_Message(NOTE_OFF, 68, 127));
  REQUIRE(out1->io_messages[3] == Pm_Message(NOTE_OFF, 71, 64));

  out1->clear_io_messages();
  clock_ticks(24);
  REQUIRE(out1->num_io_messages == 25);
  for (int i = 0; i < 24; ++i)
    REQUIRE(out1->io_messages[i] == CLOCK_MESSAGE);
  REQUIRE(out1->io_messages[24] == Pm_Message(CONTROLLER, CC_SUSTAIN, 0));

  delete mrm;
}

TEST_CASE("send all notes off when stopped", CATCH_CATEGORY) {
  MrMKCS *mrm = create_test_data();
  Output *out1 = mrm->outputs()[0];

  SequencePlayer *player = mrm->play_sequence(char_to_seq_number('A'));
  clock_ticks(1);

  REQUIRE(out1->num_io_messages == 5);

  out1->clear_io_messages();
  player->stop();

  REQUIRE(out1->num_io_messages == 3);
  REQUIRE(out1->io_messages[0] == Pm_Message(NOTE_OFF, 64, 127));
  REQUIRE(out1->io_messages[1] == Pm_Message(NOTE_OFF, 68, 127));
  REQUIRE(out1->io_messages[2] == Pm_Message(NOTE_OFF, 71, 64));

  delete mrm;
}

// This test also ensures that a sequence player that is stopped also stops
// all of its sub-sequence players.
TEST_CASE("parent transposes notes in sequence, including stop note offs", CATCH_CATEGORY) {
  MrMKCS *mrm = create_test_data();
  Output *out1 = mrm->outputs()[0];

  SequencePlayer *player = mrm->play_sequence(char_to_seq_number('X'));
  clock_ticks(1);

  REQUIRE(out1->num_io_messages == 5);
  REQUIRE(out1->io_messages[0] == CLOCK_MESSAGE);
  REQUIRE(out1->io_messages[1] == Pm_Message(NOTE_ON, 64 + 12, 127));
  REQUIRE(out1->io_messages[2] == Pm_Message(NOTE_ON, 68 + 12, 99));
  REQUIRE(out1->io_messages[3] == Pm_Message(NOTE_ON, 71 + 12, 127));
  REQUIRE(out1->io_messages[4] == Pm_Message(CONTROLLER, CC_SUSTAIN, 127));

  out1->clear_io_messages();
  player->stop();

  REQUIRE(out1->num_io_messages == 3);
  REQUIRE(out1->io_messages[0] == Pm_Message(NOTE_OFF, 64 + 12, 127));
  REQUIRE(out1->io_messages[1] == Pm_Message(NOTE_OFF, 68 + 12, 127));
  REQUIRE(out1->io_messages[2] == Pm_Message(NOTE_OFF, 71 + 12, 64));

  delete mrm;
}

TEST_CASE("transpose note offs sent on stop", CATCH_CATEGORY) {
  MrMKCS *mrm = create_test_data();
  Output *out1 = mrm->outputs()[0];
  int xpose = 12;

  SequencePlayer *player = mrm->play_sequence(char_to_seq_number('A'));
  clock_ticks(1);

  REQUIRE(out1->num_io_messages == 5);

  out1->clear_io_messages();
  player->stop();

  REQUIRE(out1->num_io_messages == 3);
  REQUIRE(out1->io_messages[0] == Pm_Message(NOTE_OFF, 64, 127));
  REQUIRE(out1->io_messages[1] == Pm_Message(NOTE_OFF, 68, 127));
  REQUIRE(out1->io_messages[2] == Pm_Message(NOTE_OFF, 71, 64));

  delete mrm;
}

TEST_CASE("play whole song with waits", CATCH_CATEGORY) {
  MrMKCS *mrm = create_test_data();
  Output *out1 = mrm->outputs()[0];
  Output *out2 = mrm->outputs()[1];

  SequencePlayer *player = mrm->play_sequence(char_to_seq_number('S'));

  delete mrm;
}
