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

#include "SObject.hpp"
#include "SCamera.hpp"

/******************************************************************************
 *  Definition for scene graph render order sorting                             *
 ******************************************************************************/

struct SSortedObject {
    
    SObject* object;
    float z_value;
    
};

class SSceneGraph {
    
    public:
    
        virtual ~SSceneGraph();
    
        virtual void makeCurrent() = 0;
    
        virtual void render(SCamera& camera, bool render_material,  double interpolation) = 0;
    
        virtual void addObject(SObject* object) = 0;
        virtual void removeObject(SObject* object) = 0;

        SPhysicsGraph* physics_graph;
    
};

/******************************************************************************
 *  Definition for simple scene graph                                         *
 ******************************************************************************/

class SSimpleSceneGraph : public SSceneGraph {
    
    public:
    
        SSimpleSceneGraph();
    
        virtual void makeCurrent();
    
        virtual void render(SCamera& camera, bool render_material, double interpolation);
    
        virtual void addObject(SObject* object);
        virtual void removeObject(SObject* object);
    
        virtual ~SSimpleSceneGraph();

    private:
    
        std::list<SObject*>objects;
    
};

#endif /* SSceneGraph_hpp */