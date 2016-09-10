//
//  SModel.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/2/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SMesh_hpp
#define SMesh_hpp

#include "SResourceManager.hpp"

/******************************************************************************
 *  Definition for model                                                      *
 ******************************************************************************/

class SModel : public SResource {
    
    public:
    
        static SResource* allocate();
    
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
    
        virtual void upload();
};

#endif /* SModel_hpp */
