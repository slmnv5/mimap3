#ifndef ALSAMAPPER_H
#define ALSAMAPPER_H

#include "pch.h"
#include "MidiEvent.h"
#include "RuleMapper.h"

using namespace std;

// =======================================================================
//                                 ALSA
// =======================================================================

class AlsaMapper {
private:

	int inport = 0;
	int outport = 0;
	snd_seq_t* sequencer = NULL;
	void readMidiEvent(snd_seq_event_t* event, TripleVal& val, char& tp) const;
	void writeMidiEvent(snd_seq_event_t* event, const TripleVal& val,
			const char& tp);
	RuleMapper& rmp;
public:
	AlsaMapper(RuleMapper& rules) :
			rmp(rules) {
		open_alsa_connection();
		process_events();
	}

	void open_alsa_connection();
	void process_events();
};

#endif //ALSAMAPPER_H
