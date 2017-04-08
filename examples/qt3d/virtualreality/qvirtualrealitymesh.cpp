/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef _USE_MATH_DEFINES
# define _USE_MATH_DEFINES // For MSVC
#endif

// TO DO: Refactor from Cone

#include "qvirtualrealitymesh.h"
#include "qvirtualrealitygeometry.h"
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/qbufferdatagenerator.h>
#include <Qt3DRender/qattribute.h>
#include <qmath.h>
#include <QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

/*!
 * \qmltype VirtualrealityMesh
 * \instantiates Qt3DExtras::QVirtualrealityMesh
 * \inqmlmodule Qt3D.Extras
 * \brief A conical mesh.
 */

/*!
 * \qmlproperty int VirtualrealityMesh::rings
 *
 * Holds the number of rings in the mesh.
 */

/*!
 * \qmlproperty int VirtualrealityMesh::slices
 *
 * Holds the number of slices in the mesh.
 */

/*!
 * \qmlproperty bool VirtualrealityMesh::hasTopEndcap
 *
 * Determines if the virtualreality top is capped or open.
 */

/*!
 * \qmlproperty bool VirtualrealityMesh::hasBottomEndcap
 *
 * Determines if the virtualreality bottom is capped or open.
 */

/*!
 * \qmlproperty real VirtualrealityMesh::topRadius
 *
 * Holds the top radius of the virtualreality.
 */

/*!
 * \qmlproperty real VirtualrealityMesh::bottomRadius
 *
 * Holds the bottom radius of the virtualreality.
 */

/*!
 * \qmlproperty real VirtualrealityMesh::length
 *
 * Holds the length of the virtualreality.
 */

/*!
 * \class Qt3DExtras::QVirtualrealityMesh
 * \inheaderfile Qt3DExtras/QVirtualrealityMesh
 * \inmodule Qt3DExtras
 *
 * \inherits Qt3DRender::QGeometryRenderer
 *
 * \brief A conical mesh.
 */

QVirtualrealityMesh::QVirtualrealityMesh(QNode *parent)
    : QGeometryRenderer(parent)
{
    QVirtualRealityGeometry *geometry = new QVirtualRealityGeometry(this);
    QObject::connect(geometry, &QVirtualRealityGeometry::hasTopEndcapChanged, this, &QVirtualrealityMesh::hasTopEndcapChanged);
    QObject::connect(geometry, &QVirtualRealityGeometry::hasBottomEndcapChanged, this, &QVirtualrealityMesh::hasBottomEndcapChanged);
    QObject::connect(geometry, &QVirtualRealityGeometry::topRadiusChanged, this, &QVirtualrealityMesh::topRadiusChanged);
    QObject::connect(geometry, &QVirtualRealityGeometry::bottomRadiusChanged, this, &QVirtualrealityMesh::bottomRadiusChanged);
    QObject::connect(geometry, &QVirtualRealityGeometry::ringsChanged, this, &QVirtualrealityMesh::ringsChanged);
    QObject::connect(geometry, &QVirtualRealityGeometry::slicesChanged, this, &QVirtualrealityMesh::slicesChanged);
    QObject::connect(geometry, &QVirtualRealityGeometry::lengthChanged, this, &QVirtualrealityMesh::lengthChanged);

    QGeometryRenderer::setGeometry(geometry);
}

/*! \internal */
QVirtualrealityMesh::~QVirtualrealityMesh()
{
}

void QVirtualrealityMesh::setHasTopEndcap(bool hasTopEndcap)
{
    static_cast<QVirtualRealityGeometry *>(geometry())->setHasTopEndcap(hasTopEndcap);
}

void QVirtualrealityMesh::setHasBottomEndcap(bool hasBottomEndcap)
{
    static_cast<QVirtualRealityGeometry *>(geometry())->setHasBottomEndcap(hasBottomEndcap);
}

void QVirtualrealityMesh::setTopRadius(float topRadius)
{
    static_cast<QVirtualRealityGeometry *>(geometry())->setTopRadius(topRadius);
}

void QVirtualrealityMesh::setBottomRadius(float bottomRadius)
{
    static_cast<QVirtualRealityGeometry *>(geometry())->setBottomRadius(bottomRadius);
}

void QVirtualrealityMesh::setRings(int rings)
{
    static_cast<QVirtualRealityGeometry *>(geometry())->setRings(rings);
}

void QVirtualrealityMesh::setSlices(int slices)
{
    static_cast<QVirtualRealityGeometry *>(geometry())->setSlices(slices);
}

void QVirtualrealityMesh::setLength(float length)
{
    static_cast<QVirtualRealityGeometry *>(geometry())->setLength(length);
}

/*!
 * \property QVirtualrealityMesh::hasTopEndcap
 *
 * Determines if the virtualreality top is capped or open.
 */
bool QVirtualrealityMesh::hasTopEndcap() const
{
    return static_cast<QVirtualRealityGeometry *>(geometry())->hasTopEndcap();
}

/*!
 * \property QVirtualrealityMesh::hasBottomEndcap
 *
 * Determines if the virtualreality bottom is capped or open.
 */
bool QVirtualrealityMesh::hasBottomEndcap() const
{
    return static_cast<QVirtualRealityGeometry *>(geometry())->hasBottomEndcap();
}

/*!
 * \property QVirtualrealityMesh::topRadius
 *
 * Holds the top radius of the virtualreality.
 */
float QVirtualrealityMesh::topRadius() const
{
    return static_cast<QVirtualRealityGeometry *>(geometry())->topRadius();
}

/*!
 * \property QVirtualrealityMesh::bottomRadius
 *
 * Holds the bottom radius of the virtualreality.
 */
float QVirtualrealityMesh::bottomRadius() const
{
    return static_cast<QVirtualRealityGeometry *>(geometry())->bottomRadius();
}

/*!
 * \property QVirtualrealityMesh::rings
 *
 * Holds the number of rings in the mesh.
 */
int QVirtualrealityMesh::rings() const
{
    return static_cast<QVirtualRealityGeometry *>(geometry())->rings();
}

/*!
 * \property QVirtualrealityMesh::slices
 *
 * Holds the number of slices in the mesh.
 */
int QVirtualrealityMesh::slices() const
{
    return static_cast<QVirtualRealityGeometry *>(geometry())->slices();
}

/*!
 * \property QVirtualrealityMesh::length
 *
 * Holds the length of the virtualreality.
 */
float QVirtualrealityMesh::length() const
{
    return static_cast<QVirtualRealityGeometry *>(geometry())->length();
}

} // namespace Qt3DExtras

QT_END_NAMESPACE
