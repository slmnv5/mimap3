#include "pch.h"
#include "RuleMapper.h"

using namespace std;

bool RuleMapper::preProcessString(string& str) {
	str = str.substr(0, str.find(";"));
	bool terminating = (replaceAll(str, ">", "=") > 0);
	replaceAll(str, " ", "");
	replaceAll(str, "\t", "");
	replaceAll(str, "mute", "c,120,0");
	replaceAll(str, "any", "a,,,");
	replaceAll(str, "exit", "x,,,");
	replaceAll(str, "ignore", "x,0,0,0");
	return terminating;
}

void RuleMapper::parseRuleString(string& str) {
	bool terminating = preProcessString(str);

	if (str.size() == 0)
		return;
	vector<string> twoParts;

	splitString(str, "=", twoParts);
	if (twoParts.size() != 2) {
		throw string(__func__) + "  Rule must have 2 parts separated by '=': "
				+ str;
	}

	MidiEventRule oneRule(terminating);
	oneRule.inEvent.parse(twoParts[0], false);
	oneRule.outEvent.parse(twoParts[1], true);

	if (!oneRule.isSafe())
		throw string(__func__)
				+ "  Rule is unsafe! Note on/off event may get to different channels: "
				+ str;

	rules.push_back(move(oneRule));
}

void RuleMapper::parseFileStream(const string& fileName) {
	ifstream f(fileName);
	if (!f.is_open())
		throw string(__func__) + "  Error while opening file: " + fileName;
	string str;
	while (getline(f, str)) {
		try {
			parseRuleString(str);
		} catch (const string& err) {
			cout << string(__func__) << "  Error in rule: " << err << endl;
		} catch (...) {
			cout << string(__func__) << "  Unexpected error!" << endl;
		}
	}
	f.close();
}

int RuleMapper::findMatchingRule(const MidiEvent& ev, int startPos) const {
	for (size_t i = startPos; i < getSize(); i++) {
		const MidiEventRule& oneRule = rules[i];
		if (oneRule.getInEvent().match(ev))
			return i;
	}
	return -1;
}

bool RuleMapper::applyRules(MidiEvent& ev) {
	bool changed = false;

	for (size_t i = flagPosition; i < getSize(); i++) {
		const MidiEventRule& oneRule = rules[i];
		const MidiEvent& inEvent = oneRule.getInEvent();
		const MidiEvent& outEvent = oneRule.getOutEvent();

		if (!inEvent.match(ev))
			continue;

		if (verbose > 1)
			cout << "Found match for: " << ev.toString() << ", in rule: "
					<< oneRule.toString() << endl;

		outEvent.transform(ev);
		changed = true;
		if (ev.evtype == MidiEvType::SETFLAG) {
			int newPos = findMatchingRule(ev);
			flagPosition = newPos < 0 ? flagPosition : newPos;
			if (verbose > 1) {
				string message = newPos < 0 ? "Not found" : "Found";
				cout << message << " flag for: " << ev.toString()
						<< ", in rule: " << oneRule.toString() << endl;
			}
		}
		if (oneRule.getTermiante())
			break;
	}
	return changed;
}

const string RuleMapper::toString() const {
	stringstream ss;
	for (size_t i = 0; i < getSize(); i++) {
		ss << "#" << i << '\t' << (rules[i]).toString() << endl;
	}
	return ss.str();
}
