//
//  SBoundingBox.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 10/3/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SBoundingBox.hpp"

/******************************************************************************
 *  Implementation for bounding box                                           *
 ******************************************************************************/

SBoundingBox::SBoundingBox(glm::vec3 _mins, glm::vec3 _maxes, STransform* _transform) {
    
    // Save the extents of the bounding box
    mins = _mins;
    maxes = _maxes;
    
    // Save the transform of the box
    transform = _transform;
    
}

void SBoundingBox::project(const glm::mat4& matrix, bool homogonized) {
    
    glm::mat4 model_matrix = SGL::transformToMatrix(*transform);
    
    // Make 8 points to project
    glm::vec4 points[8] = {
    
        glm::vec4(mins.x, mins.y, mins.z, 1.0),
        glm::vec4(maxes.x, mins.y, mins.z, 1.0),
        glm::vec4(maxes.x, maxes.y, mins.z, 1.0),
        glm::vec4(mins.x, maxes.y, mins.z, 1.0),
        glm::vec4(mins.x, mins.y, maxes.z, 1.0),
        glm::vec4(maxes.x, mins.y, maxes.z, 1.0),
        glm::vec4(maxes.x, maxes.y, maxes.z, 1.0),
        glm::vec4(mins.x, maxes.y, maxes.z, 1.0)
    
    };
	
	glm::mat4 input_matrix_model_matrix = matrix * model_matrix;
	
    // Project all the points, also homogonize it
    for (int i = 0; i < 8; i++) {
        
        glm::vec4 point_projected = input_matrix_model_matrix * points[i];
        
        if (homogonized)
            point_projected = point_projected / point_projected.w;
		
        if (i == 0) {
            
            // If this is the first point, it is immidately the min and max
            projected_mins = (glm::vec3)point_projected;
            projected_maxes = (glm::vec3)point_projected;
            
        } else {
        
            // If this is a min or a max on any of the axises, save it
            projected_mins.x = glm::min(point_projected.x, projected_mins.x);
            projected_mins.y = glm::min(point_projected.y, projected_mins.y);
            projected_mins.z = glm::min(point_projected.z, projected_mins.z);
        
            projected_maxes.x = glm::max(point_projected.x, projected_maxes.x);
            projected_maxes.y = glm::max(point_projected.y, projected_maxes.y);
            projected_maxes.z = glm::max(point_projected.z, projected_maxes.z);
            
        }
        
    }
    
}

bool SBoundingBox::frustrumCull(const SFrustum& frustum) {

	// First we project ourselves with a identity matrix
	// This transforms the bounding box to transformed space (rotation, translation, scale, etc)
	project(glm::mat4(1.0), false);
	
	// Get the points we need to test
	glm::vec3 points[8] = {
		
		glm::vec3(projected_mins.x, projected_mins.y, projected_mins.z),
		glm::vec3(projected_maxes.x, projected_mins.y, projected_mins.z),
		glm::vec3(projected_maxes.x, projected_maxes.y, projected_mins.z),
		glm::vec3(projected_mins.x, projected_maxes.y, projected_mins.z),
		glm::vec3(projected_mins.x, projected_mins.y, projected_maxes.z),
		glm::vec3(projected_maxes.x, projected_mins.y, projected_maxes.z),
		glm::vec3(projected_maxes.x, projected_maxes.y, projected_maxes.z),
		glm::vec3(projected_mins.x, projected_maxes.y, projected_maxes.z)
		
	};
	
	// If all 8 points are outside a plane, we are culled, otherwise we are good to go
	for (int plane = 0; plane < 6; plane++) {
		
		int inside = 8;
		
		for (int p = 0; p < 8; p++) {
			
			// Do a regular point frustum cull
			if (glm::dot(points[p], glm::vec3(frustum.planes[plane])) + frustum.planes[plane].w < 0.0)
				inside--;
			
		}
		
		// If we had 0 inside, then we are outside
		if (inside == 0)
			return false;
		
	}
	
	
    return true;
	
}

void SBoundingBox::getOrientedPoints(glm::vec3* _points) const {
	
	// Make 8 points to project
	glm::vec4 points[8] = {
		
		glm::vec4(mins.x, mins.y, mins.z, 1.0),
		glm::vec4(maxes.x, mins.y, mins.z, 1.0),
		glm::vec4(maxes.x, maxes.y, mins.z, 1.0),
		glm::vec4(mins.x, maxes.y, mins.z, 1.0),
		glm::vec4(mins.x, mins.y, maxes.z, 1.0),
		glm::vec4(maxes.x, mins.y, maxes.z, 1.0),
		glm::vec4(maxes.x, maxes.y, maxes.z, 1.0),
		glm::vec4(mins.x, maxes.y, maxes.z, 1.0)
		
	};
	
	
	glm::mat4 model_matrix = SGL::transformToMatrix(*transform);
	
	// Project all the points
	for (int i = 0; i < 8; i++) {
		
		glm::vec4 point_projected = model_matrix * points[i];
		_points[i] = glm::vec3(point_projected);
		
	}

}
