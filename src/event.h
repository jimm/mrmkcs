#ifndef EVENT_H
#define EVENT_H

#include <cstdint>
#include <portmidi.h>
#include "output.h"

class Event {
public:
  uint16_t tick_offset;         // num ticks from previous event
  Output *output;               // null if sequence event
  uint8_t status;               // MIDI status or seq number 0-35 
  uint8_t data1;
  uint8_t data2;
  bool wait;                    // if seq

  Event();
  Event(const Event &other);
  Event(uint16_t tick_offset, Output *output, uint8_t status, uint8_t data1, uint8_t data2);
  Event(uint16_t tick_offset, Output *output, PmMessage msg);
  Event(uint8_t tick_offset, int sequence_number, int transpose, int repeats, bool wait);

  inline bool is_midi() { return status >= 0x80; }
  inline bool is_sequence() { return status <= 35; }
  inline bool is_note() { return status >= 0x80 && status <= 0xaf; } // on, off, poly press

  inline int sequence_number() { return (int)status; }
  inline int transpose() { return (int)data1; }
  inline int repeats() { return (int)data2; }

  void send();
};

#endif /* EVENT_H */
