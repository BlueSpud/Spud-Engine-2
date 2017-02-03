//
//  SGbufferShader.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 2/1/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SGbufferShader_hpp
#define SGbufferShader_hpp

#include "SShader.hpp"

/******************************************************************************
 *  Definition for Gbuffer shader                                             *
 ******************************************************************************/

class SGbufferShader {
	
	public:
	
		void compileShader(SPath shader_vert, const SPath& master_vert, const std::string& extension);
	
};

#endif /* SGbufferShader_hpp */
