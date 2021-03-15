/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"



class OD808LookAndFeel : public juce::LookAndFeel_V4
{
public:

	OD808LookAndFeel() {

		pos[0] = juce::Drawable::createFromImageData(Assets::switch_1_png, Assets::switch_1_pngSize);
		pos[1] = juce::Drawable::createFromImageData(Assets::switch_2_png, Assets::switch_2_pngSize);

	}

	OD808LookAndFeel(juce::String knobPrefix, int positionNumbers = 100)  {

		for (int i = 0; i < positionNumbers; i++) {
						
			juce::String res_name = knobPrefix + "_";
			auto index = juce::String(i);

			for (int i = 0; i < (4 - index.length()); ++i)
				res_name += "0";

			res_name += index + "_png";
			//juce::Logger::getCurrentLogger()->outputDebugString(res_name);

			int bytes;
			const char* res = Assets::getNamedResource(res_name.getCharPointer(), bytes);

			pos[i] = juce::Drawable::createFromImageData(res, bytes);
		}
	}

	void drawRotarySlider(juce::Graphics& g,
		int x, int y,
		int width, int height,
		float sliderPosProportional,
		float, float, juce::Slider&) override
	{
		
		int index = static_cast<int>(sliderPosProportional * 100);
		
		if (index >= 100)
			index = 99;

		auto& knob = pos[index];

		float h = knob->getHeight();
		juce::AffineTransform transform = juce::AffineTransform::scale(height / h);
		knob->setTransform(transform);
	
		knob->drawAt(g, 0, 0, 1);
	}

	void drawToggleButton(juce::Graphics& g, juce::ToggleButton& tb,
		bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {

		int index;

		if (tb.getToggleState()) {
			//g.setColour(juce::Colours::black);
			//g.fillAll();
			index = 1;
		}
		else {
			//g.setColour(juce::Colours::red);
			//g.fillAll();
			index = 0;
		}

		auto& button = pos[index];
		float h = button->getHeight();
		juce::AffineTransform transform = juce::AffineTransform::scale(tb.getHeight() / h);
		button->setTransform(transform);

		button->drawAt(g, 0, 0, 1.0);
	}

private:

	std::unique_ptr<juce::Drawable> pos[100];

};


//==============================================================================
/**
*/
class OD808AudioProcessorEditor  : 
	public juce::AudioProcessorEditor, 
	public juce::Slider::Listener,
	public juce::ToggleButton::Listener,
	public juce::Timer
{
public:
    OD808AudioProcessorEditor (OD808AudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~OD808AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

	void timerCallback() override;

	juce::Rectangle<int> getBounds(float windowW, float windowH, float relativeW, float relativeH, float relativeY, float relativeX);

	void buttonClicked(juce::Button* btn) override;
	void sliderValueChanged(juce::Slider* slider) override;
	
	juce::Colour getPeakColor(float pk);

private:

    OD808AudioProcessor& audioProcessor;

	using  SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
	using ToggleButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

	juce::AudioProcessorValueTreeState& valueTreeState;

	std::unique_ptr<SliderAttachment> driveAttachment;
	std::unique_ptr<SliderAttachment> toneAttachment;
	std::unique_ptr<SliderAttachment> gainAttachment;
	
	std::unique_ptr<ToggleButtonAttachment> bypassAttachment;

	juce::Slider driveSlider;
	juce::Slider gainSlider;
	juce::Slider toneSlider;

	juce::ToggleButton bypassButton;

	OD808LookAndFeel* knobA1Laf;
	OD808LookAndFeel* knobA2Laf;
	OD808LookAndFeel* knobBLaf;

	OD808LookAndFeel* buttonLaf;

	float peak = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OD808AudioProcessorEditor)
};
