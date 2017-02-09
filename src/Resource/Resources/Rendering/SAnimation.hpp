//
//  SAnimation.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 2/9/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SAnimation_hpp
#define SAnimation_hpp

#include "SResourceManager.hpp"
#include "SGL.hpp"

/******************************************************************************
 *  Definition for animation tweening					     			      *
 ******************************************************************************/

enum SAnimationTweening {
	
	SAnimationTweeningNormal
	
};

/******************************************************************************
 *  Definition for animation keyframe					     			      *
 ******************************************************************************/

struct SAnimationKeyFrame {
	
	SAnimationTweening tweener;
	float time = 0.0;
	
	glm::mat4 matrix;
	
};

/******************************************************************************
 *  Definition for animation											      *
 ******************************************************************************/

class SAnimation : public SResource {
	
	public:
	
		glm::mat4 getMatrixForBone(int bone, float time);
	
		bool loops = false;
	
	protected:
	
		virtual bool load(const SPath& path);
		virtual void unload();
	
		std::vector<std::vector<SAnimationKeyFrame>> keyframes;
	
	
};

#endif /* SAnimation_hpp */
