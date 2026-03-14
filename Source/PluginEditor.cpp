#include "PluginEditor.h"

VJCosmosEditor::VJCosmosEditor(VJCosmosProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p)
{
    setSize(800, 600);
    setResizable(true, true);

    openGLContext.setRenderer(this);
    openGLContext.attachTo(*this);

    startTimerHz(30); // UI refresh rate
}

VJCosmosEditor::~VJCosmosEditor()
{
    stopTimer();
    openGLContext.detach();
}

void VJCosmosEditor::newOpenGLContextCreated()
{
    // Phase 3+: initialize shaders, FBOs, etc.
}

void VJCosmosEditor::renderOpenGL()
{
    using namespace juce::gl;
    auto& gl = openGLContext;

    const float desktopScale = static_cast<float>(gl.getRenderingScale());
    const int w = static_cast<int>(getWidth() * desktopScale);
    const int h = static_cast<int>(getHeight() * desktopScale);

    glViewport(0, 0, w, h);
    glClearColor(0.05f, 0.02f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Phase 2: minimal visual — audio-reactive background color
    float b = displayBass;
    float m = displayMid;
    float hi = displayHigh;

    glClearColor(b * 0.3f, m * 0.15f, hi * 0.4f + 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void VJCosmosEditor::openGLContextClosing()
{
    // Phase 3+: cleanup shaders, FBOs, etc.
}

void VJCosmosEditor::timerCallback()
{
    auto& analyzer = processorRef.getAnalyzer();
    displayBass = analyzer.getBass();
    displayMid  = analyzer.getMid();
    displayHigh = analyzer.getHigh();

    repaint();
}

void VJCosmosEditor::paint(juce::Graphics& g)
{
    // Phase 2: simple overlay text
    g.setColour(juce::Colours::white);
    g.setFont(16.0f);

    juce::String info;
    info << "VJCosmos VST3 — Phase 2\n"
         << "Bass: "  << juce::String(displayBass, 3) << "\n"
         << "Mid: "   << juce::String(displayMid, 3) << "\n"
         << "High: "  << juce::String(displayHigh, 3) << "\n"
         << "Params: " << processorRef.apvts.state.getNumProperties() << " exposed to DAW";

    g.drawMultiLineText(info, 20, 30, getWidth() - 40);
}

void VJCosmosEditor::resized()
{
    // Phase 4: layout GUI components
}
