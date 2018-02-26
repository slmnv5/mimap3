#include "pch.h"
#include "RuleMapper.h"
#include "AlsaMapper.h"
#include "MidiEvent.h"

using namespace std;

bool AlsaMapper::translate(snd_seq_event_t* event) {
	MidiEvType tp = MidiEvType::NONE;
	TripleVal val;

	readMidiEvent(event, val, tp);
	bool changed = rmp.checkRules(val, tp);
	if (!changed)
		return true; // pass as is

	if (changed && tp == MidiEvType::NONE)
		return false; // drop event

	writeMidiEvent(event, val, tp);
	if (rmp.getVerbose() > 1)
		cout << "---" << static_cast<char>(tp) << val.toString();

	return changed;
}

void AlsaMapper::writeMidiEvent(snd_seq_event_t* event, const TripleVal& val,
		const MidiEvType& tp) {
	event->data.note.channel = val.ch;
	event->data.note.note = val.v1;
	event->data.note.velocity = val.v2;

	switch (tp) {
	case MidiEvType::NOTE:
		event->type = SND_SEQ_EVENT_NOTEON;
		break;
	case MidiEvType::PROGCHANGE:
		event->type = SND_SEQ_EVENT_PGMCHANGE;
		break;
	case MidiEvType::CONTROLCHANGE:
		event->type = SND_SEQ_EVENT_CONTROLLER;
		break;
	default:
		break;
	}
}

void AlsaMapper::readMidiEvent(snd_seq_event_t* event, TripleVal& val,
		MidiEvType& tp) const {

	switch (event->type) {
// snd_seq_ev_note

	case SND_SEQ_EVENT_NOTEOFF:
	case SND_SEQ_EVENT_NOTE:
	case SND_SEQ_EVENT_NOTEON:
	case SND_SEQ_EVENT_KEYPRESS:
		tp = MidiEvType::NOTE;
		val.ch = event->data.note.channel;
		val.v1 = event->data.note.note;
		val.v2 = event->data.note.velocity;
		break;

// snd_seq_ev_ctrl
	case SND_SEQ_EVENT_PGMCHANGE:
		tp = MidiEvType::PROGCHANGE;
		val.ch = event->data.control.channel;
		val.v1 = event->data.control.value;
		val.v2 = TripleVal::NONE;
		break;

	case SND_SEQ_EVENT_CONTROLLER:
	case SND_SEQ_EVENT_CHANPRESS:
	case SND_SEQ_EVENT_PITCHBEND:
	case SND_SEQ_EVENT_CONTROL14:
	case SND_SEQ_EVENT_NONREGPARAM:
	case SND_SEQ_EVENT_REGPARAM:
		tp = MidiEvType::CONTROLCHANGE;
		val.ch = event->data.control.channel;
		val.v1 = event->data.control.param;
		val.v2 = event->data.control.value;
		break;
// everything else
	default:
		tp = MidiEvType::NONE;
		val.ch = TripleVal::NONE;
		val.v1 = TripleVal::NONE;
		val.v2 = TripleVal::NONE;
		break;
	}
}

