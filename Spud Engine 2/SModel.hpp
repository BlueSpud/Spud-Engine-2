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
#include "SGL.hpp"

/******************************************************************************
 *  Definition for enum for VBOs                                              *
 ******************************************************************************/

enum SModelBuffers {
    
    buffer_position,
    buffer_normal,
    buffer_tex_coord,
    //buffer_indicies
    
    buffer_count
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
    
        virtual void upload();
    
    private:
    
        // Storage for the data before we upload it
        float* verts;
        float* normals;
        float* tex_coords;
    
        unsigned int* indicies;
    
        unsigned int face_count;
    
        // Storage for the VAO
        GLuint array_id;
    
        // Storage for the VBOs
        GLuint buffer_ids[buffer_count];
    
};

#endif /* SModel_hpp */
