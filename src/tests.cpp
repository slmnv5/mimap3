#include "pch.h"

#include "tests.h"
#include "RuleMapper.h"

using namespace std;

void test1() {
	int verbose = 2;
	RuleMapper rmp(verbose);
	try {
		string ss1("n,5,,>c,1+2,,");
		rmp.parseRuleString(ss1);
		string ss2("n,,10:50,>c,1+3,,");
		rmp.parseRuleString(ss2);
		string ss3("c,,20:50,>n,,,");
		rmp.parseRuleString(ss3);

		MidiEvent ev1;
		cout << rmp.toString() << endl;
		ev1.parse("c,2,30,10", true);
		cout << ev1.toString() << endl;
		int k = rmp.findMatchingRule(ev1, 0);

		cout << "found at: " << k << endl;

	} catch (string err) {
		cout << err << endl;
	} catch (...) {
		cout << __func__ << " error!" << endl;
	}
	cout << "done" << endl;

}

