#ifndef QVRRENDERTARGET
#define QVRRENDERTARGET

#include <QOpenGLFunctions_3_2_Core>
#include <QOpenGLFramebufferObject>
#include <QVector>
#include <iostream>

//#include "GL/CAPI_GLE.h"
#include "Extras/OVR_Math.h"
#include "OVR_CAPI_GL.h"
#include <assert.h>

#if defined(_WIN32)
    #include <dxgi.h> // for GetDefaultAdapterLuid
    //#pragma comment(lib, "dxgi.lib")
#endif

using namespace OVR;

class OvrFramebuffer;

class OvrSwapChain
{
    QSize               m_texSize;
    ovrSession          m_session;
    //GLuint              m_texIdDepthBuffer;
    QOpenGLFunctions_3_2_Core *m_funcs;
    QVector<OvrFramebuffer *> m_framebuffers;
    ovrTextureSwapChain m_textureChain;
public:
    OvrSwapChain(ovrSession session, QSize size);
    ~OvrSwapChain();

    QSize size() const;
    void   bindCurrentChainIndexFramebuffer();
    void   bindFramebuffer(int index);
    int    chainLength() const;
    void   commit();
    const ovrTextureSwapChain &ovrTextureChain() const;
};

#endif
