/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic startup code for a Juce application.
 
 ==============================================================================
 */

#ifndef __PLUGINPROCESSOR_H_FC6E1D92__
#define __PLUGINPROCESSOR_H_FC6E1D92__

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
 */
class MotionSicknessAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    MotionSicknessAudioProcessor();
    ~MotionSicknessAudioProcessor();
	
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();
	
    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);
	
    //==============================================================================
    AudioProcessorEditor* createEditor();
    bool hasEditor() const;
	
    //==============================================================================
    const String getName() const;
	
    int getNumParameters();
	
    float getParameter (int index);
    void setParameter (int index, float newValue);
	
    const String getParameterName (int index);
    const String getParameterText (int index);
	
    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;
	
    bool acceptsMidi() const;
    bool producesMidi() const;
    bool silenceInProducesSilenceOut() const;
    double getTailLengthSeconds() const;
	
    //==============================================================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);
	
    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);
	
	double _wtHz;
    double _wtDepth;
	double _mix;
	int _wt;
	
	
	int Parameters;
	enum Parameters
	{
		WaveSpeed,
		WetDry,
		Wavetable,
		Wavedepth
	};
	
	
	
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MotionSicknessAudioProcessor)
	
	double _sample_rate;
    double _step;
	double readhead, writehead, wtindex;
	double mil;
	
	float* _triwave;
    float* _sinewave;
    float* _squarewave;
    float* _sawwave;
    float* _current_wavetable;
	
    double _wt_index;
    double _wt_step;
    int _wavetable_length;

	
	float* delayleft;
	float* delayright;
	float* dryleft;
	float* dryright;
	int srate;
	int wavetables;
	
	int delcount;
	bool gotime;
};

#endif   //__PLUGINPROCESSOR_H_FC6E1D92__
