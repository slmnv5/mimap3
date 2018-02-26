#ifndef RULEMAPPER_H
#define RULEMAPPER_H

#include "pch.h"
#include "MidiEvent.h"

using namespace std;

class RuleMapper {
public:
	RuleMapper(int vbLevel) :
			verbose(vbLevel) {
	}
	void parseFileStream(const string&);
	int findMatchingRule(const TripleVal&, const MidiEvType&, int) const;
	bool checkRules(TripleVal&, MidiEvType&);
	size_t getSize() const {
		return rules.size();
	}
	const string toString() const;
	int getVerbose() {
		return verbose;
	}
protected:
	int verbose;
	vector<MidiEventDuo>& getRules() {
		return rules;
	}
private:
	int flagPosition = 0;
	vector<MidiEventDuo> rules;

	void preProcessString(string&) const;
	void parseRuleString(string&);
};

#endif //RULEMAPPER_H
