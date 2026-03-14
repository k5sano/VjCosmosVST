#pragma once

#include <memory>

// Forward-declare pimpl (hides Objective-C from pure C++ translation units)
struct SyphonOutputImpl;

class SyphonOutput
{
public:
    SyphonOutput();
    ~SyphonOutput();

    // Call from newOpenGLContextCreated() — pass the raw CGLContextObj
    void start(void* cglContext);

    // Call from renderOpenGL() after drawing into the FBO.
    // Publishes the FBO texture via Syphon.
    void publishTexture(unsigned int texID, int width, int height);

    // Call from openGLContextClosing()
    void stop();

    // FBO management
    void ensureFBO(int width, int height);
    void bindFBO();
    void unbindFBO(int viewportW, int viewportH);
    unsigned int getFBOTextureID() const;

private:
    std::unique_ptr<SyphonOutputImpl> impl;
};
