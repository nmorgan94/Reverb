#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    static constexpr juce::uint32 primaryRed = 0xffe94560;
    static constexpr juce::uint32 lightPink = 0xffff6b9d;
    static constexpr juce::uint32 darkBlue = 0xff0f3460;
    
    CustomLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colour(primaryRed));
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(primaryRed));
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(darkBlue).withAlpha(0.6f));
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
        g.setColour(juce::Colour(darkBlue).withAlpha(0.3f));
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
                juce::Colour(primaryRed),
                bounds.getCentreX(), bounds.getY(),
                juce::Colour(lightPink),
                bounds.getCentreX(), bounds.getBottom(),
                false);
            g.setGradientFill(gradient);
            g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        // Draw pointer/thumb
        juce::Path pointer;
        auto pointerLength = radius * 0.55f;
        auto pointerThickness = 4.0f;
        auto cornerRadius = pointerThickness * 0.5f;
        auto pointerOffset = radius * 0.2f;
        pointer.addRoundedRectangle(-pointerThickness * 0.5f, -radius + lineW + pointerOffset, pointerThickness, pointerLength, cornerRadius);
        pointer.applyTransform(juce::AffineTransform::rotation(toAngle).translated(bounds.getCentreX(), bounds.getCentreY()));

        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        g.fillPath(pointer);

        // Draw center circle with gradient
        auto centerCircleSize = radius * 0.18f;
        juce::ColourGradient centerGradient(
            juce::Colour(primaryRed),
            bounds.getCentreX(), bounds.getCentreY() - centerCircleSize,
            juce::Colour(lightPink),
            bounds.getCentreX(), bounds.getCentreY() + centerCircleSize,
            false);
        g.setGradientFill(centerGradient);
        g.fillEllipse(bounds.getCentreX() - centerCircleSize,
                     bounds.getCentreY() - centerCircleSize,
                     centerCircleSize * 2.0f,
                     centerCircleSize * 2.0f);
    }

    [[nodiscard]] static juce::FontOptions orbitronRegular()
    {
        static auto typeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::OrbitronRegular_ttf,
            BinaryData::OrbitronRegular_ttfSize);
        return juce::FontOptions(typeface);
    }
    
    [[nodiscard]] static juce::FontOptions orbitronBold()
    {
        static auto typeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::OrbitronBold_ttf,
            BinaryData::OrbitronBold_ttfSize);
        return juce::FontOptions(typeface);
    }
};
