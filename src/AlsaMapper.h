#ifndef ALSAMAPPER_H
#define ALSAMAPPER_H

#include "pch.h"
#include "MidiEvent.h"
#include "RuleMapper.h"
#include "MidiFilter.h"

using namespace std;

// =======================================================================
//                                 ALSA
// =======================================================================

class AlsaMapper: public MidiTranslator {
private:
	void readMidiEvent(snd_seq_event_t* event, TripleVal& val, MidiEvType& tp) const;
	void writeMidiEvent(snd_seq_event_t* event, const TripleVal& val,
			const MidiEvType& tp);
	RuleMapper& rmp;
public:
	AlsaMapper(RuleMapper& rules) :
			rmp(rules) {
	}
	virtual ~AlsaMapper() {}
	// map event and if true send the event to OUT
	virtual bool sendMappedMidiEvent(snd_seq_event_t* event);

};

#endif
