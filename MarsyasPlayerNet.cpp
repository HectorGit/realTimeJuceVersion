
//#include "stdafx.h"
#include "MarsyasPlayerNet.h"

using namespace Marsyas;
using namespace std;

MarsyasPlayerNet::MarsyasPlayerNet(string soundFileNameParam) {
	cout << "Constructing MarsyasPlayerNet" << endl;

	soundFileName = soundFileNameParam;

	net = mng.create("Series", "net");

	net->addMarSystem(mng.create("SoundFileSource", "src"));
	net->addMarSystem(mng.create("Gain", "gain"));
	net->addMarSystem(mng.create("AudioSink", "adest"));
	//net->addMarSystem(mng.create("AudioSource", "asrc")); // do we need this?
	//net->addMarSystem(mng.create("SoundFileSink", "dest"));


	// net->updControl("mrs_real/israte", 44100.0);
	// net->updControl("AudioSource/asrc/mrs_natural/nChannels", 1);


	net->updControl("SoundFileSource/src/mrs_string/filename", soundFileName);
	//net->updControl("SoundFileSink/dest/mrs_string/filename", "duplex2.wav");
	net->updControl("AudioSink/adest/mrs_bool/initAudio", true);
	//net->updControl("AudioSource/asrc/mrs_bool/initAudio", true);

}

MarsyasPlayerNet::~MarsyasPlayerNet() {
	delete net;
}

//change the control
void MarsyasPlayerNet::setSoundFileName(string newSoundFileName) {
	net->updControl("SoundFileSource/src/mrs_string/filename", newSoundFileName);
	//is this enough? 
	//what else should be done?
}

//tick
void MarsyasPlayerNet::playSound() {

	cout << "playing sound" << endl;

	while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
	{
		net->tick();
	}
}