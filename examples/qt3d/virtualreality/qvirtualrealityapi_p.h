#ifndef QT3DVIRTUALREALITYAPI_P_H
#define QT3DVIRTUALREALITYAPI_P_H

#include "qvirtualrealityapi.h"

#define QT3DVR_COMPILE_WITH_OVR true
#define QT3DVR_COMPILE_WITH_OPENVR false
#define QT3DVR_COMPILE_WITH_OSVR false

#if(QT3DVR_COMPILE_WITH_OVR)
#  include "backends/ovr/vrapiovr.h"
#endif
#if(QT3DVR_COMPILE_WITH_OPENVR)
#  include "backends/openvr/vrapiopenvr.h"
#endif
#if(QT3DVR_COMPILE_WITH_OSVR)
#  include "backends/osvr/vrapiosvr.h"
#endif

#include <QtOpenGL/QGL>
#include <private/qobject_p.h>
#include "qvirtualrealityapibackend.h"

QT_BEGIN_NAMESPACE

#if(QT3DVR_COMPILE_WITH_OVR)
#  include "backends/ovr/vrapiovr.h"
#endif

namespace Qt3DVirtualReality {

class QVirtualRealityApiPrivate : public QObjectPrivate
{
public:

    Q_DECLARE_PUBLIC(QVirtualRealityApi)

    //VrApiBackend
    QVirtualRealityApiBackend *m_apibackend;

    void setType(QVirtualRealityApi::Type vendor) {
#if(QT3DVR_COMPILE_WITH_OVR)
        if(QVirtualRealityApi::Ovr == vendor) {
            m_apibackend = new VirtualRealityApiOvr();
        }
#endif
    }
    QVirtualRealityApiPrivate()
      : m_apibackend(nullptr)
    {

    }

    ~QVirtualRealityApiPrivate() {
        if(m_apibackend)
            delete m_apibackend;
    }
};

}

QT_END_NAMESPACE

#endif
