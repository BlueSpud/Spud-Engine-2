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
#include "SShader.hpp"

/******************************************************************************
 *  Definition for basic light graph                                          *
 ******************************************************************************/

class SLightGraph {
    
    public:
    
        SLightGraph();
        virtual ~SLightGraph();
    
        virtual void cullLights(glm::mat4& projection_view_matrix) = 0;
        virtual void updateShadows(SSceneGraph& scene_graph, glm::mat4& projection_matrix, glm::mat4& view_matrix, double interpolation) = 0;
    
        virtual void addLight(SLight* light) = 0;
        virtual void removeLight(SLight* light) = 0;
	
		virtual void uploadCulledLightData(SShader* shader, double interpolation) = 0;
	
		virtual std::vector<SLight*>& getCulledLights() = 0;
	
        SFramebuffer* shadow_map_buffer;
        bool** shadow_map_atlas;
	
	protected:
	
		void blurLightTile(glm::ivec2& tile);
	
		static SFramebuffer* intermediate_blur_buffer;
		static SShader* shadow_blur_shader_h;
		static SShader* shadow_blur_shader_v;
	
};

/******************************************************************************
 *  Definition for simple light graph                                         *
 ******************************************************************************/

class SSimpleLightGraph : public SLightGraph {
    
    public:
    
        SSimpleLightGraph();
    
        virtual void cullLights(glm::mat4& projection_view_matrix);
        virtual void updateShadows(SSceneGraph& scene_graph, glm::mat4& projection_matrix, glm::mat4& view_matrix, double interpolation);
    
        virtual void addLight(SLight* light);
        virtual void removeLight(SLight* light);
	
		virtual void uploadCulledLightData(SShader* shader, double interpolation);
	
		virtual std::vector<SLight*>& getCulledLights();
	
        virtual ~SSimpleLightGraph();
    
    private:
    
        std::list<SLight*> lights;
        std::vector<SLight*> culled_lights;
        glm::vec3* light_positions;
    
};

#endif /* SLightGraph_hpp */
