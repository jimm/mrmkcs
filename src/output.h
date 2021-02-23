#ifndef OUTPUT_H
#define OUTPUT_H

#include "instrument.h"
#include "input.h"

class Output : public Instrument {
public:
  Output(PmDeviceID device_id, const char *device_name, const char *name = nullptr);

  virtual bool is_output() { return true; }

  void write(PmEvent *buf, int len);
  void write_midi(uint8_t byte1, uint8_t byte2, uint8_t byte3);

protected:
  mutex output_mutex;

  virtual bool start_midi();
  virtual void handle_error(PmError err);
};

#endif /* OUTPUT_H */
