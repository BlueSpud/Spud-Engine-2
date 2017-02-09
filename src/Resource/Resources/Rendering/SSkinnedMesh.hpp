//
//  SSkinnedMesh.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 2/8/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SSkinnedMesh_hpp
#define SSkinnedMesh_hpp

#include "SSkinnedModel.hpp"
#include "SAnimation.hpp"
#include "STime.hpp"

#include "SObject.hpp"

/******************************************************************************
 *  Definition for skinned mesh                                               *
 ******************************************************************************/

class SSkinnedMesh : public SObject {
	
	public:
	
			SSkinnedMesh(SSkinnedModel* _parent_mesh);
	
			virtual void render(bool render_material, double interpolation);
			virtual void update(const SEvent& event);
	
			glm::mat4 getMatrixForBone(int bone, float time);
	
			SAnimation* animation = nullptr;
			SStopwatch timer;
	
		private:
	
			SSkinnedModel* parent_mesh;
			std::vector<SMaterial*> materials;
	
	
};


#endif /* SSkinnedMesh_hpp */
