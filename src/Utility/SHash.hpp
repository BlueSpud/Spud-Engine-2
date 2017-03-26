//
//  SHash.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/26/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SHash_hpp
#define SHash_hpp

#include <iostream>

/******************************************************************************
 *  Definition for hasher													  *
 ******************************************************************************/

class SHash {
	
	public:
	
		static size_t hashString(const std::string& string);
	
	private:
	
		static std::hash<std::string> hasher;
	
};

#endif /* SHash_hpp */
