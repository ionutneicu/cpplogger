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
#if 0
	log::init();
	auto  console_backend = log::backends::add( log::backends::console_backend, log::at_least(log::warn) );
	auto  file_backend = log::backends::add( log::backends::file_backend, log::all() );

	console_backend::add_conditional_prefix( [bool](const auto log::entry& entry) { return entry.level == warn; }, log::color_prefix(log::color::yellow) );
	log::backends::all::add_prefix( log::setup_timestamp("hh:mm::ss::MM"));
	log::thread::queue_size( 4096 );

	log::cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
#endif
	LOG( verbose::get_instance(), "Log Init %s, %d", "here", 1 );
	return 0;
}
