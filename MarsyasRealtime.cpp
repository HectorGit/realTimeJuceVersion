#pragma once

//#include "stdafx.h"
//#include "juce_audio_devices\audio_io\"
//#include "../JuceLibraryCode/JuceHeader.h"

//#include "juce_audio_basics\buffers\juce_AudioSampleBuffer.h" //is this what we need?
#include "MarsyasRealtime.h"

MarsyasRealtime::MarsyasRealtime(int bufferSize) : readPos(0)
{

	//myClassifier = new OpenNNClassifier();
	inputBuffer = new AudioBuffer<float>(1,bufferSize);//init with 
	inputBuffer->clear();

	sropt = 41000.0;//changed this
	copt = 1;
	bufferSize = 1024; //vs 512 //ALWAYS DO 1024 BC THATS HOW I EXTRACTED FEATURES.

	cout << "realtime feat extract into a single vector" << endl;

	//then the extractor net
	net = mng.create("Series", "net");
	net->updControl("mrs_real/israte", sropt);
	net->updControl("mrs_natural/inSamples", bufferSize);

	//print info out
	srate = net->getctrl("mrs_real/israte")->to<mrs_real>();
	cout << "srate =  " << srate << endl;
	inSamples = net->getctrl("mrs_natural/inSamples")->to<mrs_natural>();
	cout << "net inSamples = " << inSamples << endl;

	/*NEW*/
	MarSystem* spectimeFanout = mng.create("Fanout", "spectimeFanout");
	spectimeFanout->addMarSystem(mng.create("ZeroCrossings", "zcrs")); //1
	spectimeFanout->addMarSystem(mng.create("Rms", "rms"));            //2
	/*NEW*/																   /*NEW*/

	MarSystem* spectrumOperation = mng.create("Series", "spectrumOperation");
	spectrumOperation->addMarSystem(mng.create("Windowing", "ham"));
	spectrumOperation->addMarSystem(mng.create("Spectrum", "spec"));
	spectrumOperation->addMarSystem(mng.create("PowerSpectrum", "pspk"));

	//net->addMarSystem(spectrumOperation);

	MarSystem* featureFanout = mng.create("Fanout", "featureFanout");
	featureFanout->addMarSystem(mng.create("Centroid", "centroid"));
	featureFanout->addMarSystem(mng.create("Rolloff", "rolloff"));
	featureFanout->addMarSystem(mng.create("MFCC", "mfcc"));
	featureFanout->addMarSystem(mng.create("Kurtosis", "kurtosis"));
	featureFanout->addMarSystem(mng.create("Skewness", "skewness"));
	/*NEW*/
	featureFanout->addMarSystem(mng.create("SFM", "sfm"));
	featureFanout->addMarSystem(mng.create("SCF", "scf"));
	/*NEW*/

	spectrumOperation->addMarSystem(featureFanout);      // theExtractorNet contains(spectTimeFanout which contains ((theSpectralNet which contains featureFanout)))
	spectimeFanout->addMarSystem(spectrumOperation);
	net->addMarSystem(spectimeFanout);

	//net->addMarSystem(featureFanout);
	// FOR SOME REASON SETTING UP THIS - might be unnecessary

	//MarSystem* acc = mng.create("Accumulator", "acc");
	//acc->addMarSystem(net);

	//total = mng.create("Series", "total");
	//total->addMarSystem(acc);
	//total->updControl("Accumulator/acc/mrs_natural/nTimes", 15); //was 10 with SR = 44100
	//total->addMarSystem(mng.create("Mean", "mn"));
	//total->updControl("mrs_real/israte", 48000.0); // the plugin is doing 48000 so I'm going to change it
												   // it was originally 44100
	//total->updControl("mrs_natural/inSamples", bufferSize); //origin 512
	//total->addMarSystem(net);

	//technically should be net-> onObservations, but its the same size so nevermind
	//might change it later. 
	//this might complicate classification accuracy - because can't average
	//with other frames... as I was able to before.
	out2.create(net->getctrl("mrs_natural/onObservations")->to<mrs_natural>()); //hmm weird.
}

MarsyasRealtime::~MarsyasRealtime() { 
	delete net;
	//delete total;
	delete inputBuffer; //does this work?
}

//problem simplification
//this is a cheat - assuming that the buffer is exactly 1024 samples, 
//just read them in and copy them to the buffer. that's it.
//must clear the buffer before the next call to this method.
void MarsyasRealtime::runDetection2(const float *samples, int numSamples) {
	//inputBuffer.clear(); //clear it because it had samples from previous call.
	
	int samplesRead = 0;
	float* input = inputBuffer->getWritePointer(0);
	cout << "\n (runDetection) first sample: " << samples[0] << endl;
	//could use numSamples here too , or bufferSize,
	//but in this case we are assuming they are equivalent
	// = problem simplification
	//using i instead of readPos.
	for (int i = 0; i < numSamples; i++, samplesRead++) {
		//copied value
		//cout << "\n within runDetection2 : copied value : " << samples[samplesRead];
		input[i] = samples[samplesRead];
	}

	cout << "finished running sample grabber / e.g. runDetection2" << endl;
	//cout << "\n out of runDetection with input buffer: " <<  << endl;
	cout << "out of runDetection - last value : " << samples[samplesRead - 1] << endl;

}

void MarsyasRealtime::runDetection(const float *samples, int numSamples) {
	int samplesRead = 0;
	float* input = inputBuffer->getWritePointer(0);

	while (samplesRead < numSamples)
	{
		if (readPos >= inputBuffer->getNumSamples())
		{
			//readPos = 0;
			//applyAnalysis(/*expected an mrs_realvec... hmmm*/);//COMMENTING OUT
																 //PROBLEMATIC.
		}
		else
		{
			input[readPos] = samples[samplesRead];

			readPos++;
			samplesRead++;
		}
	}
	cout << "finished running sample grabber / e.g. runDetection" << endl;
	cout << "out of runDetection - last value : " << samples[samplesRead-1] << endl;
}

//void MarsyasRealtime::applyAnalysis()
//{
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
//}

//might have to move this to main (in realTimeClassificaiton.cpp)
//THIS MIGHT NOT WORK, BECAUSE WE NEED TO RECEIVE THE MRS REALVEC IN THE PROCESSBLOCK.
//AND CLASSIFY IT THERE. WITH OPENNN Classifier.
mrs_realvec MarsyasRealtime::applyAnalysis() {
	

	//need to create mrs_realvec in.
	int samplesRead = 0;
	mrs_realvec inMarsyas(inputBuffer->getNumSamples()); //inMarsyas(512) size has not been specified / allocated.
	mrs_realvec outMarsyas;

	const float* input = inputBuffer->getReadPointer(0);
	int numSamples = inputBuffer->getNumSamples();

	while (samplesRead < numSamples) {
		inMarsyas(samplesRead) = input[samplesRead];//inputBuffer(samplesRead); //wtf
		samplesRead++;
	}
	cout << "done reading samples in from applyAnalysis in MarsyasRealtime.cpp" << endl;
	cout << "\n (within applyAnalysis) inMarsyas: \n" << inMarsyas << endl;
	//hmm this seems wonky. --- 
	//could throw shitty error
	//mrs_realvec out; // how big does this need to be? do I need to tell it?
	cout << "about to process samples" << endl;
	//total-> process(inMarsyas, out2);
	net-> process(inMarsyas, out2);
	cout << "\n (within applyAnalysis) processed samples : " << out2 << endl;
	return out2; //total doesn't know what it's input is - //its no longer audiosource.


	/*total->tick();
	const mrs_realvec src_data =
		total->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
	return src_data;
	*/
}