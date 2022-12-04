
add_library(logger SHARED src/log.cpp)
target_link_libraries(logger pthread )
target_include_directories(logger PUBLIC  include )
target_include_directories(logger PUBLIC  compat )

