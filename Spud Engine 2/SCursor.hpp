//
//  SCursor.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/5/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SCursor_hpp
#define SCursor_hpp

#include <FreeImage.h>

#include "SResourceManager.hpp"
#include "SGL.hpp"

/******************************************************************************
 *  Definition for cursor                                                     *
 ******************************************************************************/

class SCursor : public SResource {
    
    friend class SGL;
    
    public:
    
        static SResource* allocate();
        void bind();
    
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
    
    private:
    
        GLFWcursor* cursor;
    
};

#endif /* SCursor_hpp */
