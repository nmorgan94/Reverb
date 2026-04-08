#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <atomic>

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

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
    
    //==============================================================================
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    juce::AudioBuffer<float>& getAudioFifo() { return audioFifo; }
    juce::AbstractFifo& getAbstractFifo() { return abstractFifo; }
    std::atomic<bool>& getHasNewData() { return hasNewData; }

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout() };
    
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;
    
    // Highpass filter for reverb send - 24 dB/oct (two cascaded 12 dB/oct stages)
    using FilterType = juce::dsp::IIR::Filter<float>;
    using CoefficientType = juce::dsp::IIR::Coefficients<float>;
    juce::dsp::ProcessorDuplicator<FilterType, CoefficientType> highpassFilter1;
    juce::dsp::ProcessorDuplicator<FilterType, CoefficientType> highpassFilter2;
    
    void updateReverbParameters();
    void updateHighpassFilter();
    
    juce::AudioBuffer<float> audioFifo;      // Circular buffer for audio samples
    juce::AbstractFifo abstractFifo{48000};  // Lock-free FIFO (1 sec at 48kHz)
    std::atomic<bool> hasNewData{false};     // Signals UI thread that data is available
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
