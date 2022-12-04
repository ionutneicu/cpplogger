/*
 * log.h
 *
 *  Created on: Oct 12, 2021
 *      Author: ionut
 */

#ifndef LOG_H_
#define LOG_H_

#include <string>
#include <atomic>
#include <mutex>
#include <stdarg.h>
#include <memory>
#include <thread>
#include <future>
#include <condition_variable>
#include <queue>
#include <sstream>
#include <iostream>


// if c++ version less than 2017, use own implementation of std::apply
#if(__cplusplus <  201703L)
#include "std_cpp17_compat.h"
#endif

// if c++ version less than 2011, use own implementation of std::make_shared
#if( __cplusplus <  201402L)
#include "std_cpp11_compat.h"
#endif


enum log_level_t
{
	LOG_LEVEL_NONE=0,
	LOG_LEVEL_CRITICAL=1,
	LOG_LEVEL_ERROR=2,
	LOG_LEVEL_WARNING=3,
	LOG_LEVEL_INFO=4,
	LOG_LEVEL_DEBUG=5,
	LOG_LEVEL_VERBOSE=6
};


static inline std::ostream& operator << ( std::ostream & stream, const log_level_t& level )
{
	switch( level )
	{
		case LOG_LEVEL_NONE: stream << "NONE"; break;
		case LOG_LEVEL_CRITICAL: stream << "CRITICAL"; break;
		case LOG_LEVEL_ERROR: stream << "ERROR"; break;
		case LOG_LEVEL_WARNING: stream << "WARNING"; break;
		case LOG_LEVEL_INFO: stream << "INFO"; break;
		case LOG_LEVEL_DEBUG : stream << "DEBUG"; break;
		case LOG_LEVEL_VERBOSE : stream << "VERBOSE"; break;
	}
	return stream;
}

struct log_entry_base
{
	virtual std::string to_string(void) = 0;
    explicit log_entry_base()=default;
    log_entry_base(const log_entry_base&) = delete;
    log_entry_base& operator=(const log_entry_base&) = delete;
    ~log_entry_base() = default;
};



template<typename ... Args>
std::string format_log_string( const std::string& format, const Args & ... args )
{
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    char* buf =  new char[ size ];
    std::snprintf( buf, size, format.c_str(), args ... );
    return std::string( buf, buf + size - 1 );
}



template <typename ...Ts>
struct log_entry : public log_entry_base
{
	const log_level_t&		m_level;
	const char*				m_format;
	const char*				m_file_name;
	const size_t			m_file_line;
	std::tuple<Ts...> 		m_args;

	explicit log_entry(   const log_level_t& level,
			  	 const char* 		format,
			  	 const char* 		file_name,
				 const size_t 		file_line,
				 std::tuple<Ts...>   args)
			: m_level( level ),
			  m_format( format ),
			  m_file_name( file_name),
			  m_file_line( file_line),
			  m_args( args )
		{

		}
	log_entry(log_entry &&) = delete;
	log_entry(const log_entry&) = delete;
	log_entry& operator=(const log_entry&) = delete;
    ~log_entry() = default;


	std::string to_string(void) override
	{
		std::ostringstream stream;
	    stream << this->m_level << " " ;
	    stream << this->m_file_name << ":";
	    stream << this->m_file_line << " ";
	    auto fd_bind = [this]( Ts... args )
	    		{
	    				return format_log_string(this->m_format, args...);
	    		};
	    auto formated_line = std::apply(fd_bind, this->m_args);
	    stream << formated_line;
	    return stream.str();
	}
};




class logger
{
	static std::mutex m_mutex;
	static std::atomic<logger*> m_instance;
	std::thread m_background_thread;
	std::mutex m_log_queue_mutex;
	std::condition_variable m_log_queue_has_data_condition;
	std::queue<std::unique_ptr<log_entry_base>> m_log_queue;
	std::atomic<bool> m_log_running;
	static void log_fn( logger * instance)
	{
		while( true )
		{
			{
				bool running = instance->m_log_running.load(std::memory_order_acquire);
				std::unique_lock<std::mutex> lock( instance->m_log_queue_mutex );
				if( instance->m_log_queue.empty() )
				{
					if( ! running )
						break;
					else
					{
						auto now = std::chrono::system_clock::now();
						instance->m_log_queue_has_data_condition.wait_until( lock, now + std::chrono::milliseconds(100));
						continue;
					}
				}
				std::unique_ptr<log_entry_base> entry;
				std::swap( instance->m_log_queue.front(), entry );
				instance->m_log_queue.pop();
				lock.unlock();
				std::cout << entry->to_string() << std::endl;
		    }

		}
	}
	logger()
	{
		m_log_running = true;
		m_background_thread = std::move( std::thread( &logger::log_fn, this ) );
	}
public:
	static logger& get_instance()
		{
			// implement double check locking:
			logger* p = m_instance.load(std::memory_order_acquire);
			if( p == nullptr )
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				p = m_instance.load(std::memory_order_relaxed);
				if( p == nullptr )
				{
					p = new logger();
					m_instance.store( p, std::memory_order_release );
				}
			}
			return *p;
		}

	template<typename LogEntryT>
	void push_entry( std::unique_ptr<LogEntryT>&& entry )
	{
		{
			std::unique_lock<std::mutex> lock( m_log_queue_mutex );
			m_log_queue.emplace(std::move(entry));
		}
		m_log_queue_has_data_condition.notify_one();
	}

	void terminate()
	{
		bool expect_running = true;
		if( m_log_running.compare_exchange_weak(expect_running, false) )
		{
			m_log_queue_has_data_condition.notify_one();
			m_background_thread.join();
		}
	}

	~logger()
	{
		terminate();
	}
};



template <typename ...Ts>
inline void make_log_entry( const log_level_t& level,
					 const char* file,
					 const size_t line,
					 const char* format,
					 Ts... args )
{
	std::unique_ptr<log_entry_base> entry = std::make_unique<log_entry<Ts...>>( level, format, file, line, std::make_tuple(args...));
	logger::get_instance().push_entry( std::move(entry) );
}



template <typename ...Ts>
static void log_entry_proxy(const log_level_t & level,
					 const char *file,
					 const size_t line,
					 const char *format,
					 Ts&&... args )
{
	make_log_entry( level, file, line, format, args... );
}

#define LOG( __level__, __format__, ... )\
{\
	static constexpr char fmt[] = __format__;\
	log_entry_proxy( __level__,  __FILE__, __LINE__, fmt, __VA_ARGS__ );\
}

#define LOG_VERBOSE( __format__, ... ) 	LOG(LOG_LEVEL_VERBOSE, __format__, __VA_ARGS__)
#define LOG_DEBUG( __format__, ... ) 	LOG(LOG_LEVEL_DEBUG, __format__, __VA_ARGS__)
#define LOG_INFO( __format__, ... ) 	LOG(LOG_LEVEL_INFO, __format__, __VA_ARGS__)
#define LOG_WARN( __format__, ... ) 	LOG(LOG_LEVEL_WARNING, __format__, __VA_ARGS__)
#define LOG_ERROR( __format__, ... ) 	LOG(LOG_LEVEL_ERROR, __format__, __VA_ARGS__)
#define LOG_CRIT( __format__, ... ) 	LOG(LOG_LEVEL_CRITICAL, __format__, __VA_ARGS__)

#define LOG_TERMINATE() logger::get_instance().terminate();


#endif /* LOG_H_ */
