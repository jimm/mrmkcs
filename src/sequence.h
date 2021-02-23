#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <string>
#include <vector>
#include "event.h"

struct Sequence {
  // FIXME remove output and channel
  string name;
  vector<Event *> events;

  ~Sequence();

  // Returns index of note off message corresponding to note on message at
  // `note_on_index`. Returns -1 if not found.
  int index_of_note_off_for_note_on(int note_on_index);
};

#endif /* SEQUENCE_H */
