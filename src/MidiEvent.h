#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include "pch.h"

using namespace std;

enum class MidiEvType
	: char {NOTYPE = 'a',
	NOTEON = 'n',
	CONTROLCHANGE = 'c',
	PROGCHANGE = 'p',
	SETFLAG = 's'
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
	MidiEvent(bool isOut = false) :
			isOutEvent(isOut) {
	}
	const string toString() const {
		stringstream ss;
		ss << "MidiEvent: " << static_cast<char>(evtype) << chan.toString()
				<< "," << val1.toString() << "," << val2.toString();
		return ss.str();
	}

	void parseEventString(const string& str );
	void transform(TripleVal&, MidiEvType&) const;
	bool match(const TripleVal&, const MidiEvType&) const;

	MidiEvType evtype = MidiEvType::NOTYPE;
	ValueRange chan;
	ValueRange val1;
	ValueRange val2;

private:
	void init(const char& tp, const string&, const string&, const string&);
	bool isOutEvent;
};
//=============================================================
class MidiEventDuo {
public:
	static char const RULESTOP = '>'; // rule terminate search of other rules
	static char const RULEKEEP = '='; // rule applied and search continues

	MidiEventDuo():inEvent( false), outEvent(true), op(RULESTOP) {}
	void transform(TripleVal&, MidiEvType&) const;
	bool match(const TripleVal&, const MidiEvType&) const;

	const string toString() const {
		stringstream ss;
		ss << "Rule: " << inEvent.toString() << op << outEvent.toString();
		return ss.str();
	}
	void init(char opChar, const string& inStr, const string& outStr) {
		op = opChar;
		inEvent.parseEventString(inStr);
		outEvent.parseEventString(outStr);
	}
	const char& getOperation() const {
		return op;
	}
	bool isSafe() const;
protected:
	MidiEvent inEvent;
	MidiEvent outEvent;
	char op;
};

#endif //MIDIEVENT_H
