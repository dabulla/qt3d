/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlogicaldevice.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DInput/qaction.h>
#include <Qt3DInput/qaxis.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QLogicalDevicePrivate : public Qt3DCore::QNodePrivate
{
public:
    QLogicalDevicePrivate()
        : Qt3DCore::QNodePrivate()
    {}

    QVector<QAction *> m_actions;
    QVector<QAxis *> m_axes;
};

QLogicalDevice::QLogicalDevice(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(*new QLogicalDevicePrivate(), parent)
{
}

QLogicalDevice::~QLogicalDevice()
{
    QNode::cleanup();
}

void QLogicalDevice::addAction(QAction *action)
{
    Q_D(QLogicalDevice);
    if (!d->m_actions.contains(action))
        d->m_actions.push_back(action);
    // TO DO: needs to be completed to set the parent and send a proper notification
}

void QLogicalDevice::removeAction(QAction *action)
{
    Q_D(QLogicalDevice);
    if (d->m_actions.contains(action)) {
        d->m_actions.removeOne(action);
        // TO DO: needs to be completed to send a proper notification
    }
}

QVector<QAction *> QLogicalDevice::actions() const
{
    Q_D(const QLogicalDevice);
    return d->m_actions;
}

void QLogicalDevice::addAxis(QAxis *axis)
{
    Q_D(QLogicalDevice);
    if (!d->m_axes.contains(axis)) {
        d->m_axes.push_back(axis);
        // TO DO: needs to be completed to set the parent and send a proper notification
    }
}

void QLogicalDevice::removeAxis(QAxis *axis)
{
    Q_D(QLogicalDevice);
    if (d->m_axes.contains(axis)) {
        d->m_axes.removeOne(axis);
        // TO DO: needs to be completed to send a proper notification
    }
}

QVector<QAxis *> QLogicalDevice::axes() const
{
    Q_D(const QLogicalDevice);
    return d->m_axes;
}

void QLogicalDevice::copy(const Qt3DCore::QNode *ref)
{
    QNode::copy(ref);
    const QLogicalDevice *device = static_cast<const QLogicalDevice *>(ref);
    Q_FOREACH (QAction *action, device->actions())
        d_func()->m_actions.push_back(qobject_cast<QAction *>(QNode::clone(action)));
}

} // Qt3DInput

QT_END_NAMESPACE
