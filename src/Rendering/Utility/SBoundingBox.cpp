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
    
    const glm::mat4 model_matrix = SGL::transformToMatrix(*transform);
    
    // Make 8 points to project
    const glm::vec4 points[8] = {
    
        glm::vec4(mins.x, mins.y, mins.z, 1.0),
        glm::vec4(maxes.x, mins.y, mins.z, 1.0),
        glm::vec4(maxes.x, maxes.y, mins.z, 1.0),
        glm::vec4(mins.x, maxes.y, mins.z, 1.0),
        glm::vec4(mins.x, mins.y, maxes.z, 1.0),
        glm::vec4(maxes.x, mins.y, maxes.z, 1.0),
        glm::vec4(maxes.x, maxes.y, maxes.z, 1.0),
        glm::vec4(mins.x, maxes.y, maxes.z, 1.0)
    
    };
	
	const glm::mat4 input_matrix_model_matrix = matrix * model_matrix;
	
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
	//project(glm::mat4(1.0), false);
	
	// Get the points we need to test
//	glm::vec3 points[8] = {
//		
//		glm::vec3(projected_mins.x, projected_mins.y, projected_mins.z),
//		glm::vec3(projected_maxes.x, projected_mins.y, projected_mins.z),
//		glm::vec3(projected_maxes.x, projected_maxes.y, projected_mins.z),
//		glm::vec3(projected_mins.x, projected_maxes.y, projected_mins.z),
//		glm::vec3(projected_mins.x, projected_mins.y, projected_maxes.z),
//		glm::vec3(projected_maxes.x, projected_mins.y, projected_maxes.z),
//		glm::vec3(projected_maxes.x, projected_maxes.y, projected_maxes.z),
//		glm::vec3(projected_mins.x, projected_maxes.y, projected_maxes.z)
//		
//	};
	
	glm::vec3 points[8];
	getOrientedPoints(&points[0]);
	
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
	const glm::vec4 points[8] = {
		
		glm::vec4(mins.x, mins.y, mins.z, 1.0),
		glm::vec4(maxes.x, mins.y, mins.z, 1.0),
		glm::vec4(maxes.x, maxes.y, mins.z, 1.0),
		glm::vec4(mins.x, maxes.y, mins.z, 1.0),
		glm::vec4(mins.x, mins.y, maxes.z, 1.0),
		glm::vec4(maxes.x, mins.y, maxes.z, 1.0),
		glm::vec4(maxes.x, maxes.y, maxes.z, 1.0),
		glm::vec4(mins.x, maxes.y, maxes.z, 1.0)
		
	};
	
	
	const glm::mat4 model_matrix = SGL::transformToMatrix(*transform);
	
	// Project all the points
	for (int i = 0; i < 8; i++) {
		
		const glm::vec4 point_projected = model_matrix * points[i];
		_points[i] = glm::vec3(point_projected);
		
	}

}

float SBoundingBox::rayTrace(const glm::vec3& origin, const glm::vec3& direction, float length) const {
	
	// First we do what we do in getOrientedPoints, however we cant use the function because we need the matrix
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
	
	
	const glm::mat4 model_matrix = SGL::transformToMatrix(*transform);
	glm::vec3 mins_p;
	glm::vec3 maxes_p;
	
	// Project all the points
	for (int i = 0; i < 8; i++) {
		
		points[i] = model_matrix * points[i];
		
		if (i == 0) {
			
			// If this is the first point, it is immidately the min and max
			mins_p = (glm::vec3)points[i];
			maxes_p = (glm::vec3)points[i];
			
		} else {
			
			// If this is a min or a max on any of the axises, save it
			mins_p.x = glm::min(points[i].x, mins_p.x);
			mins_p.y = glm::min(points[i].y, mins_p.y);
			mins_p.z = glm::min(points[i].z, mins_p.z);
			
			maxes_p.x = glm::max(points[i].x, maxes_p.x);
			maxes_p.y = glm::max(points[i].y, maxes_p.y);
			maxes_p.z = glm::max(points[i].z, maxes_p.z);
			
		}
		
	}
	
	float time_min, time_max;
	
	// X direction
	float time1 = (mins_p.x - origin.x) / direction.x;
	float time2 = (maxes_p.x - origin.x) / direction.x;
	
	time_max = glm::max(time1, time2);
	time_min = glm::min(time1, time2);
	
	// Y direction
	time1 = (mins_p.y - origin.y) / direction.y;
	time2 = (maxes_p.y - origin.y) / direction.y;
	
	time_max = glm::min(time_max, glm::max(time1, time2));
	time_min = glm::max(time_min, glm::min(time1, time2));
	
	// Z direction
	time1 = (mins_p.z - origin.z) / direction.z;
	time2 = (maxes_p.z - origin.z) / direction.z;
	
	time_max = glm::min(time_max, glm::max(time1, time2));
	time_min = glm::max(time_min, glm::min(time1, time2));
	
	// Check if there was an intersection
	if (time_max >= time_min && time_min <= length) {
		
		if (time_min >= 0.0)
			return time_min;
		else {
			
			// Check if there was initial overlap
			if (origin.x < maxes.x && origin.y < maxes.y && origin.z < maxes.z &&
				origin.x > mins.x  && origin.y > mins.y  && origin.z > mins.z)
				return time_max;
			
			
		}
		
	}
	
	// No intersection
	return -1.0;
	
}
