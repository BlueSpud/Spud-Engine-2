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

/******************************************************************************
 *  Definition for enum for VBOs                                              *
 ******************************************************************************/

enum SModelBuffers {
    
    buffer_position,
    buffer_normal,
    buffer_tex_coord,
    buffer_tangent,
    //buffer_indicies
    
    buffer_count
};

/******************************************************************************
 *  Definition for upload                                                     *
 ******************************************************************************/

struct SModelUpload : public SGLUpload {
    
    virtual void upload();
    virtual void unload();
    
    float* verts;
    float* normals;
    float* tex_coords;
    float* tangents;
    
    unsigned int* indicies;
    
    unsigned int face_count;
    
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
 *  Definition for model                                                      *
 ******************************************************************************/

class SModel : public SResource {
    
    public:
    
        static SResource* allocate();
    
        void render();
    
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
        virtual void hotload(const SPath& path);
    
    private:
    
        SFile* file;
    
        // Storage for the data before we upload it
        float* verts;
        float* normals;
        float* tex_coords;
        float* tangents;
    
        unsigned int* indicies;
    
        unsigned int face_count;
    
        // Storage for the VAO
        GLuint array_id;
    
        // Storage for the VBOs
        GLuint buffer_ids[buffer_count];
    
        SModelUpload* upload;
    
};

#endif /* SModel_hpp */
