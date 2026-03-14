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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VJCosmosEditor)
};
