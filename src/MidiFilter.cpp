#include "pch.h"
#include "MidiFilter.h"

using namespace std;

void MidiFilter::open_alsa_connection() {

	if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0)
		throw "Error opening ALSA seq_handle.\n";

	snd_seq_set_client_name(seq_handle, "MIDIRouter");
	client = snd_seq_client_id(seq_handle);

	inport = snd_seq_create_simple_port(seq_handle, "IN",
	SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
	SND_SEQ_PORT_TYPE_APPLICATION);
	if (inport < 0)
		throw "Error creating seq_handle IN port.\n";

	outport = snd_seq_create_simple_port(seq_handle, "OUT",
	SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
	SND_SEQ_PORT_TYPE_APPLICATION);
	if (outport < 0)
		throw "Error creating seq_handle OUT port.\n";

	cout << "IN=" << client << ":" << inport << "   OUT=" << client << ":"
			<< outport << endl;
}

void MidiFilter::process_events() {
	snd_seq_event_t* event = nullptr;

	while (true) {
		int result = snd_seq_event_input(seq_handle, &event);
		if (result < 0) {
			cerr << "Possible loss of events! " << result << endl;
			continue;
		}

		if (!trans.sendMappedMidiEvent(event)) {
			snd_seq_free_event(event);
			continue;
		}

		snd_seq_ev_set_subs(event);
		snd_seq_ev_set_direct(event);
		snd_seq_ev_set_source(event, outport);
		snd_seq_event_output_direct(seq_handle, event);
		snd_seq_free_event(event);

	}

}
