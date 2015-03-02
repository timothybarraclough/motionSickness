/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic startup code for a Juce application.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>


//==============================================================================
MotionSicknessAudioProcessor::MotionSicknessAudioProcessor()
{
	_wavetable_length = 4096;
    _triwave = new float[_wavetable_length];
    _sinewave = new float[_wavetable_length];
    _sawwave = new float[_wavetable_length];
    _squarewave = new float[_wavetable_length];
    for (int i = 0; i < _wavetable_length; i++)
    {
		_sinewave[i] = (sin((double)i/(double)_wavetable_length * 2 * M_PI));
        _sawwave[i] = ((float)i/(float)_wavetable_length)*2 - 1;
        
        if (i < _wavetable_length/2) _squarewave[i] = -1.0;
        else _squarewave[i] = 1.0;
        
        _triwave[i] = fabsf(_sawwave[i])*2 - 1;
    }
    
    _step = 1;
    _current_wavetable = _sinewave;
    
}

MotionSicknessAudioProcessor::~MotionSicknessAudioProcessor()
{
}

//==============================================================================
const String MotionSicknessAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int MotionSicknessAudioProcessor::getNumParameters()
{
    return 4;
}

float MotionSicknessAudioProcessor::getParameter (int index)
{
    switch (index) {
			
        case WetDry:
            return _mix;
			
		case Wavedepth:
            return _wtDepth;	
			
		case WaveSpeed:
            return _wtHz;
            
        case Wavetable:
        {
            if (_current_wavetable == _triwave)return 2;
            if (_current_wavetable == _sinewave) return 1;
            if (_current_wavetable == _sawwave) return 3;
            if (_current_wavetable == _squarewave) return 4;
        }
            
            
        default:
            return 0.0f;
    }
}

void MotionSicknessAudioProcessor::setParameter (int index, float newValue)
{
    switch (index) {
            
        case WetDry:
		{
            _mix = newValue;
            break;
		}
			
		case Wavedepth:
		{
            _wtDepth = newValue/100.0;
            break;
		}	
			
		case WaveSpeed:
		{
            _wtHz = newValue/5;
            break;
		}	
			
        case Wavetable:
        {
			_wt = newValue;
            switch ((int)newValue)
            {
                case 2:
                    _current_wavetable = _triwave;
                    break;
				
                case 1:
                    _current_wavetable = _sinewave;
                    break;
                    
                case 3:
                    _current_wavetable = _sawwave;
                    break;
                    
                case 4:
                    _current_wavetable = _squarewave;
                    break;
                    
                default:
                    break;
            }
        }
            
        default:
            break;
    }
}

const String MotionSicknessAudioProcessor::getParameterName (int index)
{
    return String::empty;
}

const String MotionSicknessAudioProcessor::getParameterText (int index)
{
    return String::empty;
}

const String MotionSicknessAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String MotionSicknessAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool MotionSicknessAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool MotionSicknessAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool MotionSicknessAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool MotionSicknessAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool MotionSicknessAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double MotionSicknessAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MotionSicknessAudioProcessor::getNumPrograms()
{
    return 0;
}

int MotionSicknessAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MotionSicknessAudioProcessor::setCurrentProgram (int index)
{
}

const String MotionSicknessAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void MotionSicknessAudioProcessor::changeProgramName (int index, const String& newName)
{
}

int tmod( double a, int b) {
	float ret = fmod(a, b);
	if (ret < 0)
		ret = ret + b;
	return ret;
}   

double tmod2( double a, int b) {
	float ret = fmod(a, b);
	if (ret < 0)
		ret = ret + b;
	return ret;
}

double timterpolate(
					double y1,double y2,
					double mu)
{
	double mu2;
	mu2 = (1-cos(mu * M_PI))/2;
	return(y1*(1-mu2)+y2*mu2);
}

double timterpolate(
						double y0,double y1,
						double y2,double y3,
						double mu)
{
	double a0,a1,a2,a3,mu2;
	
	mu2 = mu*mu;
	a0 = y3 - y2 - y0 + y1;
	a1 = y0 - y1 - a0;
	a2 = y2 - y0;
	a3 = y1;
	
	return(a0*mu*mu2+a1*mu2+a2*mu+a3);
}




//==============================================================================
void MotionSicknessAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
		srate = sampleRate;
		mil =	srate / 1000;
		writehead = 100;
		readhead = 0;
		
		delcount = 0;
		gotime = false;
		
		delayleft = new float[srate];
		delayright = new float[srate];
		dryleft = new float[srate];
		dryright = new float[srate];
		for(int x=0; x < srate; ++x)
		{
			delayleft[x]=0;
			delayright[x]=0;
		}
	
	
	
}

void MotionSicknessAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
	delete [] delayleft;
    delete [] delayright;
}

void MotionSicknessAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	//Create an instance of my temporary read heads and write head
	
	double tempwritehead = writehead;
	double tempreadhead = readhead;
	double tempwtindex = wtindex;
	
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    for (int channel = 0; channel < getNumInputChannels(); ++channel)
    {
        float* channelData = buffer.getSampleData (channel);
		float samp = 0.0f;
		
		
		//	Make sure these are set to the same value for both channels
		tempwritehead = writehead;
		tempreadhead = readhead;
		tempwtindex = wtindex;
		
		for (int i = 0; i < buffer.getNumSamples(); i++)
        {
			
            //write this block into the buffer
            if (channel == 0)
                delayleft[(int)tempwritehead] = channelData[i];
            else if (channel == 1)
                delayright[(int)tempwritehead] = channelData[i];
			// 0.1 is the  time for the warble
			
			//MAKE SURE THAT THERE IS OVER 100 SAMPLES IN THE BUFFER
			if (delcount < 100){
				
			delcount++;
			}
			else if (delcount >= 100){
				gotime = true;
			}
			
			if (gotime == true){
			
			tempwtindex = tmod2((tempwtindex + (_wtHz)), _wavetable_length);
			float wtindexpoint = fabs(tempwtindex - (int)tempwtindex);
			float warbleamount = timterpolate(
									   _current_wavetable[tmod(tempwtindex - 1, _wavetable_length)],
									   _current_wavetable[tmod(tempwtindex, _wavetable_length)],
									   _current_wavetable[tmod(tempwtindex + 1, _wavetable_length)],
									   _current_wavetable[tmod(tempwtindex + 2, _wavetable_length)],
									   wtindexpoint);
			// 100 is the severity of the warble, ie depth
			//float warbf = fabs(warbleamount - (int)warbleamount);
			//float warble = (((_wtDepth + 0.1) + 0.5) * warbleamount) / 20.0f;
			float warble = (_wtDepth * warbleamount);
			
			
			tempwritehead = tmod(((int)tempwritehead + 1), srate); 
			tempreadhead = tmod2((tempreadhead + warble + 1), srate);
			//readheadwarble = warble;
			//float samp = 0.0f;
			float timterpol = warble - (int)warble;
			//Send the output
			//float warbf = fabs(tempreadhead - (int)tempreadhead);
			
			if (channel == 0){
				//Timterpolation

				
				samp = 
				
				timterpolate(
									delayleft[tmod((int)tempreadhead - 1,srate)],
									delayleft[tmod((int)tempreadhead,srate)],
									delayleft[tmod((int)tempreadhead + 1,srate)],
									delayleft[tmod((int)tempreadhead + 2,srate)],
									timterpol);
				
				channelData[i] = timterpolate(delayleft[tmod(((int)tempwritehead)-100,srate)], samp, _mix);
			}
			
			else if (channel == 1){
				
				samp = 
				
				timterpolate(		
									delayright[tmod((int)tempreadhead - 1,srate)],
									delayright[tmod((int)tempreadhead, srate)],
									delayright[tmod((int)tempreadhead + 1, srate)],
									delayright[tmod((int)tempreadhead + 2,srate)],
									timterpol);
			
			channelData[i] = timterpolate(delayright[tmod(((int)tempwritehead)-100,srate)], samp, _mix);

			}	
				//channelData[i] = timterpolate(channelData[i], samp, _mix);																				   }
			//channelData[i] = (channelData[i] * (1 - _mix)) + (_mix * samp);
			
		}
	}
		
			
		
	}
	writehead = tempwritehead;
	readhead = tempreadhead;
	wtindex = tempwtindex;
	
    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool MotionSicknessAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* MotionSicknessAudioProcessor::createEditor()
{
    return new MotionSicknessAudioProcessorEditor (this);
}

//==============================================================================
void MotionSicknessAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
	
	// You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
	
	XmlElement xml ("MYPLUGINSETTINGS");
	
    // add some attributes to it..
    xml.setAttribute ("mix", _mix);
    xml.setAttribute ("depth", _wtDepth);
    xml.setAttribute ("table", _wt);
    xml.setAttribute ("Hz", _wtHz);
	
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);
    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    
	

}

void MotionSicknessAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
	if (xmlState != nullptr)
    {
        // make sure that it's actually our type of XML object..
        if (xmlState->hasTagName ("MYPLUGINSETTINGS"))
        {
            // ok, now pull out our parameters..
			
			
            _mix  = (float) xmlState->getDoubleAttribute ("mix", _mix);
            _wtDepth = (float) xmlState->getDoubleAttribute ("depth", _wtDepth);
			_wtHz  = (float) xmlState->getDoubleAttribute ("Hz", _wtHz);
            _wt = xmlState->getIntAttribute ("table", _wt);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MotionSicknessAudioProcessor();
}
