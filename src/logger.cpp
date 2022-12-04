//============================================================================
// Name        : logger.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include "log.h"
#include <iostream>
using namespace std;



int main() {
	LOG( LOG_LEVEL_VERBOSE, "Log Init %s, %d", "here", 1 );
	LOG( LOG_LEVEL_VERBOSE, "Log Init %s, %d", "here", 2 );
	for( int i = 0; i < 100000; ++ i)
	        LOG( LOG_LEVEL_VERBOSE, "Log MUV %s, %d", "here", i );
	LOG_TERMINATE();
	return 0;
}
