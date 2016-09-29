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

/******************************************************************************
 *  Definition for basic light graph                                          *
 ******************************************************************************/

class SLightGraph {
    
    public:
    
        virtual void cullLights() = 0;
        virtual void updateShadows(SCamera& scene_camera, SSceneGraph& scene_graph, double interpolation) = 0;
    
        virtual void addLight(SLight* light) = 0;
        virtual void removeLight(SLight* light) = 0;
    
        virtual int getLightCount() = 0;
        virtual glm::vec3* getLightPositions(double interpolation) = 0;
    
        virtual ~SLightGraph();
    
};

/******************************************************************************
 *  Definition for simple light graph                                         *
 ******************************************************************************/

class SSimpleLightGraph : public SLightGraph {
    
    public:
    
        virtual void cullLights();
        virtual void updateShadows(SCamera& scene_camera, SSceneGraph& scene_graph, double interpolation);
    
        virtual void addLight(SLight* light);
        virtual void removeLight(SLight* light);
    
        virtual int getLightCount();
        virtual glm::vec3* getLightPositions(double interpolation);
    
        virtual ~SSimpleLightGraph();
    
    private:
    
        std::list<SLight*> lights;
        std::vector<SLight*> culled_lights;
        glm::vec3* light_positions;
    
};

#endif /* SLightGraph_hpp */
