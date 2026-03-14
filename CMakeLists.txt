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
    Source/SyphonOutput.mm
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

# Syphon framework (macOS only)
if(APPLE)
    # Syphon framework の場所を指定
    set(SYPHON_FRAMEWORK "${CMAKE_SOURCE_DIR}/Frameworks/Syphon.framework")

    target_include_directories(VJCosmosVST PRIVATE
        ${SYPHON_FRAMEWORK}/Headers
    )
    target_link_libraries(VJCosmosVST PRIVATE
        "${SYPHON_FRAMEWORK}"
    )

    # CMake自動rpathを無効化（絶対パスが埋め込まれるのを防ぐ）
    set_target_properties(VJCosmosVST_VST3 PROPERTIES SKIP_BUILD_RPATH TRUE)
    set_target_properties(VJCosmosVST_Standalone PROPERTIES SKIP_BUILD_RPATH TRUE)

    # VST3: Syphon.framework をコピー + rpath修正
    set(VST3_BUNDLE "${CMAKE_BINARY_DIR}/VJCosmosVST_artefacts/Release/VST3/VJCosmos.vst3")
    set(INSTALLED_VST3 "$ENV{HOME}/Library/Audio/Plug-Ins/VST3/VJCosmos.vst3")
    add_custom_command(TARGET VJCosmosVST_VST3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${VST3_BUNDLE}/Contents/Frameworks"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${SYPHON_FRAMEWORK}" "${VST3_BUNDLE}/Contents/Frameworks/Syphon.framework"
        COMMAND install_name_tool -add_rpath "@loader_path/../Frameworks" "${VST3_BUNDLE}/Contents/MacOS/VJCosmos"
        COMMENT "Embedding Syphon.framework in VST3 bundle"
    )

    # Standalone: Syphon.framework をコピー + rpath修正
    set(STANDALONE_BUNDLE "${CMAKE_BINARY_DIR}/VJCosmosVST_artefacts/Release/Standalone/VJCosmos.app")
    add_custom_command(TARGET VJCosmosVST_Standalone POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${STANDALONE_BUNDLE}/Contents/Frameworks"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${SYPHON_FRAMEWORK}" "${STANDALONE_BUNDLE}/Contents/Frameworks/Syphon.framework"
        COMMAND install_name_tool -add_rpath "@loader_path/../Frameworks" "${STANDALONE_BUNDLE}/Contents/MacOS/VJCosmos"
        COMMENT "Embedding Syphon.framework in Standalone bundle"
    )

    # COPY_PLUGIN_AFTER_BUILD でインストールされた先にもフレームワークをコピー + rpath修正
    add_custom_command(TARGET VJCosmosVST_VST3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALLED_VST3}/Contents/Frameworks"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${SYPHON_FRAMEWORK}" "${INSTALLED_VST3}/Contents/Frameworks/Syphon.framework"
        COMMAND install_name_tool -add_rpath "@loader_path/../Frameworks" "${INSTALLED_VST3}/Contents/MacOS/VJCosmos"
        COMMENT "Copying Syphon.framework to installed VST3 and fixing rpath"
    )
endif()

# コンパイル定義
target_compile_definitions(VJCosmosVST
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)
