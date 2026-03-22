#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), spectrumAnalyzer(p)
{
    juce::ignoreUnused (processorRef);
    
    // Set custom LookAndFeel globally for this editor
    setLookAndFeel(&customLookAndFeel);

    // Setup all sliders
    setupSlider(roomSizeSlider, 0.5);
    setupSlider(dampingSlider, 0.5);
    setupSlider(widthSlider, 1.0);
    setupSlider(wetLevelSlider, 0.33);
    
    addAndMakeVisible(spectrumAnalyzer);
    
    // Create attachments to connect sliders to parameters
    auto& apvts = processorRef.getAPVTS();
    roomSizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "roomSize", roomSizeSlider);
    dampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "damping", dampingSlider);
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "width", widthSlider);
    wetLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "wetLevel", wetLevelSlider);

    setSize(500, 250);
}

void AudioPluginAudioProcessorEditor::setupSlider(juce::Slider& slider, double initialValue)
{
    addAndMakeVisible(slider);
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setRange(0.0, 1.0, 0.01);
    slider.setValue(initialValue);
    
    // Add listeners to track when slider is being interacted with
    slider.onValueChange = [this]() { onSliderValueChange(); };
    slider.onDragStart = [this, &slider]() { activeSlider = &slider; repaint(); };
    slider.onDragEnd = [this]() { activeSlider = nullptr; repaint(); };
}

void AudioPluginAudioProcessorEditor::onSliderValueChange()
{
    if (activeSlider != nullptr)
        repaint();
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
        juce::Colour(0xff1a1a2e), bounds.getWidth() / 2, 0,
        juce::Colour(0xff16213e), bounds.getWidth() / 2, bounds.getHeight(),
        false);
    g.setGradientFill(gradient);
    g.fillAll();
    

    // Shadow
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.setFont(juce::FontOptions(28.0f).withStyle("Bold"));
    g.drawText("REVERB", titleBounds.translated(2, 2), juce::Justification::centred);
    
    // Main title
    g.setColour(juce::Colour(0xfff0f0f0));
    g.drawText("REVERB", titleBounds, juce::Justification::centred);
    
    // Show version label on hover
    if (isHoveringTitle)
    {
        auto versionArea = titleBounds.withTrimmedTop(titleBounds.getHeight() - 18);
        g.setFont(juce::FontOptions(11.0f));
        g.setColour(juce::Colour(0xffe94560).withAlpha(0.9f));
        g.drawText("v" + juce::String(JucePlugin_VersionString), versionArea, juce::Justification::centred);
    }
    
    
    // Draw labels below each slider
    g.setFont(juce::FontOptions(13.0f).withStyle("Bold"));
    g.setColour(juce::Colour(0xff0f3460).withAlpha(0.5f));
    
    auto drawLabel = [&](const juce::Slider& slider, const juce::String& text, const juce::String& valueText)
    {
        auto sliderBounds = slider.getBounds();
        auto labelY = sliderBounds.getBottom() + 5;
        auto labelRect = juce::Rectangle<float>(
            sliderBounds.getX(),
            labelY,
            sliderBounds.getWidth(),
            25.0f);
        
        g.fillRoundedRectangle(labelRect, 5.0f);
        
        g.setColour(juce::Colour(0xffe94560));
        if (&slider == activeSlider)
            g.drawText(valueText, labelRect.toNearestInt(), juce::Justification::centred);
        else
            g.drawText(text, labelRect.toNearestInt(), juce::Justification::centred);
        
        g.setColour(juce::Colour(0xff0f3460).withAlpha(0.5f));
    };
    
    // Draw all labels
    drawLabel(roomSizeSlider, "ROOM SIZE", juce::String(roomSizeSlider.getValue(), 2));
    drawLabel(dampingSlider, "DAMPING", juce::String(dampingSlider.getValue(), 2));
    drawLabel(widthSlider, "WIDTH", juce::String(widthSlider.getValue(), 2));
    drawLabel(wetLevelSlider, "WET LEVEL", juce::String(wetLevelSlider.getValue(), 2));
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    auto analyzerArea = juce::Rectangle<int>(0, 0, bounds.getWidth(), 100);
    spectrumAnalyzer.setBounds(analyzerArea);
    
    titleBounds = juce::Rectangle<int>(0, 0, bounds.getWidth(), 100);
    
    int sliderSize = 100;
    int labelHeight = 30;
    int sliderTotalHeight = sliderSize + labelHeight;
    auto sliderArea = juce::Rectangle<int>(0, 100, bounds.getWidth(), bounds.getHeight() - 100);
    
    // Layout sliders in a row within slider area
    int spacing = 20;
    int totalWidth = (sliderSize * 4) + (spacing * 3);
    int startX = (sliderArea.getWidth() - totalWidth) / 2;
    int startY = (sliderArea.getHeight() - sliderTotalHeight) / 2;
    
    roomSizeSlider.setBounds(startX, 100 + startY, sliderSize, sliderSize);
    dampingSlider.setBounds(startX + sliderSize + spacing, 100 + startY, sliderSize, sliderSize);
    widthSlider.setBounds(startX + (sliderSize + spacing) * 2, 100 + startY, sliderSize, sliderSize);
    wetLevelSlider.setBounds(startX + (sliderSize + spacing) * 3, 100 + startY, sliderSize, sliderSize);
}

void AudioPluginAudioProcessorEditor::mouseMove(const juce::MouseEvent& event)
{
    bool wasHovering = isHoveringTitle;
    isHoveringTitle = titleBounds.contains(event.getPosition());
    
    if (wasHovering != isHoveringTitle)
        repaint(titleBounds);
}

void AudioPluginAudioProcessorEditor::mouseExit(const juce::MouseEvent&)
{
    if (isHoveringTitle)
    {
        isHoveringTitle = false;
        repaint(titleBounds);
    }
}
