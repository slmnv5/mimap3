#include "pch.h"

#include "AlsaMapper.h"
#include "MidiEvent.h"
#include "RuleMapper.h"
#include "MidiFilter.h"


using namespace std;

int main() {
	RuleMapper rmp;
	try {

		rmp.parseFileStream("/home/erik/psr300.map");
		cout << rmp.toString();

		AlsaMapper amp(rmp);

		MidiFilter mf(amp);
		mf.open_alsa_connection();
		mf.process_events();

	} catch (const string& err) {
		cerr << __func__ << err << endl;
	} catch (...) {
		cerr << __func__ << "  Unexpected error!" << endl;
	}
	cout << "done" << endl;

}
