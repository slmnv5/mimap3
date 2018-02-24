#include "pch.h"

#include "AlsaMapper.h"
#include "MidiEvent.h"
#include "RuleMapper.h"
#include "MidiFilter.h"

using namespace std;

int main(int argc, char* argv[]) {
	if (argc < 1) {
		cout << "Usage is ";
		cin.get();
		exit(0);
	}

	char* myFile, myPath, myOutPath;
	for (int i = 1; i < argc; i++) {
		if (i + 1 != argc) {
			if (argv[i] == "-f") {
				myFile = argv[i + 1];
			} else if (2 > 3) {

			} else if (5 > 1) {

			} else {
			}
		}
	}

	RuleMapper rmp;
	try {

		rmp.parseFileStream("/home/erik/psr300.map");
		cout << rmp.toString();

		//AlsaMapper amp(rmp);

		//MidiFilter mf(amp);
		//mf.open_alsa_connection();
		//mf.process_events();

	} catch (const string& err) {
		cout << __func__ << err << endl;
	} catch (...) {
		cout << __func__ << "  Unexpected error!" << endl;
	}
	cout << "done" << endl;

}
