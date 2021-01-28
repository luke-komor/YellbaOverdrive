/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class OD808AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    OD808AudioProcessor();
    ~OD808AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	void setDistortion(float val) { *distortion = val; }

private:

	float dVdt(float V, float In, float D) {
		float Cc = 51e-12;
		float R2 = 51e3 + D * 500e3;

		float Is = 1e-12;
		float Vt = 26e-3;

		float dV = In / Cc - V / (R2*Cc) - 2 * Is * sinh(V / Vt);
		return dV;
	}

	float dIdt(float I, float Vi, float Vi1, float h) {
		float R = 4.7e3;
		float C = 0.047e-6;

		float dI = -I / (R * C) + (1 / R) * ((Vi - Vi1) / h);
		return dI;
	}

	float h;
	float fs;

	float Vn = 0;
	float Vnn;

	float In = 0;
	float Inn;

	float last_in = 0;

	juce::AudioParameterFloat* distortion;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OD808AudioProcessor)
};
