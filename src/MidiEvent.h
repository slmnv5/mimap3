#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include "pch.h"

using namespace std;

enum class MidiEvType
	: char {NOTYPE = ' ', NOTEON = 'n', CONTROLCHANGE = 'c', PROGCHANGE = 'p'
};

//=============================================================

class TripleVal {
public:
	static int const MISSINGVAL = -1;
	int ch = TripleVal::MISSINGVAL; // midi channel
	int v1 = TripleVal::MISSINGVAL; // midi note or cc
	int v2 = TripleVal::MISSINGVAL; // midi velocity or cc value
	const string toString() const {
		stringstream ss;
		ss << ch << "," << v1 << "," << v2 << endl;
		return ss.str();
	}
};
//=============================================================

class ValueRange {
public:
	int lower = TripleVal::MISSINGVAL;
	int upper = TripleVal::MISSINGVAL;
	void parseRange(const string&, bool);
	void parseRangeOut(const string& str);
	void parseRangeIn(const string& str);
	int countborders() const;
	bool match(int) const;
	int transform(TripleVal, int) const;

	const string& toString() const {
		return description;
	}

private:
	string description;
};

//=============================================================
class MidiEvent {
public:

	const string toString() const {
		stringstream ss;
		ss << "MidiEvent: " << static_cast<char>(evtype) << chan.toString()
				<< "," << val1.toString() << "," << val2.toString();
		return ss.str();
	}

	void parseEventString(const string& str, bool isOut);
	void transform(TripleVal&, MidiEvType&) const;
	bool match(const TripleVal&, const MidiEvType&) const;

	MidiEvType evtype = MidiEvType::NOTYPE;
	ValueRange chan;
	ValueRange val1;
	ValueRange val2;

private:
	void init(const char& tp, const string&, const string&, const string&,
			bool);
	bool isOutEvent;
};
//=============================================================
class MidiEventDuo {
public:
	static char const RULETYPE1 = '>'; // rule terminate search of other rules
	static char const RULETYPE2 = '='; // rule applied and search continues
	void transform(TripleVal&, MidiEvType&) const;
	bool match(const TripleVal&, const MidiEvType&) const;

	const string toString() const {
		stringstream ss;
		ss << "Rule: " << inEvent.toString() << op << outEvent.toString();
		return ss.str();
	}
	void init(char opChar, const string& inStr, const string& outStr) {
		op = opChar;
		inEvent.parseEventString(inStr, false);
		outEvent.parseEventString(outStr, true);
	}
	const char& getOperation() const {
		return op;
	}
	bool isSafe() const;
protected:
	char op = RULETYPE1;
	MidiEvent inEvent;
	MidiEvent outEvent;

};

#endif //MIDIEVENT_H
