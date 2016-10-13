//
//  SMesh.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SMesh_hpp
#define SMesh_hpp

#include "SObject.hpp"

#include "SModel.hpp"
#include "SBoundingBox.hpp"
#include "SMaterial.hpp"

/******************************************************************************
 *  Defiunition for mesh                                                      *
 ******************************************************************************/

class SMesh : public SObject {
    
    public:
    
        SMesh(const SPath& path);
        SMesh(SModel* _model, SMaterialInstance* _mat_instance);
    
        virtual void render(SShader* shader, double interpolation);
        virtual void update();
    
        virtual bool shouldBeRendered(const glm::mat4& projection_view_matrix);
    
    protected:
    
        bool loaded = false;
    
        SModel* model;
        SBoundingBox bounding_box;
        SMaterialInstance* mat_instance;
    
};

#endif /* SMesh_hpp */
