#pragma once

#include "PluginProcessor.h"
#include "ui/CustomLookAndFeel.h"
// #include <BinaryData.h> add BinaryData here

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void mouseMove(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

private:
    void setupSlider(juce::Slider& slider, double initialValue);
    void onSliderValueChange();
    
    bool isHoveringTitle = false;
    juce::Rectangle<int> titleBounds;
    
    juce::Slider* activeSlider = nullptr;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    juce::Slider roomSizeSlider, dampingSlider, widthSlider, wetLevelSlider;
    
    // Slider attachments to connect UI to parameters
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> roomSizeAttachment, dampingAttachment, widthAttachment, wetLevelAttachment;
    
    CustomLookAndFeel customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
