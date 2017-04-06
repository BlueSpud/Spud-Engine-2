//
//  SRenderSystem.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/12/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SRenderSystem_hpp
#define SRenderSystem_hpp

#include "SSubsystem.hpp"

#include "SCubeMap.hpp"

/******************************************************************************
 *  Definition for the render system                                          *
 ******************************************************************************/

class SRenderSystem : public SSubsystem {
    
    public:
    
        static void startup();
        static void shutdown();
	
        static void render(double interpolation);
    
        static SRenderingPipeline* rendering_pipeline;
		static SRenderingPipeline* cubemap_pipeline;
	
        static SSceneGraph* current_scene_graph;
	
		static void generateBRDF();
		static void bindBRDF(int texture);
	
		static SCubeMap* generateCubeMap(const glm::vec3& position);
	
	private:
	
		static std::shared_ptr<SShader> brdf_shader;
		static SFramebuffer* brdf_buffer;
	
};

#endif /* SRenderSystem_hpp */
