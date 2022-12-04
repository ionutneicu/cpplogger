//============================================================================
// Name        : logger.cpp
// Author      : Ionut Neicu
// Version     : 1.0
// Copyright   : 2021-2022
// Description : Simple Logging Usage
//============================================================================


#include "log.h"

static const char LOG_MODULE_NAME[]="test";

using namespace std;




int main() {
	LOG_DEBUG( "Starting application %d", 1 );
	LOG_DEBUG( "The module name is: %s", LOG_MODULE_NAME );
	for( int i = 0; i < 10; ++ i)
	{
	        LOG_VERBOSE( "Test log %s, %d", "here", i );
	}
	LOG_TERMINATE();
	return 0;
}
