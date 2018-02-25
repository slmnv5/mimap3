#include "pch.h"
#include "RuleMapper.h"

using namespace std;

void RuleMapper::preProcessString(string& str) const {
	str = str.substr(0, str.find(";"));
	replaceAll(str, " ", "");
	replaceAll(str, "\t", "");
	replaceAll(str, "mute", "c,120,0");
	replaceAll(str, "any", "a,,,");
	replaceAll(str, "exit", "x,,,");
	replaceAll(str, "ignore", "x,0,0,0");
}

void RuleMapper::parseRuleString(string& str) {
	preProcessString(str);

	if (str.size() == 0)
		return;
	vector<string> twoParts;
	splitString(str, "=", twoParts);
	if (twoParts.size() != 2) {
		throw string(__func__) + "  Rule must have 2 parts separated by '=': "
				+ str;
	}

	vector<string> part1, part2;
	splitString(twoParts[0], ",", part1);
	splitString(twoParts[1], ",", part2);

	if (verbose > 0)
		cout << string(__func__) << "  " << str << " tokens= " << part1.size()
				<< "/" << part2.size() << endl;


	if (part1.size() != 4 || part2.size() != 4)
		throw string(__func__) + "  Rule parts must have 4 elements: " + str;

	MidiEventDuo ev;
	ev.init(part1, part2);
	if (!ev.isSafe())
		throw string(__func__)
				+ "  Rule is unsafe! Note on/off event may get to different channels: " + str;

	rules.push_back(move(ev));
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

int RuleMapper::findMatch(const TripleVal& val, const MidiEvType& tp,
		int startFrom) const {
	for (size_t i = startFrom; i < getSize(); i++) {
		const MidiEventDuo& ev = rules[i];
		if (ev.match(val, tp))
			return i;
	}
	return -1;
}

bool RuleMapper::checkRules(TripleVal& val, MidiEvType& tp) {
	bool changed = false;
	int newFlagPosition = -1;
	for (size_t i = flagPosition; i < getSize(); i++) {
		const MidiEventDuo& ev = rules[i];
		if (!ev.match(val, tp))
			continue;

#ifdef DEBUG
		cout << "Found match for: " << static_cast<char>(tp) << val.toString()
				<< ", rule: " << ev.toString() << endl;
#endif

		ev.transform(val, tp);
		if (tp == MidiEvType::SETFLAG) {
			newFlagPosition = findMatch(val, tp, 0);
			if (newFlagPosition < 0) {
				cout << "No match for: " << static_cast<char>(tp)
						<< val.toString() << ", rule: " << ev.toString()
						<< "\nReset starting flag to zero" << endl;
			} else {
				flagPosition = newFlagPosition;
				cout << "Reset starting flag to " << flagPosition << endl;
			}
			val.v1 = 120;
			val.v2 = 0;
			tp = MidiEvType::CONTROLCHANGE; // all notes off
			return true;
		}

		changed = true;

	}
	return changed;
}

const string RuleMapper::toString() const {
	stringstream ss;
	for (size_t i = 0; i < getSize(); i++) {
		ss << endl << "#" << i << '\t' << (rules[i]).toString() << endl;
	}
	return ss.str();
}
