#pragma once

//#include "stdafx.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iomanip>

#include <marsyas/common_source.h>
#include <marsyas/Collection.h>
#include <marsyas/NumericLib.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/FileName.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/MarSystemTemplateBasic.h>
#include <marsyas/marsystems/MarSystemTemplateAdvanced.h>
#include <marsyas/sched/EvValUpd.h>

#include <marsyas/marsystems/Spectrum2ACMChroma.h>

#include <time.h>

#ifdef MARSYAS_MIDIIO
#include <RtMidi.h>
#endif

#ifdef MARSYAS_AUDIOIO
#include <RtAudio.h>
#endif

#ifdef MARSYAS_PNG
#include <pngwriter.h>
#endif


using namespace std;
using namespace Marsyas;

class MarsyasPlayerNet {

public:
	MarsyasPlayerNet(string soundFileNameParam);
	~MarsyasPlayerNet();
	void playSound(/*string soundFileNameParam*/); //calls tick
	void setSoundFileName(string newSoundFileName); //changes the filneame control
private:
	string soundFileName;
	MarSystemManager mng; //is this typed correctly?
						  //all the other marsystems connected, or maybe just the net.
	MarSystem* net;
};