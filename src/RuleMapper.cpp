#include "pch.h"
#include "RuleMapper.h"

using namespace std;

void RuleMapper::parseRuleString(const string& str) {
	string str1 = str.substr(0, str.find(";"));
	str1 = regex_replace(str1, regex("\\s+"), string(""));

	if (str1.size() == 0)
		return;
	static const char* regexRule = "([ncpsa][\\d,:]*)(=|>)([ncpsa][\\d,+-]*)";
	static regex expr(regexRule);
	smatch sm;    // same as std::match_results<string::const_iterator> sm;
	if (!regex_match(str1, sm, expr))
		throw string(__func__) + "  Rule has incorrect format: " + str1;

#ifdef DEBUG
	cout << string(__func__) << "  " << str1 << " tokens= " << sm.size()
	<< endl;
#endif
	if (sm.size() != 4)
		throw string(__func__) + "  Rule has incorrect elements: " + str1;

	MidiEventDuo ev;
	ev.init(sm[2].str().at(0), sm[1], sm[3]);
	if (!ev.isSafe())
		throw string(__func__)
				+ "  Rule is unsafe! Note off event may go to other channel: "
				+ str1;

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
			cerr << string(__func__) << "  Error in rule: " << err << endl;
		} catch (...) {
			cerr << string(__func__) << "  Unexpected error!" << endl;
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
		cout << "Found match for: " << static_cast<char>(tp) << val.toString() << ", rule: "
		<< ev.toString() << endl;
#endif

		ev.transform(val, tp);
		if (tp == MidiEvType::SETFLAG) {
			newFlagPosition = findMatch(val, tp, 0);
			if (newFlagPosition < 0) {
				cerr << "No match for: " << static_cast<char>(tp)
						<< val.toString() << ", rule: " << ev.toString()
						<< endl;
			} else {
				flagPosition = newFlagPosition;
			}
			val.v1 = 120;
			val.v2 = 0;
			tp = MidiEvType::CONTROLCHANGE; // all notes off
			return true;
		}

		changed = true;
		if (ev.getOperation() == MidiEventDuo::RULESTOP)
			break;
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
