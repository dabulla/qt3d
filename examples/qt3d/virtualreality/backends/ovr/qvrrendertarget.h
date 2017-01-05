#ifndef QVRRENDERTARGET
#define QVRRENDERTARGET

#include <QOpenGLFunctions_3_2_Core>
#include <QOpenGLFramebufferObject>

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

class QVrRendertarget
{

    //GLuint              m_texId;
    //GLuint              m_fboId;
    Sizei               m_texSize;
    ovrSession          m_session;
    //GLuint              m_texIdDepthBuffer;
    QOpenGLFunctions_3_2_Core *m_funcs;
public:
    QVrRendertarget(ovrSession session, ovrSizei size);
    ~QVrRendertarget();

    ovrTextureSwapChain m_oVrTextureChain;

    Sizei size() const;
    //GLuint fboId();
    GLuint texId();
//    void SetAndClearRenderSurface();
//    void UnsetRenderSurface();
    void Commit();
};

#endif
