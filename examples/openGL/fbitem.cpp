// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "fbitem.h"
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QMatrix4x4>

FbItem::FbItem(QQuickItem *parent)
    : QQuickFramebufferObject(parent),
      m_target(0, 0, -1),
      m_syncState(AllNeedsSync),
      m_multisample(false)
{
}

QQuickFramebufferObject::Renderer *FbItem::createRenderer() const
{
    return new FbItemRenderer(m_multisample);
}

void FbItem::setEye(const QVector3D &v)
{
    if (m_eye != v) {
        m_eye = v;
        m_syncState |= CameraNeedsSync;
        update();
    }
}

void FbItem::setTarget(const QVector3D &v)
{
    if (m_target != v) {
        m_target = v;
        m_syncState |= CameraNeedsSync;
        update();
    }
}

void FbItem::setRotation(const QVector3D &v)
{
    if (m_rotation != v) {
        m_rotation = v;
        m_syncState |= RotationNeedsSync;
        update();
    }
}

int FbItem::swapSyncState()
{
    int s = m_syncState;
    m_syncState = 0;
    return s;
}

FbItemRenderer::FbItemRenderer(bool multisample)
    : m_inited(false),
      m_multisample(multisample),
      m_dirty(DirtyAll)
{
    m_camera.setToIdentity();
    m_baseWorld.setToIdentity();
    m_baseWorld.translate(0, 0, -1);
    m_world = m_baseWorld;
}

void FbItemRenderer::synchronize(QQuickFramebufferObject *qfbitem)
{
    FbItem *item = static_cast<FbItem *>(qfbitem);
    int syncState = item->swapSyncState();
    if (syncState & FbItem::CameraNeedsSync) {
        m_camera.setToIdentity();
        m_camera.lookAt(item->eye(), item->eye() + item->target(), QVector3D(0, 1, 0));
        m_dirty |= DirtyCamera;
    }
    if (syncState & FbItem::RotationNeedsSync) {
        m_rotation = item->rotation();
        m_dirty |= DirtyWorld;
    }
}

struct StateBinder
{
    StateBinder(FbItemRenderer *r)
        : m_r(r) {
        QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
        f->glEnable(GL_DEPTH_TEST);
        f->glEnable(GL_CULL_FACE);
        f->glDepthMask(GL_TRUE);
        f->glDepthFunc(GL_LESS);
        f->glFrontFace(GL_CCW);
        f->glCullFace(GL_BACK);
        m_r->m_program->bind();
    }
    ~StateBinder() {
        QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
        m_r->m_program->release();
        f->glDisable(GL_CULL_FACE);
        f->glDisable(GL_DEPTH_TEST);
    }
    FbItemRenderer *m_r;
};

void FbItemRenderer::updateDirtyUniforms()
{
    if (m_dirty & DirtyProjection)
        m_program->setUniformValue(m_projMatrixLoc, m_proj);

    if (m_dirty & DirtyCamera)
        m_program->setUniformValue(m_camMatrixLoc, m_camera);

    if (m_dirty & DirtyWorld) {
        m_program->setUniformValue(m_worldMatrixLoc, m_world);
        QMatrix3x3 normalMatrix = m_world.normalMatrix();
        m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);
    }

    if (m_dirty & DirtyLight)
        m_program->setUniformValue(m_lightPosLoc, QVector3D(0, 0, 70));

    m_dirty = 0;
}

void FbItemRenderer::render()
{
    ensureInit();

    if (m_vao.isCreated())
        m_vao.bind();
    else
        setupVertexAttribs();

    StateBinder state(this);

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0, 0, 0, 0);
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_dirty & DirtyWorld) {
        m_world = m_baseWorld;
        m_world.rotate(m_rotation.x(), 1, 0, 0);
        m_world.rotate(m_rotation.y(), 0, 1, 0);
        m_world.rotate(m_rotation.z(), 0, 0, 1);
    }

    updateDirtyUniforms();

    f->glDrawArrays(GL_TRIANGLES, 0, m_logo.vertexCount());

    if (m_vao.isCreated())
        m_vao.release();
}

QOpenGLFramebufferObject *FbItemRenderer::createFramebufferObject(const QSize &size)
{
    m_dirty |= DirtyProjection;
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(size.width()) / size.height(), 0.01f, 100.0f);

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(m_multisample ? 4 : 0);
    return new QOpenGLFramebufferObject(size, format);
}

void FbItemRenderer::ensureInit()
{
    if (m_inited)
        return;

    m_inited = true;

    initBuf();
    initProgram();
}

void FbItemRenderer::initBuf()
{
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    m_logoVbo.create();
    m_logoVbo.bind();
    m_logoVbo.allocate(m_logo.constData(), m_logo.count() * sizeof(GLfloat));

    setupVertexAttribs();
}

void FbItemRenderer::setupVertexAttribs()
{
    m_logoVbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    m_logoVbo.release();
}

static const char *vertexShaderSource =
    "attribute vec4 vertex;\n"
    "attribute vec3 normal;\n"
    "varying vec3 vert;\n"
    "varying vec3 vertNormal;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 camMatrix;\n"
    "uniform mat4 worldMatrix;\n"
    "uniform mat3 normalMatrix;\n"
    "void main() {\n"
    "   vert = vertex.xyz;\n"
    "   vertNormal = normalMatrix * normal;\n"
    "   gl_Position = projMatrix * camMatrix * worldMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying highp vec3 vert;\n"
    "varying highp vec3 vertNormal;\n"
    "uniform highp vec3 lightPos;\n"
    "void main() {\n"
    "   highp vec3 L = normalize(lightPos - vert);\n"
    "   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
    "   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
    "   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
    "   gl_FragColor = vec4(col, 1.0);\n"
    "}\n";

void FbItemRenderer::initProgram()
{
    m_program.reset(new QOpenGLShaderProgram);
    m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->link();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_camMatrixLoc = m_program->uniformLocation("camMatrix");
    m_worldMatrixLoc = m_program->uniformLocation("worldMatrix");
    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    m_lightPosLoc = m_program->uniformLocation("lightPos");
}
