#ifndef _OSMAND_CORE_ATLAS_MAP_RENDERER_SKY_STAGE_OPENGL_H_
#define _OSMAND_CORE_ATLAS_MAP_RENDERER_SKY_STAGE_OPENGL_H_

#include "stdlib_common.h"

#include <glm/glm.hpp>

#include "QtExtensions.h"

#include "OsmAndCore.h"
#include "CommonTypes.h"
#include "AtlasMapRendererStage_OpenGL.h"
#include "GPUAPI_OpenGL.h"

namespace OsmAnd
{
    class AtlasMapRendererSkyStage_OpenGL : public AtlasMapRendererStage_OpenGL
    {
    private:
    protected:
        GLname _skyplaneVAO;
        GLname _skyplaneVBO;
        GLname _skyplaneIBO;

        struct Program {
            GLname id;

            struct {
                // Input data
                struct {
                    GLlocation vertexPosition;
                } in;

                // Parameters
                struct {
                    // Common data
                    GLlocation mProjectionViewModel;
                    GLlocation planeSize;
                } param;
            } vs;

            struct {
                // Parameters
                struct {
                    // Common data
                    GLlocation skyColor;
                } param;
            } fs;
        } _program;
    public:
        AtlasMapRendererSkyStage_OpenGL(AtlasMapRenderer_OpenGL* const renderer);
        virtual ~AtlasMapRendererSkyStage_OpenGL();

        virtual void initialize();
        virtual void render();
        virtual void release();
    };
}

#endif // !defined(_OSMAND_CORE_ATLAS_MAP_RENDERER_SKY_STAGE_OPENGL_H_)