#include "pch.h"
#include "MidiEvent.h"

using namespace std;

//==================== utility functions ===================================
void splitString(const string line1, const string& delimiter,
		vector<string>& tokens) {
	string line = line1;
	size_t pos = 0;
	string token;
	while ((pos = line.find(delimiter)) != std::string::npos) {
		token = line.substr(0, pos);
		tokens.push_back(token);
		line.erase(0, pos + delimiter.length());
	}
	tokens.push_back(line);
}

int replaceAll(string& line, const string& del, const string& repl) {
	int count = 0;
	string::size_type n = 0;
	while ((n = line.find(del, n)) != string::npos) {
		line.replace(n, del.size(), repl);
		n += repl.size();
		count++;
	}
	return count;
}

int ValueRange::convertToInt(const string& str) {
	int sz = str.size();
	if (sz == 0)
		return -1;
	bool onlyDigits = str.find_first_not_of("0123456789") == string::npos;
	if (sz > 3 || !onlyDigits)
		throw string(__func__) + "  Not correct integer value: [" + str + "]";

	return stoi(str);
}

void ValueRange::init(const string& str, const string& delim) {
	description = str;
	lower = TripleVal::NONE;
	upper = TripleVal::ZERO;

	vector<string> twoParts;
	splitString(str, delim, twoParts);

	if (twoParts.size() > 2)
		throw string(string(__func__)) + "  Input string has incorrect format: "
				+ str;
	lower = convertToInt(twoParts[0]);
	if (lower < TripleVal::NONE) {
		throw string(__func__) + "  Lower value must be non negative: [" + str
				+ "]";
	}

	if (twoParts.size() == 2) {
		upper = convertToInt(twoParts[1]);

	}
}

int ValueRange::countborders() const {
	int cnt = 2;
	if (lower <= TripleVal::NONE)
		cnt = 0;
	else if (lower != TripleVal::NONE and upper == TripleVal::ZERO)
		cnt = 1;
	return cnt;
}
bool ValueRange::match(int val) const {
	if (countborders() == 0)
		return true;
	else if (countborders() == 1)
		return val == lower;
	else
		return val <= upper && val >= lower;
}
int ValueRange::transform(const TripleVal& baseVal, int someValue) const {
	if (countborders() == 0)
		return someValue;
	else if (countborders() == 1)
		return lower;
	else {
		if (lower == 1)
			return baseVal.ch + upper;
		else if (lower == 2)
			return baseVal.v1 + upper;
		else if (lower == 3)
			return baseVal.v2 + upper;
		else
			throw string(__func__) + "  Incorrect Value for out Midi range: "
					+ this->toString();
	}
}

//========================================================

void MidiEvent::transform(TripleVal& someVal,
		MidiEvType& someType) const {
	if (!isOut())
		throw string(__func__) + "  Input event can not transform MidiMessage";

	const TripleVal saveVal = someVal;
	someVal.ch = chan.transform(saveVal, someVal.ch);
	someVal.v1 = val1.transform(saveVal, someVal.v1);
	someVal.v2 = val2.transform(saveVal, someVal.v2);
	someType = evtype;
}

bool MidiEvent::match(const TripleVal& someVal,
		const MidiEvType& someType) const {
	if (isOutEvent)
		throw string(__func__) + "  Out event can not match MidiMessage";
	return (evtype == someType || evtype == MidiEvType::ANYTHING)
			&& chan.match(someVal.ch) && val1.match(someVal.v1)
			&& val2.match(someVal.v2);
}

void MidiEvent::init(const vector<string>& vect, bool isOut) {
	this->isOutEvent = isOut;
	if (vect.size() != 4)
		throw string(__func__) + "  Event vector must have 4 parts";

	char ev = vect[0].at(0);
	evtype = static_cast<MidiEvType>(ev);
	bool ok = (evtype == MidiEvType::ANYTHING
			|| evtype == MidiEvType::CONTROLCHANGE || evtype == MidiEvType::NOTE
			|| evtype == MidiEvType::PROGCHANGE || evtype == MidiEvType::SETFLAG
			|| evtype == MidiEvType::NONE);
	if (!ok)
		throw string(__func__) + "  Event type is incorrect: [" + vect[0] + "]";

	string delim = isOut ? "+" : ":";
	chan.init(vect[1], delim);
	val1.init(vect[2], delim);
	val2.init(vect[3], delim);
}

//===================================================

bool MidiEventRule::isSafe() const {
	// out channel changes - we may miss note off event
	if (outEvent.evtype != MidiEvType::NOTE
			&& outEvent.evtype != MidiEvType::ANYTHING)
		return true;

	if (outEvent.chan.countborders() == 0)
		return true;

	if (outEvent.chan.countborders() == 1 && inEvent.val1.countborders() == 0
			&& inEvent.val2.countborders() == 0)
		return true;

	return false;

}

