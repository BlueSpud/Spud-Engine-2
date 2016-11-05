//
//  SCamera.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SCamera_hpp
#define SCamera_hpp

#include "SUniform.hpp"
#include "SGL.hpp"

/******************************************************************************
 *  Definition for camera                                                     *
 ******************************************************************************/

class SCamera {
    
    public:
    
        STransform transform;
    
        glm::mat4 getCameraMatrix();
        glm::mat4 getCameraMatrix(double interpolation);
    
        glm::mat4 translateToCameraSpace();
        glm::mat4 translateToCameraSpace(double interpolation);
    
        static glm::vec3* getFrustumWithPlanes(glm::mat4& projection_matrix, glm::mat4& view_matrix, float near_plane, float far_plane);
    
        // Store the last camera position
        static glm::vec3 view_position;
    
};

#endif /* SCamera_hpp */
