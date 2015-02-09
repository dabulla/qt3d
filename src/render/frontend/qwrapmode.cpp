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

#include "qwrapmode.h"
#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QTextureWrapModePrivate : public QObjectPrivate
{
public:
    QTextureWrapModePrivate(QTextureWrapMode *qq)
        : QObjectPrivate()
        , q_ptr(qq)
        , m_x(QTextureWrapMode::ClampToEdge)
        , m_y(QTextureWrapMode::ClampToEdge)
        , m_z(QTextureWrapMode::ClampToEdge)
    {
    }

    Q_DECLARE_PUBLIC(QTextureWrapMode)
    QTextureWrapMode *q_ptr;
    QTextureWrapMode::WrapMode m_x;
    QTextureWrapMode::WrapMode m_y;
    QTextureWrapMode::WrapMode m_z;
};

/*!
    \class Qt3D::QTextureWrapMode
    \since 5.5

    \brief Qt3D::QTextureWrapMode defines the wrap mode a
    Qt3D::QAbstractTextureProvider should apply to a texture.
 */

QTextureWrapMode::QTextureWrapMode(WrapMode wrapMode, QObject *parent)
    : QObject(*new QTextureWrapModePrivate(this), parent)
{
    d_func()->m_x = wrapMode;
    d_func()->m_y = wrapMode;
    d_func()->m_z = wrapMode;
}

/*!
    Contrusts a new Qt3D::QTextureWrapMode instance with the wrap mode to apply to
    each dimension \a x, \a y \a z of the texture and \a parent as parent.
 */
QTextureWrapMode::QTextureWrapMode(WrapMode x,WrapMode y, WrapMode z, QObject *parent)
    : QObject(*new QTextureWrapModePrivate(this), parent)
{
    d_func()->m_x = x;
    d_func()->m_y = y;
    d_func()->m_z = z;
}

/*!
    Sets the wrap mode of the x dimension to \a x.
 */
void QTextureWrapMode::setX(WrapMode x)
{
    Q_D(QTextureWrapMode);
    if (d->m_x != x) {
        d->m_x = x;
        emit xChanged();
    }
}

/*!
    \returns the wrap mode of the x dimension.
 */
QTextureWrapMode::WrapMode QTextureWrapMode::x() const
{
    Q_D(const QTextureWrapMode);
    return d->m_x;
}

/*!
    Sets the wrap mode of the y dimension to \a y.
    \note this is not available on 1D textures.
 */
void QTextureWrapMode::setY(WrapMode y)
{
    Q_D(QTextureWrapMode);
    if (d->m_y != y) {
        d->m_y = y;
        emit yChanged();
    }
}

/*!
    \returns the wrap mode of the y dimension.
 */
QTextureWrapMode::WrapMode QTextureWrapMode::y() const
{
    Q_D(const QTextureWrapMode);
    return d->m_y;
}

/*!
    Sets the wrap mode of the z dimension to \a z.
    \note this is only available on 3D textures.
 */
void QTextureWrapMode::setZ(WrapMode z)
{
    Q_D(QTextureWrapMode);
    if (d->m_z != z) {
        d->m_z = z;
        emit zChanged();
    }
}

/*!
    \returns the wrap mode of the y dimension.
 */
QTextureWrapMode::WrapMode QTextureWrapMode::z() const
{
    Q_D(const QTextureWrapMode);
    return d->m_z;
}

} // Qt3D

QT_END_NAMESPACE
