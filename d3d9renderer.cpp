#include "d3d9renderer.h"

static const char * vertexShader_matrix = {
           "varying mediump vec2 interp_tc;\n"
           "attribute mediump vec4 in_pos;\n"
           "attribute mediump vec4 in_tc;\n"
           "\n"
           "void main() {\n"
           "    interp_tc = in_tc.xy;\n"
           "    gl_Position = in_pos;\n"
           "}\n"
    };

static const char * fragmentShader_matrix = {
    "uniform sampler2D frameTexture; \n"
    "varying mediump vec2 interp_tc;\n"
    "void main() \n"
    "{ \n"
    "    gl_FragColor = texture2D(frameTexture, interp_tc);\n"
    "}\n"
};

D3D9Renderer::D3D9Renderer() :
    m_program(nullptr)
{
    _allocProgram();
}

D3D9Renderer::~D3D9Renderer()
{
    _deallocProgram();
}

void D3D9Renderer::render()
{
    glDepthMask(true);

    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);


    if (!m_frame.isNull() && (m_program != nullptr)) {
        GLuint name;
        glGenTextures(1, &name);

        if (m_frame->map(name)) {
            m_program->bind();

            renderFrame();

            m_program->release();
            glFlush();
        }
        m_frame->unmap();
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void D3D9Renderer::initialize()
{
    initializeOpenGLFunctions();

    glClearColor(0.f, 0.f, 0.f, 1.0f);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void D3D9Renderer::renderFrame()
{
    m_program->setUniformValue("frameTexture", 0);

    int in_pos_attr = m_program->attributeLocation("in_pos");
    m_program->enableAttributeArray(in_pos_attr);
    m_program->setAttributeArray(in_pos_attr, in_pos.constData());

    int in_tc_attr = m_program->attributeLocation("in_tc");
    m_program->enableAttributeArray(in_tc_attr);
    m_program->setAttributeArray(in_tc_attr, in_tc.constData());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_program->disableAttributeArray(in_pos_attr);
    m_program->disableAttributeArray(in_tc_attr);
}

void D3D9Renderer::_allocProgram()
{
    m_program = new QOpenGLShaderProgram;

    QOpenGLShader * vertexShader = new QOpenGLShader(QOpenGLShader::Vertex, m_program);
    vertexShader->compileSourceCode(vertexShader_matrix);
    if (!m_program->addShader(vertexShader)) {
        qWarning() << "QOpenGLShaderProgram program could not add vertex shader";
        _deallocProgram();
        return;
    }

    QOpenGLShader * fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment, m_program);
    fragmentShader->compileSourceCode(fragmentShader_matrix);
    if (!m_program->addShader(fragmentShader)) {
        qWarning() << "QOpenGLShaderProgram program could not add fragment shader";
        _deallocProgram();
        return;
    }

    if (!m_program->link()) {
        qWarning() << Q_FUNC_INFO << "QOpenGLShaderProgram link error:" << m_program->log();
        _deallocProgram();
        return;
    }

    _initGeometries();
}

void D3D9Renderer::_deallocProgram()
{
    if (m_program != nullptr) delete m_program;
    m_program = nullptr;
}

void D3D9Renderer::_initGeometries()
{
    in_pos.clear();
    in_tc.clear();

    in_pos << QVector3D(-1.0, -1.0, 0);
    in_pos << QVector3D(1.0, -1.0, 0);
    in_pos << QVector3D(-1.0, 1.0, 0);

    in_pos << QVector3D(-1.0, 1.0, 0);
    in_pos << QVector3D(1.0, -1.0, 0);
    in_pos << QVector3D(1.0, 1.0, 0);

    in_tc << QVector3D(0.0, 0.0, 0);
    in_tc << QVector3D(1.0, 0.0, 0);
    in_tc << QVector3D(0.0, 1.0, 0);

    in_tc << QVector3D(0.0, 1.0, 0);
    in_tc << QVector3D(1.0, 0.0, 0);
    in_tc << QVector3D(1.0, 1.0, 0);
}
