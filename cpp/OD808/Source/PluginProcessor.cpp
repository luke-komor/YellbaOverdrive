/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OD808AudioProcessor::OD808AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

OD808AudioProcessor::~OD808AudioProcessor()
{
}

//==============================================================================
const juce::String OD808AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OD808AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OD808AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OD808AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OD808AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OD808AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OD808AudioProcessor::getCurrentProgram()
{
    return 0;
}

void OD808AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String OD808AudioProcessor::getProgramName (int index)
{
    return {};
}

void OD808AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void OD808AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	juce::dsp::ProcessSpec specs;
	specs.sampleRate = sampleRate;
	specs.maximumBlockSize = samplesPerBlock;
	specs.numChannels = 2;
	
	processorChain.prepare(specs);

	//processorChain.get<upSamplingId>.
	processorChain.get<balanceId>().setGainLinear(0.5f);
	processorChain.get<driveId>().setDistortion(0.5f);
	
}

void OD808AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OD808AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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


void OD808AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

	juce::dsp::AudioBlock<float> io(buffer);
	juce::dsp::ProcessContextReplacing<float> context(io);
	
	processorChain.process(context);

	auto out = context.getOutputBlock();
	float avg = 0.0f;
	auto numSamples = out.getNumChannels();

	for (int i = 0; i < numSamples; ++i) {
		avg += abs(out.getSample(0, i));
	}

	avg_out_val = avg / numSamples;

}

//==============================================================================
bool OD808AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OD808AudioProcessor::createEditor()
{
    return new OD808AudioProcessorEditor (*this);
}

//==============================================================================
void OD808AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
	//juce::MemoryOutputStream(destData, true).writeFloat(*distortion);
}

void OD808AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
//*distortion = juce::MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readFloat();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OD808AudioProcessor();
}
