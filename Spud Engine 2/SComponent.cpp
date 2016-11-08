//
//  SComponent.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/8/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SComponent.hpp"

void SComponent::render(bool render_material, double interpolation) {
    
    // First we render this component, but there is nothing to render in the default component
    
    // Next we render all of the children, this should be called from subclasses
    for (int i = 0; i < child_components.size(); i++)
        child_components[i]->render(render_material, interpolation);

}

void SComponent::update() { /* components updat enothing by default */ }
