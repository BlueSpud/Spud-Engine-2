//
//  SLevelFactory.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/20/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "SLevelFactory.hpp"

/******************************************************************************
 *  Implementation for factory manager										  *
 ******************************************************************************/

SLevelFactoryRegistry* SLevelFactoryRegistry::instance() {
	
	static SLevelFactoryRegistry* _instance = nullptr;
	
	if (!_instance)
		_instance = new SLevelFactoryRegistry();
	
	return _instance;
	
}

