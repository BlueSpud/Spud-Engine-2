//
//  SStaticMesh.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/2/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SStaticMesh_hpp
#define SStaticMesh_hpp

#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

#include "SResourceManager.hpp"
#include "SGLUpload.hpp"
#include "SMaterial.hpp"

#define MODEL_BOUNDING_BOX_PADDING 0.1f

// Forward declarations
class SStaticMeshInstance;

/******************************************************************************
 *  Definition for enum for VBOs                                              *
 ******************************************************************************/

enum SStaticMeshBuffers {
    
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

struct SStaticMeshUpload : public SGLUpload {
    
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

struct SStaticMeshUnload : public SGLUpload {
  
    virtual void upload();
    virtual void unload();
    
    GLuint array_id;
    GLuint buffer_ids[buffer_count];
    
};

/******************************************************************************
 *  Declaration for tokens                                                    *
 ******************************************************************************/

#define END_OF_FILE_TOKEN 0x00
#define COLLISION_TOKEN 0x01
#define NEW_MATERIAL_TOKEN 0xFF

/******************************************************************************
 *  Definition for static mesh                                                *
 ******************************************************************************/

class SStaticMesh : public SResource {
    
    friend class SStaticMeshInstance;
    
    public:
    
        static SResource* allocate();
        virtual SResource* resource();
    
        void render(bool render_material, const std::vector<SMaterial*>& instance_material);
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
    
        std::vector<SMaterial*> materials;
        std::vector<unsigned int> draw_calls;
    
        // Storage for the VAO
        GLuint array_id;
    
        // Storage for the VBOs
        GLuint buffer_ids[buffer_count];
    
        SStaticMeshUpload* upload;
    
        // Min and maxes of the model
        glm::vec3 mins;
        glm::vec3 maxes;
    
        // Collision stuff
        btConvexHullShape* collision_shape;
    
};

#endif /* SStaticMesh_hpp */
