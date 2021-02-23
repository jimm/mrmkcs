#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <string>
#include <portmidi.h>

#define MIDI_BUFSIZ 128

using std::string;

class Instrument {
public:
  PmDeviceID device_id;
  string name;
  string device_name;
  PortMidiStream *stream;
  bool enabled;

  // testing only
  PmMessage io_messages[MIDI_BUFSIZ];
  int num_io_messages;
  inline void clear_io_messages() { num_io_messages = 0; }

  Instrument(PmDeviceID device_id, const char *device_name,
             const char *name = nullptr);
  virtual ~Instrument() {}

  virtual bool is_input() { return false; }
  virtual bool is_output() { return false; }

  virtual void start();
  virtual void stop();
  bool real_port();

  void clear();                 // testing only

protected:
  virtual bool start_midi() { return false; }
  virtual void stop_midi();
};

#endif /* INSTRUMENT_H */
