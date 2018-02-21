#ifndef RULEMAPPER_H
#define RULEMAPPER_H

#include "pch.h"
#include "MidiEvent.h"

using namespace std;

class RuleMapper {
public:
	RuleMapper() {
	}

	void parseRuleString(const string&);
	void parseFileStream(const string&);
	int findMatch(const TripleVal&, const MidiEvType&, int) const;
	bool checkRules(TripleVal&, MidiEvType&);
	size_t getSize() const {
		return rules.size();
	}
	const string toString() const;
protected:

	vector<MidiEventDuo>& getRules() {
		return rules;
	}
private:
	int flagPosition = 0;

	vector<MidiEventDuo> rules;
};

#endif //RULEMAPPER_H
