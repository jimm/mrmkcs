#include "test_helper.h"
#include "../src/mrmkcs.h"

PmMessage CLOCK_MESSAGE = Pm_Message(CLOCK, 0, 0);

MrMKCS *create_test_data() {
  int seq_num;
  Output *output;
  MrMKCS *mrm = new MrMKCS();
  Song *song = new Song();

  mrm->song(song);
  mrm->set_testing(true);

  // ==== seq A: major chord
  seq_num = char_to_seq_number('A');
  Sequence &seq_a = song->sequence(seq_num);
  seq_a.name = "Chord";
  output = new Output(pmNoDevice, "out1", "out 1 port");
  mrm->add_output(output);

  // major chord at time 0
  seq_a.events.push_back(new Event(0, output, NOTE_ON + 0, 64, 127));
  seq_a.events.push_back(new Event(0, output, NOTE_ON + 0, 68, 99));
  seq_a.events.push_back(new Event(0, output, NOTE_ON + 0, 71, 127));
  // sustain pedal on at the same time
  seq_a.events.push_back(new Event(0, output, CONTROLLER + 0, CC_SUSTAIN, 127));
  // notes offs two beats later
  seq_a.events.push_back(new Event(48, output, NOTE_OFF + 0, 64, 127));
  seq_a.events.push_back(new Event(0, output, NOTE_OFF + 0, 68, 127));
  seq_a.events.push_back(new Event(0, output, NOTE_OFF + 0, 71, 64));
  // sustain pedal off one beat later
  seq_a.events.push_back(new Event(24, output, CONTROLLER + 0, CC_SUSTAIN, 0));

  // ==== seq B: bass note
  seq_num = char_to_seq_number('B');
  Sequence &seq_b = song->sequence(seq_num);
  seq_b.name = "Bass Note";
  output = new Output(pmNoDevice, "out2", "out 2 port");
  mrm->add_output(output);

  // note on for four beats, then note off (note on, velocity zero)
  seq_b.events.push_back(new Event(0, output, NOTE_ON + 1, 40, 127));
  seq_b.events.push_back(new Event(96, output, NOTE_ON + 1, 40, 0));

  // ==== seq P: programs
  seq_num = char_to_seq_number('P');
  Sequence &seq_p = song->sequence(seq_num);
  seq_p.name = "Programs";

  seq_p.events.push_back(new Event(0, seq_a.events[0]->output,
                                    PROGRAM_CHANGE + 0, 42, 0));
  seq_p.events.push_back(new Event(0, seq_b.events[0]->output,
                                    PROGRAM_CHANGE + 1, 99, 0));

  // ==== seq X: xposed seq A
  seq_num = char_to_seq_number('X');
  Sequence &seq_x = song->sequence(seq_num);
  seq_x.name = "Transposed Sequence A";
  // play sequence A once
  seq_x.events.push_back(new Event(0, char_to_seq_number('A'), 12, 1, false));

  // ==== seq S: song
  seq_num = char_to_seq_number('S');
  Sequence &seq = song->sequence(seq_num);
  seq.name = "Song";
  // send program changes
  seq.events.push_back(new Event(0, char_to_seq_number('P'), 0, 1, false));
  // play A and B twice, waiting for B to finish
  seq.events.push_back(new Event(0, char_to_seq_number('A'), 0, 2, false));
  seq.events.push_back(new Event(0, char_to_seq_number('B'), 0, 2, true));
  // play B one more time
  seq.events.push_back(new Event(0, char_to_seq_number('B'), 0, 1, false));

  return mrm;
}

void clock_ticks(int n) {
  Clock &clock = MrMKCS_instance()->clock();
  while (n-- > 0)
    clock.tick();
}

void print_messages(Instrument *inst) {
  printf("%s messages\n", inst->name.c_str());
  for (int i = 0; i < inst->num_io_messages; ++i)
    printf("  %03d %08x\n", i, inst->io_messages[i]);
}
