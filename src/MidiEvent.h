#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include "pch.h"

using namespace std;

//==================== utility functions ===================================
void splitString(string line, const string& Delimiter, vector<string>& tokens);

void replaceAll(string line, const string& del, const string& repl);

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
	void init(const string&, const string&);
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
		ss << static_cast<char>(evtype) << chan.toString() << ","
				<< val1.toString() << "," << val2.toString();
		return ss.str();
	}

	void init(const vector<string>&, bool);
	void transform(TripleVal&, MidiEvType&) const;
	bool match(const TripleVal&, const MidiEvType&) const;


	MidiEvType evtype = MidiEvType::NOTYPE;
	ValueRange chan;
	ValueRange val1;
	ValueRange val2;

private:

	bool isOutEvent;
};
//=============================================================
class MidiEventDuo {
public:
	static char const RULEKEEP = '='; // rule applied and search continues

	MidiEventDuo() :
			inEvent(false), outEvent(true) {
	}
	void transform(TripleVal&, MidiEvType&) const;
	bool match(const TripleVal&, const MidiEvType&) const;

	const string toString() const {
		stringstream ss;
		ss << "Rule: " << inEvent.toString() << outEvent.toString();
		return ss.str();
	}
	void init(vector<string> part1, vector<string> part2) {
		inEvent.init(part1, false);
		outEvent.init(part2, true);
	}

	bool isSafe() const;
protected:
	MidiEvent inEvent;
	MidiEvent outEvent;
};

#endif //MIDIEVENT_H
