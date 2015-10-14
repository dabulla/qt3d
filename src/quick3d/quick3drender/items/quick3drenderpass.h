/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
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

#ifndef QT3DRENDER_RENDER_QUICK_QUICK3DRENDERPASS_H
#define QT3DRENDER_RENDER_QUICK_QUICK3DRENDERPASS_H

#include <Qt3DQuickRender/qt3dquickrender_global.h>
#include <Qt3DRender/qrenderpass.h>
#include <QQmlListProperty>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

class QT3DQUICKRENDERSHARED_EXPORT Quick3DRenderPass : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DRender::QAnnotation> annotations READ annotationList)
    Q_PROPERTY(QQmlListProperty<Qt3DRender::QParameterMapping> bindings READ bindingList)
    Q_PROPERTY(QQmlListProperty<Qt3DRender::QRenderState> renderStates READ renderStateList)
    Q_PROPERTY(QQmlListProperty<Qt3DRender::QParameter> parameters READ parameterList)
public:
    explicit Quick3DRenderPass(QObject *parent = 0);

    QQmlListProperty<QAnnotation> annotationList();
    QQmlListProperty<QParameterMapping> bindingList();
    QQmlListProperty<QRenderState> renderStateList();
    QQmlListProperty<QParameter> parameterList();

    inline QRenderPass *parentRenderPass() const { return qobject_cast<QRenderPass *>(parent()); }

private:
    static void appendAnnotation(QQmlListProperty<QAnnotation> *list, QAnnotation *criterion);
    static QAnnotation *annotationAt(QQmlListProperty<QAnnotation> *list, int index);
    static int annotationsCount(QQmlListProperty<QAnnotation> *list);
    static void clearAnnotations(QQmlListProperty<QAnnotation> *list);

    static void appendBinding(QQmlListProperty<QParameterMapping> *list, QParameterMapping *binding);
    static QParameterMapping *bindingAt(QQmlListProperty<QParameterMapping> *list, int index);
    static int bindingsCount(QQmlListProperty<QParameterMapping> *list);
    static void clearBindings(QQmlListProperty<QParameterMapping> *list);

    static void appendRenderState(QQmlListProperty<QRenderState> *list, QRenderState *state);
    static QRenderState *renderStateAt(QQmlListProperty<QRenderState> *list, int index);
    static int renderStateCount(QQmlListProperty<QRenderState> *list);
    static void clearRenderStates(QQmlListProperty<QRenderState> *list);

    static void appendParameter(QQmlListProperty<QParameter> *list, QParameter *param);
    static QParameter *parameterAt(QQmlListProperty<QParameter> *list, int index);
    static int parametersCount(QQmlListProperty<QParameter> *list);
    static void clearParameterList(QQmlListProperty<QParameter> *list);
};

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QUICK_QUICK3DRENDERPASS_H