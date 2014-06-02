#include "VBO.hpp"
#include "GraphicDefs.hpp"
#include "OpenglErrorChecker.hpp"

namespace deimos
{

    struct GLTargetToVBOTarget
    {
        int vbotarget;
        int glTarget;
    };

    struct GLModeToVBO
    {
        int vboMode;
        int glMode;
    };

    struct GLDataTypeToVBO
    {
        int vboType;
        int glType;
    };

    //default behavior is to draw lines!!!
    static GLModeToVBO glToVBOMode[] =
    {
        { Primitives::POINTS,           GL_LINES },
        { Primitives::LINES,            GL_LINES },
        { Primitives::LINE_STRIP,       GL_LINE_STRIP },
        { Primitives::LINE_LOOP,        GL_LINE_LOOP },
        { Primitives::TRIANGLES,        GL_TRIANGLES },
        { Primitives::TRIANGLE_STRIP,   GL_TRIANGLE_STRIP },
        { Primitives::TRIANGLE_FAN,     GL_TRIANGLE_FAN},
        { Primitives::QUADS,            GL_LINES },
        { Primitives::QUAD_STRIP,       GL_LINES },
        { Primitives::POLYGON,          GL_LINES }
    };

    static GLTargetToVBOTarget glToVBOTarget[] =
    {
        { VBOTarget::STATIC,  GL_STATIC_DRAW },
        { VBOTarget::DYNAMIC, GL_DYNAMIC_DRAW },
        { VBOTarget::STREAM,  GL_STREAM_DRAW }
    };

    static GLDataTypeToVBO glDataTypeToVBO[] =
    {
        { VBODataType::SHORT,   GL_SHORT },
        { VBODataType::INT,     GL_INT },
        { VBODataType::FLOAT,   GL_FLOAT },
        { VBODataType::DOUBLE,  GL_DOUBLE }
    };

    VBO::VBO() :
        m_created(false),
        m_id(0),
        m_size(0)
    {}

    VBO::VBO(const VBOData& data, int vboTarget,
        const VBOConfig& vertexConfig,
        const VBOConfig& colorConfig,
        const VBOConfig& textureConfig,
        bool freeClientData)
    {
        create();
        upload(data, vboTarget);
        configVertex(vertexConfig);
        configColor(colorConfig);
        configTexture(textureConfig);
    }

    VBO::~VBO()
    {
        destroy();
    }

    void VBO::create()
    {
        if (m_created)
            destroy();

        DEIMOS_GL_CHECK(::glGenBuffers(1, &m_id));
        m_created = true;
        DEIMOS_GL_CHECK(::glEnableClientState(GL_VERTEX_ARRAY));
        DEIMOS_GL_CHECK(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
    }

    void VBO::destroy()
    {
        if (!m_created)
            return;

        ::glDeleteBuffers(1, &m_id);
        m_created = false;
        m_size = 0;
    }

    void VBO::bind() const
    {
        if (m_created)
            DEIMOS_GL_CHECK(::glBindBuffer(GL_ARRAY_BUFFER, m_id));
    }

    void VBO::unbind() const
    {
        //that is the proof that id is always > 0
        DEIMOS_GL_CHECK(::glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    void VBO::upload(const VBOData& data, int vboTarget)
    {
        //TODO: throw exception
        if (!m_created || data.empty())
            return;

        if (m_size <= 0)
            m_size = data.size();
        
        bind();
        DEIMOS_GL_CHECK(::glBufferData(GL_ARRAY_BUFFER, data.size(),
            static_cast<const void*>(&data[0]),
            glToVBOTarget[vboTarget].glTarget));
    }

    void VBO::draw(int drawMode, int start, int count) const
    {
        if (!m_created)
            return;

        if (start < 0)
            return; //TODO: throw exceptions

        if (count <= 0)
            count = m_size;

        //TODO: assertion HERE!!!: drawMode < LINES and drawMode > TRIANGLE_FAN

        bind();
        DEIMOS_GL_CHECK(::glDrawArrays(glToVBOMode[drawMode].glMode, start, count));
    }

    void VBO::configVertex(const VBOConfig& cfg) const
    {
        if (!m_created)
            return;

        bind();
        DEIMOS_GL_CHECK(::glVertexPointer(cfg.size,
            glDataTypeToVBO[cfg.type].glType,
            cfg.stride,
            reinterpret_cast<void *>(cfg.pointer)));
    }

    void VBO::configTexture(const VBOConfig& cfg) const
    {
        if (!m_created)
            return;

        bind();
        DEIMOS_GL_CHECK(::glTexCoordPointer(cfg.size,
            glDataTypeToVBO[cfg.type].glType,
            cfg.stride,
            reinterpret_cast<void *>(cfg.pointer)));
    }

    void VBO::configColor(const VBOConfig& cfg) const
    {
        if (!m_created)
            return;

        bind();
        DEIMOS_GL_CHECK(::glColorPointer(cfg.size,
            glDataTypeToVBO[cfg.type].glType,
            cfg.stride,
            reinterpret_cast<void *>(cfg.pointer)));
    }
}