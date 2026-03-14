#include "PluginEditor.h"

VJCosmosEditor::VJCosmosEditor(VJCosmosProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p)
{
    setSize(900, 700);
    setResizable(true, true);

    openGLContext.setRenderer(this);
    openGLContext.attachTo(*this);

    // ── Fluid knobs ──
    setupKnob(knobDissipation,    ParamIDs::dissipation,    "Dissipation");
    setupKnob(knobVelDissipation, ParamIDs::velDissipation, "Vel Dissip");
    setupKnob(knobGravity,        ParamIDs::gravity,        "Gravity");

    // ── Bloom knobs ──
    setupKnob(knobBloomIntensity, ParamIDs::bloomIntensity, "Bloom Int");
    setupKnob(knobBloomThreshold, ParamIDs::bloomThreshold, "Bloom Thr");

    // ── Audio knobs ──
    setupKnob(knobBassSens, ParamIDs::bassSens, "Bass Sens");
    setupKnob(knobMidSens,  ParamIDs::midSens,  "Mid Sens");
    setupKnob(knobHighSens, ParamIDs::highSens,  "High Sens");

    // ── Visual knobs ──
    setupKnob(knobRotSpeed,     ParamIDs::rotSpeed,        "Rot Speed");
    setupKnob(knobDisplacement, ParamIDs::displacement,    "Displace");
    setupKnob(knobGlitchFreq,   ParamIDs::glitchFreq,      "Glitch Frq");
    setupKnob(knobFluidSat,     ParamIDs::fluidSaturation, "Fluid Sat");
    setupKnob(knobPlexusSpeed,  ParamIDs::plexusSpeed,     "Plexus Spd");
    setupKnob(knobMasterBright, ParamIDs::masterBrightness,"Master Brt");

    // ── Effect param knobs ──
    setupKnob(knobWaveAmp,      ParamIDs::waveAmplitude, "Wave Amp");
    setupKnob(knobWaveSpeed,    ParamIDs::waveSpeed,     "Wave Spd");
    setupKnob(knobEdgeStrength, ParamIDs::edgeStrength,  "Edge Str");
    setupKnob(knobEdgeMix,      ParamIDs::edgeMix,       "Edge Mix");
    setupKnob(knobMonoMode,     ParamIDs::monoMode,      "Mono Mode");
    setupKnob(knobMirrorMode,   ParamIDs::mirrorMode,    "Mirror Md");

    // ── Toggle buttons ──
    setupToggle(togKaleido, ParamIDs::fxKaleido, "Kaleido");
    setupToggle(togCrt,     ParamIDs::fxCrt,     "CRT");
    setupToggle(togWave,    ParamIDs::fxWave,    "Wave");
    setupToggle(togGlitch,  ParamIDs::fxGlitch,  "Glitch");
    setupToggle(togEdge,    ParamIDs::fxEdge,    "Edge");
    setupToggle(togMono,    ParamIDs::fxMono,    "Mono");
    setupToggle(togMirror,  ParamIDs::fxMirror,  "Mirror");

    // ── ComboBox: Color Palette ──
    comboPalette.addItem("WARM",    1);
    comboPalette.addItem("COOL",    2);
    comboPalette.addItem("MONO",    3);
    comboPalette.addItem("RAINBOW", 4);
    addAndMakeVisible(comboPalette);
    attachPalette = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processorRef.apvts, ParamIDs::colorPalette, comboPalette);

    labelPalette.setText("Palette", juce::dontSendNotification);
    labelPalette.setJustificationType(juce::Justification::centred);
    labelPalette.setColour(juce::Label::textColourId, juce::Colours::white);
    labelPalette.setFont(juce::Font(12.0f));
    addAndMakeVisible(labelPalette);

    // ── Section labels ──
    auto setupSectionLabel = [&](juce::Label& lbl, const juce::String& text, juce::Colour col) {
        lbl.setText(text, juce::dontSendNotification);
        lbl.setColour(juce::Label::textColourId, col);
        lbl.setFont(juce::Font(14.0f, juce::Font::bold));
        lbl.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(lbl);
    };

    auto cyan   = juce::Colour(0xFF00DDDD);
    auto yellow = juce::Colour(0xFFDDDD00);

    setupSectionLabel(lblFluid,       "FLUID",        cyan);
    setupSectionLabel(lblBloom,       "BLOOM",        cyan);
    setupSectionLabel(lblAudio,       "AUDIO",        cyan);
    setupSectionLabel(lblVisuals,     "VISUALS",      yellow);
    setupSectionLabel(lblEffects,     "EFFECTS ON/OFF", yellow);
    setupSectionLabel(lblEffectParams,"EFFECT PARAMS", cyan);
    setupSectionLabel(lblPalette,     "PALETTE",      cyan);

    startTimerHz(30);
}

VJCosmosEditor::~VJCosmosEditor()
{
    stopTimer();
    openGLContext.detach();
}

void VJCosmosEditor::setupKnob(KnobWithLabel& k, const juce::String& paramID, const juce::String& name)
{
    k.slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    k.slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 16);
    k.slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    k.slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xFF00AAFF));
    k.slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xFF333344));
    k.slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(k.slider);

    k.label.setText(name, juce::dontSendNotification);
    k.label.setJustificationType(juce::Justification::centred);
    k.label.setColour(juce::Label::textColourId, juce::Colours::white);
    k.label.setFont(juce::Font(11.0f));
    addAndMakeVisible(k.label);

    k.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, paramID, k.slider);
}

void VJCosmosEditor::setupToggle(ToggleWithLabel& t, const juce::String& paramID, const juce::String& name)
{
    t.button.setButtonText(name);
    t.button.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    t.button.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xFF00FF88));
    addAndMakeVisible(t.button);

    t.attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, paramID, t.button);
}

void VJCosmosEditor::layoutKnob(KnobWithLabel& k, int x, int y, int w, int h)
{
    const int labelH = 18;
    k.label.setBounds(x, y, w, labelH);
    k.slider.setBounds(x, y + labelH, w, h - labelH);
}

void VJCosmosEditor::layoutSectionLabel(juce::Label& lbl, int x, int y, int w)
{
    lbl.setBounds(x, y, w, 18);
}

// ── OpenGL ──

void VJCosmosEditor::newOpenGLContextCreated()
{
    syphonOutput.start(openGLContext.getRawContext());
}

void VJCosmosEditor::renderOpenGL()
{
    using namespace juce::gl;
    auto& gl = openGLContext;

    const float desktopScale = static_cast<float>(gl.getRenderingScale());
    const int w = static_cast<int>(getWidth() * desktopScale);
    const int h = static_cast<int>(getHeight() * desktopScale);

    // Syphon FBO: cap at 1280x1280 for Touch Designer
    const int maxSyphon = 1280;
    const int fboW = std::min(w, maxSyphon);
    const int fboH = std::min(h, maxSyphon);

    float b = displayBass;
    float m = displayMid;
    float hi = displayHigh;

    // Render into FBO (for Syphon output)
    syphonOutput.ensureFBO(fboW, fboH);
    syphonOutput.bindFBO();

    glClearColor(0.08f + b * 0.15f, 0.06f + m * 0.1f, 0.13f + hi * 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO: Phase 3+ — fluid simulation, effects, 3D drawing here

    // Publish FBO texture to Syphon, then blit to screen
    syphonOutput.publishTexture(syphonOutput.getFBOTextureID(), fboW, fboH);
    syphonOutput.unbindFBO(w, h);
}

void VJCosmosEditor::openGLContextClosing()
{
    syphonOutput.stop();
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
    // ── Audio level bars (top 50px) ──
    const int barAreaH = 50;
    const int barW = getWidth() / 4;
    const int barH = 20;
    const int barY = 18;

    auto drawBar = [&](int x, float level, juce::Colour col, const juce::String& name) {
        g.setColour(juce::Colour(0xFF222233));
        g.fillRoundedRectangle((float)x, (float)barY, (float)barW - 10, (float)barH, 4.0f);
        g.setColour(col);
        g.fillRoundedRectangle((float)x, (float)barY, (float)((barW - 10) * juce::jlimit(0.0f, 1.0f, level)), (float)barH, 4.0f);
        g.setColour(juce::Colours::white);
        g.setFont(11.0f);
        g.drawText(name, x, 2, barW - 10, 14, juce::Justification::centredLeft);
    };

    drawBar(10,             displayBass, juce::Colour(0xFFFF4444), "Bass");
    drawBar(10 + barW,      displayMid,  juce::Colour(0xFF44FF44), "Mid");
    drawBar(10 + barW * 2,  displayHigh, juce::Colour(0xFF4488FF), "High");

    // Title
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(12.0f);
    g.drawText("VJCosmos VST3", 10 + barW * 3, 2, barW - 10, barAreaH - 4, juce::Justification::centred);
}

void VJCosmosEditor::resized()
{
    const int W = getWidth();
    const int margin = 10;
    const int topOffset = 55;   // below level bars

    // ── Row 1 (y=55): FLUID (3) | BLOOM (2) | AUDIO (3) — total 8 knobs ──
    {
        const int row1Y = topOffset;
        const int rowH  = 140;
        const int knobW = (W - margin * 2) / 8;

        int x = margin;
        layoutSectionLabel(lblFluid, x, row1Y, knobW * 3);
        layoutKnob(knobDissipation,    x,              row1Y + 20, knobW, rowH - 20);
        layoutKnob(knobVelDissipation, x + knobW,      row1Y + 20, knobW, rowH - 20);
        layoutKnob(knobGravity,        x + knobW * 2,  row1Y + 20, knobW, rowH - 20);

        x = margin + knobW * 3;
        layoutSectionLabel(lblBloom, x, row1Y, knobW * 2);
        layoutKnob(knobBloomIntensity, x,              row1Y + 20, knobW, rowH - 20);
        layoutKnob(knobBloomThreshold, x + knobW,      row1Y + 20, knobW, rowH - 20);

        x = margin + knobW * 5;
        layoutSectionLabel(lblAudio, x, row1Y, knobW * 3);
        layoutKnob(knobBassSens, x,              row1Y + 20, knobW, rowH - 20);
        layoutKnob(knobMidSens,  x + knobW,      row1Y + 20, knobW, rowH - 20);
        layoutKnob(knobHighSens, x + knobW * 2,  row1Y + 20, knobW, rowH - 20);
    }

    // ── Row 2 (y=195): VISUALS (6 knobs) ──
    {
        const int row2Y = topOffset + 150;
        const int rowH  = 140;
        const int knobW = (W - margin * 2) / 6;

        layoutSectionLabel(lblVisuals, margin, row2Y, W - margin * 2);
        layoutKnob(knobRotSpeed,     margin,              row2Y + 20, knobW, rowH - 20);
        layoutKnob(knobDisplacement, margin + knobW,      row2Y + 20, knobW, rowH - 20);
        layoutKnob(knobGlitchFreq,   margin + knobW * 2,  row2Y + 20, knobW, rowH - 20);
        layoutKnob(knobFluidSat,     margin + knobW * 3,  row2Y + 20, knobW, rowH - 20);
        layoutKnob(knobPlexusSpeed,  margin + knobW * 4,  row2Y + 20, knobW, rowH - 20);
        layoutKnob(knobMasterBright, margin + knobW * 5,  row2Y + 20, knobW, rowH - 20);
    }

    // ── Row 3 (y=345): EFFECTS ON/OFF (7 toggles) ──
    {
        const int row3Y = topOffset + 300;
        const int togW  = (W - margin * 2) / 7;

        layoutSectionLabel(lblEffects, margin, row3Y, W - margin * 2);

        ToggleWithLabel* toggles[] = { &togKaleido, &togCrt, &togWave, &togGlitch, &togEdge, &togMono, &togMirror };
        for (int i = 0; i < 7; ++i)
            toggles[i]->button.setBounds(margin + togW * i, row3Y + 20, togW, 50);
    }

    // ── Row 4 (y=425): EFFECT PARAMS (6 knobs) + PALETTE (combo) ──
    {
        const int row4Y = topOffset + 390;
        const int rowH  = 140;
        const int knobW = (W - margin * 2) / 7;  // 7 columns: 6 knobs + 1 combo

        layoutSectionLabel(lblEffectParams, margin, row4Y, knobW * 6);
        layoutKnob(knobWaveAmp,      margin,              row4Y + 20, knobW, rowH - 20);
        layoutKnob(knobWaveSpeed,    margin + knobW,      row4Y + 20, knobW, rowH - 20);
        layoutKnob(knobEdgeStrength, margin + knobW * 2,  row4Y + 20, knobW, rowH - 20);
        layoutKnob(knobEdgeMix,      margin + knobW * 3,  row4Y + 20, knobW, rowH - 20);
        layoutKnob(knobMonoMode,     margin + knobW * 4,  row4Y + 20, knobW, rowH - 20);
        layoutKnob(knobMirrorMode,   margin + knobW * 5,  row4Y + 20, knobW, rowH - 20);

        // Palette combo
        const int comboX = margin + knobW * 6;
        layoutSectionLabel(lblPalette, comboX, row4Y, knobW);
        labelPalette.setBounds(comboX, row4Y + 20, knobW, 18);
        comboPalette.setBounds(comboX + 5, row4Y + 45, knobW - 10, 28);
    }
}
