# CLAUDE.md — VJCosmos VST3 開発ガイド

## プロジェクト概要
oF版VJCosmos（https://github.com/k5sano/VJCosmos）をJUCE VST3プラグインとして移植する。
参考: VisualSynth（https://github.com/k5sano/Visualsynth）— 同じ作者のoF+GLSLビジュアライザ

## 環境
- JUCE 8.0.12: ~/JUCE
- CMakeベース（Projucer不使用）
- 参考既存プラグイン: ~/MyPlugin/
- macOS / OpenGL 3.2+ (GLSL 150)

## アーキテクチャ
- **PluginProcessor** (オーディオスレッド): DAWオーディオ入力 → FFT解析 → APVTS全パラメータ公開
- **PluginEditor** (GUIスレッド): OpenGLRenderer → 流体 / エフェクト / 3D描画 → Syphon出力
- **AudioAnalyzer**: FFT解析（atomic変数でスレッド間受渡し）
- **Parameters.h**: 全パラメータID定義 + createParameterLayout()

## oF→JUCE 移植マッピング
| oF | JUCE |
|---|---|
| ofxFft | juce::dsp::FFT |
| ofxMidi | DAWネイティブMIDI (NEEDS_MIDI_INPUT) |
| ofFbo | OpenGL FBO直接操作 |
| ofShader | OpenGL shader直接操作 |
| ofxSyphon | juce_sharedtexture モジュール |
| プリセット | getStateInformation/setStateInformation |
| sampler2DRect (GL2.1) | sampler2D (GL3.2), texCoord正規化 |
| gl_FragColor | out vec4 fragColor |
| gl_TexCoord[0].st | in vec2 vTexCoord |
| ftransform() | uniform mat4 projectionMatrix |

## DAW公開パラメータ（30個）
Fluid: dissipation, velDissipation, gravity
Bloom: intensity, threshold
Audio: bassSens, midSens, highSens
Visuals: rotSpeed, displacement, glitchFreq, fluidSaturation, plexusSpeed, masterBrightness
Effect toggles (7): kaleido, crt, wave, glitch, edge, mono, mirror
Effect params: waveAmplitude, waveSpeed, edgeStrength, edgeMix, monoMode, mirrorMode, colorPalette

## oF版MIDI CC/Note マッピング（参考）
CC 21-28: 上段ノブ（dissipation, velDissipation, bloomIntensity, bloomThreshold, bassSens, midSens, highSens, plexusThresh）
CC 41-48: 下段ノブ（rotSpeed, displacement, glitchFreq, gravity, fluidSat, plexusSpeed, reserved, masterBright）
Note 9-12: fluidClear, paletteNext, glitchBurst, meshNext
Note 25-28: plexusToggle, bloomToggle, freeze, screenshot
CC 114-117: 矢印（gravityUp/Down, windLeft/Right）

## Phase計画
- Phase 2（今）: 骨格 + FFT + パラメータ → VST3ビルド → Ableton確認
- Phase 3: 流体シミュレーション + FBO + エフェクトチェーン
- Phase 4: 3D + プレクサス + グリッチ + Syphon + GUI

## ビルド
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## 注意事項
- macOS .appバンドル: CFBundleGetMainBundle()でリソースパス取得必須
- データは Contents/Resources/data/ に配置
- xattr -cr で隔離属性除去
- oF版シェーダーは Shaders/oF_originals/ に参考用として保存（GL 2.1, sampler2DRect）
- JUCE版では GL 3.2 / GLSL 150 に変換して使う
