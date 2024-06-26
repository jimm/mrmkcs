#include <stdlib.h>
#include <string.h>
#include <portmidi.h>
#include "error.h"
#include "output.h"

Output::Output(PmDeviceID device_id, const char *device_name, const char *name)
  : Instrument(device_id, device_name, name)
{
}

bool Output::start_midi() {
  PmError err = Pm_OpenOutput(&stream, device_id, 0, 128, 0, 0, 0);
  if (err == 0)
    return true;

  char buf[BUFSIZ];
  snprintf(buf, BUFSIZ, "error opening output stream %s: %s\n", name.c_str(),
          Pm_GetErrorText(err));
  error_message(buf);
  return false;
}

void Output::write(PmEvent *buf, int len) {
  if (!real_port()) {
    for (int i = 0; i < len && num_io_messages < MIDI_BUFSIZ-1; ++i)
      io_messages[num_io_messages++] = buf[i].message;
    return;
  }

  if (!enabled)
    return;

  output_mutex.lock();
  PmError err = Pm_Write(stream, buf, len);
  output_mutex.unlock();
  handle_error(err);
}

void Output::write(PmMessage msg) {
  PmEvent event = {msg, 0};
  write(&event, 1);
}

void Output::write(uint8_t byte1, uint8_t byte2, uint8_t byte3) {
  PmEvent event = {(PmMessage)Pm_Message(byte1, byte2, byte3), 0};
  write(&event, 1);
}

void Output::handle_error(PmError err) {
  if (err == 0)
    return;

  char err_msg_buf[BUFSIZ];
  snprintf(err_msg_buf, BUFSIZ, "error writing MIDI to %s: %s\n",
          name.c_str(), Pm_GetErrorText(err));
  error_message(err_msg_buf);
}
