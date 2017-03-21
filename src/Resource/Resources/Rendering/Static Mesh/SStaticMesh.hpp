//
//  SStaticMesh.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/7/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SStaticMeshInstance_hpp
#define SStaticMeshInstance_hpp

#include "SModel.hpp"
#include "SObject.hpp"
#include "SBoundingBox.hpp"

#include "SRigidBody.hpp"

/******************************************************************************
 *  Definition for static mesh                                                *
 ******************************************************************************/

class SStaticMesh : public SObject {
    
    public:
	
		SStaticMesh();
        SStaticMesh(SModel* _parent_mesh);
    
        virtual void render(double interpolation);
		virtual void render(SGbufferShader* shader, double interpolation);
		virtual bool shouldBeRendered(const SFrustum& frustum);
	
        virtual void update(const SEvent& event);
    
        void setMaterial(SMaterial* new_material, int material_domain);
    
        virtual void onMoveToSceneGraph(SPhysicsGraph* physics_graph);
        virtual void onRemoveFromSceneGraph(SPhysicsGraph* physics_graph);
	
		virtual void serialize(SSerializer& serializer);
		virtual void deserialize(SDeserializer& deserializer);
	
    private:
    
        SModel* parent_mesh;
        std::vector<SMaterial*> materials;
		bool materials_identical = true;
	
        SRigidBody* rigid_body = nullptr;
    
};

#endif /* SStaticMeshInstance_hpp */
