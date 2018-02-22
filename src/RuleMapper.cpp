#include "pch.h"
#include "RuleMapper.h"

using namespace std;

void RuleMapper::preProcessString(string& str) const {
	str = str.substr(0, str.find(";"));
	str = regex_replace(str, regex("\\s+"), string(""));
	str = regex_replace(str, regex("exit.*", regex_constants::icase),
			string("a,,>c,120,0"));
	str = regex_replace(str, regex("enter(=|>)*(.*)", regex_constants::icase),
			string("$2=c,120,0"));
}

void RuleMapper::parseRuleString(string& str) {
	preProcessString(str);

	if (str.size() == 0)
		return;
	static const char* regexRule = "([ncpsa][\\d,:]*)(=|>)([ncpsa][\\d,+-]*)";
	static regex expr(regexRule);
	smatch sm;    // same as std::match_results<string::const_iterator> sm;
	if (!regex_match(str, sm, expr))
		throw string(__func__) + "  Rule has incorrect format: " + str;

#ifdef DEBUG
	cout << string(__func__) << "  " << str << " tokens= " << sm.size()
	<< endl;
#endif
	if (sm.size() != 4)
		throw string(__func__) + "  Rule has incorrect elements: " + str;

	MidiEventDuo ev;
	ev.init(sm[2].str().at(0), sm[1], sm[3]);
	if (!ev.isSafe())
		throw string(__func__)
				+ "  Rule is unsafe! Note off event may get lost: " + str;

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
