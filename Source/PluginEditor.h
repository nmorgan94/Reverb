#pragma once

#include "PluginProcessor.h"
#include "ui/CustomLookAndFeel.h"
#include "ui/SpectrumAnalyzer.h"
#include <BinaryData.h>

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void setupSlider(juce::Slider& slider, double initialValue);
    void onSliderValueChange();
    void drawSeparatorLine(juce::Graphics& g, const juce::Slider& leftSlider, const juce::Slider& rightSlider);
    
    juce::Rectangle<int> titleBounds;
    
    juce::Slider* activeSlider = nullptr;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    juce::Slider roomSizeSlider, dampingSlider, widthSlider, mixSlider, sendGainSlider, highpassFreqSlider;
    
    // Slider attachments to connect UI to parameters
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> roomSizeAttachment, dampingAttachment, widthAttachment, mixAttachment, sendGainAttachment, highpassFreqAttachment;
    
    CustomLookAndFeel customLookAndFeel;
    
    SpectrumAnalyzer spectrumAnalyzer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
