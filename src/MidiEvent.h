#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include "pch.h"

using namespace std;

typedef unsigned char UCHAR;
//==================== utility functions ===================================
void splitString(string line, const string& Delimiter, vector<string>& tokens);

int replaceAll(string& line, const string& del, const string& repl);

enum class MidiEvType
	: UCHAR {NONE = 'x',
	ANYTHING = 'a',
	NOTE = 'n',
	CONTROLCHANGE = 'c',
	PROGCHANGE = 'p',
	SETFLAG = 's'
};

class TripleVal;

//=============================================================
class ValueRange {
public:
// ==== staic =====
	static UCHAR inline getNone() {
		return 128;
	}
	static bool inline isNone(const int& val) {
		return val == getNone();
	}
// ==== ctor =====
	ValueRange() {
		init();
	}
	void init(UCHAR a, UCHAR b) {
		lower = a;
		upper = b;
	}
	void init(UCHAR a) {
		lower = a;
		upper = getNone();
	}
	void init() {
		lower = getNone();
		upper = getNone();
	}
	const UCHAR& get() const {
		return lower;
	}
	UCHAR& get() {
		return lower;
	}
	void init(const string&, const string&);
	int countborders() const;
	bool match(int) const;
	void transform(const TripleVal&, ValueRange&) const;

	const string toString() const {
		if (isNone(lower))
			return "";
		stringstream ss;
		ss << static_cast<int>(lower);
		if (!isNone(upper))
			ss << delimiter << static_cast<int>(upper);
		return ss.str();
	}
private:
	UCHAR lower;
	UCHAR upper;

	int convertToInt(const string&);
	char delimiter = ':';
};

class TripleVal {
public:
	void init(const UCHAR& a, const UCHAR& b, const UCHAR& c) {
		ch.init(a);
		v1.init(b);
		v2.init(c);
	}
	void init(const UCHAR& a, const UCHAR& b) {
		ch.init(a);
		v1.init(b);
		v2.init();
	}

	const string toString() const {
		stringstream ss;
		ss << ch.toString() << "," << v1.toString() << "," << v2.toString()
				<< '\t';
		return ss.str();
	}

	const UCHAR& get(int index) const {
		if (index == 0)
			return ch.get();
		else if (index == 1)
			return v1.get();
		else if (index == 2)
			return v2.get();
		else
			throw string(__func__) + "  Out event can not match MidiMessage";

	}
	ValueRange ch; // midi channel
	ValueRange v1; // midi note or cc
	ValueRange v2; // midi velocity or cc value

};

//=============================================================
class MidiEvent: public TripleVal {
public:
	MidiEvent(bool isOut = false) :
			TripleVal(), isOutEvent(isOut) {
	}
	const string toString() const {
		stringstream ss;
		ss << static_cast<char>(evtype) << "," << TripleVal::toString();
		return ss.str();
	}

	void parse(const string&, bool);
	void transform(MidiEvent&) const;
	bool match(const MidiEvent&) const;

	MidiEvType evtype = MidiEvType::NONE;

	bool isOut() const {
		return isOutEvent;
	}

private:
	bool isOutEvent;
};
//=============================================================
class MidiEventRule {
public:
	MidiEventRule(bool term) :
			terminate(term), inEvent(false), outEvent(true) {
	}
	const string toString() const {
		stringstream ss;
		string separator = terminate ? ">" : "=";
		ss << "Rule: " << inEvent.toString() << separator
				<< outEvent.toString();
		return ss.str();
	}
	bool isSafe() const;
	bool getTermiante() const {
		return terminate;
	}
	const MidiEvent& getOutEvent() const {
		return outEvent;
	}
	const MidiEvent& getInEvent() const {
		return inEvent;
	}

	bool terminate = false;
	MidiEvent inEvent;
	MidiEvent outEvent;
};

#endif //MIDIEVENT_H
