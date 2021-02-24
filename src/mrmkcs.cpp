#include "mrmkcs.h"
#include "input.h"
#include "output.h"
#include "sequence_recorder.h"

static MrMKCS *mrm_instance = nullptr;

MrMKCS *MrMKCS_instance() {
  return mrm_instance;
}

// ================ allocation ================

MrMKCS::MrMKCS()
  : _running(false), _testing(false), _clock(_outputs), _song(nullptr),
    _recorder(nullptr)
{
  mrm_instance = this;
}

MrMKCS::~MrMKCS() {
  if (mrm_instance == this)
    mrm_instance = nullptr;

  stop_clock();

  for (auto& in : _inputs)
    delete in;
  for (auto& out : _outputs)
    delete out;
}

// ================ running ================

void MrMKCS::start() {
  for (auto& out : _outputs)
    out->start();
  for (auto& in : _inputs)
    in->start();
  _running = true;
}

void MrMKCS::stop() {
  stop_clock();
  _running = false;
  for (auto& in : _inputs)
    in->stop();
  for (auto& out : _outputs)
    out->stop();
}

void MrMKCS::midi_in(PmMessage msg) {
  if (_recorder != nullptr)
    _recorder->midi_in(msg);
  else
    // TODO midi through
    ;
}

// ================ initialization ================

void MrMKCS::initialize() {
}

// ================ clock ================

void MrMKCS::start_clock() {
  _clock.start();
}

void MrMKCS::continue_clock() {
  _clock.continue_clock();
}

void MrMKCS::stop_clock() {
  _clock.stop();
  cleanup();
}

void MrMKCS::toggle_clock() {
  if (clock_is_running())
    stop_clock();
  else
    start_clock();
}

bool MrMKCS::clock_is_running() {
  return _clock.is_running();
}

void MrMKCS::set_clock_bpm(int bpm) {
  _clock.set_bpm(bpm);
}

// ================ memory_management ================

void MrMKCS::register_player_for_cleanup(SequencePlayer *sp) {
  _finished_sequence_players.insert(sp);
}

void MrMKCS::register_recorder(SequenceRecorder *rec) {
  _recorder = rec;
}

void MrMKCS::register_recorder_for_cleanup(SequenceRecorder *rec) {
  _recorder = nullptr;
  _finished_sequence_recorders.insert(rec);
}

void MrMKCS::cleanup() {
  _finished_sequence_players_mutex.lock();
  for (auto sp : _finished_sequence_players)
    delete sp;
  _finished_sequence_players.clear();
  _finished_sequence_players_mutex.unlock();

  _finished_sequence_recorders_mutex.lock();
  for (auto sr : _finished_sequence_recorders)
    delete sr;
  _finished_sequence_recorders.clear();
  _finished_sequence_recorders_mutex.unlock();
}
