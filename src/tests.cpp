#include "pch.h"

#include "tests.h"
#include "RuleMapper.h"

using namespace std;

void test1() {
	int verbose = 2;
	RuleMapper rmp(verbose);
	try {
		string ss1("n,,,>c,1+2,,");
		rmp.parseRuleString(ss1);
		MidiEvent ev1;
		ev1.parse("n,2,22,10", true);
		MidiEvent ev = rmp.findMatchingRule(ev1, 0);
		cout << ev.toString() << endl;

	} catch (string err) {
		cout << err << endl;
	} catch (...) {
		cout << __func__ << " error!" << endl;
	}
	cout << "done" << endl;

}

