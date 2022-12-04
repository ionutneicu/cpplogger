#pragma once

#include <ostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <functional>

struct logger
{
        class category_wrapper;
        static constexpr size_t VERBOSE_LEVEL = 0;
        static constexpr size_t DEBUG_LEVEL = 1;
        static constexpr size_t WARN_LEVEL = 2;
        static constexpr size_t ERROR_LEVEL = 3;
        static std::mutex       m_global_mutex;
        static size_t           m_current_level;
        using  backend_t = std::function<void( const size_t level, const std::string& category_label_str, const std::string& file, const std::size_t line, const std::string& content )>;
        static backend_t m_backend;

        class category
        {
            friend class category_wrapper;
            std::string m_label;
            size_t      m_level;
            std::ostringstream m_stream;
            std::string m_current_file;
            size_t      m_current_line;
            protected:
                void release_line()
                {
                    if( logger::m_current_level > m_level )
                        return;

                    std::ostringstream ss;
                    {
                        std::unique_lock<std::mutex> lk(logger::m_global_mutex);
                        logger::m_backend( m_level, m_label, m_current_file, m_current_line, m_stream.str() );
                        std::swap( m_stream, ss ) ;
                    }
                }

            public:
            category( const category & rhs ) = delete;

            category( std::string label, size_t level)
                :  m_label(label), m_level( level )
            {

            }

            template<typename T>
            category& operator << ( const T & value )
            {
                if( logger::m_current_level <= m_level )
                {
                    m_stream << value;
                }
                return *this;
            }

            category& set_source_location( std::string file, size_t line )
            {
                m_current_file = file;
                m_current_line = line;
                return *this;
            }

        };

        static thread_local category m_verbose;
        static thread_local category m_debug;
        static thread_local category m_warn;
        static thread_local category m_error;


        class category_wrapper
        {
            category& m_cat;
            public:
            category_wrapper( category& cat )
                : m_cat(cat)
            {

            }

            ~category_wrapper()
            {
                 m_cat.release_line();
            }

            template<typename T>
            category_wrapper& operator << ( const T & value )
            {
                m_cat << value;
                return *this;
            }
        };



    static category_wrapper verbose( std::string file = __FILE__, size_t line = __LINE__ )
    {
        return category_wrapper(m_verbose.set_source_location( file, line ));
    }

    static category_wrapper debug(std::string file = __FILE__, size_t line = __LINE__ )
    {
        return category_wrapper(m_debug.set_source_location( file, line ));
    }

    static category_wrapper warn( std::string file = __FILE__, size_t line = __LINE__ )
    {
        return category_wrapper(m_warn.set_source_location( file, line ));
    }

    static category_wrapper error(std::string file = __FILE__, size_t line = __LINE__ )
    {
        return category_wrapper(m_error.set_source_location( file, line ));
    }

    static void set_level( size_t new_level )
    {
        m_current_level = new_level;
    }

    static void set_backend( backend_t new_backend )
    {
        m_backend = new_backend;
    }
};


