//
//  SModel.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/2/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SStaticMesh_hpp
#define SStaticMesh_hpp

#include "Resource/SGLUpload.hpp"
#include "SMaterial.hpp"

#include "Physics/SPhysicsSystem.hpp"

#define MODEL_BOUNDING_BOX_PADDING 0.1f

/******************************************************************************
 *  Definition for enum for VBOs                                              *
 ******************************************************************************/

#define SKINNING_DATA_COUNT 2

enum SStaticMeshBuffers {
    
    buffer_position,
    buffer_normal,
    buffer_tex_coord,
    buffer_tangent,
	buffer_bone_indicies,
	buffer_weights,
    
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
	
	glm::vec4* vertex_weights;
	glm::vec4* bone_indicies;
    
    std::vector<glm::ivec3>* indicies;
    
    unsigned int face_count;
    unsigned int vertex_count;
	unsigned int m_buffer_count;
    
    // Storage for the VAO
    GLuint* array_id;
    
    // Storage for the VBOs
    GLuint* buffer_ids;
	GLuint* indicies_id;
    
};

/******************************************************************************
 *  Definition for unload                                                     *
 ******************************************************************************/

struct SModelUnload : public SGLUpload {
  
    virtual void upload();
    virtual void unload();
    
    GLuint array_id;
    GLuint* buffer_ids;
	GLuint indicies_id;
	
	unsigned int buffer_count;
    
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

class SModel : public SResource {
    
    friend class SStaticMesh;

	SRESOURCE(SModel, "smdl")

    public:
    
		void render(const std::vector<std::shared_ptr<SMaterial>>& instance_material);
		void render(std::shared_ptr<SGbufferShader> shader);
	
        void getModelExtents(glm::vec3& _mins, glm::vec3& _maxes);
    
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
        virtual void hotload(const SPath& path);
    
        SFile* file;
    
        // Storage for the data before we upload it
        glm::vec3* verts;
        glm::vec3* normals;
        glm::vec2* tex_coords;
        glm::vec3* tangents;
	
		unsigned int vertex_count;
        std::vector<glm::ivec3>* indicies;
    
        std::vector<std::shared_ptr<SMaterial>> materials;
        std::vector<unsigned int> draw_calls;
		unsigned int index_count;
	
        // Storage for the VAO
        GLuint array_id;
    
        // Storage for the VBOs
        GLuint buffer_ids[buffer_count];
		unsigned int m_buffer_count;
		GLuint indicies_id;
	
        SModelUpload* upload;
    
        // Min and maxes of the model
        glm::vec3 mins;
        glm::vec3 maxes;
    
        // Collision stuff
        physx::PxTriangleMeshGeometry* collision_geometry = nullptr;
        physx::PxTriangleMesh* collision_mesh = nullptr;
    
        physx::PxConvexMeshGeometry* dynamic_collision_geometry = nullptr;
        physx::PxConvexMesh* dynamic_collision_mesh = nullptr;
    
};

#endif /* SStaticMesh_hpp */
