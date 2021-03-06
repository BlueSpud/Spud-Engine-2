//
//  SSkinnedMesh.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 2/8/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SSkinnedMesh_hpp
#define SSkinnedMesh_hpp

#include "Resource/Resources/Rendering/Skinned Mesh/SSkinnedModel.hpp"
#include "SAnimation.hpp"
#include "Engine/Utility/STime.hpp"

#include "Rendering/Objects/SObject.hpp"

/******************************************************************************
 *  Definition for skinned mesh                                               *
 ******************************************************************************/

class SSkinnedMesh : public SObject {

	public:

            SSkinnedMesh();
			SSkinnedMesh(std::shared_ptr<SSkinnedModel> _parent_mesh);
	
			virtual void render(double interpolation);
			virtual void render(std::shared_ptr<SGbufferShader> shader, double interpolation);
			virtual void update(const SEvent& event);

            virtual void serialize(SSerializer& serializer);
            virtual void deserialize(SDeserializer& deserializer);
	
			glm::mat4 getMatrixForBone(int bone, float time);
	
			SAnimation* animation = nullptr;
			SStopwatch timer;
	
		private:

            std::shared_ptr<SSkinnedModel> parent_mesh;
			std::vector<std::shared_ptr<SMaterial>> materials;
	

};


#endif /* SSkinnedMesh_hpp */
