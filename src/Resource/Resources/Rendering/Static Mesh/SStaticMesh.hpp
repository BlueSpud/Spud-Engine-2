//
//  SStaticMesh.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/7/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SStaticMeshInstance_hpp
#define SStaticMeshInstance_hpp

#include "Resource/Resources/Rendering/SModel.hpp"
#include "Rendering/Objects/SObject.hpp"
#include "Rendering/Utility/SBoundingBox.hpp"

#include "Physics/SRigidBody.hpp"

/******************************************************************************
 *  Definition for static mesh                                                *
 ******************************************************************************/

class SStaticMesh : public SObject {

	SCLASS(SStaticMesh)

    public:
	
		SStaticMesh();
        SStaticMesh(std::shared_ptr<SModel> _parent_mesh);
	
		void setModel(std::shared_ptr<SModel> model);
	
        virtual void render(double interpolation);
		virtual void render(std::shared_ptr<SGbufferShader> shader, double interpolation);
		virtual bool shouldBeRendered(const SFrustum& frustum);
	
        virtual void update(const SEvent& event);
    
        void setMaterial(std::shared_ptr<SMaterial> new_material, int material_domain);
    
        virtual void onMoveToSceneGraph(SPhysicsGraph* physics_graph);
        virtual void onRemoveFromSceneGraph(SPhysicsGraph* physics_graph);
	
		virtual void serialize(SSerializer& serializer);
		virtual void deserialize(SDeserializer& deserializer);
	
    private:
    
        std::shared_ptr<SModel> parent_mesh;
        std::vector<std::shared_ptr<SMaterial>> materials;
		bool materials_identical = true;
	
        SRigidBody* rigid_body = nullptr;
    
};

#endif /* SStaticMeshInstance_hpp */
