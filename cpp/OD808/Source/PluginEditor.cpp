/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OD808AudioProcessorEditor::OD808AudioProcessorEditor (OD808AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
	distSl = new juce::Slider(juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxBelow);
	distSl->setRange(0.0, 1.0, 0.01);
	distSl->setValue(0.5);
	distSl->addListener(this);
	addAndMakeVisible(distSl);

    setSize (400, 300);
}

OD808AudioProcessorEditor::~OD808AudioProcessorEditor()
{
	delete distSl;
}

//==============================================================================
void OD808AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void OD808AudioProcessorEditor::resized()
{
	auto r = getLocalBounds();
	distSl->setBounds(r.removeFromTop(100));
}
