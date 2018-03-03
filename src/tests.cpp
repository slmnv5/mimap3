#include "pch.h"

#include "AlsaMapper.h"
#include "MidiEvent.h"
#include "RuleMapper.h"
#include "MidiFilter.h"

using namespace std;


void test1() {
	int verbose = 2;
	RuleMapper rmp(verbose);
	try {
		string ss1("n,,,,>c,1+2,,,");
		rmp.parseRuleString(ss1);
		TripleVal tpv;
		MidiEvent ev = rmp.findMatchingRule(tpv, MidiEvType::NONE);
	} catch (...) {
		cout << __func__ << " error!" << endl;
	}
	cout << "done" << endl;

}

