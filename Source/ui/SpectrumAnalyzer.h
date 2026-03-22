#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

// Forward declaration to avoid circular dependency
class AudioPluginAudioProcessor;

//==============================================================================
class SpectrumAnalyzer : public juce::Component, private juce::Timer
{
public:
    explicit SpectrumAnalyzer(AudioPluginAudioProcessor& proc);
    
    void paint(juce::Graphics& g) override;
    
private:
    // Timer callback runs on UI thread at 30Hz
    void timerCallback() override;
    
    // Process samples from FIFO into FFT buffer
    void pushSamplesIntoFifo(const float* samples, int numSamples);
    
    // Perform FFT and update display data
    void drawNextFrameOfSpectrum();
    
    // Draw frequency markers on the display
    void drawFrequencyMarkers(juce::Graphics& g, float width, float height, bool shouldDraw);
    
    // Reference to processor for accessing FIFO
    AudioPluginAudioProcessor& processor;
    
    // FFT configuration
    static constexpr auto fftOrder = 11;           // 2^11 = 2048 samples
    static constexpr auto fftSize = 1 << fftOrder; // 2048
    static constexpr auto scopeSize = 512;        // Number of display points
    
    // Frequency range constants
    static constexpr float minFreq = 20.0f;        // 20 Hz
    static constexpr float maxFreq = 20000.0f;     // 20 kHz
    
    juce::dsp::FFT forwardFFT{fftOrder};
    
    // FFT data buffer (needs to be 2x size for complex numbers)
    std::array<float, fftSize * 2> fftData;
    
    // Input FIFO for collecting samples
    std::array<float, fftSize> fifo;
    int fifoIndex = 0;
    
    // Display data (normalized 0-1)
    std::array<float, scopeSize> scopeData;
    
    // Flag indicating FFT is ready to process
    bool nextFFTBlockReady = false;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumAnalyzer)
};
