/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
//#include "MarsyasProcessor.h"
#include "MarsyasRealtime.h"
#include "OpenNNClassifier.h"
#include "MarsyasPlayerNet.h"

//==============================================================================
/**
*/
class PluginTest1AudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    PluginTest1AudioProcessor();
    ~PluginTest1AudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect () const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
	//==============================================================================
	//ScopedPointer<MarsyasProcessor> myMarsyasProcessor;
	ScopedPointer<MarsyasRealtime> marsyasRealtime;
	ScopedPointer<OpenNNClassifier> openNNClassifier;
	ScopedPointer<MarsyasPlayerNet> marsyasPlayerNet;
	float rolling_average_increase;
	float rolling_average_decrease;
	float current_rms;
	float previous_rms;
	float alpha_incr;
	float alpha_decr;
	bool not_processing;
	bool processing;
	int iteration;
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginTest1AudioProcessor)
};
