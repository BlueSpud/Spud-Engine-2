//
//  SCamera.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SCamera_hpp
#define SCamera_hpp

#include "SGL.hpp"

/******************************************************************************
 *  Definition for camera                                                     *
 ******************************************************************************/

class SCamera {
    
    public:
    
        STransform transform;
    
        glm::mat4 getCameraMatrix();
        void translateToCameraSpace();
    
};

#endif /* SCamera_hpp */
