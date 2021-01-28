/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class OD808AudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    OD808AudioProcessorEditor (OD808AudioProcessor&);
    ~OD808AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

	void sliderValueChanged(juce::Slider* slider) override {
		audioProcessor.setParameter(0, (float) distSl->getValue());
	}

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OD808AudioProcessor& audioProcessor;
	juce::Slider* distSl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OD808AudioProcessorEditor)
};
