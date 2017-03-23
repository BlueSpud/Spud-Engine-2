//
//  SAnimation.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 2/9/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "SAnimation.hpp"

/******************************************************************************
 *  Registration for supported model extensions                               *
 ******************************************************************************/

REGISTER_RESOURCE_CLASS(sanim, SAnimation)

/******************************************************************************
 *  Implementation for animation								     	      *
 ******************************************************************************/

float SAnimation::getInterpolation(float percent, SAnimationTweening tweener) const {
	
	switch (tweener) {
			
		case SAnimationTweeningNormal:
			return percent;
			break;

		case SAnimationTweeningCos:
			
			// Cos interpolation
			return 1.0 - (cosf(M_PI * percent) + 1.0) / 2.0;
			
			break;
  
	}
	
	return 0.0;
	
}

glm::mat4 SAnimation::getMatrixForBone(int bone, float time) {
	
	// If we loop and we are over the time, we do a mod
	if (loops)
		time = fmod(time, keyframes[bone].back().time);
	
	// First we have to find the two matricies that we are interpolating
	int start_keyframe = -1;
	int end_keyframe = -1;
	
	for (int i = 0; i < keyframes[bone].size() - 1; i++) {
		
		// Check the next keyframe and see if it is greater than this, meaning that is what we are interpolating to
		if (keyframes[bone][i + 1].time > time) {
		
			start_keyframe = i;
			end_keyframe = i + 1;
			break;
		
		}
		
	}
	
	// If we didnt find the matrix, it means we've reached the end of the animation
	if (start_keyframe == -1) {
		
		// End keyframe
		return keyframes[bone].back().matrix;
		
	}
	
	// Calculate the percent that we are between the two keyframes
	float delta_t = keyframes[bone][end_keyframe].time - keyframes[bone][start_keyframe].time;
	float progress = time - keyframes[bone][start_keyframe].time;
	
	// Calculate percent, for now only use linear
	float percent = progress / delta_t;
	float interpolation = getInterpolation(percent, keyframes[bone][start_keyframe].tweener);
	
	glm::mat4& start_matrix = keyframes[bone][start_keyframe].matrix;
	glm::mat4& end_matrix = keyframes[bone][end_keyframe].matrix;
	
	// Now we can interpolate the matrix, we use quaternians for the roataion
	glm::quat start_quat = glm::quat_cast(start_matrix);
	glm::quat end_quat = glm::quat_cast(end_matrix);
	
	glm::quat interpolated_quat = glm::slerp(start_quat, end_quat, interpolation);
	glm::mat4 interpolated_matrix = glm::mat4_cast(interpolated_quat);
	
	// Now we interpolate the transform components
	glm::vec4 start_translation = glm::vec4(start_matrix[0][3], start_matrix[1][3], start_matrix[2][3], start_matrix[3][3]);
	glm::vec4 end_translation = glm::vec4(end_matrix[0][3], end_matrix[1][3], end_matrix[2][3], end_matrix[3][3]);
	
	glm::vec4 interpolated_translation = glm::mix(start_translation, end_translation, interpolation);
	
	// We have the interpolated rotation and translation, combine them into the final matrix
	interpolated_matrix[0][3] = interpolated_translation.x;
	interpolated_matrix[1][3] = interpolated_translation.y;
	interpolated_matrix[2][3] = interpolated_translation.z;
	interpolated_matrix[3][3] = interpolated_translation.w;
	
	return interpolated_matrix;
	
}

bool SAnimation::load(const SPath& path) {

	SFile* file = SFileSystem::loadFile(path);
	
	if (file) {

		// Get the number of bones that were in this animations
		unsigned int bone_count;
		file->read(&bone_count, sizeof(unsigned int));

		// Each bone
		for (int i = 0; i < bone_count; i++) {

			// Create a new spot for the keyframe
			keyframes.push_back(std::vector<SAnimationKeyFrame>());
			
			// Read the number of keyframes for the bone
			unsigned int keyframe_count;
			file->read(&keyframe_count, sizeof(unsigned int));

			for (int j = 0; j < keyframe_count; j++) {

				keyframes[i].push_back(SAnimationKeyFrame());
				SAnimationKeyFrame& keyframe = keyframes[i].back();
				
				// Read how we interpolate this keyframe
				file->read(&keyframe.tweener, sizeof(SAnimationTweening));
				
				// Read the time of the keyframe
				file->read(&keyframe.time, sizeof(float));
				
				// Read the matrix for it
				file->read(&keyframe.matrix, sizeof(glm::mat4));

			}

		}
		
		return true;

	}

	return false;

}


void SAnimation::unload() { /* intentionally blank */ }
