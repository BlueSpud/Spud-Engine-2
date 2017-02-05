//
//  SComponent.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/8/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SComponent.hpp"

/******************************************************************************
 *  Implementation for generic component                                      *
 ******************************************************************************/

void SComponent::render(bool render_material, double interpolation) {
    
    // First we render this component, but there is nothing to render in the default component
    
    // Get the transform of this
    glm::mat4 model_matrix = SGL::transformToMatrix(transform);
    SGL::mulMatrix(model_matrix, MAT_MODEL);
    
    // Get the modified model matrix
    model_matrix = SGL::getMatrix(MAT_MODEL);
    
    // Next we render all of the children, this should be called from subclasses
    for (int i = 0; i < child_components.size(); i++) {
        
        // Because the children can change the model matrix, we need to upload the original again
        SGL::loadMatrix(model_matrix, MAT_MODEL);
        child_components[i]->render(render_material, interpolation);
        
    }
    
}

void SComponent::update(const SEvent& event) { /* components update nothing by default */ }

void SComponent::onMoveToSceneGraph(SPhysicsGraph* physics_graph) {

    // Call on all children components
    for (int i = 0; i < child_components.size(); i++)
        child_components[i]->onMoveToSceneGraph(physics_graph);

}

void SComponent::onRemoveFromSceneGraph(SPhysicsGraph* physics_graph) {

    // Call on all children components
    for (int i = 0; i < child_components.size(); i++)
        child_components[i]->onRemoveFromSceneGraph(physics_graph);

}

void SComponent::attatchComponent(SComponent* new_child_componenet) {
    
    // Set the parent of the new child and add it to this
    new_child_componenet->parent_component = this;
    child_components.push_back(new_child_componenet);
    
}
