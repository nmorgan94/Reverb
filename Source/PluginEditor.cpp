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
    auto bounds = getLocalBounds().toFloat();
    juce::ColourGradient gradient(
        juce::Colour(0xff1a1a2e), bounds.getX(), bounds.getY(),
        juce::Colour(0xff16213e), bounds.getX(), bounds.getBottom(),
        false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    // Draw title with shadow effect
    auto titleArea = getLocalBounds().removeFromTop(60);
    
    // Shadow
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.setFont(juce::FontOptions(28.0f).withStyle("Bold"));
    g.drawText("REVERB", titleArea.translated(2, 2), juce::Justification::centred);
    
    // Main title
    g.setColour(juce::Colour(0xfff0f0f0));
    g.drawText("REVERB", titleArea, juce::Justification::centred);
    
    
    // Draw labels
    g.setFont(juce::FontOptions(13.0f).withStyle("Bold"));
    
    int sliderSize = 100;
    int spacing = 20;
    int totalWidth = (sliderSize * 4) + (spacing * 3);
    int startX = (getWidth() - totalWidth) / 2;
    
    // Calculate label Y position based on slider position
    auto labelBounds = getLocalBounds();
    labelBounds.removeFromTop(60);
    labelBounds.removeFromBottom(35);
    int sliderY = (labelBounds.getHeight() - sliderSize) / 2;
    int labelY = 5 + sliderY + sliderSize; 
    
    // Draw rounded rectangles behind labels
    g.setColour(juce::Colour(0xff0f3460).withAlpha(0.5f));
    for (int i = 0; i < 4; ++i)
    {
        auto labelRect = juce::Rectangle<float>(
            startX + (sliderSize + spacing) * i,
            labelY,
            sliderSize,
            25);
        g.fillRoundedRectangle(labelRect, 5.0f);
    }
    
    // Draw label text
    g.setColour(juce::Colour(0xffe94560));
    g.drawText("ROOM SIZE", startX, labelY, sliderSize, 25, juce::Justification::centred);
    g.drawText("DAMPING", startX + sliderSize + spacing, labelY, sliderSize, 25, juce::Justification::centred);
    g.drawText("WIDTH", startX + (sliderSize + spacing) * 2, labelY, sliderSize, 25, juce::Justification::centred);
    g.drawText("WET LEVEL", startX + (sliderSize + spacing) * 3, labelY, sliderSize, 25, juce::Justification::centred);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(60); 
    bounds.removeFromBottom(35); 
    
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
