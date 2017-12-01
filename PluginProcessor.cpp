/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#pragma once

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
PluginTest1AudioProcessor::PluginTest1AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	//myMarsyasProcessor = new MarsyasProcessor(1024);//changed it to 1024
	marsyasRealtime = new MarsyasRealtime(1024);
	openNNClassifier = new OpenNNClassifier();
	marsyasPlayerNet = new MarsyasPlayerNet("center.wav");//pass in the soundFileName
	rolling_average_increase = 0.0;
	rolling_average_decrease = 0.0;
	current_rms = 0.0;
	previous_rms = 0.0;
	alpha_incr = 0.75;
	alpha_decr = 0.25;
	not_processing = true;
	processing = false;
	rolling_classification = -1; // do this so it doesn't play anything back initially.
	classificationCounters = new OpenNN::Vector<int>(3); //will hold the count for the intermediate classifications.
												 //how can we reset this to zero?
	iteration = 0;
}

PluginTest1AudioProcessor::~PluginTest1AudioProcessor()
{
}

//==============================================================================
const String PluginTest1AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginTest1AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginTest1AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginTest1AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginTest1AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginTest1AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginTest1AudioProcessor::getCurrentProgram()
{
    return 0;
}

void PluginTest1AudioProcessor::setCurrentProgram (int index)
{
}

const String PluginTest1AudioProcessor::getProgramName (int index)
{
    return {};
}

void PluginTest1AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void PluginTest1AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void PluginTest1AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
	delete openNNClassifier; //does this work?
	delete marsyasRealtime;
	//delete marsyasRealtime->total; //release
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginTest1AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PluginTest1AudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

	//WITH AVERAGING THE CLASSIFICATION

	current_rms = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
	previous_rms = buffer.getRMSLevel(0, 0, buffer.getNumSamples()); //necessary?

	if (processing) {
		cout << "processing !" << endl;
		rolling_average_decrease = alpha_decr*(rolling_average_decrease)+(1 - alpha_decr)*(current_rms);
		//MOVING THIS TO THE TIME WHEN THE STATE IS EXITED.
		//rolling_classification = classificationCounters->calculate_maximal_index(); //didn't give problem
																					//cool
		//NOTE TO SELF - MAYBE A MAXIMAL INDEX CAN'T BE CALCULATED ON A ARRAY THAT HAS ALL VALUES
		//SET TO ZERO INITIALLY ... MAYBE IT SHOULD ONLY BE CALCULATED ONCE WE EXIT THE STATE.
		//SO SHOULDN'T HAVE A ROLLING CLASSIFICATION
		//INSTEAD JUST PUT IT WHERE THE THING EXITS.
		// NOTE - WHAT HAPPENS IF TWO COUNTERS HAVE THE SAME VALUE?
		cout << "these are the counters at start of loop" << classificationCounters->to_string() << endl;
		//cout << "calculating maximal index == rolling classification " << rolling_classification << endl;


		if (current_rms < rolling_average_decrease && current_rms < 0.1) {
			//ONLY CALCULATE MAX AFTER SOME PROCESSING HAS HAPPENED
			rolling_classification = classificationCounters->calculate_maximal_index();
			cout << "these are the counters at exit of processing state : \n" << classificationCounters->to_string() << endl;
			cout << "calculating maximal index == rolling classification at exit of processing state : \n " << rolling_classification << endl;

			if (rolling_classification == 0) {
				cout << "classif == 0" << endl;
				marsyasPlayerNet->setSoundFileName("center.wav");
				marsyasPlayerNet->playSound();
			}
			else if (rolling_classification == 1) {
				cout << "classif == 1" << endl;
				marsyasPlayerNet->setSoundFileName("halfedge.wav");
				marsyasPlayerNet->playSound();
			}
			else if (rolling_classification == 2) {
				cout << "classif == 2" << endl;
				marsyasPlayerNet->setSoundFileName("rimshot.wav");
				marsyasPlayerNet->playSound();
			}
			else {
				cout << "classif == neither 0, 1 or 2";
			}
			//reset everything
			classificationCounters->initialize(0);
			rolling_classification = -1;
			processing = false;
			not_processing = true;
			rolling_average_decrease = 0.0;

		}
		else {
			//accumulate a value for the classification...
			//add to a counter of that type of classification [i,j,k] and then
			//once the

			cout << "running detection " << endl;
			marsyasRealtime->runDetection2(buffer.getReadPointer(0), buffer.getNumSamples()); //this gets
																							  //the samples from juce
																							  //uses the output form
			cout << "\n (procBloc) first sample from the buffer: " << buffer.getReadPointer(0)[0] << endl;
			//apply analysis

			cout << "applying analysis " << endl;

			mrs_realvec processedData = marsyasRealtime->applyAnalysis();

			cout << "\n (processBlock) processedData: " << processedData << endl;

			OpenNN::Vector<double> toClassify = openNNClassifier->convert(processedData);
			OpenNN::Vector<double> outputs = openNNClassifier->obtainOutputs(toClassify);
			size_t classificationIndex = openNNClassifier->obtainClassification(outputs);
			double value = outputs.at(classificationIndex);

			if (value < 1.7) { //changed this based on the results - can try to find another reason why this
							   //is weird.
				cout << "Below Threshold" << endl;

			}
			else {
				cout << "Above Threshold" << endl;
				if (classificationIndex == 0) {
					//add to the counter of this classification
					classificationCounters->at(0) += 1;
					cout << "updated classificationCounters" << classificationCounters->to_string() << endl;
					//classificationCounters[0] += 1;
				}
				else if (classificationIndex == 1) {
					//add to the counter of this classification
					classificationCounters->at(1) += 1;
					//classificationCounters[1] += 1;
					cout << "updated classificationCounters" << classificationCounters->to_string() << endl;

				}
				else if (classificationIndex == 2) {
					//add to the counter of this classification
					classificationCounters->at(2) += 1;
					//classificationCounters[2] += 1;
					cout << "updated classificationCounters" << classificationCounters->to_string() << endl;

				}
				else {
					//nothing
				}
			}
			if (openNNClassifier->debug) { cout << "------------------------------------------" << endl; }

		}
	}
	else { //not processing
		cout << "not processing !" << endl;
		rolling_average_increase = alpha_incr*(rolling_average_increase)+(1 - alpha_incr)*(current_rms);

		if (current_rms > rolling_average_increase && current_rms >0.1) {
			processing = true;
			not_processing = false; //unnecessary
			rolling_average_increase = 0.0; //reset - should it be this harsh?
		}
	}
	
}

//==============================================================================
bool PluginTest1AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* PluginTest1AudioProcessor::createEditor()
{
    return new PluginTest1AudioProcessorEditor (*this);
}

//==============================================================================
void PluginTest1AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PluginTest1AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginTest1AudioProcessor();
}


/*WORKING CODE

//WITH RMS
cout << "iteration : " << iteration << endl;
iteration++;

current_rms = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
previous_rms = buffer.getRMSLevel(0, 0, buffer.getNumSamples()); //necessary?

if (processing) {
cout << "processing !" << endl;
rolling_average_decrease = alpha_decr*(rolling_average_decrease)+(1 - alpha_decr)*(current_rms);

if (current_rms < rolling_average_decrease && current_rms < 0.1) {
processing = false;
not_processing = true; // unnecessary
rolling_average_decrease = 0.0;
}
else { // how do we break out of here?

cout << "running detection " << endl;
marsyasRealtime->runDetection2(buffer.getReadPointer(0), buffer.getNumSamples()); //this gets
//the samples from juce
//uses the output form
cout << "\n (procBloc) first sample from the buffer: " << buffer.getReadPointer(0)[0] << endl;
//apply analysis

cout << "applying analysis " << endl;

mrs_realvec processedData = marsyasRealtime->applyAnalysis();

cout << "\n (processBlock) processedData: " << processedData << endl;

OpenNN::Vector<double> toClassify = openNNClassifier->convert(processedData);
OpenNN::Vector<double> outputs = openNNClassifier->obtainOutputs(toClassify);
size_t classificationIndex = openNNClassifier->obtainClassification(outputs);
double value = outputs.at(classificationIndex);

if (value < 1.7) { //changed this based on the results - can try to find another reason why this
//is weird.
cout << "Below Threshold" << endl;

}
else {
cout << "Above Threshold" << endl;
if (classificationIndex == 0) {
cout << "classif == 0" << endl;
marsyasPlayerNet->setSoundFileName("center.wav");
marsyasPlayerNet->playSound();
}
else if (classificationIndex == 1) {
cout << "classif == 1" << endl;
marsyasPlayerNet->setSoundFileName("halfedge.wav");
marsyasPlayerNet->playSound();
}
else if (classificationIndex == 2) {
cout << "classif == 2" << endl;
marsyasPlayerNet->setSoundFileName("rimshot.wav");
marsyasPlayerNet->playSound();
}
else {
//nothing
}
}
if (openNNClassifier->debug) { cout << "------------------------------------------" << endl; }

}
}
else { //not processing
cout << "not processing !" << endl;
rolling_average_increase = alpha_incr*(rolling_average_increase)+(1 - alpha_incr)*(current_rms);

if (current_rms > rolling_average_increase && current_rms > 0.1) { //setting a threshold as well.
processing = true;
not_processing = false; //unnecessary
rolling_average_increase = 0.0; //reset - should it be this harsh?
}
}


*/


//cout << "Size in samples of the buffer: " << buffer.getNumSamples() << endl;

/*

//NO RMS

cout << "running detection " << endl;
marsyasRealtime->runDetection2(buffer.getReadPointer(0), buffer.getNumSamples()); //this gets
//the samples from juce
//the classification
//uses the output form
cout << "\n (procBloc) first sample from the buffer: " << buffer.getReadPointer(0)[0] << endl;
//apply analysis

cout << "applying analysis " << endl;

mrs_realvec processedData = marsyasRealtime->applyAnalysis();

cout << "\n (processBlock) processedData: " << processedData << endl;

OpenNN::Vector<double> toClassify = openNNClassifier->convert(processedData);
OpenNN::Vector<double> outputs = openNNClassifier->obtainOutputs(toClassify);
size_t classificationIndex = openNNClassifier->obtainClassification(outputs);
double value = outputs.at(classificationIndex);

if (value < 1.7) { //changed this based on the results - can try to find another reason why this
//is weird.
cout << "Below Threshold" << endl;

}
else {
cout << "Above Threshold" << endl;
if (classificationIndex == 0) {
cout << "classif == 0" << endl;
marsyasPlayerNet->setSoundFileName("center.wav");
marsyasPlayerNet->playSound();
}
else if (classificationIndex == 1) {
cout << "classif == 1" << endl;
marsyasPlayerNet->setSoundFileName("halfedge.wav");
marsyasPlayerNet->playSound();
}
else if (classificationIndex == 2) {
cout << "classif == 2" << endl;
marsyasPlayerNet->setSoundFileName("rimshot.wav");
marsyasPlayerNet->playSound();
}
else {
//nothing
}
}
if (openNNClassifier->debug) { cout << "------------------------------------------" << endl; }
*/


/*

//with RMS

cout << "iteration : " << iteration << endl;
iteration++;

current_rms = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
previous_rms = buffer.getRMSLevel(0, 0, buffer.getNumSamples()); //necessary?

if (processing) {
cout << "processing !" << endl;
rolling_average_decrease = alpha_decr*(rolling_average_decrease)+(1 - alpha_decr)*(current_rms);

if (current_rms < rolling_average_decrease && current_rms < 0.1) {
processing = false;
not_processing = true; // unnecessary
rolling_average_decrease = 0.0;
}else { // how do we break out of here?

cout << "running detection " << endl;
marsyasRealtime->runDetection2(buffer.getReadPointer(0), buffer.getNumSamples()); //this gets
//the samples from juce
//uses the output form
cout << "\n (procBloc) first sample from the buffer: " << buffer.getReadPointer(0)[0] << endl;
//apply analysis

cout << "applying analysis " << endl;

mrs_realvec processedData = marsyasRealtime->applyAnalysis();

cout << "\n (processBlock) processedData: " << processedData << endl;

OpenNN::Vector<double> toClassify = openNNClassifier->convert(processedData);
OpenNN::Vector<double> outputs = openNNClassifier->obtainOutputs(toClassify);
size_t classificationIndex = openNNClassifier->obtainClassification(outputs);
double value = outputs.at(classificationIndex);

if (value < 1.7) { //changed this based on the results - can try to find another reason why this
//is weird.
cout << "Below Threshold" << endl;

}
else {
cout << "Above Threshold" << endl;
if (classificationIndex == 0) {
cout << "classif == 0" << endl;
marsyasPlayerNet->setSoundFileName("center.wav");
marsyasPlayerNet->playSound();
}
else if (classificationIndex == 1) {
cout << "classif == 1" << endl;
marsyasPlayerNet->setSoundFileName("halfedge.wav");
marsyasPlayerNet->playSound();
}
else if (classificationIndex == 2) {
cout << "classif == 2" << endl;
marsyasPlayerNet->setSoundFileName("rimshot.wav");
marsyasPlayerNet->playSound();
}
else {
//nothing
}
}
if (openNNClassifier->debug) { cout << "------------------------------------------" << endl; }

}
}
else { //not processing
cout << "not processing !" << endl;
rolling_average_increase = alpha_incr*(rolling_average_increase)+(1 - alpha_incr)*(current_rms);

if (current_rms > rolling_average_increase && current_rms > 0.1) { //setting a threshold as well.
processing = true;
not_processing = false; //unnecessary
rolling_average_increase = 0.0; //reset - should it be this harsh?
}
}

*/

/*
//WITH AVERAGING THE CLASSIFICATION

current_rms = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
previous_rms = buffer.getRMSLevel(0, 0, buffer.getNumSamples()); //necessary?

if (processing) {
cout << "processing !" << endl;
rolling_average_decrease = alpha_decr*(rolling_average_decrease)+(1 - alpha_decr)*(current_rms);
rolling_classification = classificationCounters->calculate_maximal_index(); //didn't give problem
//cool

if (current_rms < rolling_average_decrease && current_rms < 0.1) {

if (rolling_classification == 0) {
cout << "classif == 0" << endl;
marsyasPlayerNet->setSoundFileName("center.wav");
marsyasPlayerNet->playSound();
}
else if (rolling_classification == 1) {
cout << "classif == 1" << endl;
marsyasPlayerNet->setSoundFileName("halfedge.wav");
marsyasPlayerNet->playSound();
}
else if (rolling_classification == 2) {
cout << "classif == 2" << endl;
marsyasPlayerNet->setSoundFileName("rimshot.wav");
marsyasPlayerNet->playSound();
}
else {
cout << "classif == neither 0, 1 or 2";
}
//reset everything
classificationCounters->initialize(0);
rolling_classification = -1;
processing = false;
not_processing = true;
rolling_average_decrease = 0.0;

}
else {
//accumulate a value for the classification...
//add to a counter of that type of classification [i,j,k] and then
//once the

cout << "running detection " << endl;
marsyasRealtime->runDetection2(buffer.getReadPointer(0), buffer.getNumSamples()); //this gets
//the samples from juce
//uses the output form
cout << "\n (procBloc) first sample from the buffer: " << buffer.getReadPointer(0)[0] << endl;
//apply analysis

cout << "applying analysis " << endl;

mrs_realvec processedData = marsyasRealtime->applyAnalysis();

cout << "\n (processBlock) processedData: " << processedData << endl;

OpenNN::Vector<double> toClassify = openNNClassifier->convert(processedData);
OpenNN::Vector<double> outputs = openNNClassifier->obtainOutputs(toClassify);
size_t classificationIndex = openNNClassifier->obtainClassification(outputs);
double value = outputs.at(classificationIndex);

if (value < 1.7) { //changed this based on the results - can try to find another reason why this
//is weird.
cout << "Below Threshold" << endl;

}
else {
cout << "Above Threshold" << endl;
if (classificationIndex == 0) {
//add to the counter of this classification
//classificationCounters->at(0) += 1;
classificationCounters[0] += 1;
}
else if (classificationIndex == 1) {
//add to the counter of this classification
//classificationCounters->at(1) += 1;
classificationCounters[1] += 1;
}
else if (classificationIndex == 2) {
//add to the counter of this classification
//classificationCounters->at(2) += 1;
classificationCounters[2] += 1;
}
else {
//nothing
}
}
if (openNNClassifier->debug) { cout << "------------------------------------------" << endl; }

}
}
else { //not processing
cout << "not processing !" << endl;
rolling_average_increase = alpha_incr*(rolling_average_increase)+(1 - alpha_incr)*(current_rms);

if (current_rms > rolling_average_increase && current_rms >0.1) {
processing = true;
not_processing = false; //unnecessary
rolling_average_increase = 0.0; //reset - should it be this harsh?
}
}
*/