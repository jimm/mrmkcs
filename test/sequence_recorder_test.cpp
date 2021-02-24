#include "catch.hpp"
#include "test_helper.h"
#include "../src/sequence_recorder.h"

#define CATCH_CATEGORY "[sequence_recorder]"

SequenceRecorder *set_up_recording() {
  MrMKCS *mrm = new MrMKCS();
  Song *song = new Song();
  Input *input = new Input(pmNoDevice, "in1", "in 1 port");
  Output *output = new Output(pmNoDevice, "out1", "out 1 port");
  SequenceRecorder *rec = new SequenceRecorder(song->sequence(0), input, output);

  mrm->song(song);
  mrm->set_testing(true);
  mrm->add_input(input);
  mrm->add_output(output);

  return rec;
}

TEST_CASE("nothing recorded if recording not started", CATCH_CATEGORY) {
  SequenceRecorder *rec = set_up_recording();
  MrMKCS *mrm = MrMKCS_instance();
  Sequence &seq = mrm->song()->sequence(0);
  Input *input = mrm->inputs()[0];
  Output *output = mrm->outputs()[0];

  PmMessage note_on = Pm_Message(NOTE_ON, 64, 127);

  input->read(note_on);

  // make sure it was not recorded
  REQUIRE(seq.events.size() == 0);

  // make sure it was sent through to the output, though
  REQUIRE(output->num_io_messages == 1);
  REQUIRE(output->io_messages[0] == note_on);
}

TEST_CASE("recorded if recording started", CATCH_CATEGORY) {
  SequenceRecorder *rec = set_up_recording();
  MrMKCS *mrm = MrMKCS_instance();
  Sequence &seq = mrm->song()->sequence(0);
  Input *input = mrm->inputs()[0];
  Output *output = mrm->outputs()[0];

  PmMessage note_on = Pm_Message(NOTE_ON, 64, 127);

  // So we can make sure first event is at offset 0.
  clock_ticks(3);

  rec->record();
  input->read(note_on);

  // make sure it was recorded
  REQUIRE(seq.events.size() == 1);
  Event *e = seq.events[0];
  REQUIRE(e->tick_offset == 0);
  REQUIRE(e->output == output);
  REQUIRE(e->status == NOTE_ON);
  REQUIRE(e->data1 == 64);
  REQUIRE(e->data2 == 127);
  REQUIRE(e->wait == false);

  // make sure it was sent through to the output, though
  REQUIRE(output->num_io_messages == 4);
  for (int i = 0; i < 3; ++i)
    REQUIRE(output->io_messages[i] == CLOCK_MESSAGE);
  REQUIRE(output->io_messages[3] == note_on);
}

TEST_CASE("offsets recorded properly", CATCH_CATEGORY) {
  SequenceRecorder *rec = set_up_recording();
  MrMKCS *mrm = MrMKCS_instance();
  Sequence &seq = mrm->song()->sequence(0);
  Input *input = mrm->inputs()[0];
  Output *output = mrm->outputs()[0];

  PmMessage note_on = Pm_Message(NOTE_ON, 64, 127);
  PmMessage note_off = Pm_Message(NOTE_OFF, 64, 99);


  // So we can make sure first event is at offset 0.
  clock_ticks(3);

  rec->record();
  input->read(note_on);

  // make sure it was recorded
  REQUIRE(seq.events.size() == 1);
  REQUIRE(seq.events[0]->tick_offset == 0);
  REQUIRE(seq.events[0]->status == NOTE_ON);

  clock_ticks(4);
  input->read(note_off);

  // offset is correct for note off
  REQUIRE(seq.events.size() == 2);
  REQUIRE(seq.events[1]->tick_offset == 4);
  REQUIRE(seq.events[1]->status == NOTE_OFF);
  
  // make sure both were sent through to the output, though
  REQUIRE(output->num_io_messages == 9);
  for (int i = 0; i < 3; ++i)
    REQUIRE(output->io_messages[i] == CLOCK_MESSAGE);
  REQUIRE(output->io_messages[3] == note_on);
  for (int i = 4; i < 8; ++i)
    REQUIRE(output->io_messages[i] == CLOCK_MESSAGE);
  REQUIRE(output->io_messages[8] == note_off);
}
