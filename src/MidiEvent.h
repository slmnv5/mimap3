#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include "pch.h"

using namespace std;

static char const NOTYPE = ' ';
static int const MISSING = -1;
static int const NOTE = int('n');
static int const CONTROLCHANGE = int('c');
static int const PROGCHANGE = int('p');

class TripleVal {
public:
	int ch = MISSING; // midi channel
	int v1 = MISSING; // midi note or cc
	int v2 = MISSING; // midi velocity or cc value
	const string toString() const {
		stringstream ss;
		ss << ch << "," << v1 << "," << v2 << endl;
		return ss.str();
	}
};

class ValueRange {
public:

	int lower = MISSING;
	int upper = MISSING;
	void parseRange(const string&, bool);
	void parseRangeOut(const string& str);
	void parseRangeIn(const string& str);

	bool match(int) const;
	int transform(TripleVal, int) const;

	const string& toString() const {
		return description;
	}

private:
	string description;
};
class MidiEvent {
public:

	const string toString() const {
		string str = "MidiEvent: ";
		str.append(&evtype, 1);
		str += chan.toString() + "," + val1.toString() + "," + val2.toString();
		return str;
	}

	void parseEventString(const string& str, bool isOut);
	void transform(TripleVal&, char&) const;
	bool match(const TripleVal&, const char&) const;

private:
	char evtype = NOTYPE;
	ValueRange chan;
	ValueRange val1;
	ValueRange val2;

	void init(const char& tp, const string&, const string&, const string&,
			bool);
	bool isOutEvent;
};

class MidiEventWrap {
public:

	void transform(TripleVal&, char&) const;
	bool match(const TripleVal&, const char&) const;

	const string toString() const {
		string str = "Rule: " + inEvent.toString();
		str.append(&op, 1);
		str += outEvent.toString();
		return str;
	}
	void init(char opChar, const string& inStr, const string& outStr) {
		op = opChar;
		inEvent.parseEventString(inStr, false);
		outEvent.parseEventString(outStr, true);
	}
	const char& getOperation() const {
		return op;
	}
protected:
	char op = NOTYPE;
	MidiEvent inEvent;
	MidiEvent outEvent;

};

#endif //MIDIEVENT_H
