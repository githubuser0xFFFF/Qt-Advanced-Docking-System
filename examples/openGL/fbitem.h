// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef FBITEM_H
#define FBITEM_H

#include <QQuickFramebufferObject>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QVector3D>
#include "logo.h"

struct StateBinder;

class FbItemRenderer : public QQuickFramebufferObject::Renderer
{
public:
    FbItemRenderer(bool multisample);
    void synchronize(QQuickFramebufferObject *item) override;
    void render() override;
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

private:
    void ensureInit();
    void initBuf();
    void setupVertexAttribs();
    void initProgram();
    void updateDirtyUniforms();

    bool m_inited;
    bool m_multisample;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_baseWorld;
    QMatrix4x4 m_world;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_logoVbo;
    Logo m_logo;
    QScopedPointer<QOpenGLShaderProgram> m_program;
    int m_projMatrixLoc;
    int m_camMatrixLoc;
    int m_worldMatrixLoc;
    int m_normalMatrixLoc;
    int m_lightPosLoc;
    QVector3D m_rotation;

    enum Dirty {
        DirtyProjection = 0x01,
        DirtyCamera = 0x02,
        DirtyWorld = 0x04,
        DirtyLight = 0x08,
        DirtyAll = 0xFF
    };
    int m_dirty;

    friend struct StateBinder;
};

class FbItem : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(QVector3D eye READ eye WRITE setEye)
    Q_PROPERTY(QVector3D target READ target WRITE setTarget)
    Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation)
    Q_PROPERTY(bool multisample READ multisample WRITE setMultisample)
    QML_ELEMENT

public:
    explicit FbItem(QQuickItem *parent = nullptr);

    QQuickFramebufferObject::Renderer *createRenderer() const override;

    QVector3D eye() const { return m_eye; }
    void setEye(const QVector3D &v);
    QVector3D target() const { return m_target; }
    void setTarget(const QVector3D &v);

    QVector3D rotation() const { return m_rotation; }
    void setRotation(const QVector3D &v);

    enum SyncState {
        CameraNeedsSync = 0x01,
        RotationNeedsSync = 0x02,
        AllNeedsSync = 0xFF
    };
    int swapSyncState();

    bool multisample() const { return m_multisample; }
    void setMultisample(bool m) { m_multisample = m; }

private:
    QVector3D m_eye;
    QVector3D m_target;
    QVector3D m_rotation;
    int m_syncState;
    bool m_multisample;
};

#endif // FBITEM_H
