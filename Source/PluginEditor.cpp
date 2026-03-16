#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    
    // Set custom LookAndFeel globally for this editor
    setLookAndFeel(&customLookAndFeel);
    
    // Setup all sliders
    setupSlider(roomSizeSlider, 0.5);
    setupSlider(dampingSlider, 0.5);
    setupSlider(widthSlider, 1.0);
    setupSlider(wetLevelSlider, 0.33);
    
    // Create attachments to connect sliders to parameters
    auto& apvts = processorRef.getAPVTS();
    roomSizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "roomSize", roomSizeSlider);
    dampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "damping", dampingSlider);
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "width", widthSlider);
    wetLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "wetLevel", wetLevelSlider);

    setSize(500, 300);
}

void AudioPluginAudioProcessorEditor::setupSlider(juce::Slider& slider, double initialValue)
{
    addAndMakeVisible(slider);
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setRange(0.0, 1.0, 0.01);
    slider.setValue(initialValue);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    // Remove global LookAndFeel
    setLookAndFeel(nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fill background
    g.fillAll (juce::Colours::darkgrey);

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions(20.0f).withStyle("Bold"));
    g.drawText ("Reverb Plugin", getLocalBounds().removeFromTop(40), juce::Justification::centred);
    
    // Draw labels below each slider
    g.setFont (juce::FontOptions(14.0f));
    auto bounds = getLocalBounds();
    bounds.removeFromTop(60);
    
    int sliderSize = 100;
    int spacing = 20;
    int totalWidth = (sliderSize * 4) + (spacing * 3);
    int startX = (bounds.getWidth() - totalWidth) / 2;
    int labelY = bounds.getHeight() - 40;
    
    g.drawText ("Room Size", startX, labelY, sliderSize, 20, juce::Justification::centred);
    g.drawText ("Damping", startX + sliderSize + spacing, labelY, sliderSize, 20, juce::Justification::centred);
    g.drawText ("Width", startX + (sliderSize + spacing) * 2, labelY, sliderSize, 20, juce::Justification::centred);
    g.drawText ("Wet Level", startX + (sliderSize + spacing) * 3, labelY, sliderSize, 20, juce::Justification::centred);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(60); // Space for title
    bounds.removeFromBottom(50); // Space for labels
    
    // Layout sliders in a row
    int sliderSize = 100;
    int spacing = 20;
    int totalWidth = (sliderSize * 4) + (spacing * 3);
    int startX = (bounds.getWidth() - totalWidth) / 2;
    int startY = (bounds.getHeight() - sliderSize) / 2;
    
    roomSizeSlider.setBounds(startX, startY, sliderSize, sliderSize);
    dampingSlider.setBounds(startX + sliderSize + spacing, startY, sliderSize, sliderSize);
    widthSlider.setBounds(startX + (sliderSize + spacing) * 2, startY, sliderSize, sliderSize);
    wetLevelSlider.setBounds(startX + (sliderSize + spacing) * 3, startY, sliderSize, sliderSize);
}
