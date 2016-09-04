//
//  SMesh.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/2/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SMesh_hpp
#define SMesh_hpp

#include "SResourceManager.hpp"

class SMesh : public SResource {
    
    public:
    
        static SResource* allocate();
        static void registerAllocators();
    
    protected:
    
        virtual void load(const SPath& path);
        virtual void unload();
    
        virtual void upload();
};

#endif /* SMesh_hpp */
