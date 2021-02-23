#include "sequence.h"

Sequence::~Sequence()
{
  for (auto e : events)
    delete e;
}

// Returns index of note off message corresponding to note on message at
// `note_on_index`. Returns -1 if not found or if the event at the given
// index is not a note on.
int Sequence::index_of_note_off_for_note_on(int note_on_index) {
  Event *note_on_event = events[note_on_index];
  uint8_t note_on_status = note_on_event->status;

  if ((note_on_status & 0xf0) != NOTE_ON)
    return -1;

  for (int i = note_on_index + 1; i < events.size(); ++i) {
    Event *event = events[i];
    if (!event->is_midi()                          // not a MIDI event
        || event->data1 != note_on_event->data1    // note numbers differ
        || event->output != note_on_event->output) // different outputs
      continue;

    // note on, velocity 0
    if (event->status == note_on_status && event->data2 == 0)
      return i;

    // note off, same channel
    if ((event->status + 0x10) == note_on_status)
      return i;
  }
  return -1;
}
