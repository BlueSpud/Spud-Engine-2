//
//  SScreenSpaceReflections.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 4/8/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SScreenSpaceReflections_hpp
#define SScreenSpaceReflections_hpp

#include "Rendering/Pipeline/SFramebuffer.hpp"
#include "Resource/Resources/Rendering/Shader/SShader.hpp"
#include "Rendering/Pipeline/Renderers/GBuffer.h"

/******************************************************************************
 *  Definition for the screen space reflection pass                           *
 ******************************************************************************/

class SScreenSpaceReflectionPass {
	
	public:
	
		SScreenSpaceReflectionPass(glm::vec2 main_framebuffer_size);
	
		void generateReflections(glm::mat4& projection_view_matrix, glm::mat4& inv_matrix, SFramebuffer* rendered_buffer);
		void bindReflections(int texture);
	
	private:
	
		SUniform* view_pos_u;
	
		std::shared_ptr<SShader> reflection_shader;
		std::shared_ptr<SShader> blur_shader;
		SFramebuffer* reflection_buffer;
		SFramebuffer* blur_buffer;
	
		glm::vec2 size;
	
};


#endif /* SScreenSpaceReflections_hpp */
