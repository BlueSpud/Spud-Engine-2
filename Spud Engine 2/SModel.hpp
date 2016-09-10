//
//  SModel.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/2/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SMesh_hpp
#define SMesh_hpp

#include <glm/glm.hpp>

#include "SResourceManager.hpp"
#include "SGL.hpp"

/******************************************************************************
 *  Definition for model                                                      *
 ******************************************************************************/

class SModel : public SResource {
    
    public:
    
        static SResource* allocate();
    
        // TEMP! they shouldnt be public
        GLuint verts_id;
        GLuint normals_id;
        GLuint tex_coords_id;
        GLuint indicies_id;
        unsigned int face_count;
    
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
    
    
    
};

#endif /* SModel_hpp */
