#include "Rope.h"
#include "ShaderManager.h"
#include "Shaders.h"

//======================================================================================================================
Rope::Rope() : mColour(1,1,1,0.1f)
{
    TRACE_METHOD_ONLY(1);
    RenderManager::GetInstance().RegisterRenderObject(this, RENDER_LEVEL_OBJECTS);

}

//======================================================================================================================
Rope::~Rope()
{
    TRACE_METHOD_ONLY(1);
    RenderManager::GetInstance().UnregisterRenderObject(this, RENDER_LEVEL_OBJECTS);
}

//======================================================================================================================
void Rope::RenderUpdate(class Viewport* viewport, int renderLevel)
{
    TRACE_METHOD_ONLY(2);
    size_t numPoints = mPoints.size();

    if (numPoints < 2)
        return;

    EnableBlend enableBlend;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    const SimpleShader* simpleShader = (SimpleShader*) ShaderManager::GetInstance().GetShader(SHADER_SIMPLE);

    if (gGLVersion == 1)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, &mPoints[0].x);
        glColor4f(mColour.x, mColour.y, mColour.z, mColour.w);
    }
    else
    {
        simpleShader->Use();

        // Get the variable locations
        glVertexAttribPointer(simpleShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, &mPoints[0].x);
        glEnableVertexAttribArray(simpleShader->a_position);

        glVertexAttrib4fv(simpleShader->a_colour, &mColour.x);
        glDisableVertexAttribArray(simpleShader->a_colour);
    }

    esSetModelViewProjectionMatrix(simpleShader->u_mvpMatrix);
    glDrawArrays(GL_LINE_STRIP, 0, numPoints);

    if (gGLVersion == 1)
        glDisableClientState(GL_VERTEX_ARRAY);
    else
        glDisableVertexAttribArray(simpleShader->a_position);

}

