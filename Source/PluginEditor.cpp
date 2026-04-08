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
    setupSlider(mixSlider, 0.33);
    setupSlider(sendGainSlider, 1.0);
    setupSlider(highpassFreqSlider, 100.0);
    
    addAndMakeVisible(spectrumAnalyzer);
    
    // Create attachments to connect sliders to parameters
    auto& apvts = processorRef.getAPVTS();
    roomSizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "roomSize", roomSizeSlider);
    dampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "damping", dampingSlider);
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "width", widthSlider);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "mix", mixSlider);
    sendGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "sendGain", sendGainSlider);
    highpassFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "highpassFreq", highpassFreqSlider);

    setSize(700, 250);
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

    g.setColour(juce::Colour(0xfff0f0f0));
    juce::AttributedString titleText;
    titleText.setJustification(juce::Justification::centred);
    titleText.append("SampleRealm: ", CustomLookAndFeel::orbitronBold().withPointHeight(28.0f), juce::Colour(0xfff0f0f0));
    titleText.append("REVERB", CustomLookAndFeel::orbitronRegular().withPointHeight(28.0f), juce::Colour(0xfff0f0f0));
    titleText.draw(g, titleBounds.toFloat());
    
    // Draw version number small in bottom right of title area
    g.setFont(CustomLookAndFeel::orbitronRegular().withPointHeight(9.0f));
    g.setColour(juce::Colour(0xffe94560).withAlpha(0.8f));
    auto versionArea = juce::Rectangle<int>(titleBounds.getRight() - 50, titleBounds.getBottom() - 12, 50, 12);
    g.drawText("v" + juce::String(JucePlugin_VersionString), versionArea, juce::Justification::centredRight);
    
    
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
    drawLabel(mixSlider, "MIX", juce::String(mixSlider.getValue(), 2));
    drawLabel(sendGainSlider, "SEND GAIN", juce::String(sendGainSlider.getValue(), 2));
    drawLabel(highpassFreqSlider, "HP FREQ", juce::String(highpassFreqSlider.getValue(), 0) + " Hz");
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
    int spacing = 15;
    int totalWidth = (sliderSize * 6) + (spacing * 5);
    int startX = (sliderArea.getWidth() - totalWidth) / 2;
    int startY = (sliderArea.getHeight() - sliderTotalHeight) / 2;
    
    highpassFreqSlider.setBounds(startX, 100 + startY, sliderSize, sliderSize);
    sendGainSlider.setBounds(startX + (sliderSize + spacing), 100 + startY, sliderSize, sliderSize);
    roomSizeSlider.setBounds(startX + (sliderSize + spacing) * 2, 100 + startY, sliderSize, sliderSize);
    dampingSlider.setBounds(startX + (sliderSize + spacing) * 3, 100 + startY, sliderSize, sliderSize);
    widthSlider.setBounds(startX + (sliderSize + spacing) * 4, 100 + startY, sliderSize, sliderSize);
    mixSlider.setBounds(startX + (sliderSize + spacing) * 5, 100 + startY, sliderSize, sliderSize);
}
