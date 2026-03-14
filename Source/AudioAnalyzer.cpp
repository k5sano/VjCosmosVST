#include "AudioAnalyzer.h"
#include <cmath>
#include <algorithm>
#include <numeric>

AudioAnalyzer::AudioAnalyzer()
{
    prevMagnitudes.fill(0.0f);
}

void AudioAnalyzer::prepare(double sampleRate, int /*samplesPerBlock*/)
{
    currentSampleRate = sampleRate;
    fifoIndex = 0;
    fifo.fill(0.0f);
    fftData.fill(0.0f);
    prevMagnitudes.fill(0.0f);
    sBass = sMid = sHigh = sRms = sCentroid = sFlux = 0.0f;
}

void AudioAnalyzer::processBlock(const float* inputData, int numSamples)
{
    // RMS calculation
    float sumSq = 0.0f;
    for (int i = 0; i < numSamples; ++i)
        sumSq += inputData[i] * inputData[i];
    float rawRms = std::sqrt(sumSq / static_cast<float>(numSamples));
    sRms = asymSmooth(sRms, rawRms, 0.8f, 0.95f);
    rms.store(sRms, std::memory_order_relaxed);

    // Fill FIFO, run FFT when full
    for (int i = 0; i < numSamples; ++i)
    {
        fifo[static_cast<size_t>(fifoIndex)] = inputData[i];
        ++fifoIndex;

        if (fifoIndex >= fftSize)
        {
            fifoIndex = 0;
            performFFTAnalysis();
        }
    }
}

void AudioAnalyzer::performFFTAnalysis()
{
    // Copy fifo → fftData and apply window
    std::copy(fifo.begin(), fifo.end(), fftData.begin());
    std::fill(fftData.begin() + fftSize, fftData.end(), 0.0f);
    window.multiplyWithWindowingTable(fftData.data(), static_cast<size_t>(fftSize));

    forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());

    const int numBins = fftSize / 2;
    const float binFreq = static_cast<float>(currentSampleRate) / static_cast<float>(fftSize);

    // Band boundaries (matching oF version)
    const int bassLo = 0,  bassHi = 4;
    const int midLo  = 5,  midHi  = 46;
    const int highLo = 47, highHi = std::min(185, numBins - 1);

    auto bandEnergy = [&](int lo, int hi) {
        float s = 0.0f;
        for (int i = lo; i <= hi; ++i)
            s += fftData[static_cast<size_t>(i)];
        return s / static_cast<float>(hi - lo + 1);
    };

    float rawBass = bandEnergy(bassLo, bassHi);
    float rawMid  = bandEnergy(midLo, midHi);
    float rawHigh = bandEnergy(highLo, highHi);

    // Spectral centroid
    float sumAF = 0.0f, sumA = 0.0f;
    int centroidHi = std::min(static_cast<int>(8000.0f / binFreq), numBins - 1);
    for (int i = 0; i <= centroidHi; ++i)
    {
        float freq = static_cast<float>(i) * binFreq;
        sumAF += freq * fftData[static_cast<size_t>(i)];
        sumA  += fftData[static_cast<size_t>(i)];
    }
    float rawCentroid = (sumA > 1e-6f) ? std::clamp(sumAF / sumA / 8000.0f, 0.0f, 1.0f) : 0.0f;

    // Spectral flux
    float rawFlux = 0.0f;
    for (int i = 0; i < numBins; ++i)
    {
        float diff = fftData[static_cast<size_t>(i)] - prevMagnitudes[static_cast<size_t>(i)];
        rawFlux += std::max(0.0f, diff);
        prevMagnitudes[static_cast<size_t>(i)] = fftData[static_cast<size_t>(i)];
    }

    // Onset detection
    bool rawOnset = rawFlux > 0.5f;

    // Asymmetric smoothing (matching oF version)
    sBass     = asymSmooth(sBass,     rawBass,     0.8f, 0.95f);
    sMid      = asymSmooth(sMid,      rawMid,      0.8f, 0.95f);
    sHigh     = asymSmooth(sHigh,     rawHigh,     0.8f, 0.95f);
    sCentroid = asymSmooth(sCentroid, rawCentroid, 0.5f, 0.98f);
    sFlux     = asymSmooth(sFlux,     rawFlux,     0.8f, 0.95f);

    // Store for GUI thread
    bass.store(sBass,       std::memory_order_relaxed);
    mid.store(sMid,         std::memory_order_relaxed);
    high.store(sHigh,       std::memory_order_relaxed);
    centroid.store(sCentroid, std::memory_order_relaxed);
    flux.store(sFlux,       std::memory_order_relaxed);
    onset.store(rawOnset,   std::memory_order_relaxed);
}

float AudioAnalyzer::asymSmooth(float prev, float raw, float attack, float release)
{
    return raw > prev ? prev + attack * (raw - prev) : prev * release;
}
