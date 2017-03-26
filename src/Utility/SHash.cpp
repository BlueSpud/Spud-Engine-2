//
//  SHash.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/26/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "SHash.hpp"

std::hash<std::string> SHash::hasher;

/******************************************************************************
 *  Implementation for hasher												  *
 ******************************************************************************/

size_t SHash::hashString(const std::string& string) { return hasher(string); }
