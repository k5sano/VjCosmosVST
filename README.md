# VJCosmos VST3

openFrameworks版 [VJCosmos](https://github.com/k5sano/VJCosmos) のJUCE VST3移植プロジェクト。

## Phase計画

- **Phase 2** (現在): JUCE骨格 — Processor + FFT解析 + パラメータ公開 + 最小Editor → VST3ビルド → Ableton読み込み確認
- **Phase 3**: 流体シミュレーションシェーダー移植 + FBOピンポン + エフェクトチェーン
- **Phase 4**: 3Dワイヤーフレーム + プレクサス + グリッチ + Syphon出力 + GUI

## 環境

- JUCE 8.0.12 (`~/JUCE`)
- CMake ベース（Projucer不使用）
- ターゲット: VST3 / AU / Standalone
- macOS / OpenGL 3.2+ (GLSL 150)

## ビルド

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## oF版の機能（移植対象）

- GPU流体シミュレーション（Navier-Stokes）
- FFT音声解析（bass/mid/high/centroid/flux/onset）
- 3Dワイヤーフレーム（icosphere/cube/sphere、音で頂点変位）
- プレクサスネットワーク（120パーティクル）
- グリッチ多角形オーバーレイ
- エフェクトチェーン: Bloom → Kaleidoscope → Wave → Glitch → Edge → Mono → Mirror → CRT
- Syphon出力
- プリセット保存/ロード
- カラーパレット4種
