#include <iostream>
#include "cpplog.h"


thread_local logger::category logger::m_verbose { "VERBOSE", logger::VERBOSE_LEVEL } ;
thread_local logger::category logger::m_debug {  "DEBUG", logger::DEBUG_LEVEL } ;
thread_local logger::category logger::m_warn { "WARN", logger::WARN_LEVEL} ;
thread_local logger::category logger::m_error { "ERROR", logger::ERROR_LEVEL } ;

std::mutex logger::m_global_mutex;
size_t     logger::m_current_level = logger::VERBOSE_LEVEL;
logger::backend_t logger::m_backend = []( const size_t level, const std::string category_label_str, const std::string file, const std::size_t line, const std::string content ) -> void
                    {
                        std::cout << "[" << category_label_str << "][" << file << "(" << line << ")]: " << content << std::endl;
                    };
