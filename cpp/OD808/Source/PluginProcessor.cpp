/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OD808AudioProcessor::OD808AudioProcessor() 
	: parameters(*this, nullptr, juce::Identifier("Parameters"), {
			std::make_unique<juce::AudioParameterFloat> ("gain", "Gain", 0.0f, 1.0f, 0.5f),
			std::make_unique<juce::AudioParameterFloat> ("drive", "Drive", 0.0f, 1.0f, 0.5f),
			std::make_unique<juce::AudioParameterFloat> ("tone", "Tone", 0.0f, 1.0f, 0.5f),
			std::make_unique<juce::AudioParameterBool> ("bypass", "Bypass_OD808", false),
		})
#ifndef JucePlugin_PreferredChannelConfigurations
     , AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{

	distortion = parameters.getRawParameterValue("drive");
	tone = parameters.getRawParameterValue("tone");
	gain = parameters.getRawParameterValue("gain");
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
    return 1;  
}

int OD808AudioProcessor::getCurrentProgram()
{
    return 0;
}

void OD808AudioProcessor::setCurrentProgram (int index)
{
	index;
}

const juce::String OD808AudioProcessor::getProgramName (int index)
{
	index;
    return {};
}

void OD808AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
	index;
	newName;
}

//==============================================================================
void OD808AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	juce::dsp::ProcessSpec specs;
	specs.sampleRate = sampleRate;
	specs.maximumBlockSize = samplesPerBlock;
	specs.numChannels = 2;
	
	processorChain.prepare(specs);

	processorChain.get<toneId>().setTone(parameters.getParameterAsValue("tone").getValue());
	processorChain.get<outBufferId>().setGain(parameters.getParameterAsValue("gain").getValue());
	processorChain.get<driveId>().setDistortion(parameters.getParameterAsValue("drive").getValue());

	processorChain.get<trimGainId>().setGainDecibels(-3.0);
}

void OD808AudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OD808AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
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
    return new OD808AudioProcessorEditor (*this, parameters);
}

//==============================================================================
void OD808AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
	auto state = parameters.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void OD808AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{

	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(parameters.state.getType()))
			parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OD808AudioProcessor();
}
