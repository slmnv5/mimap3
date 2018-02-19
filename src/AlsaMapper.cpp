#include "pch.h"
#include "RuleMapper.h"
#include "AlsaMapper.h"
#include "MidiEvent.h"

using namespace std;

bool AlsaMapper::translate(snd_seq_event_t* event) {
	char tp = MidiValue::NOTYPE;
	TripleVal val;

	readMidiEvent(event, val, tp);
	bool changed = rmp.checkRules(val, tp);
	if (changed) {
		writeMidiEvent(event, val, tp);
		cout << tp << val.toString();
	}
	return changed;
}

void AlsaMapper::writeMidiEvent(snd_seq_event_t* event, const TripleVal& val,
		const char& tp) {
	event->data.note.channel = val.ch;
	event->data.note.note = val.v1;
	event->data.note.velocity = val.v2;

	switch (tp) {
	case MidiValue::NOTEON:
		event->type = SND_SEQ_EVENT_NOTEON;
		break;
	case MidiValue::PROGCHANGE:
		event->type = SND_SEQ_EVENT_PGMCHANGE;
		break;
	case MidiValue::CONTROLCHANGE:
		event->type = SND_SEQ_EVENT_CONTROLLER;
		break;
	default:
		break;
	}
}

void AlsaMapper::readMidiEvent(snd_seq_event_t* event, TripleVal& val,
		char& tp) const {

	switch (event->type) {
// snd_seq_ev_note

	case SND_SEQ_EVENT_NOTEOFF:
		tp = MidiValue::NOTEOFF;
		val.ch = event->data.note.channel;
		val.v1 = event->data.note.note;
		val.v2 = event->data.note.velocity;
		break;

	case SND_SEQ_EVENT_NOTE:
	case SND_SEQ_EVENT_NOTEON:
	case SND_SEQ_EVENT_KEYPRESS:
		tp = MidiValue::NOTEON;
		val.ch = event->data.note.channel;
		val.v1 = event->data.note.note;
		val.v2 = event->data.note.velocity;
		break;

// snd_seq_ev_ctrl
	case SND_SEQ_EVENT_PGMCHANGE:
		tp = MidiValue::PROGCHANGE;
		val.ch = event->data.control.channel;
		val.v1 = event->data.control.value;
		val.v2 = MidiValue::MISSINGVAL;
		break;

	case SND_SEQ_EVENT_CONTROLLER:
	case SND_SEQ_EVENT_CHANPRESS:
	case SND_SEQ_EVENT_PITCHBEND:
	case SND_SEQ_EVENT_CONTROL14:
	case SND_SEQ_EVENT_NONREGPARAM:
	case SND_SEQ_EVENT_REGPARAM:
		tp = MidiValue::CONTROLCHANGE;
		val.ch = event->data.control.channel;
		val.v1 = event->data.control.param;
		val.v2 = event->data.control.value;
		break;
// everything else
	default:
		tp = MidiValue::NOTYPE;
		val.ch = MidiValue::MISSINGVAL;
		val.v1 = MidiValue::MISSINGVAL;
		val.v2 = MidiValue::MISSINGVAL;
		break;
	}
}

