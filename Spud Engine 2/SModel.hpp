//
//  SModel.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/2/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SModel_hpp
#define SModel_hpp

#include <glm/glm.hpp>

#include "SResourceManager.hpp"
#include "SGLUpload.hpp"

#define MODEL_BOUNDING_BOX_PADDING 0.1f

/******************************************************************************
 *  Definition for enum for VBOs                                              *
 ******************************************************************************/

enum SModelBuffers {
    
    buffer_position,
    buffer_normal,
    buffer_tex_coord,
    buffer_tangent,
    buffer_indicies,
    
    buffer_count
};

/******************************************************************************
 *  Definition for upload                                                     *
 ******************************************************************************/

struct SModelUpload : public SGLUpload {
    
    virtual void upload();
    virtual void unload();
    
    glm::vec3* verts;
    glm::vec3* normals;
    glm::vec2* tex_coords;
    glm::vec3* tangents;
    
    std::vector<glm::ivec3>* indicies;
    
    unsigned int face_count;
    unsigned int vertex_count;
    
    // Storage for the VAO
    GLuint* array_id;
    
    // Storage for the VBOs
    GLuint* buffer_ids;
    
};

/******************************************************************************
 *  Definition for unload                                                     *
 ******************************************************************************/

struct SModelUnload : public SGLUpload {
  
    virtual void upload();
    virtual void unload();
    
    GLuint array_id;
    GLuint buffer_ids[buffer_count];
    
};

/******************************************************************************
 *  Declaration for tokens                                                    *
 ******************************************************************************/

#define END_OF_FILE_TOKEN 0x00
#define NEW_MATERIAL_TOKEN 0xFF

/******************************************************************************
 *  Definition for model                                                      *
 ******************************************************************************/

class SModel : public SResource {
    
    public:
    
        static SResource* allocate();
    
        void render();
        void getModelExtents(glm::vec3& _mins, glm::vec3& _maxes);

    
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
        virtual void hotload(const SPath& path);
    
    private:
    
        SFile* file;
    
        // Storage for the data before we upload it
        glm::vec3* verts;
        glm::vec3* normals;
        glm::vec2* tex_coords;
        glm::vec3* tangents;
    
        std::vector<glm::ivec3>* indicies;
    
        std::vector<unsigned int> draw_calls;
    
        // Storage for the VAO
        GLuint array_id;
    
        // Storage for the VBOs
        GLuint buffer_ids[buffer_count];
    
        SModelUpload* upload;
    
        // Min and maxes of the model
        glm::vec3 mins;
        glm::vec3 maxes;
    
};

#endif /* SModel_hpp */
