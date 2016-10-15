//
//  SAmbientOcclusionPass.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 10/14/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SAmbientOcclusionPass_hpp
#define SAmbientOcclusionPass_hpp

#include <random>

#include "SFramebuffer.hpp" 
#include "SShader.hpp"

/******************************************************************************
 *  Definition for the ambient occlusion pass                                 *
 ******************************************************************************/

#define AO_KERNAL_SIZE 8
#define AO_NOISE_SIZE 16

class SAmbientOcclusionPass {
    
    public:
    
        SAmbientOcclusionPass(glm::ivec2 size);
        void unload();
    
        void renderAmbientOcclusion(int depth, int normal, int to_place_noise, glm::ivec2 screen_size, glm::mat4& projection_matrix,
                                     glm::mat4& inv_projection_matrix, glm::mat4& view_matrix);
    
    private:
    
        SFramebuffer* framebuffer;
        SViewport viewport;
    
        static glm::vec3 kernel[AO_KERNAL_SIZE];
        static GLuint noise_texture_id;
        static SShader* ambient_occlusion_shader;
    
    
};

#endif /* SAmbientOcclusionPass_hpp */
