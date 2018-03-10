#include "pch.h"

#include "AlsaMapper.h"
#include "MidiEvent.h"
#include "RuleMapper.h"
#include "MidiFilter.h"
#include "tests.h"

using namespace std;

void help();

int main(int argc, char* argv[]) {

	test2();
	exit(0);

	char* ruleFile = nullptr;
	int verbose = false;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-f") == 0 && i + 1 <= argc) {
			ruleFile = argv[i + 1];
		} else if (strcmp(argv[i], "-v") == 0) {
			verbose = 1;
		} else if (strcmp(argv[i], "-vv") == 0) {
			verbose = 2;
		} else if (strcmp(argv[i], "-h") == 0) {
			help();
		} else {
		}
	}
	if (ruleFile == nullptr) {
		help();
	}

	RuleMapper rmp(verbose);
	try {

		rmp.parseFileStream(ruleFile);
		cout << rmp.toString();

		AlsaMapper amp(rmp);

		MidiFilter mf(amp);
		mf.open_alsa_connection();
		mf.process_events();

	} catch (const string& err) {
		cout << __func__ << err << endl;
	} catch (...) {
		cout << __func__ << "  Unexpected error!" << endl;
	}
	cout << "done" << endl;

}

void help() {
	cout
			<< "Usage: mimap3 <options>\n"
					"<options> are:\n"
					"  -h\n"
					"    Displays this info\n"
					"  -f <rules file>\n"
					"    load file with MIDI mapping rules\n"
					"  -v\n"
					"    verbose info\n"
					"  -vv\n"
					"    verbose info and runtime MIDI info\n"
					"Rules are lines in the file, comment character is ';'. Example \n"
					"n,2,,0-30 = n,,,0; rule to mute any notes with velocity < 31 on channel 2 \n"
					"n,1-16,, = p1-1,,2+1,; rule to map notes to  program change\n"
					"\n";
	exit(0);
}
