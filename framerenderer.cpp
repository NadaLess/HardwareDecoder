#include "framerenderer.h"

FrameRenderer::FrameRenderer()
{
    initShaderProgram();
    initGeometry();
}

FrameRenderer::~FrameRenderer()
{
    glDeleteTextures(1, &m_texture);
}

void FrameRenderer::initialize()
{
    initializeOpenGLFunctions();

    glClearColor(0.f, 0.f, 0.f, 1.0f);

    glGenTextures(1,&m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void FrameRenderer::setFrame(VideoFramePtr frame)
{
    m_frame = frame;
}

void FrameRenderer::initGeometry()
{
    m_vertices.clear();
    m_vertices.clear();

    m_vertices << QVector3D(-1.0, -1.0, 0);
    m_vertices << QVector3D(1.0, -1.0, 0);
    m_vertices << QVector3D(-1.0, 1.0, 0);

    m_vertices << QVector3D(-1.0, 1.0, 0);
    m_vertices << QVector3D(1.0, -1.0, 0);
    m_vertices << QVector3D(1.0, 1.0, 0);

    m_normals << QVector3D(0.0, 0.0, 0);
    m_normals << QVector3D(1.0, 0.0, 0);
    m_normals << QVector3D(0.0, 1.0, 0);

    m_normals << QVector3D(0.0, 1.0, 0);
    m_normals << QVector3D(1.0, 0.0, 0);
    m_normals << QVector3D(1.0, 1.0, 0);
}

void FrameRenderer::render() {

    glDepthMask(GL_TRUE);

    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    if (!m_frame.isNull())
        m_frame->map(m_texture);

    m_shaderProgram.setUniformValue("frameTexture", m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_shaderProgram.bind();

    m_shaderProgram.enableAttributeArray(m_in_tc);
    m_shaderProgram.enableAttributeArray(m_in_pos);
    m_shaderProgram.setAttributeArray(m_in_pos, m_vertices.constData());
    m_shaderProgram.setAttributeArray(m_in_tc, m_normals.constData());

    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());

    m_shaderProgram.disableAttributeArray(m_in_tc);
    m_shaderProgram.disableAttributeArray(m_in_pos);

    m_shaderProgram.release();

    if (!m_frame.isNull())
        m_frame->unmap();

    glFlush();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void FrameRenderer::initShaderProgram()
{
    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, &m_shaderProgram);
    const char *vsrc =
            "varying mediump vec2 interp_tc;\n"
            "attribute mediump vec4 in_pos;\n"
            "attribute mediump vec4 in_tc;\n"
            "\n"
            "void main() {\n"
            "    interp_tc = in_tc.xy;\n"
            "    gl_Position = in_pos;\n"
            "}\n";
    vshader->compileSourceCode(vsrc);

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, &m_shaderProgram);
    const char *fsrc =
            "uniform sampler2D frameTexture; \n"
            "varying mediump vec2 interp_tc;\n"
            "void main() \n"
            "{ \n"
            "    gl_FragColor = texture2D(frameTexture, interp_tc);\n"
            "}\n";
    fshader->compileSourceCode(fsrc);

    m_shaderProgram.addShader(vshader);
    m_shaderProgram.addShader(fshader);
    m_shaderProgram.link();

    m_in_pos = m_shaderProgram.attributeLocation("in_pos");
    m_in_tc = m_shaderProgram.attributeLocation("in_tc");
}
