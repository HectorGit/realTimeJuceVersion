#pragma once

//#include "stdafx.h"
#include "MarsyasProcessor.h"
//CommandLineOptions cmd_options;
//string toy_withName;
//int helpopt;
//int usageopt;
//int verboseopt;

MarsyasProcessor::MarsyasProcessor(int bufferSize):readPos(0)
{
	cout << "constructor " << endl;
	cout << "buffer size : " << bufferSize << endl;
	net = mng.create("Series", "net");
	cout << "created series" << endl;
	/*inputBuffer.setSize(1, bufferSize);
	net = mng.create("Series", "net");
	myGain = mng.create("Gain", "myGain");
	net->addMarSystem(myGain);
	net->updctrl("Gain/myGain/mrs_real/gain",0.5); //i hope this works.
	*/
												   
	//were like this 
	/*std::string mystr("Series/net");
	MarSystem* net = mng_.create(mystr);
	MarSystem* cnet = mng_.create("Series","cnet");
	cnet->addMarSystem(mng_.create("Gain","gain"));
	cnet->addMarSystem(mng_.create("Windowing","ham"));
	cnet->addMarSystem(mng_.create("Spectrum","spk"));
	cnet->addMarSystem(mng_.create("PowerSpectrum","pspk"));
	cnet->addMarSystem(mng_.create("Centroid","cntrd"));*/

}

MarsyasProcessor::~MarsyasProcessor()
{
}

void MarsyasProcessor::runDetection(const float *samples, int numSamples) {
	/*int samplesRead = 0;
	float* input = inputBuffer.getWritePointer(0);

	while (samplesRead < numSamples)
	{
		if (readPos >= inputBuffer.getNumSamples())
		{
			readPos = 0;
			applyGain();
		}
		else
		{
			input[readPos] = samples[samplesRead];
			
			readPos++;
			samplesRead++;
		}
	}
	*/
}

void MarsyasProcessor::applyGain()
{
		/*int samplesRead = 0;
		mrs_realvec inMarsyas(inputBuffer.getNumSamples()); //inMarsyas(512) size has not been specified / allocated.
		mrs_realvec outMarsyas;
		
		const float* input = inputBuffer.getReadPointer(0);
		int numSamples = inputBuffer.getNumSamples();
		
		while (samplesRead < numSamples) {
			inMarsyas(samplesRead) = input[samplesRead];//inputBuffer(samplesRead); //wtf
			samplesRead++;
		}

		net->process(inMarsyas, outMarsyas); // i hope tihs works.

		//return the samples and convert back to the correct format so the buffer can output (JUCE)
	*/
}