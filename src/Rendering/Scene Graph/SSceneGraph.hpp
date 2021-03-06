//
//  SSceneGraph.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/11/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SSceneGraph_hpp
#define SSceneGraph_hpp

/******************************************************************************
 *  Definition for scene graph                                                *
 ******************************************************************************/

#include <vector>
#include <list>

#include "Rendering/Light/SLightGraph.hpp"

// Forward declaration
class SLevel;

/******************************************************************************
 *  Definition for scene graph render order sorting                            *
 ******************************************************************************/

struct SSortedObject {
    
    SObject* object;
    float z_value;
    
};

class SSceneGraph {
	
	friend class SLevel;
	
    public:
	
		SSceneGraph();
        virtual ~SSceneGraph();
    
        virtual void makeCurrent();
    
		void render(SCamera& camera, double interpolation);
		void render(SCamera& camera, std::shared_ptr<SGbufferShader> shader, double interpolation);
	
		template <class T>
		void addObject(T* object) {
		
			// Check if this belongs in the light graph or the scene graph
			if (std::is_base_of<SLight, T>::value) {
				
				// We need to do this so that it compiles for objects that are not subclasses of light
				light_graph->addLight((SLight*)(SObject*)object);
				
			} else performAddObject(object);
		
		}
	
		template <class T>
		void removeObject(SObject* object) {
		
			// Check if this belongs in the light graph or the scene graph
			if (std::is_base_of<SLight, T>::value) {
			
				// We need to do this so that it compiles for objects that are not subclasses of light
				light_graph->removeLight((SLight*)(SObject*)object);
			
			} else performRemoveObject(object);
		
		}

		virtual void linearizeObjects(std::vector<SObject*>& objects) = 0;
		virtual SObject* pickObject(const glm::vec3& origin, const glm::vec3& direction, float length) = 0;
	
        SPhysicsGraph* physics_graph;
		SLightGraph* light_graph;
	
	protected:
	
		virtual void performAddObject(SObject* object) = 0;
		virtual void performRemoveObject(SObject* object) = 0;
	
		virtual void collectObjects(SCamera& camera, double interpolation, std::list<SSortedObject>& sorted_objects) = 0;
	
};

/******************************************************************************
 *  Definition for simple scene graph                                         *
 ******************************************************************************/

class SSimpleSceneGraph : public SSceneGraph {
	
	friend class SLevel;
	
    public:
    
        virtual ~SSimpleSceneGraph();
		virtual void linearizeObjects(std::vector<SObject*>& objects);
		virtual SObject* pickObject(const glm::vec3& origin, const glm::vec3& direction, float length);
	
	protected:
	
		virtual void performAddObject(SObject* object);
		virtual void performRemoveObject(SObject* object);
	
		virtual void collectObjects(SCamera& camera, double interpolation, std::list<SSortedObject>& sorted_objects);

    private:
    
        std::list<SObject*>objects;
    
};

#endif /* SSceneGraph_hpp */
