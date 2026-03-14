cmake_minimum_required(VERSION 3.22)
project(VJCosmosVST VERSION 0.1.0)

# JUCE のパスを設定
set(JUCE_DIR "$ENV{HOME}/JUCE" CACHE PATH "Path to JUCE")

# JUCE を追加
add_subdirectory(${JUCE_DIR} ${CMAKE_BINARY_DIR}/JUCE)

# プラグインターゲット
juce_add_plugin(VJCosmosVST
    COMPANY_NAME "k5sano"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE K5sn
    PLUGIN_CODE VjCs
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "VJCosmos"
)

# ソースファイル
target_sources(VJCosmosVST PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/AudioAnalyzer.cpp
)

# インクルードディレクトリ
target_include_directories(VJCosmosVST PRIVATE Source)

# JUCE モジュール
target_link_libraries(VJCosmosVST
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_opengl
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# コンパイル定義
target_compile_definitions(VJCosmosVST
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)
