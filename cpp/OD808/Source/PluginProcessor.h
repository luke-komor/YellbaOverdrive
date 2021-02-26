/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


template <typename Type>
class Clipper {

public:
	Clipper() {
		oversampling = new juce::dsp::Oversampling<Type>(2, oversampling_factor, juce::dsp::Oversampling<Type>::FilterType::filterHalfBandPolyphaseIIR);
	}

	template <typename ProcessContext>
	void process(const ProcessContext& context) noexcept {

		auto&& inBlock = context.getInputBlock();
		auto&& outBlock = context.getOutputBlock();
	
		jassert(inBlock.getNumChannels() == outBlock.getNumChannels());
		jassert(inBlock.getNumSamples() == outBlock.getNumSamples());

		auto buff = oversampling->processSamplesUp(inBlock);

		auto* ioL = buff.getChannelPointer(0);
		auto* ioR = buff.getChannelPointer(1);

		auto len = buff.getNumSamples();
		static  double min = 0;
		static double max = 0;

		for (size_t i = 0; i < len; ++i) {

			double ibuff = ioL[i];

			min = juce::jmin(min, ibuff);
			max = juce::jmax(max, ibuff);

			ioL[i] = Vn + ibuff;
			ioR[i] = ioL[i];

			//Inn = In + h * dIdt(In, ibuff, last_in, h);
			In = ibuff / 4.7e3;

			Vnn = Vn + 0.5 * h * (dVdt(Vn, In, distortion) + dVdt(Vnn, In, distortion));
			jassert(!isnan(Vnn));
			jassert(!isinf(Vnn));

			Vn = Vnn;
			
			Inn = In;
			//last_in = ibuff;
		}
			
		oversampling->processSamplesDown(outBlock);
	}

	template <typename ProcessContext>
	void process2(const ProcessContext& context) noexcept {
		auto&& inBlock = context.getInputBlock();
		auto&& outBlock = context.getOutputBlock();

		auto buff = oversampling->processSamplesUp(inBlock);
		auto* ioL = buff.getChannelPointer(0);
		auto* ioR = buff.getChannelPointer(1);

		auto len = buff.getNumSamples();

		for (size_t i = 0; i < len; ++i) {

			for (int j = 0; j < max_iter; ++j) {
				float v1 = ioL[i];
				float vd1 = 0 - v3;
				float ed1 = exp(vd1 / vt1);
				float id1 = is1 * ed1 - is1;
				float gd1 = is1 * ed1 / vt1;
				float id1eq = id1 - gd1 * vd1;

				float vd2 = v3 - 0;
				float ed2 = exp(vd2 / vt2);
				float id2 = is2 * ed2 - is2;
				float gd2 = is2 * ed2 / vt2;
				float id2eq = id2 - gd2 * vd2;

				v2 = (gr1 * v1 + gc1 * v3 - ic1eq) / (gc1 + gr1);
				v3 = (gc1*v2 + ic1eq + ic2eq + id1eq - id2eq) / (gc1 + gc2 + gd1 + gd2 + gr2);

				if (abs(vlast2 - v2) <= cov && abs(vlast3 - v3) <= cov){
					break;
				}

				vlast2 = v2;
				vlast3 = v3;

			}

			ic1eq += minv * (gc1 * (v3 - v2) - ic1eq);
			ic2eq += minv * (gc2 * (v3 - 0) - ic2eq);

			ioL[i] = v3;
			ioR[i] = v3;
		}

		oversampling->processSamplesDown(outBlock);
	}

	~Clipper() {
		delete oversampling;
	}

	void prepare(const juce::dsp::ProcessSpec& spec) {
		fs = spec.sampleRate * pow(2, oversampling_factor);
		h = 1 / fs;
	//	tempBlock = juce::dsp::AudioBlock<float>(heapBlock, spec.numChannels, spec.maximumBlockSize * pow(2, oversampling_factor));
		oversampling->initProcessing(spec.maximumBlockSize);
		oversampling->reset();

		Is = 1e-12;
		is1 = Is;
		is2 = Is;

		Vt = 26e-3;
		vt1 = Vt;
		vt2 = Vt;

		gr1 = 1 / 2.2e3;
		gr2 = 1 / 6.8e3;

		m = 0.5;
		minv = 1 / m;

		gc1 = 0.47e-6 * fs * minv;
		gc2 = 0.01e-6 * fs * minv;

		ic1eq = 0;
		ic2eq = 0;

	}

	void setDistortion(float val) { distortion = val; }

	double dVdt(double V, double In, double D) {
		double Cc = 51e-12;
		double R2 = 51e3 + D * 500e3;

		double Is = 1e-12;
		double Vt = 26e-3;

		double dV = In/Cc - V / (Cc * R2) - 2.0 * Is * sinh(V / Vt) / Cc;
		
		//jassert(abs(dV) < 100.0f);
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

	double Vn = 0;
	double Vnn = 0;

	double In = 0;
	double Inn;

	float last_in = 0;
	float distortion = 1.0f;
	const int oversampling_factor = 4;

	float Is = 1e-12;
	float is1 = Is;
	float is2 = Is;

	float Vt = 26e-3;
	float vt1 = Vt;
	float vt2 = Vt;

	float gr1 = 1 / 2.2e3;
	float gr2 = 1 / 6.8e3;

	float m = 0.5;
	float minv = 1 / m;

	float gc1 = 0.47e-6 * fs * minv;
	float gc2 = 0.01e-6 * fs * minv;

	float ic1eq = 0;
	float ic2eq = 0;

	int max_iter = 400;
	float cov = 0.001;

	float v2 = 0;
	float v3 = 0;

	float vlast2 = 0;
	float vlast3 = 0;

	juce::HeapBlock<char> heapBlock;
	juce::dsp::AudioBlock<float> tempBlock;
	juce::dsp::Oversampling<Type>* oversampling;

};

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

	void setOverdrive(float val) { processorChain.get<driveId>().setDistortion(val); }
	void setBalance(float val) { processorChain.get<balanceId>().setGainLinear(val); }
	void setTone(float val) {}

	float getLevel() { return avg_out_val; };

private:

	enum {
		driveId,
		balanceId, 
		toneId
	};

	juce::dsp::ProcessorChain<
		Clipper<float>,
		juce::dsp::Gain<float>
	> processorChain;

	float avg_out_val = 0.0f;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OD808AudioProcessor)
};
