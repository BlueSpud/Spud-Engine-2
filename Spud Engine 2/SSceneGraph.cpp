//
//  SSceneGraph.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/11/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SSceneGraph.hpp"

SSceneGraph::~SSceneGraph() { /* stub, destroy objects, scene graph manages memory for them */ }

/******************************************************************************
 *  Functions for basic scene graph                                           *
 ******************************************************************************/

void SSimpleSceneGraph::render(SCamera& camera, bool render_material, double interpolation) {

    // Translate everytihng for view space BEFORE so we can perform frustrum and occlusion culling
    SGL::clearMatrix(MAT_VIEW_MATRIX);
    glm::mat4 view_matrix = camera.translateToCameraSpace(interpolation);
    glm::mat4 projection_view_matrix = SGL::getMatrix(MAT_PROJECTION_MATRIX) * view_matrix;
    
    // Make sure that anything we want to render is added to the reder que
    // The array is sorted by z value of an object relative to the camera
    std::list<SSortedObject>rendered_objects;
    
    std::list<SObject*>::iterator i = objects.begin();
    std::list<SSortedObject>::iterator j;
    
    while (i != objects.end()) {
        
        // Check if we should render this object
        if ((*i)->shouldBeRendered(projection_view_matrix)) {
            
            //Save the object and sort it based on how close its transform is to the camera to reduce overdraw
            SSortedObject object_s;
            object_s.object = *i;
            
            //Calculate z value
            object_s.z_value = (view_matrix * glm::vec4((object_s.object->transform.translation +
                                                           object_s.object->transform.translation_velocity * (float)interpolation), 1.0)).z;
            
            // Do an insertion sort of the new object into the array
            j = rendered_objects.begin();
            bool added = false;
            while (j != rendered_objects.end()) {
                
                // Check if the object has a z value greater than us
                if ((*j).z_value < object_s.z_value) {
                
                    // Insert the object here and break
                    rendered_objects.insert(j, object_s);
                    added = true;
                    break;
                    
                }
                    
                j++;
            }
            
            // If it wasnt added, add it back
            if (!added)
                rendered_objects.push_back(object_s);
            
        }
        
        i++;
        
    }
    
    // Objects are collected, now they are rendered
    j = rendered_objects.begin();
    while (j != rendered_objects.end()) {
        
        // Set up the matricies for this
        SGL::clearMatrix(MAT_MODEL_MATRIX);
        
        // Render the object
        (*j).object->render(render_material, interpolation);
        
        j++;
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
