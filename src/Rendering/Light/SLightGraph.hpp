//
//  SLightGraph.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/29/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SLightGraph_hpp
#define SLightGraph_hpp

#include <list>
#include <vector>

#include "SLight.hpp"
#include "Resource/Resources/Rendering/Shader/SShader.hpp"

/******************************************************************************
 *  Definition for basic light graph                                          *
 ******************************************************************************/

class SLightGraph {
    
    public:
    
        SLightGraph();
        virtual ~SLightGraph();
    
        virtual void cullLights(glm::mat4& projection_view_matrix) = 0;
		void updateShadows(SSceneGraph& scene_graph, glm::mat4& projection_matrix, glm::mat4& view_matrix, double interpolation);
    
        virtual void addLight(SLight* light) = 0;
        virtual void removeLight(SLight* light) = 0;
	
		void uploadCulledLightData(std::shared_ptr<SShader> shader, double interpolation);
	
		std::vector<SLight*>& getCulledLights();
	
        SFramebuffer* shadow_map_buffer;
        bool** shadow_map_atlas;
	
		virtual void linearizeLights(std::vector<SObject*>& lights) = 0;
	
	protected:
	
		void blurLightTile(glm::ivec2& tile);
	
		static SFramebuffer* intermediate_blur_buffer;
		static std::shared_ptr<SShader> shadow_blur_shader_h;
		static std::shared_ptr<SShader> shadow_blur_shader_v;
	
		std::vector<SLight*> culled_lights;
	
};

/******************************************************************************
 *  Definition for simple light graph                                         *
 ******************************************************************************/

class SSimpleLightGraph : public SLightGraph {
    
    public:
	
		virtual ~SSimpleLightGraph();
	
        virtual void cullLights(glm::mat4& projection_view_matrix);
    
        virtual void addLight(SLight* light);
        virtual void removeLight(SLight* light);
	
		virtual void linearizeLights(std::vector<SObject*>& lights);
	
    private:
    
        std::list<SLight*> lights;
    
};

#endif /* SLightGraph_hpp */
