#include "pch.h"
#include "MidiEvent.h"

using namespace std;

void ValueRange::parseRangeIn(const string& str) {
	static string regexRange = "^(\\d{0,3})?(?:(\\:)(\\d{1,3}))?";
	static regex expr(regexRange);
	smatch sm;
	if (!regex_match(str, sm, expr))
		throw string(string(__func__)) + "  Input string has incorrect format: "
				+ str;

	if (sm.size() != 4)
		throw string(__func__) + "  Input string has incorrect elements: "
				+ str;

	if (sm[1].str().size() > 0)
		lower = stoi(sm[1]);
	if (sm[3].str().size() > 0)
		upper = stoi(sm[3]);

	if (upper >= 0 && lower >= upper)
		throw string(__func__) + "  Lower value must be less than upper value: "
				+ str;

}
void ValueRange::parseRangeOut(const string& str) {
	static string regexRange = "^(\\d{0,3})?(?:(\\+|-)(\\d{1,3}))?";
	static regex expr(regexRange);
	smatch sm;
	if (!regex_match(str, sm, expr))
		throw string(string(__func__)) + "  Range has incorrect format: " + str;
	if (sm.size() != 4)
		throw string(__func__) + "  Range has incorrect elements: " + str;

	if (sm[1].str().size() > 0)
		lower = stoi(sm[1]);
	if (sm[3].str().size() > 0) {
		upper = stoi(sm[3]);
		if (lower < 1 || lower > 3)
			throw string(__func__) + "  Output range has incorrect format: "
					+ str;
		if (sm[2] == "+")
			lower = lower + 10;
	}

}
void ValueRange::parseRange(const string& str, bool isOutEvent) {
	if (isOutEvent)
		parseRangeOut(str);
	else
		parseRangeIn(str);

	description = str;
}

bool ValueRange::match(int val) const {
	int cnt = 2;
	if (lower < 0)
		cnt = 0;
	else if (upper < 0)
		cnt = 1;

	if (cnt == 0)
		return true;
	else if (cnt == 1)
		return val >= lower;
	else
		return val <= upper && val >= lower;
}
int ValueRange::transform(TripleVal val, int dflt) const {
	int cnt = 2;
	if (lower < 0)
		cnt = 0;
	else if (upper < 0)
		cnt = 1;

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

void MidiEvent::transform(TripleVal& in, char& tp) const {
	if (!isOutEvent)
		throw string(__func__) + "  Input event can not transform MidiMessage";
	TripleVal newVal;
	newVal.ch = chan.transform(in, 1);
	newVal.v1 = val1.transform(in, 2);
	newVal.v2 = val2.transform(in, 3);
	in = newVal;
	tp = evtype;
}

bool MidiEvent::match(const TripleVal& in, const char& tp) const {
	if (isOutEvent)
		throw string(__func__) + "  Out event can not match MidiMessage";
	return evtype == tp && chan.match(in.ch) && val1.match(in.v1)
			&& val2.match(in.v2);
}

void MidiEvent::init(const char& tp, const string& chn, const string& vl1,
		const string& vl2, bool isOut) {
	isOutEvent = isOut;
	evtype = tp;
	chan.parseRange(chn, isOutEvent);
	val1.parseRange(vl1, isOutEvent);
	val2.parseRange(vl2, isOutEvent);
}

void MidiEvent::parseEventString(const string& str, bool isOut) {

	static const char* regexEvent = "(n|c|p)([\\d:+-]*),([\\d:+-]*),([\\d:+-]*)";
	regex expr(regexEvent);
	smatch sm;
	if (!regex_match(str, sm, expr)) {
		throw string(__func__) + "  Event string has incorrect format: " + str;
	}
#ifdef DEBUG
	cout << string(__func__) << "  " << str << " tokens= " << sm.size() << endl;
#endif
	if (sm.size() != 5) {
		throw string(__func__) + "  Event string has incorrect elements: "
				+ str;
	}

	init(sm[1].str().at(0), sm[2], sm[3], sm[4], isOut);
}

//===================================================
bool MidiEventWrap::match(const TripleVal& in, const char& tp) const {
	return inEvent.match(in, tp);
}

void MidiEventWrap::transform(TripleVal& in, char& tp) const {
	outEvent.transform(in, tp);

}

