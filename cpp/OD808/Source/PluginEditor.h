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
class OD808AudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener, public juce::Timer
{
public:
    OD808AudioProcessorEditor (OD808AudioProcessor&);
    ~OD808AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

	void timerCallback() override {

		//static int index = 0;
		static float alpha = 0.95f;
		static float sum = 0.0f;
		const static int N = 10;
		static int index = 0;
		static float last = 0.0f;
		static float buff[N]{0.0f};
		//static float pvals[5]{0.1f, 0.3f, 0.6f, 0.9f, 1.1f};
		//peak = pvals[index++ % 5];

		// filtering
		//peak = last_peak +  alpha * (audioProcessor.getPeak() - last_peak);
		
		// sma and max_peak and release

		float actual = audioProcessor.getLevel();

		sum -= buff[index];
		sum += actual;
		buff[index++] = actual;
		//
		index %= N;

		actual = sum  / N;
		
		if (actual > peak) {
			peak = actual;
		}
		else {
			peak *= alpha;
		}

		repaint();
	}

	void sliderValueChanged(juce::Slider* slider) override {
		
		if (slider == overDriveSl) {
			auto pos = overDriveSl->getValue();
			// R = 1.0
			// b = 
			// adding tamper resistor to linear potentiometer
			//auto skew = 16.0f;
			//auto proportion = 1.0f / ((1.0 - pos) / pos + (1.0f - pos) / skew + 1.0f);

			auto proportion = exp(log(pos) / 2.0f);
			auto val = proportion;

			juce::Logger::getCurrentLogger()->outputDebugString("pos : " + juce::String(pos) + " val : "+ juce::String(val));
			audioProcessor.setOverdrive(val);
		}
		else if (slider == balanceSl)
			audioProcessor.setBalance(balanceSl->getValue());
		else if(slider == toneSl)
			audioProcessor.setTone(toneSl->getValue());
	}

	juce::Colour getPeakColor(float peak) {
	
		//auto level_0{ juce::Colour(29,72,119) };
		//auto level_1{ juce::Colour(27,138,90) };
		//auto level_2{ juce::Colour(251,176,33) };
		//auto level_3{ juce::Colour(246,136,56) };
		auto red{ juce::Colours::red };

		return red.withBrightness(peak + 0.20f);
	}

private:

	int sizeH = 400;
	int sizeW = 200;

    OD808AudioProcessor& audioProcessor;
	juce::Slider* overDriveSl;
	juce::Slider* balanceSl;
	juce::Slider* toneSl;

	juce::Label overDriveLa;
	juce::Label balanceLa;
	juce::Label toneLa;

	float peak = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OD808AudioProcessorEditor)
};
