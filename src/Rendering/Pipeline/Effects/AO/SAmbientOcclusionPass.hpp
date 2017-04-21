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

#include "Rendering/Pipeline/SRenderingPipeline.hpp"
#include "Rendering/Pipeline/Renderers/GBuffer.h"

/******************************************************************************
 *  Definition for the ambient occlusion pass                                 *
 ******************************************************************************/

#define AO_KERNAL_SIZE 16
#define AO_NOISE_SIZE 4

class SAmbientOcclusionPass : public SPostProcessPass {
    
    public:
    
        SAmbientOcclusionPass(glm::vec2 main_framebuffer_size);
        virtual ~SAmbientOcclusionPass();
    
        virtual void render(SPostProcessPassData& data);
    
    private:
    
        SFramebuffer* occlusion_framebuffer;
        SFramebuffer* blur_framebuffer_h;
		SFramebuffer* blur_framebuffer_w;
        SViewport viewport;
    
        static std::shared_ptr<SShader> ambient_occlusion_shader;
        static std::shared_ptr<SShader> blur_shader;
        static std::shared_ptr<SShader> blend_shader;

    
    
};

#endif /* SAmbientOcclusionPass_hpp */
