/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OD808AudioProcessorEditor::OD808AudioProcessorEditor (OD808AudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
{

	driveSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	driveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

	knobA1Laf =  new OD808LookAndFeel("ka1");

	driveSlider.setLookAndFeel(knobA1Laf);
	driveSlider.setRange(0.0, 1.0, 0.01);
	driveSlider.addListener(this);
	addAndMakeVisible(driveSlider);

	driveAttachment.reset(new SliderAttachment(valueTreeState, juce::String("drive"), driveSlider));

	gainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag); 
	gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

	knobA2Laf = new OD808LookAndFeel("ka2");

	gainSlider.setLookAndFeel(knobA2Laf);
	gainSlider.setRange(0.0, 1.0, 0.01);
	gainSlider.addListener(this);
	addAndMakeVisible(gainSlider);

	gainAttachment.reset(new SliderAttachment(valueTreeState, "gain", gainSlider));



	toneSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	toneSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

	knobBLaf = new OD808LookAndFeel("kb");

	toneSlider.setLookAndFeel(knobBLaf);
	toneSlider.setRange(0.0, 1.0, 0.02);
	toneSlider.addListener(this);
	addAndMakeVisible(toneSlider);

	toneAttachment.reset(new SliderAttachment(valueTreeState, "tone", toneSlider));


	buttonLaf = new OD808LookAndFeel();
	bypassButton.setLookAndFeel(buttonLaf);
	bypassButton.addListener(this);
	addAndMakeVisible(bypassButton);

	bypassAttachment.reset(new ToggleButtonAttachment(valueTreeState, "bypass", bypassButton));


	startTimerHz(60);

	setResizable(true, true);
	setResizeLimits(300, 500, 600, 1000);
	getConstrainer()->setFixedAspectRatio(3.0/5.0);
	setSize(300, 500);
}

OD808AudioProcessorEditor::~OD808AudioProcessorEditor()
{
	driveSlider.setLookAndFeel(nullptr);
	toneSlider.setLookAndFeel(nullptr);
	gainSlider.setLookAndFeel(nullptr);
	bypassButton.setLookAndFeel(nullptr);

	delete knobA1Laf;
	delete knobA2Laf;
	delete knobBLaf;
	delete buttonLaf;
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

	auto er = r.removeFromTop(200).removeFromRight(200).toFloat();
	er.setCentre(static_cast<float> (getWidth() / 2), er.getCentreY());

	g.fillEllipse(er);

	juce::Image led = juce::ImageCache::getFromMemory(Assets::h_led_png, Assets::h_led_pngSize);

	g.drawImageAt(led.rescaled(getWidth(), getHeight()), 0, 0);

	juce::Image background = juce::ImageCache::getFromMemory(Assets::h_hole_png, Assets::h_hole_pngSize);
	g.drawImageAt(background.rescaled(getWidth(), getHeight()), 0, 0);

}


void OD808AudioProcessorEditor::resized()
{
	// fixme: make some function for this stuff
	
	float h = getHeight();
	float w = getWidth();

	const float ka1_w = 0.235; // measure from blender
	const float ka1_h = 0.23; // measure from blender

	const float ka1_x = -0.127845;  // blender location x
	const float ka1_y = -0.321931;  // blender lcoation y

	driveSlider.setBounds(getBounds(w, h, ka1_w, ka1_h, ka1_y, ka1_x));

	const float ka2_w = 0.222;
	const float ka2_h = 0.222;

	const float ka2_x = 0.132155;
	const float ka2_y = -0.321931;

	gainSlider.setBounds(getBounds(w, h, ka2_w, ka2_h, ka2_y, ka2_x));
	
	const float kb_w = 0.13333;
	const float kb_h = 0.1325;

	const float kb_x = 0.0;
	const float kb_y = -0.145159;

	toneSlider.setBounds(getBounds(w, h, kb_w, kb_h, kb_y, kb_x));

	const float b_w = 0.1513;
	const float b_h = b_w;

	const float b_x = 0.001;
	const float b_y = 0.221;

	bypassButton.setBounds(getBounds(w, h, b_w, b_h, b_y, b_x));
}


juce::Rectangle<int> OD808AudioProcessorEditor::getBounds(float windowW, float windowH, float relativeW, float relativeH, float relativeY, float relativeX)
{

	float scale = 447.76119;
	float scale_window = windowW / 300.0;

	float center_x = windowW / 2;
	float center_y = windowH / 2;

	float rel_w = relativeW * scale; // measure from blender
	float rel_h = relativeH * scale; // measure from blender

	float x_offset = relativeX * scale - rel_w / 2;  // blender location x
	float y_offset = relativeY * scale - rel_h / 2;  // blender lcoation y

	float x = center_x + x_offset * scale_window;
	float y = center_y + y_offset * scale_window;

	float w = rel_w * scale_window;
	float h = rel_h * scale_window;


	return juce::Rectangle<int>(x, y, w, h);
}

inline void OD808AudioProcessorEditor::timerCallback() {

	static float alpha = 0.8f;
	static float sum = 0.0f;
	const static int N = 2;
	static int index = 0;
	static float buff[N]{ 0.0f };

	float actual = audioProcessor.getLevel();

	sum -= buff[index];
	sum += actual;
	buff[index++] = actual;
	index %= N;

	actual = sum / N;

	if (actual > peak) {
		peak = actual;
	}
	else {
		peak *= alpha;
	}

	repaint();
}



inline void OD808AudioProcessorEditor::buttonClicked(juce::Button * btn) {

	if (btn == &bypassButton) {
		auto bypass = bypassButton.getToggleState();
		audioProcessor.setBypass(bypass);
	}
}

inline void OD808AudioProcessorEditor::sliderValueChanged(juce::Slider * slider) {

	if (slider == &driveSlider) {

		auto pos = driveSlider.getValue();
		//auto val = exp(log(pos) / 2.0f);
		auto val = pow(pos, 2);

		audioProcessor.setOverdrive(val);
	}
	else if (slider == &gainSlider) {
		audioProcessor.setBalance(gainSlider.getValue());
	}
	else if (slider == &toneSlider) {

		auto pos = toneSlider.getValue();

		float ym = 0.9;
		float b = pow((1.0 / ym - 1.0), 2);
		float a = 1.0 / (b - 1.0);
		float val = a * pow(b, pos) - a;

		audioProcessor.setTone(val);
	}
}

inline juce::Colour OD808AudioProcessorEditor::getPeakColor(float pk) {

	auto red{ juce::Colours::red };
	static float level = 0.0;

	if (audioProcessor.isBypassed()) {
		pk = 0.0;
		level *= 0.6;
	}
	else {
		level = 1.0 - 1.3 * pk;
	}

	return red.withBrightness(level);
}
