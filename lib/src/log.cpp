/*
 * log.cpp
 *
 *  Created on: Nov 22, 2022
 *      Author: ionut
 */

#include <log.h>


std::mutex logger::m_mutex;
std::atomic<logger*> logger::m_instance{nullptr};


