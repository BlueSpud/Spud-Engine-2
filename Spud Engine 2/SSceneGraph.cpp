//
//  SSceneGraph.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/11/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SSceneGraph.hpp"

SSceneGraph::~SSceneGraph() { /* stub, destroy objects, scene graph manages memory for them */ }

void SSimpleSceneGraph::render(SCamera& camera, double interpolation) {

    // Translate everytihng for view space BEFORE so we can perform frustrum and occlusion culling
    SGL::clearMatrix(MAT_VIEW_MATRIX);
    camera.translateToCameraSpace(interpolation);
    
    // Make sure that anything we want to render is added to the reder que
    std::vector<SObject*>rendered_objects;
    
    std::list<SObject*>::iterator i = objects.begin();
    while (i != objects.end()) {
        
        // Check if we should render this object
        if ((*i)->shouldBeRendered())
            rendered_objects.push_back(*i);
        
        i++;
        
    }
    
    // Objects are collected, now they are rendered
    for (int j = 0; j < rendered_objects.size(); j++) {
        
        // Set up the matricies for this
        SGL::clearMatrix(MAT_MODEL_MATRIX);
        
        rendered_objects[j]->render(interpolation);
    }
    

}

void SSimpleSceneGraph::addObject(SObject* object) {

    // Add it to the list
    objects.push_back(object);

}

void SSimpleSceneGraph::removeObject(SObject* object) {

    // Remove it from the list
    objects.remove(object);

}

SSimpleSceneGraph::~SSimpleSceneGraph() {
    
    // Delete all the objects from memory
    std::vector<SObject*>rendered_objects;
    
    std::list<SObject*>::iterator i = objects.begin();
    while (i != objects.end()) {
        
        delete *i;
        
        i++;
        
    }

    
}
