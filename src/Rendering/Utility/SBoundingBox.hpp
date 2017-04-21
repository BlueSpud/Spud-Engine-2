//
//  SBoundingBox.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 10/3/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SBoundingBox_hpp
#define SBoundingBox_hpp

#include "Rendering/SGL.hpp"
#include <limits>

/******************************************************************************
 *  Definition for bounding box                                               *
 ******************************************************************************/

struct SBoundingBox {
    
    SBoundingBox(glm::vec3 _mins, glm::vec3 _maxes, STransform* _transform);
    
    void project(const glm::mat4& matrix, bool homogonized);
    bool frustrumCull(const SFrustum& frustum);
	
	void getOrientedPoints(glm::vec3* _points) const;
	float rayTrace(const glm::vec3& origin, const glm::vec3& direction, float length) const;
    
    STransform* transform;
    
    glm::vec3 mins;
    glm::vec3 maxes;
    
    glm::vec3 projected_mins;
    glm::vec3 projected_maxes;
    
    
};

#endif /* SBoundingBox_hpp */
