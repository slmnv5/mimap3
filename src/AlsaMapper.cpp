#include "pch.h"
#include "RuleMapper.h"
#include "AlsaMapper.h"
#include "MidiEvent.h"

using namespace std;

bool AlsaMapper::sendMappedMidiEvent(snd_seq_event_t* event) {
	MidiEvType tp = MidiEvType::NONE;
	MidiEvent ev;

	readMidiEvent(event, ev);
	bool changed = rmp.applyRules(ev);
	if (!changed)
		return true; // pass as is

	if (changed && tp == MidiEvType::NONE)
		return false; // drop event

	writeMidiEvent(event, ev);
	if (rmp.getVerbose() > 1)
		cout << "Will send mapped event: " << static_cast<char>(tp)
				<< ev.toString();

	return changed;
}

void AlsaMapper::writeMidiEvent(snd_seq_event_t* event, const MidiEvent& ev) {
	event->data.note.channel = ev.get(0);
	event->data.note.note = ev.get(1);
	event->data.note.velocity = ev.get(2);

	switch (ev.evtype) {
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

void AlsaMapper::readMidiEvent(snd_seq_event_t* event, MidiEvent& ev) const {

	switch (event->type) {
// snd_seq_ev_note

	case SND_SEQ_EVENT_NOTEOFF:
	case SND_SEQ_EVENT_NOTE:
	case SND_SEQ_EVENT_NOTEON:
	case SND_SEQ_EVENT_KEYPRESS:
		ev.evtype = MidiEvType::NOTE;
		ev.init(event->data.note.channel, event->data.note.note,
				event->data.note.velocity);
		break;

// snd_seq_ev_ctrl
	case SND_SEQ_EVENT_PGMCHANGE:
		ev.evtype = MidiEvType::PROGCHANGE;
		ev.init(event->data.control.channel, event->data.control.value);
		break;

	case SND_SEQ_EVENT_CONTROLLER:
	case SND_SEQ_EVENT_CHANPRESS:
	case SND_SEQ_EVENT_PITCHBEND:
	case SND_SEQ_EVENT_CONTROL14:
	case SND_SEQ_EVENT_NONREGPARAM:
	case SND_SEQ_EVENT_REGPARAM:
		ev.evtype = MidiEvType::CONTROLCHANGE;
		ev.init(event->data.control.channel, event->data.control.param,
				event->data.control.value);
		break;
// everything else
	default:
		ev.evtype = MidiEvType::NONE;
		break;
	}
}

