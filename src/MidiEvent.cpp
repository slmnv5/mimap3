#include "pch.h"
#include "MidiEvent.h"

using namespace std;

//==================== utility functions ===================================
void splitString(string line, const string& delimiter, vector<string>& tokens) {
	size_t pos = 0;
	string token;
	while ((pos = line.find(delimiter)) != std::string::npos) {
		token = line.substr(0, pos);
		tokens.push_back(token);
		line.erase(0, pos + delimiter.length());
	}
}

void replaceAll(string line, const string& del, const string& repl) {
	string::size_type n = 0;
	while ((n = line.find(del, n)) != string::npos) {
		line.replace(n, del.size(), repl);
		n += repl.size();
	}
}

void ValueRange::init(const string& str, const string& delim) {
	description = str;
	vector<string> twoParts;
	splitString(str, delim, twoParts);

	if (twoParts.size() > 2)
		throw string(string(__func__)) + "  Input string has incorrect format: "
				+ str;
	try {
		if (twoParts.size() == 2) {
			lower = stoi(twoParts[0]);
			upper = stoi(twoParts[1]);
		} else if (twoParts.size() == 1) {
			lower = stoi(twoParts[0]);
		}
	} catch (...) {
		throw string(__func__) + "  Could not parse integer values: " + str;
	}
	if (upper >= 0 && lower >= upper)
		throw string(__func__) + "  Lower value must be less than upper value: "
				+ str;

}

int ValueRange::countborders() const {
	int cnt = 2;
	if (lower < 0)
		cnt = 0;
	else if (upper < 0)
		cnt = 1;
	return cnt;
}
bool ValueRange::match(int val) const {
	int cnt = countborders();

	if (cnt == 0)
		return true;
	else if (cnt == 1)
		return val >= lower;
	else
		return val <= upper && val >= lower;
}
int ValueRange::transform(TripleVal val, int dflt) const {
	int cnt = countborders();

	if (cnt == 0)
		return dflt;
	else if (cnt == 1)
		return lower;
	else {
		if (lower == 1)
			return val.ch - upper;
		else if (lower == 2)
			return val.v1 - upper;
		else if (lower == 3)
			return val.v2 - upper;
		else if (lower == 11)
			return val.ch + upper;
		else if (lower == 12)
			return val.v1 + upper;
		else if (lower == 13)
			return val.v2 + upper;
		else
			throw string(__func__) + "  Incorrect Value for out Midi range: "
					+ this->toString();
	}
}

//========================================================

void MidiEvent::transform(TripleVal& in, MidiEvType& tp) const {
	if (!isOutEvent)
		throw string(__func__) + "  Input event can not transform MidiMessage";
	TripleVal newVal;
	newVal.ch = chan.transform(in, 1);
	newVal.v1 = val1.transform(in, 2);
	newVal.v2 = val2.transform(in, 3);
	in = newVal;
	tp = evtype;
}

bool MidiEvent::match(const TripleVal& in, const MidiEvType& tp) const {
	if (isOutEvent)
		throw string(__func__) + "  Out event can not match MidiMessage";
	return (evtype == tp || evtype == MidiEvType::NOTYPE) && chan.match(in.ch)
			&& val1.match(in.v1) && val2.match(in.v2);
}

void MidiEvent::init(const vector<string>& vect, bool isOut) {
	this->isOutEvent = isOut;
	if (vect.size() != 4)
		throw string(__func__) + "  Event vector must have 4 parts";

	evtype = static_cast<MidiEvType>(vect[0].at(0));
	string delim = isOut ? ":" : "+";
	chan.init(vect[1], delim);
	val1.init(vect[2], delim);
	val2.init(vect[3], delim);
}

//===================================================
bool MidiEventDuo::match(const TripleVal& in, const MidiEvType& tp) const {
	return inEvent.match(in, tp);
}

void MidiEventDuo::transform(TripleVal& in, MidiEvType& tp) const {
	outEvent.transform(in, tp);
}

bool MidiEventDuo::isSafe() const {
	// out channel changes - we may miss note off event
	if (outEvent.evtype != MidiEvType::NOTEON)
		return true;

	if (outEvent.chan.countborders() == 0)
		return true;

	if (outEvent.chan.countborders() == 1 && inEvent.val1.countborders() == 0
			&& inEvent.val2.countborders() == 0)
		return true;

	return false;

}

