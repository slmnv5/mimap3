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
	if (delim != "+" || delim != "-" || delim != ":")
		throw string(__func__) + "  Not correct delimiter: [" + delim + "]";

	delimiter = delim.at(0);
	vector<string> twoParts;
	splitString(str, delim, twoParts);

	if (twoParts.size() > 2)
		throw string(string(__func__)) + "  Input string has incorrect format: "
				+ str;
	lower = convertToInt(twoParts[0]);
	if (lower < ZERO) {
		throw string(__func__) + "  Lower value must be non negative: [" + str
				+ "]";
	}

	if (twoParts.size() == 2) {
		upper = convertToInt(twoParts[1]);

	}
}

int ValueRange::countborders() const {
	if (isNone())
		return 0;
	else if (upper == ZERO)
		return 1;
	return 2;
}
bool ValueRange::match(int val) const {
	if (delimiter != ':')
		throw string(__func__) + "  match cannot be done for this range: "
				+ this->toString();
	if (countborders() == 0)
		return true;
	else if (countborders() == 1)
		return val == lower;
	else
		return val <= upper && val >= lower;
}
void ValueRange::transform(const TripleVal& baseVal, UCHAR& someValue) const {
	if (countborders() == 0)
		;
	else if (countborders() == 1)
		someValue = lower;
	else {
		if (lower == 1)
			someValue = baseVal.ch.get() + upper;
		else if (lower == 2)
			someValue = baseVal.v1.get() + upper;
		else if (lower == 3)
			someValue = baseVal.v2.get() + upper;
		else if (lower == 11)
			someValue = baseVal.v1.get() - upper;
		else if (lower == 12)
			someValue = baseVal.v2.get() - upper;
		else if (lower == 13)
			someValue = baseVal.v2.get() - upper;
		else
			throw string(__func__) + "  Incorrect Value for out Midi range: "
					+ this->toString();
	}
}

//========================================================

void MidiEvent::transform(TripleVal& someVal, MidiEvType& someType) const {
	if (!isOut())
		throw string(__func__) + "  Input event can not transform MidiMessage";

	const TripleVal saveVal = someVal;
	this->ch.transform(saveVal, someVal.ch.get());
	this->v1.transform(saveVal, someVal.v1.get());
	this->v2.transform(saveVal, someVal.v2.get());
	someType = evtype;
}

bool MidiEvent::match(const MidiEvent& ev) const {
	if (isOutEvent)
		throw string(__func__) + "  Out event can not match MidiMessage";
	return (evtype == ev.evtype || evtype == MidiEvType::ANYTHING)
			&& ch.match(ev.ch.get()) && v1.match(ev.v1.get())
			&& v2.match(ev.v2.get());
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
	ch.init(vect[1], delim);
	v1.init(vect[2], delim);
	v2.init(vect[3], delim);
}

//===================================================

bool MidiEventRule::isSafe() const {
// out channel changes - we may miss note off event
	if (outEvent.evtype != MidiEvType::NOTE
			&& outEvent.evtype != MidiEvType::ANYTHING)
		return true;

	if (outEvent.ch.countborders() == 0)
		return true;

	if (outEvent.ch.countborders() == 1 && inEvent.v1.countborders() == 0
			&& inEvent.v2.countborders() == 0)
		return true;

	return false;

}

