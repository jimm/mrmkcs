#include "sequence_recorder.h"
#include "mrmkcs.h"
#include "vector_utils.h"

/** NOTE: SequenceRecorders delete themselves! **/

SequenceRecorder::SequenceRecorder(Sequence &s, Input *in, Output *out)
  : clock(MrMKCS_instance()->clock()), seq(s), input(in), output(out),
    state(srs_stopped), previous_event_time(-1)
{
  MrMKCS_instance()->register_recorder(this);
}

void SequenceRecorder::record() {
  previous_event_time = -1;
  state = srs_recording;
}

void SequenceRecorder::stop() {
  state = srs_stopped;
  cleanup_notes_and_controllers();
}

void SequenceRecorder::midi_in(PmMessage msg) {
  output->write(msg);
  if (state != srs_recording)
    return;

  long curr_time = clock.time();
  long tick_offset = previous_event_time == -1 ? 0 : (curr_time - previous_event_time);
  previous_event_time = curr_time;
  seq.events.push_back(new Event(tick_offset, output, Pm_MessageStatus(msg),
                                 Pm_MessageData1(msg), Pm_MessageData2(msg)));
}

void SequenceRecorder::done_recording() {
  state = srs_finished;
  MrMKCS_instance()->register_recorder_for_cleanup(this);
}

// ================ cleanup ================

void SequenceRecorder::cleanup_notes_and_controllers() {
  for (int i = 0; i < 16; ++i)
    output->write(CONTROLLER + i, CM_ALL_NOTES_OFF, 0);
}
