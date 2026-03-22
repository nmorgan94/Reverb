#include "SpectrumAnalyzer.h"
#include "../PluginProcessor.h"

//==============================================================================
SpectrumAnalyzer::SpectrumAnalyzer(AudioPluginAudioProcessor& proc)
    : processor(proc)
{
    // Make component transparent
    setOpaque(false);
    
    // Initialize arrays to zero
    std::fill(fftData.begin(), fftData.end(), 0.0f);
    std::fill(fifo.begin(), fifo.end(), 0.0f);
    std::fill(scopeData.begin(), scopeData.end(), 0.0f);
    
    startTimerHz(30);
}

//==============================================================================
void SpectrumAnalyzer::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto width = bounds.getWidth();
    auto height = bounds.getHeight();
    
    // Draw frequency markers
    drawFrequencyMarkers(g, width, height, false);
    
    // Create spectrum path
    juce::Path spectrumPath;
    spectrumPath.startNewSubPath(0, height);
    
    for (int i = 0; i < scopeSize; ++i)
    {
        auto x = juce::jmap(static_cast<float>(i), 0.0f, static_cast<float>(scopeSize - 1), 0.0f, width);
        auto y = juce::jmap(scopeData[static_cast<size_t>(i)], 0.0f, 1.0f, height, 0.0f);
        spectrumPath.lineTo(x, y);
    }
    
    // Draw the spectrum line
    g.setColour(juce::Colour(0xffe94560));
    g.strokePath(spectrumPath, juce::PathStrokeType(2.0f));
    
    // Draw filled area with gradient
    spectrumPath.lineTo(width, height);
    spectrumPath.closeSubPath();
    
    juce::ColourGradient gradient(
        juce::Colour(0xffe94560).withAlpha(0.4f), width / 2, 0,
        juce::Colour(0xffe94560).withAlpha(0.1f), width / 2, height,
        false);
    g.setGradientFill(gradient);
    g.fillPath(spectrumPath);
}

void SpectrumAnalyzer::drawFrequencyMarkers(juce::Graphics& g, float width, float height, bool shouldDraw)
{
    g.setFont(9.0f);
    g.setColour(juce::Colour(0xffe94560).withAlpha(0.5f));
    
    // Lambda to draw a single frequency marker
    auto drawMarker = [&](float freq, const juce::String& label)
    {
        if (freq < minFreq || freq > maxFreq) return;
        
        auto freqProportion = std::log(freq / minFreq) / std::log(maxFreq / minFreq);
        auto x = freqProportion * width;
        
        if (x >= 0 && x <= width)
        {
            g.drawLine(x, 0, x, height, 1.0f);
            g.drawText(label, static_cast<int>(x - 15), 2, 30, 12, juce::Justification::centred);
        }
    };
    
    // Draw standard frequency markers
    if(shouldDraw) {
        drawMarker(20.0f, "20");
        drawMarker(50.0f, "50");
        drawMarker(100.0f, "100");
        drawMarker(200.0f, "200");
        drawMarker(500.0f, "500");
        drawMarker(1000.0f, "1k");
        drawMarker(2000.0f, "2k");
        drawMarker(5000.0f, "5k");
        drawMarker(10000.0f, "10k");
        drawMarker(20000.0f, "20k");
    }
}

//==============================================================================
void SpectrumAnalyzer::timerCallback()
{
    // Check if processor has new audio data available
    if (processor.getHasNewData().load(std::memory_order_acquire))
    {
        auto& abstractFifo = processor.getAbstractFifo();
        auto& audioFifo = processor.getAudioFifo();
        
        // Read available samples from the lock-free FIFO
        int start1, size1, start2, size2;
        abstractFifo.prepareToRead(abstractFifo.getNumReady(), start1, size1, start2, size2);
        
        // Handle both regions (wraparound in circular buffer)
        if (size1 > 0) pushSamplesIntoFifo(audioFifo.getReadPointer(0, start1), size1);
        if (size2 > 0) pushSamplesIntoFifo(audioFifo.getReadPointer(0, start2), size2);
        
        // Mark samples as read
        abstractFifo.finishedRead(size1 + size2);
        processor.getHasNewData().store(false, std::memory_order_release);
    }
    
    // Process and display FFT if ready
    if (nextFFTBlockReady)
    {
        drawNextFrameOfSpectrum();
        nextFFTBlockReady = false;
        repaint();
    }
}

void SpectrumAnalyzer::pushSamplesIntoFifo(const float* samples, int numSamples)
{
    // Copy samples into our local FIFO buffer
    for (int i = 0; i < numSamples; ++i)
    {
        fifo[static_cast<size_t>(fifoIndex)] = samples[i];
        fifoIndex++;
        
        // When we have enough samples for FFT, prepare for processing
        if (fifoIndex == fftSize)
        {
            // Copy to FFT buffer (zero-pad the second half for complex FFT)
            std::fill(fftData.begin(), fftData.end(), 0.0f);
            std::copy(fifo.begin(), fifo.end(), fftData.begin());
            
            nextFFTBlockReady = true;
            fifoIndex = 0;
        }
    }
}

void SpectrumAnalyzer::drawNextFrameOfSpectrum()
{
    // Perform FFT on the collected samples
    forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());
    
    // Find the range of values for normalization
    auto maxLevel = juce::FloatVectorOperations::findMinAndMax(fftData.data(), fftSize / 2);
    const float maxMagnitude = juce::jmax(maxLevel.getEnd(), 1e-5f);
    
    // Get actual sample rate from processor
    const float sampleRate = static_cast<float>(processor.getSampleRate());
    
    // Map each display point to FFT data with interpolation
    for (auto i = 0; i < scopeSize; ++i)
    {
        auto proportion = static_cast<float>(i) / static_cast<float>(scopeSize - 1);
        
        // Logarithmic frequency mapping
        auto freq = minFreq * std::pow(maxFreq / minFreq, proportion);
        
        // Convert frequency to FFT bin (as float for interpolation)
        auto binFloat = (freq / (sampleRate / 2.0f)) * (fftSize / 2);
        auto binIndex = static_cast<int>(binFloat);
        auto binFraction = binFloat - static_cast<float>(binIndex);
        
        // Clamp to valid range
        binIndex = juce::jlimit(0, fftSize / 2 - 2, binIndex);
        
        // Linear interpolation between adjacent bins
        auto mag1 = fftData[static_cast<size_t>(binIndex)];
        auto mag2 = fftData[static_cast<size_t>(binIndex + 1)];
        auto interpolatedMag = mag1 + binFraction * (mag2 - mag1);
        
        // Normalize to 0-1 range
        scopeData[static_cast<size_t>(i)] = juce::jmap(interpolatedMag, 0.0f, maxMagnitude, 0.0f, 1.0f);
    }
}
