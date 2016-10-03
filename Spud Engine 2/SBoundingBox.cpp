//
//  SBoundingBox.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 10/3/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SBoundingBox.hpp"

/******************************************************************************
 *  Functions for bounding box                                                *
 ******************************************************************************/

SBoundingBox::SBoundingBox(glm::vec3 _mins, glm::vec3 _maxes) {
    
    // Save the extents of the bounding box
    mins = _mins;
    maxes = _maxes;
    
}

void SBoundingBox::project(glm::mat4& matrix, bool homogonized) {
    
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
    
    // Project all the points, also homogonize it
    for (int i  = 0; i < 8; i++) {
        
        glm::vec4 point_projected = matrix * points[i];
        
        if (homogonized)
            point_projected = point_projected / point_projected.w;
        
        if (!i) {
            
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

bool SBoundingBox::frustrumCull(glm::mat4& projection_view_matrix) {
    
    // Project ourselves
    project(projection_view_matrix, true);
    
    // Check if it is behind the camera 
    if (projected_maxes.z <= 0.0)
        return false;
    
    // Check X
    if (projected_maxes.x <= -1.0 || projected_mins.x >= 1.0)
        return false;
    
    // Check Y
    if (projected_maxes.y <= -1.0 || projected_mins.y >= 1.0)
        return false;
    
    // If no condition was found to be true, then we were inside the camera frustrum
    return true;
    
}
