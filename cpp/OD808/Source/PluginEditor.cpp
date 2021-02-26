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

	overDriveSl = new juce::Slider(juce::Slider::SliderStyle::Rotary, juce::Slider::TextBoxAbove);
	//overDriveSl->setSkewFactorFromMidPoint(0.8);
	//overDriveSl->
	overDriveSl->setRange(0.0, 1.0, 0.01);
	overDriveSl->setValue(0.5);
	overDriveSl->addListener(this);
	addAndMakeVisible(overDriveSl);

	balanceSl = new juce::Slider(juce::Slider::SliderStyle::Rotary, juce::Slider::NoTextBox);
	balanceSl->setRange(0.0, 1.0, 0.01);
	balanceSl->setValue(0.5);
	balanceSl->addListener(this);
	addAndMakeVisible(balanceSl);

	toneSl = new juce::Slider(juce::Slider::SliderStyle::Rotary, juce::Slider::NoTextBox);
	toneSl->setRange(0.0, 1.0, 0.01);
	toneSl->setValue(0.5);
	toneSl->addListener(this);
	addAndMakeVisible(toneSl);

	overDriveLa.setText("OVERDRIVE", juce::dontSendNotification);
	overDriveLa.setSize(80, 10);
	addAndMakeVisible(overDriveLa);

	balanceLa.setText("BALANCE", juce::dontSendNotification);
	balanceLa.setSize(80, 20);
	addAndMakeVisible(balanceLa);

	toneLa.setText("TONE", juce::dontSendNotification);
	toneLa.setSize(50, 20);
	addAndMakeVisible(toneLa);

	startTimerHz(60);

    setSize (200, 400);
}

OD808AudioProcessorEditor::~OD808AudioProcessorEditor()
{
	delete overDriveSl;
	delete balanceSl;
	delete toneSl;
}

//==============================================================================
void OD808AudioProcessorEditor::paint (juce::Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
	auto r = getLocalBounds();

	g.setColour(juce::Colours::black);
	g.drawText( "v" + juce::String(__TIME__) , r.removeFromBottom(50), juce::Justification::centred);

	r.removeFromTop(10);

	g.setColour(getPeakColor(peak));
	auto er = r.removeFromTop(20).removeFromRight(20).toFloat();
	er.setCentre(getWidth() / 2, er.getCentreY());

	g.fillEllipse(er);

}

void OD808AudioProcessorEditor::resized()
{
	auto r = getLocalBounds();

	r.removeFromTop(20);

	auto top = r.removeFromTop(120);
	auto topLeft = top.removeFromLeft(100);
	auto topRight = top.removeFromRight(100);

	auto topLeftSl = topLeft.removeFromTop(90);
	overDriveSl->setBounds(topLeftSl);
	overDriveLa.setCentrePosition(sizeW / 4, topLeftSl.getBottomLeft().getY());

	auto topRightSl = topRight.removeFromTop(90);
	balanceSl->setBounds(topRightSl);
	balanceLa.setCentrePosition(sizeW * 0.75, topRightSl.getBottomLeft().getY());

	toneSl->setBounds(r.removeFromTop(80));
	auto tr = r.removeFromTop(20);
	toneLa.setCentrePosition(sizeW / 2, tr.getTopLeft().getY());

}
