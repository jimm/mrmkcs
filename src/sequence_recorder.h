#ifndef SEQUENCE_RECORDER_H
#define SEQUENCE_RECORDER_H

#include "clock.h"
#include "sequence.h"
#include "sequence_recorder.h"

enum SequenceRecorderState {
  srs_stopped,
  srs_recording,
  srs_finished
};

class SequenceRecorder {
public:
  SequenceRecorder(Sequence &seq, Input *input, Output *output);

  void record();
  void stop();
  void return_from_wait(SequenceRecorder *child);

  void midi_in(PmMessage msg);

private:
  Clock &clock;
  Sequence &seq;
  Input *input;
  Output *output;
  long previous_event_time;
  SequenceRecorderState state;

  void done_recording();

  void cleanup_notes_and_controllers();
};

#endif /* SEQUENCE_RECORDER_H */
