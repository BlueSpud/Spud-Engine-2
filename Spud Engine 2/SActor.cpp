//
//  SActor.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/8/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SActor.hpp"

/******************************************************************************
 *  Functions for actor                                                       *
 ******************************************************************************/

SActor::SActor() {
    
    // Create a root component for the children to attatch to
    root_component = new SComponent();
    
}

void SActor::render(bool render_material, double interpolation) {

    // Get the transform matrix and make it the model matrix
    glm::mat4 model_matrix = SGL::transformToMatrix(transform);
    SGL::mulMatrix(model_matrix, MAT_MODEL_MATRIX);
    
    // Render the root component, the others should follow
    root_component->render(render_material, interpolation);

}

void SActor::update() { /* nothing is done by default here */ }

void SActor::onMoveToSceneGraph(SPhysicsGraph* physics_graph) { root_component->onMoveToSceneGraph(physics_graph); }
void SActor::onRemoveFromSceneGraph(SPhysicsGraph* physics_graph) { root_component->onRemoveFromSceneGraph(physics_graph); }
