#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include "clock.h"
#include "output.h"

#define MICROSECS_PER_SECOND 1000000

static PmMessage START_MESSAGE = Pm_Message(START, 0, 0);
static PmMessage CONTINUE_MESSAGE = Pm_Message(CONTINUE, 0, 0);
static PmMessage STOP_MESSAGE = Pm_Message(STOP, 0, 0);
static PmMessage CLOCK_MESSAGE = Pm_Message(CLOCK, 0, 0);

// ================ periodic time management ================
// See https://www.2net.co.uk/tutorial/periodic_threads

struct periodic_info {
  sigset_t alarm_sig;
};

static int make_periodic(unsigned long period_microsecs, struct periodic_info *info) {
  int ret;
  struct itimerval value;

  // Block SIGALRM in this thread
  sigemptyset(&(info->alarm_sig));
  sigaddset(&(info->alarm_sig), SIGALRM);
  pthread_sigmask(SIG_BLOCK, &(info->alarm_sig), NULL);

  // Set the timer to go off after the first period and then repetitively
  value.it_value.tv_sec = period_microsecs / 1000000L;
  value.it_value.tv_usec = period_microsecs % 1000000L;
  value.it_interval.tv_sec = period_microsecs / 1000000L;
  value.it_interval.tv_usec = period_microsecs % 1000000L;
  ret = setitimer(ITIMER_REAL, &value, NULL);
  if (ret != 0)
    perror("Failed to set timer");

  return ret;
}

static void wait_period(struct periodic_info *info) {
  int sig;

  /* Wait for the next SIGALRM */
  sigwait(&(info->alarm_sig), &sig);
}

static void init_signals() {
  sigset_t alarm_sig;
  sigemptyset(&alarm_sig);
  sigaddset(&alarm_sig, SIGALRM);
  sigprocmask(SIG_BLOCK, &alarm_sig, NULL);
}

// ================ clock ================

void *clock_send_thread(void *clock_ptr) {
  Clock *clock = (Clock *)clock_ptr;
  struct periodic_info info;

  if (make_periodic(clock->microsecs_per_tick, &info) != 0)
    return nullptr;

  while (clock->is_running()) {
     clock->tick();
    wait_period(&info);
  }
  return nullptr;
}

Clock::Clock(vector<Output *> &km_outputs)
  : outputs(km_outputs), thread(nullptr), _ticks_since_start(0)
{
  set_bpm(120);
  init_signals();
}

Clock::~Clock() {
  if (is_running())
    stop();
}

void Clock::set_bpm(float new_val) {
  if (_bpm != new_val) {
    _bpm = new_val;

    // 60 secs per minute / 24 ticks per beat = (2.5 secs-beats / minute-ticks)
    // (2.5 secs-beats / minute-ticks) * MICROSECS_PER_SECOND = (2.5e6 millisecs-beats / minute-ticks)
    // (2.5e6 millisecs-beat / minute-tick) / X beats per minute = X millisecs/tick
    microsecs_per_tick = (long)(2.5e6 / _bpm);
  }
}

void Clock::start() {
  _ticks_since_start = 0;
  start_or_continue(START_MESSAGE);
}

void Clock::continue_clock() {
  start_or_continue(CONTINUE_MESSAGE);
}

void Clock::stop() {
  if (!is_running())
    return;

  send(STOP_MESSAGE);
  thread = nullptr;
}

// Sends CLOCK_MESSAGE and sends update to observers.
void Clock::tick() {
  ++_ticks_since_start;
  send(CLOCK_MESSAGE);
  changed(&_ticks_since_start);
}

void Clock::send(PmMessage msg) {
  PmEvent event {msg, 0};

  for (auto output : outputs)
    output->write(&event, 1);
}

void Clock::start_or_continue(PmMessage msg) {
  if (is_running())
    return;

  send(msg);
  int status = pthread_create(&thread, 0, clock_send_thread, this);
  // TODO check status
}
