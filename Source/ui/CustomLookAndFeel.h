#pragma once

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::orange);
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::grey.withAlpha(0.5f));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = juce::jmin(8.0f, radius * 0.5f);
        auto arcRadius = radius - lineW * 0.5f;

        // Draw full range outline (min to max path)
        juce::Path backgroundArc;
        backgroundArc.addCentredArc(bounds.getCentreX(),
                                   bounds.getCentreY(),
                                   arcRadius,
                                   arcRadius,
                                   0.0f,
                                   rotaryStartAngle,
                                   rotaryEndAngle,
                                   true);

        g.setColour(slider.findColour(juce::Slider::rotarySliderOutlineColourId));
        g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Draw filled arc (current value)
        if (slider.isEnabled())
        {
            juce::Path valueArc;
            valueArc.addCentredArc(bounds.getCentreX(),
                                  bounds.getCentreY(),
                                  arcRadius,
                                  arcRadius,
                                  0.0f,
                                  rotaryStartAngle,
                                  toAngle,
                                  true);

            g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
            g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }


    }
};
