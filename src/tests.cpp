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

		cout << rmp.toString() << endl;

		MidiEvent ev1, ev2, ev3;
		int k;
		ev1.parse("c,2,30,10", true);
		ev2.parse("n,,30,10", true);
		ev3.parse("n,,,", true);

		cout << ev3.toString() << endl;
		k = rmp.findMatchingRule(ev3, 0);
		cout << "found at: " << k << endl;

		cout << ev1.toString() << endl;
		k = rmp.findMatchingRule(ev1, 0);
		cout << "found at: " << k << endl;

		cout << ev2.toString() << endl;
		k = rmp.findMatchingRule(ev2, 0);
		cout << "found at: " << k << endl;



	} catch (string err) {
		cout << err << endl;
	} catch (...) {
		cout << __func__ << " error!" << endl;
	}
	cout << "done" << endl;

}

