/*
  ==============================================================================

    MarsyasProcessor.h
    Created: 9 Nov 2017 7:13:15pm
    Author:  Owner

  ==============================================================================
*/

#pragma once

/*
==============================================================================

MarsyasProcessor.h

==============================================================================
*/

//#include "../JuceLibraryCode/JuceHeader.h"

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
//#include <marsyas/system/MarSystem.h>
//#include <marsyas/marsystems/Gain.h>

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

//CommandLineOptions cmd_options;
//string toy_withName;
//int helpopt;
//int usageopt;
//int verboseopt;

class MarsyasProcessor {

public:
	MarsyasProcessor(int bufferSize);
	~MarsyasProcessor();
	void runDetection(const float *samples, int numSamples);
	void applyGain(); // should receive a buffer of samples instead.																						//and what classification it was assigned																													//void feedForwardSingleInstance(/*Eigen::MatrixXf instance*/); //prints whether an instance was correctly classified
private:
	int readPos;
	//AudioBuffer<float> inputBuffer; //lost it because we commented out the thing.
	MarSystemManager mng;
	MarSystem* net;
	MarSystem* myGain;
};
