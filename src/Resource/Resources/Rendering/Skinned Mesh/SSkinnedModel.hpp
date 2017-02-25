//
//  SSkinnedModel.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 2/8/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SSkinnedModel_hpp
#define SSkinnedModel_hpp

#include "SModel.hpp"

/******************************************************************************
 *  Definition for bone														  *
 ******************************************************************************/

struct SBone {
	
	int parent_index;
	
	glm::mat4 matrix;
	glm::mat4 bind_matrix;
	
};

/******************************************************************************
 *  Definition for skinned model                                              *
 ******************************************************************************/

class SSkinnedModel : public SModel {
	
	friend class SSkinnedMesh;
	
	public:
	
		void render(const std::vector<SMaterial*>& instance_material, const std::vector<glm::mat4>& matricies);
		void render(SGbufferShader* shader, const std::vector<glm::mat4>& matricies);
	
	protected:
	
		virtual bool load(const SPath& path);
		virtual void unload();
		virtual void hotload(const SPath& path);
	
		glm::mat4 getMatrixForBone(int bone, float time);
	
		glm::mat4 bind_pose;
	
		std::vector<SBone> bones;
	
		glm::vec4* vertex_weights;
		glm::vec4* bone_indicies;
	
	
	
};

#endif /* SSkinnedModel_hpp */
