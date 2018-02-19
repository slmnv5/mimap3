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
	bool checkRules(TripleVal&, MidiEvType&) const;
	size_t getSize() const {
		return rules.size();
	}
	const string toString() const;
protected:
	vector<MidiEventDuo>& getRules() {
		return rules;
	}

private:
	vector<MidiEventDuo> rules;
};

#endif //RULEMAPPER_H
