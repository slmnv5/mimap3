#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include "pch.h"

using namespace std;

//==================== utility functions ===================================
void splitString(string line, const string& Delimiter, vector<string>& tokens);

int replaceAll(string& line, const string& del, const string& repl);

enum class MidiEvType
	: char {NONE = 'x',
	ANYTHING = 'a',
	NOTE = 'n',
	CONTROLCHANGE = 'c',
	PROGCHANGE = 'p',
	SETFLAG = 's'
};

//=============================================================

class TripleVal {
public:
	static char const NONE = -1;
	static char const ZERO = 0;

	char ch = NONE; // midi channel
	char v1 = NONE; // midi note or cc
	char v2 = ZERO; // midi velocity or cc value
	const string toString() const {
		stringstream ss;
		ss << ch << "," << v1 << "," << v2 << endl;
		return ss.str();
	}
};
//=============================================================

class ValueRange {
public:
	int lower = TripleVal::NONE;
	int upper = TripleVal::ZERO;
	void init(const string&, const string&);
	int countborders() const;
	bool match(int) const;
	int transform(const TripleVal&, int) const;

	const string& toString() const {
		return description;
	}

private:
	int convertToInt(const string&);
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
		ss << static_cast<char>(evtype) << "," << chan.toString() << ","
				<< val1.toString() << "," << val2.toString();
		return ss.str();
	}

	void init(const vector<string>&, bool);
	void transform(TripleVal&, MidiEvType&) const;
	bool match(const TripleVal&, const MidiEvType&) const;

	MidiEvType evtype = MidiEvType::NONE;
	ValueRange chan;
	ValueRange val1;
	ValueRange val2;
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
	void init(vector<string> part1, vector<string> part2) {
		inEvent.init(part1, false);
		outEvent.init(part2, true);
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

protected:
	bool terminate = false;
	MidiEvent inEvent;
	MidiEvent outEvent;
};

#endif //MIDIEVENT_H
