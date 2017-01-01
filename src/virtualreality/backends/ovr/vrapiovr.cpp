#include "vrapiovr.h"

bool VirtualRealityApiOvr::isHmdPresent() const
{
    return true;
//    qDebug() << "VRRender Init:" << m_p->m_frameIndex;
//    ovrResult result = ovr_Initialize(nullptr);
//    if (!OVR_SUCCESS(result))
//    {
//        qDebug() << "Failed to initialize libOVR.";
//        return;
//    }

//    result = ovr_Create(&m_p->m_session, &m_p->m_luid);
//    if (!OVR_SUCCESS(result))
//    {
//        qDebug() << "Failed to create session libOVR.";
//        return;
//    }

//    if (memcmp(&m_p->m_luid, &GetDefaultAdapterLuid(), sizeof(m_p->m_luid))) // If luid that the Rift is on is not the default adapter LUID...
//    {
//        qDebug() << "OpenGL supports only the default graphics adapter.";
//        return;
//    }

//    m_p->m_hmdDesc = ovr_GetHmdDesc(m_p->m_session);
}
