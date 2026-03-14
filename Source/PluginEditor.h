#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_opengl/juce_opengl.h>
#include "PluginProcessor.h"

class VJCosmosEditor : public juce::AudioProcessorEditor,
                        private juce::OpenGLRenderer,
                        private juce::Timer
{
public:
    explicit VJCosmosEditor(VJCosmosProcessor&);
    ~VJCosmosEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // OpenGLRenderer
    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    // Timer for UI updates
    void timerCallback() override;

    VJCosmosProcessor& processorRef;
    juce::OpenGLContext openGLContext;

    // Phase 2: simple level display
    float displayBass = 0, displayMid = 0, displayHigh = 0;

    // ── Helper struct for knob + label + attachment ──
    struct KnobWithLabel {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    // ── Knobs ──
    // Fluid
    KnobWithLabel knobDissipation, knobVelDissipation, knobGravity;
    // Bloom
    KnobWithLabel knobBloomIntensity, knobBloomThreshold;
    // Audio
    KnobWithLabel knobBassSens, knobMidSens, knobHighSens;
    // Visuals
    KnobWithLabel knobRotSpeed, knobDisplacement, knobGlitchFreq,
                  knobFluidSat, knobPlexusSpeed, knobMasterBright;
    // Effect params
    KnobWithLabel knobWaveAmp, knobWaveSpeed, knobEdgeStrength,
                  knobEdgeMix, knobMonoMode, knobMirrorMode;

    // ── Toggle buttons ──
    struct ToggleWithLabel {
        juce::ToggleButton button;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;
    };
    ToggleWithLabel togKaleido, togCrt, togWave, togGlitch, togEdge, togMono, togMirror;

    // ── ComboBox ──
    juce::ComboBox comboPalette;
    juce::Label labelPalette;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> attachPalette;

    // ── Section labels ──
    juce::Label lblFluid, lblBloom, lblAudio, lblVisuals, lblEffects, lblEffectParams, lblPalette;

    // Helper methods
    void setupKnob(KnobWithLabel& k, const juce::String& paramID, const juce::String& name);
    void setupToggle(ToggleWithLabel& t, const juce::String& paramID, const juce::String& name);
    void layoutKnob(KnobWithLabel& k, int x, int y, int w, int h);
    void layoutSectionLabel(juce::Label& lbl, int x, int y, int w);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VJCosmosEditor)
};
