#ifndef MIDIFILTER_H
#define MIDIFILTER_H

#include "pch.h"
#include <alsa/asoundlib.h>

using namespace std;

// =======================================================================
//                                 ALSA
// =======================================================================

class MidiTranslator {
public:
	virtual bool translate(snd_seq_event_t* event) = 0;
};

class MidiFilter {
private:
	int client = -1;
	int inport = -1;
	int outport = -1;
	snd_seq_t *seq_handle = nullptr;
	MidiTranslator& trans;

public:
	MidiFilter(MidiTranslator& tr) :
			trans(tr) {
	}

	void open_alsa_connection();
	void process_events();
};

#endif
