# cpplogger
A simple C++ 11 logging module

  Copyright 2024 Ionut Neicu
  This is a simple yet usable log class written in C++11
 
  Features:
     - simple
     - lightweight
     - C++ ostream-like access
           logger::verbose() << "some text " << variable << ... ;
     - no std::endl needed after every line ;)
     - supports multithreading
     - extensible with other backends ( see examples )
     - Keep It Simple and Youre Ain't Gonna Need It ;) - easy to customize
