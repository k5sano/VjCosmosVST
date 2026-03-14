#pragma once
#include <juce_dsp/juce_dsp.h>
#include <atomic>
#include <vector>

class AudioAnalyzer
{
public:
    AudioAnalyzer();

    void prepare(double sampleRate, int samplesPerBlock);
    void processBlock(const float* inputData, int numSamples);

    // Thread-safe reads (GUI thread)
    float getBass()     const { return bass.load(std::memory_order_relaxed); }
    float getMid()      const { return mid.load(std::memory_order_relaxed); }
    float getHigh()     const { return high.load(std::memory_order_relaxed); }
    float getRms()      const { return rms.load(std::memory_order_relaxed); }
    float getCentroid() const { return centroid.load(std::memory_order_relaxed); }
    float getFlux()     const { return flux.load(std::memory_order_relaxed); }
    bool  getOnset()    const { return onset.load(std::memory_order_relaxed); }

private:
    static constexpr int fftOrder = 10;  // 2^10 = 1024
    static constexpr int fftSize  = 1 << fftOrder;

    juce::dsp::FFT forwardFFT { fftOrder };
    juce::dsp::WindowingFunction<float> window { (size_t)fftSize, juce::dsp::WindowingFunction<float>::hann };

    std::array<float, fftSize * 2> fftData {};
    std::array<float, fftSize>     fifo {};
    std::array<float, fftSize / 2> prevMagnitudes {};
    int fifoIndex = 0;

    double currentSampleRate = 44100.0;

    // Smoothed output values (written on audio thread)
    std::atomic<float> bass     { 0.0f };
    std::atomic<float> mid      { 0.0f };
    std::atomic<float> high     { 0.0f };
    std::atomic<float> rms      { 0.0f };
    std::atomic<float> centroid { 0.0f };
    std::atomic<float> flux     { 0.0f };
    std::atomic<bool>  onset    { false };

    // Internal smoothed values
    float sBass = 0, sMid = 0, sHigh = 0, sRms = 0, sCentroid = 0, sFlux = 0;

    void performFFTAnalysis();
    static float asymSmooth(float prev, float raw, float attack, float release);
};
