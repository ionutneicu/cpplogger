#include <iostream>
#include "cpplog.h"


int main()
{
   /* optional, default level is verbose */
   logger::set_level( logger::WARN_LEVEL );

   /*optional, default is cout only*/
   logger::set_backend( [](  const size_t level, const std::string category_label_str, const std::string file, const std::size_t line, const std::string content ) -> void 
                        {
                            if( level >= logger::m_current_level )
                                std::cout << "[" << category_label_str << "][" << file << "(" << line << ")]: " << content << std::endl;
                            else
                                std::cerr << "*[" << category_label_str << "][" << file << "(" << line << ")]: " << content << std::endl;
                        } );
   
  
  for( int i = 0; i < 10; ++ i ) {    
    logger::verbose() << "some i = " << i;
    logger::debug() << "some i = " << i;
    logger::warn() << "some i = " << i;
    logger::error() << "some i = " << i;
  }

}
