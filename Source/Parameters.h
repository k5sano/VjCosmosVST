#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

namespace ParamIDs
{
    // Fluid
    inline constexpr const char* dissipation    = "dissipation";
    inline constexpr const char* velDissipation = "velDissipation";
    inline constexpr const char* gravity        = "gravity";

    // Bloom
    inline constexpr const char* bloomIntensity = "bloomIntensity";
    inline constexpr const char* bloomThreshold = "bloomThreshold";

    // Audio sensitivity
    inline constexpr const char* bassSens  = "bassSens";
    inline constexpr const char* midSens   = "midSens";
    inline constexpr const char* highSens  = "highSens";

    // Visuals
    inline constexpr const char* rotSpeed        = "rotSpeed";
    inline constexpr const char* displacement    = "displacement";
    inline constexpr const char* glitchFreq      = "glitchFreq";
    inline constexpr const char* fluidSaturation = "fluidSaturation";
    inline constexpr const char* plexusSpeed     = "plexusSpeed";
    inline constexpr const char* masterBrightness= "masterBrightness";

    // Effect toggles
    inline constexpr const char* fxKaleido = "fxKaleido";
    inline constexpr const char* fxCrt     = "fxCrt";
    inline constexpr const char* fxWave    = "fxWave";
    inline constexpr const char* fxGlitch  = "fxGlitch";
    inline constexpr const char* fxEdge    = "fxEdge";
    inline constexpr const char* fxMono    = "fxMono";
    inline constexpr const char* fxMirror  = "fxMirror";

    // Effect params
    inline constexpr const char* waveAmplitude = "waveAmplitude";
    inline constexpr const char* waveSpeed     = "waveSpeed";
    inline constexpr const char* edgeStrength  = "edgeStrength";
    inline constexpr const char* edgeMix       = "edgeMix";
    inline constexpr const char* monoMode      = "monoMode";
    inline constexpr const char* mirrorMode    = "mirrorMode";
    inline constexpr const char* colorPalette  = "colorPalette";
}

inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // ── Fluid ──
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::dissipation, 1}, "Dissipation",
        juce::NormalisableRange<float>(0.9f, 1.0f, 0.001f), 0.999f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::velDissipation, 1}, "Vel Dissipation",
        juce::NormalisableRange<float>(0.8f, 1.0f, 0.001f), 0.99f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::gravity, 1}, "Gravity",
        juce::NormalisableRange<float>(-0.05f, 0.05f, 0.0001f), -0.0098f));

    // ── Bloom ──
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::bloomIntensity, 1}, "Bloom Intensity",
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::bloomThreshold, 1}, "Bloom Threshold",
        juce::NormalisableRange<float>(0.1f, 0.8f, 0.01f), 0.3f));

    // ── Audio sensitivity ──
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::bassSens, 1}, "Bass Sensitivity",
        juce::NormalisableRange<float>(0.5f, 3.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::midSens, 1}, "Mid Sensitivity",
        juce::NormalisableRange<float>(0.5f, 3.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::highSens, 1}, "High Sensitivity",
        juce::NormalisableRange<float>(0.5f, 3.0f, 0.01f), 1.0f));

    // ── Visuals ──
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::rotSpeed, 1}, "Rotation Speed",
        juce::NormalisableRange<float>(0.0f, 60.0f, 0.1f), 15.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::displacement, 1}, "Displacement",
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::glitchFreq, 1}, "Glitch Frequency",
        juce::NormalisableRange<float>(0.0f, 3.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::fluidSaturation, 1}, "Fluid Saturation",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::plexusSpeed, 1}, "Plexus Speed",
        juce::NormalisableRange<float>(0.1f, 3.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::masterBrightness, 1}, "Master Brightness",
        juce::NormalisableRange<float>(0.5f, 1.5f, 0.01f), 1.0f));

    // ── Effect toggles ──
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ParamIDs::fxKaleido, 1}, "Kaleidoscope", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ParamIDs::fxCrt, 1}, "CRT", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ParamIDs::fxWave, 1}, "Wave Distort", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ParamIDs::fxGlitch, 1}, "Glitch Shift", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ParamIDs::fxEdge, 1}, "Edge Detection", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ParamIDs::fxMono, 1}, "Mono/Sepia", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ParamIDs::fxMirror, 1}, "Mirror", false));

    // ── Effect params ──
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::waveAmplitude, 1}, "Wave Amplitude",
        juce::NormalisableRange<float>(0.0f, 50.0f, 0.1f), 10.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::waveSpeed, 1}, "Wave Speed",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f), 2.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::edgeStrength, 1}, "Edge Strength",
        juce::NormalisableRange<float>(0.0f, 3.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::edgeMix, 1}, "Edge Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::monoMode, 1}, "Mono Mode",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::mirrorMode, 1}, "Mirror Mode",
        juce::NormalisableRange<float>(0.0f, 2.0f, 1.0f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID{ParamIDs::colorPalette, 1}, "Color Palette",
        0, 3, 3)); // 0=WARM, 1=COOL, 2=MONO, 3=RAINBOW

    return { params.begin(), params.end() };
}
