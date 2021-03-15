/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <memory>

using FilterMono = juce::dsp::IIR::Filter<float>;
using Coeff = juce::dsp::IIR::Coefficients<float>;

using Filter = juce::dsp::ProcessorDuplicator<FilterMono, Coeff>;

template <typename Type>
class InputBuffer {
public:

	InputBuffer() = default;
	~InputBuffer() = default;

	template <typename Context>
	void process(const Context& context) noexcept {

		for (auto& f : filters)
			f->process(context);
	}

	void prepare(const juce::dsp::ProcessSpec& spec) {

		auto& coefs =  juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(cutoff,  spec.sampleRate, order);
	
		for (auto& c : coefs) {

			RefFilter f{ new Filter() };

			*f->state = *c;

			f->prepare(spec);
			f->reset();
			filters.add(f);
		}
	}

private:
	using RefFilter = std::shared_ptr<Filter>;

	juce::Array<RefFilter> filters;

	const float cutoff = 20.0f;
	const int order = 3;
};


template <typename Type>
class Clipper {

public:
	Clipper() {
		oversampling = new juce::dsp::Oversampling<Type>(2, oversampling_factor, juce::dsp::Oversampling<Type>::FilterType::filterHalfBandPolyphaseIIR);
	}

	~Clipper() {
		delete oversampling;
	}

	void prepare(const juce::dsp::ProcessSpec& spec) {
		
		fs = spec.sampleRate * pow(2, oversampling_factor);

		oversampling->initProcessing(spec.maximumBlockSize);
		oversampling->reset();

		gc1 = 47e-9 * fs * minv;
		gc2 = 50e-12 * fs * minv;

		ic1eq = 0;
		ic2eq = 0;
	}

	template <typename ProcessContext>
	void process(const ProcessContext& context) noexcept {

		auto&& inBlock = context.getInputBlock();
		auto&& outBlock = context.getOutputBlock();
		auto bypassed = context.isBypassed;

		auto buff = oversampling->processSamplesUp(inBlock);
		auto* ioL = buff.getChannelPointer(0);
		auto* ioR = buff.getChannelPointer(1);

		auto len = buff.getNumSamples();
		covGood = false;

		gr2 = 1.0 / (distortion * 500e3 + 50e3);

		for (size_t i = 0; i < len; ++i) {

			for (int j = 0; j < max_iter; ++j) {

				//jassert(j < 35);

				v1 = ioL[i];
				float vd1 = v3 - v1;
				float ed1 = exp(vd1 / vt1);
				float id1 = is1 * ed1 - is1;
				float gd1 = is1 * ed1 / vt1;
				float id1eq = id1 - gd1 * vd1;

				float vd2 = v1 - v3;
				float ed2 = exp(vd2 / vt2);
				float id2 = is2 * ed2 - is2;
				float gd2 = is2 * ed2 / vt2;
				float id2eq = id2 - gd2 * vd2;

				v2 = (gc1 * v1 + ic1eq) / (gr1 + gc1);
				v3 = (v1 * (gr2 + gc2 + gd1 + gd2) + gr1 * v2 - ic2eq - id1eq + id2eq) / (gr2 + gc2 + gd1 + gd2);

				if (abs(vlast2 - v2) <= cov && abs(vlast3 - v3) <= cov) {
					covGood = true;
					break;
				}

				vlast2 = v2;
				vlast3 = v3;
			}

			ic1eq += minv * (gc1 * (v2 - v1) - ic1eq);
			ic2eq += minv * (gc2 * (v1 - v3) - ic2eq);

			ioL[i] = bypassed ? ioL[i] : v3;
			ioR[i] = bypassed ? ioL[i] : v3;
		}

		if (!covGood)
			fail = true;

		oversampling->processSamplesDown(outBlock);
	}

	void setDistortion(float val) { distortion = pow(val, 2); }
	float getDistortion() { return distortion; }

private:

	double fs;
	bool covGood = false;
	bool fail = false;

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
	float cov = 0.0001;

	float v1 = 0;
	float v2 = 0;
	float v3 = 0;

	float vlast2 = 0;
	float vlast3 = 0;

	juce::dsp::Oversampling<Type>* oversampling;

};

template <typename Type>
class Tone {
public:
	Tone() = default;
	~Tone() = default;

	template <typename Context>
	void process(const Context& context) noexcept {

		filter.process(context);
	}

	void prepare(const juce::dsp::ProcessSpec& spec) {
		
		fs = spec.sampleRate;
		filter.prepare(spec);

		update();
		filter.reset();
	}

	void setTone(float val) { 
		
		if (val != tone) {

			tone = val;
			update();
			//filter.reset();
		}
	}

	float getTone() { return tone; }

private:

	Coeff::Ptr bilinear(double sb1, double sb0, double sa2, double sa1, double sa0 , double fs) {
	
		double c = 2 * fs;
		double cpow = pow(c, 2);

		double az0 = sa0 + sa1 * c + sa2 * cpow;

		double az2 = sa0 - sa1 * c + sa2 * cpow;
		double az1 = 2.0 * sa0 - 2.0 * sa2 * cpow;

		double bz2 = sb0 - sb1 * c;
		double bz1 = 2.0 * sb0;
		double bz0 = sb0 + sb1 * c;

		az2 /= az0;
		az1 /= az0;

		bz2 /= az0;
		bz1 /= az0;
		bz0 /= az0;

		az0 = 1.0;

		return *new Coeff(bz0, bz1, bz2, az0, az1, az2);
	}

	void update() {
		
		double Ta = Tbase * tone;
		double Tb = Tbase - Ta;
	
		// S domain transfer functin coefficiens
		double sb1 = C2 * Ta * Tb + C2 * R4 * Ta + C2 * R4 * Tb + C2 * R1 * Ta;
		double sb0 = Ta + Tb;

		double sa2 = C1 * C2 * R2 * Ta * Tb + C1 * C2 * R2 * R4 * Ta + C1 * C2 * R2 * R4 * Tb;
		double sa1 = C2 * Ta * Tb + C2 * R4 * Ta + C2 * R4 * Tb + C1 * R2 * Ta + C1 * R2 * Tb + C2 * R2 * Tb;
		double sa0 = Ta + Tb;
	
		Coeff::Ptr zCoeffs = bilinear(sb1, sb0 , sa2, sa1, sa0, fs);

		*filter.state = *zCoeffs;
	}

	double R1{ 1.0e3 };
	double R2{ 1.0e3 };
	double R3{ 10.0e3 };
	double R4{ 220.0 };

	double C1{ 0.22e-6 };
	double C2{ 0.22e-6 };

	double Tbase{ 20.0e3 };
	double fs;
	double tone{ 0.5 };

	Filter filter;

};

template <typename Type>
class OutBuffer {
public:
	OutBuffer() = default;
	~OutBuffer() = default;

	template <typename Context>
	void process(const Context& context) noexcept {

		processorChain.process(context);
	}

	void prepare(const juce::dsp::ProcessSpec& spec) {

		auto& filter = processorChain.get<filterId>();
		*filter.state = *Coeff::makeHighPass(spec.sampleRate, 20);

		processorChain.prepare(spec);
		processorChain.reset();
	}

	void setGain(float val) {
		auto& gain = processorChain.get<gainId>();
		gain.setGainLinear(val);
	}

	float getGain() { return processorChain.get<gainId>().getGainLinear(); }

private:

	enum {
		filterId,
		gainId
	};

	juce::dsp::ProcessorChain<
		Filter,
		juce::dsp::Gain<float>

	> processorChain;
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

	void setOverdrive(double val) { 
		processorChain.get<driveId>().setDistortion(static_cast<float>(val)); 
		//distortion = val;
	}

	void setBalance(double val) { 
		// fixme : do it by value tree and atomic values -> to safe threaded
		processorChain.get<outBufferId>().setGain(static_cast<float>(val)); 
		//gain = val;
	}
	void setTone(double val) { 
		processorChain.get<toneId>().setTone(val);
		//tone = val;
	}

	float getLevel() { return avg_out_val; };
	bool isBypassed() { return bypass; }
	void setBypass(bool bp) { 

		bypass = bp; 

		processorChain.setBypassed<inBuffId>(bypass);
		processorChain.setBypassed<driveId>(bypass);
		processorChain.setBypassed<toneId>(bypass);
		processorChain.setBypassed<outBufferId>(bypass);
		processorChain.setBypassed<trimGainId>(bypass);

	}

private:

	enum {
		inBuffId,
		driveId,
		toneId,
		outBufferId,
		trimGainId
	};

	juce::dsp::ProcessorChain<
		InputBuffer<float>,
		Clipper<float>,
		Tone<float>,
		OutBuffer<float>,
		juce::dsp::Gain<float>
	> processorChain;

	float avg_out_val = 0.0f;

	//float distortion = 0.5;
	//float tone = 0.5;
	//float gain = 1.0;

	bool bypass = false;

	juce::AudioProcessorValueTreeState parameters;

	std::atomic<float>* distortion = nullptr;
	std::atomic<float>* tone = nullptr;
	std::atomic<float>* gain = nullptr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OD808AudioProcessor)
};
