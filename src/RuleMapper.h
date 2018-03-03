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
	int findMatchingRule(const MidiEvent&, int startPos = 0) const;
	bool applyRules(MidiEvent&);
	MidiEventRule& getRule(int i) {
		return rules[i];
	}
	size_t getSize() const {
		return rules.size();
	}
	const string toString() const;
	int getVerbose() const {
		return verbose;
	}
protected:
	int verbose;
	vector<MidiEventRule>& getRules() {
		return rules;
	}
private:
	int flagPosition = 0;
	vector<MidiEventRule> rules;

	static bool preProcessString(string&);
	void parseRuleString(string&);
};

#endif //RULEMAPPER_H
