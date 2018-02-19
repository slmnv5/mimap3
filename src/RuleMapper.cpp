#include "pch.h"
#include "RuleMapper.h"

using namespace std;

void RuleMapper::parseRuleString(const string& str) {
	string str1 = str.substr(0, str.find(";"));
	str1 = regex_replace(str1, regex("\\s+"), string(""));

	if (str1.size() == 0)
		return;
	static const char* regexRule = "([ncp][\\d,:]*)(=|>)([ncp][\\d,+-]*)";
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

	MidiEventWrap ev;
	ev.init(sm[2].str().at(0), sm[1], sm[3]);
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

bool RuleMapper::checkRules(TripleVal& val, char& tp) const {
	bool changed = false;
	for (size_t i = 0; i < getSize(); i++) {
		const MidiEventWrap& ev = rules[i];
		if (!ev.match(val, tp))
			continue;

#ifdef DEBUG
		cout << "found match for: " << tp << val.toString() << ", rule: "
				<< ev.toString() << endl;
#endif
		ev.transform(val, tp);
		changed = true;
		if (ev.getOperation() == '>')
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
