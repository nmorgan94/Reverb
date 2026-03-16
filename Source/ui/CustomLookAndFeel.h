#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colour(0xffe94560));
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffe94560));
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff0f3460).withAlpha(0.6f));
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

        // Draw outer glow effect
        g.setColour(juce::Colour(0xff0f3460).withAlpha(0.3f));
        g.fillEllipse(bounds.reduced(-5));

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

        // Draw filled arc (current value) with gradient
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

            // Gradient for the value arc
            juce::ColourGradient gradient(
                juce::Colour(0xffe94560),
                bounds.getCentreX(), bounds.getY(),
                juce::Colour(0xffff6b9d),
                bounds.getCentreX(), bounds.getBottom(),
                false);
            g.setGradientFill(gradient);
            g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        // Draw pointer/thumb
        juce::Path pointer;
        auto pointerLength = radius * 0.4f;
        auto pointerThickness = 4.0f;
        pointer.addRectangle(-pointerThickness * 0.5f, -radius + lineW, pointerThickness, pointerLength);
        pointer.applyTransform(juce::AffineTransform::rotation(toAngle).translated(bounds.getCentreX(), bounds.getCentreY()));

        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        g.fillPath(pointer);

        // Draw center circle with gradient
        auto centerCircleSize = radius * 0.25f;
        juce::ColourGradient centerGradient(
            juce::Colour(0xff0f3460),
            bounds.getCentreX(), bounds.getCentreY() - centerCircleSize,
            juce::Colour(0xff16213e),
            bounds.getCentreX(), bounds.getCentreY() + centerCircleSize,
            false);
        g.setGradientFill(centerGradient);
        g.fillEllipse(bounds.getCentreX() - centerCircleSize,
                     bounds.getCentreY() - centerCircleSize,
                     centerCircleSize * 2.0f,
                     centerCircleSize * 2.0f);
    }
};
