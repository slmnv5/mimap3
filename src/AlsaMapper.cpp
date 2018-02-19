#include "pch.h"
#include "RuleMapper.h"
#include "AlsaMapper.h"
#include "MidiEvent.h"

using namespace std;

void AlsaMapper::open_alsa_connection() {
	int client;

	if (snd_seq_open(&sequencer, "default", SND_SEQ_OPEN_INPUT, 0) < 0)
		throw "Couldn't open ALSA sequencer";

	snd_seq_set_client_name(sequencer, "MIDIMapper");
	client = snd_seq_client_id(sequencer);

	inport = snd_seq_create_simple_port(sequencer, "MapperIN",
	SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
	SND_SEQ_PORT_TYPE_APPLICATION);
	if (inport < 0)
		throw "Couldn't create input port";

	outport = snd_seq_create_simple_port(sequencer, "MapperOUT",
	SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
	SND_SEQ_PORT_TYPE_APPLICATION);
	if (outport < 0)
		throw "Couldn't create output port";

	cout << "IN=" << client << ":" << inport << "   OUT=" << client << ":"
			<< outport << endl;
}

/*-
 //example http://www.alsa-project.org/alsa-doc/alsa-lib/seq.html
 void alsaMapper::direct_delivery1111111(snd_seq_t *seq) {
 snd_seq_event_t ev;
 snd_seq_ev_clear(&ev);
 snd_seq_ev_set_source(&ev, outport);
 snd_seq_ev_set_subs(&ev);
 snd_seq_ev_set_direct(&ev);
 // set event type, data, so on..
 snd_seq_event_output(seq, &ev);
 snd_seq_drain_output(seq);
 }
 //example
 void alsaMapper::event_filter111111111(snd_seq_t *seq, snd_seq_event_t *ev) {
 while (snd_seq_event_input(seq, &ev) >= 0) {
 //.. modify input event ..
 snd_seq_ev_set_source(ev, outport);
 snd_seq_ev_set_subs(ev);
 snd_seq_ev_set_direct(ev);
 snd_seq_event_output(seq, ev);
 snd_seq_drain_output(seq);
 }
 }
 */
void AlsaMapper::writeMidiEvent(snd_seq_event_t* event, const TripleVal& val,
		const char& tp) {
	event->data.note.channel = val.ch;
	event->data.note.note = val.v1;
	event->data.note.velocity = val.v2;

	switch (tp) {
	case 'n':
		event->type = SND_SEQ_EVENT_NOTEON;
		break;
	case 'p':
		event->type = SND_SEQ_EVENT_PGMCHANGE;
		break;
	case 'c':
	case SND_SEQ_EVENT_CONTROLLER:
		break;
	default:
		break;
	}
}

void AlsaMapper::readMidiEvent(snd_seq_event_t* event, TripleVal& val,
		char& tp) const {

	switch (event->type) {
// snd_seq_ev_note

	case SND_SEQ_EVENT_NOTE:
	case SND_SEQ_EVENT_NOTEON:
	case SND_SEQ_EVENT_NOTEOFF:
	case SND_SEQ_EVENT_KEYPRESS:
		tp = NOTE;
		val.ch = event->data.note.channel;
		val.v1 = event->data.note.note;
		val.v2 = event->data.note.velocity;
		break;

// snd_seq_ev_ctrl
	case SND_SEQ_EVENT_PGMCHANGE:
		tp = PROGCHANGE;
		val.ch = event->data.control.channel;
		val.v1 = event->data.control.value;
		val.v2 = MISSING;
		break;

	case SND_SEQ_EVENT_CONTROLLER:
	case SND_SEQ_EVENT_CHANPRESS:
	case SND_SEQ_EVENT_PITCHBEND:
	case SND_SEQ_EVENT_CONTROL14:
	case SND_SEQ_EVENT_NONREGPARAM:
	case SND_SEQ_EVENT_REGPARAM:
		tp = CONTROLCHANGE;
		val.ch = event->data.control.channel;
		val.v1 = event->data.control.param;
		val.v2 = event->data.control.value;
		break;
// everything else
	default:
		tp = NOTYPE;
		val.ch = MISSING;
		val.v1 = MISSING;
		val.v2 = MISSING;
		break;
	}
}

void AlsaMapper::process_events() {
	snd_seq_event_t* event = NULL;
	char tp = NOTYPE;
	TripleVal val;

	for (;;) {
		if (snd_seq_event_input(sequencer, &event) < 0) {
			continue;
		}


		readMidiEvent(event, val, tp);
		if (rmp.checkRules(val, tp)) {
			writeMidiEvent(event, val, tp);
			cout << tp << val.toString();
		}
		 snd_seq_ev_set_source(event, outport);
		 snd_seq_ev_set_subs(event);
		 snd_seq_ev_set_direct(event);
		 snd_seq_event_output(sequencer, event);
		 snd_seq_drain_output(sequencer);
		 //===================
		//snd_seq_ev_set_subs(event);
		//snd_seq_ev_set_direct(event);

		//snd_seq_ev_set_source(event, outport);
		//snd_seq_event_output_direct(sequencer, event);
		//snd_seq_free_event(event);

	}
}
