#ifndef RULEMAPPER_H
#define RULEMAPPER_H

#include "pch.h"
#include "MidiEvent.h"

using namespace std;

class RuleMapper {
public:
	RuleMapper() {
	}

	void parseRuleString(const string& str);
	void parseFileStream(const string& fileName);
	bool checkRules(TripleVal&, char&) const;
	size_t getSize() const {
		return rules.size();
	}
	const string toString() const;
protected:
	vector<MidiEventWrap>& getRules() {
		return rules;
	}

private:
	vector<MidiEventWrap> rules;
};

#endif //RULEMAPPER_H
