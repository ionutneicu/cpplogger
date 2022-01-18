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


template <class T>
class singleton
{
private:
	  static std::mutex m_mutex;
	  static std::atomic<T*> m_instance;
protected:
	  singleton() = default;
public:
	static const T& get_instance()
	{
		// implement double check locking:
		singleton<T>*p = m_instance.load(std::memory_order_acquire);
		if( p == nullptr )
		{
			p = m_instance.load(std::memory_order_relaxed);
			std::lock_guard<std::mutex> lock(m_mutex);
			if( p == nullptr )
			{
				p = new T;
				m_instance.store( p, std::memory_order_release );
			}
		}
		return *p;
	}
};
class log_level_t;
class log_level_t: public singleton<log_level_t>
{
public:
	virtual const std::string& get_name() = 0;
	virtual bool operator >= ( const log_level_t& rhs ) = 0;
	virtual ~log_level_t(){}
};

class verbose : public log_level_t
{
		const std::string m_name {"VERBOSE"};
		verbose()
		{

		}
public:
		const std::string& get_name() { return m_name; }
		bool operator >= ( const log_level_t& rhs ) { return true; }
		virtual ~verbose(){}
};




class log_entry_base
{
public:
	char*		  	m_file_name;
	unsigned int	m_file_line;
	char*			m_msg;
protected:
	virtual 		  const std::string to_string() = 0;
};



template <typename ...Ts>
class log_entry: public log_entry_base
{
	std::tuple<Ts...> 		m_args;
	const log_level_t&		m_level;
	const char*				m_format;
	const char*				m_file_name;
	const size_t			m_file_line;
public:
	  log_entry( const log_level_t& level,
			  	 const char* 		format,
			  	 const char* 		file_name,
				 const size_t 		file_line,
			     Ts... args)
			: m_level( level ),
			  m_format( format ),
			  m_file_name( file_name),
			  m_file_line( file_line),
			  m_args( std::move( std::make_tuple(args...) ))
		{

		}
};

class logger;
class logger : public singleton<logger>
{
public:
	void init();
	void push_entry( const log_entry_base& entry );
	void stop();
};




#define LOG( __level__, __format__, ... )\
{\
	static const char fmt[] = __format__;\
	auto entry = log_entry( __level__, fmt, __FILE__, __LINE__, __VA_ARGS__ );\
    singleton<logger>::get_instance().push_entry(std::move(entry));\
}


#endif /* LOG_H_ */
