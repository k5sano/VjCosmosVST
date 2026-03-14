#include "SyphonOutput.h"

#if defined(__APPLE__)

#import <Syphon/Syphon.h>
#import <OpenGL/gl3.h>
#import <OpenGL/OpenGL.h>
#import <AppKit/AppKit.h>

struct SyphonOutputImpl
{
    SyphonOpenGLServer* server = nil;
    CGLContextObj cglCtx = nullptr;

    // FBO for off-screen render
    GLuint fbo = 0;
    GLuint fboTexture = 0;
    GLuint rbo = 0;  // depth/stencil renderbuffer
    int fboWidth = 0;
    int fboHeight = 0;
    GLint previousFBO = 0;  // JUCE's FBO to restore after our render
};

// ── SyphonOutput ──

SyphonOutput::SyphonOutput()
    : impl(std::make_unique<SyphonOutputImpl>())
{
}

SyphonOutput::~SyphonOutput()
{
    stop();
}

void SyphonOutput::start(void* rawContext)
{
    @autoreleasepool {
        // JUCE's getRawContext() returns NSOpenGLContext* on macOS
        NSOpenGLContext* nsglContext = (__bridge NSOpenGLContext*)rawContext;
        if (!nsglContext) {
            NSLog(@"[VJCosmos] Syphon: NSOpenGLContext is nil");
            return;
        }
        impl->cglCtx = [nsglContext CGLContextObj];
        if (!impl->cglCtx) {
            NSLog(@"[VJCosmos] Syphon: CGLContextObj is nil");
            return;
        }
        impl->server = [[SyphonOpenGLServer alloc] initWithName:@"VJCosmos"
                                                        context:impl->cglCtx
                                                        options:nil];
        if (impl->server) {
            NSLog(@"[VJCosmos] Syphon server started: %@", impl->server.name);
            fprintf(stderr, "[VJCosmos] Syphon server started: VJCosmos\n");
        } else {
            NSLog(@"[VJCosmos] Syphon server FAILED to start");
            fprintf(stderr, "[VJCosmos] Syphon server FAILED to start\n");
        }
    }
}

void SyphonOutput::publishTexture(unsigned int texID, int width, int height)
{
    if (!impl->server) return;

    @autoreleasepool {
        [impl->server publishFrameTexture:texID
                            textureTarget:GL_TEXTURE_2D
                              imageRegion:NSMakeRect(0, 0, width, height)
                        textureDimensions:NSMakeSize(width, height)
                                  flipped:NO];
    }
}

void SyphonOutput::stop()
{
    @autoreleasepool {
        // Destroy FBO
        if (impl->fbo) {
            glDeleteFramebuffers(1, &impl->fbo);
            impl->fbo = 0;
        }
        if (impl->fboTexture) {
            glDeleteTextures(1, &impl->fboTexture);
            impl->fboTexture = 0;
        }
        if (impl->rbo) {
            glDeleteRenderbuffers(1, &impl->rbo);
            impl->rbo = 0;
        }

        // Stop Syphon server
        if (impl->server) {
            [impl->server stop];
            impl->server = nil;
            NSLog(@"[VJCosmos] Syphon server stopped");
        }
    }
}

// ── FBO management ──

void SyphonOutput::ensureFBO(int width, int height)
{
    if (impl->fbo && impl->fboWidth == width && impl->fboHeight == height)
        return;

    // Clean up old
    if (impl->fbo) glDeleteFramebuffers(1, &impl->fbo);
    if (impl->fboTexture) glDeleteTextures(1, &impl->fboTexture);
    if (impl->rbo) glDeleteRenderbuffers(1, &impl->rbo);

    impl->fboWidth = width;
    impl->fboHeight = height;

    // Create texture
    glGenTextures(1, &impl->fboTexture);
    glBindTexture(GL_TEXTURE_2D, impl->fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Create FBO
    glGenFramebuffers(1, &impl->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, impl->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, impl->fboTexture, 0);

    // Depth/stencil renderbuffer
    glGenRenderbuffers(1, &impl->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, impl->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, impl->rbo);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        NSLog(@"[VJCosmos] FBO incomplete: 0x%x", status);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SyphonOutput::bindFBO()
{
    // Save JUCE's current FBO so we can restore it later
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &impl->previousFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, impl->fbo);
    glViewport(0, 0, impl->fboWidth, impl->fboHeight);
}

void SyphonOutput::unbindFBO(int viewportW, int viewportH)
{
    // Blit our FBO to JUCE's FBO (not FBO 0)
    glBindFramebuffer(GL_READ_FRAMEBUFFER, impl->fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, impl->previousFBO);
    glBlitFramebuffer(0, 0, impl->fboWidth, impl->fboHeight,
                      0, 0, viewportW, viewportH,
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, impl->previousFBO);
    glViewport(0, 0, viewportW, viewportH);
}

unsigned int SyphonOutput::getFBOTextureID() const
{
    return impl->fboTexture;
}

#else
// Non-macOS stub
SyphonOutput::SyphonOutput() : impl(nullptr) {}
SyphonOutput::~SyphonOutput() {}
void SyphonOutput::start(void*) {}
void SyphonOutput::publishTexture(unsigned int, int, int) {}
void SyphonOutput::stop() {}
void SyphonOutput::ensureFBO(int, int) {}
void SyphonOutput::bindFBO() {}
void SyphonOutput::unbindFBO(int, int) {}
unsigned int SyphonOutput::getFBOTextureID() const { return 0; }
#endif
