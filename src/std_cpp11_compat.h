/*
 * std_cpp_11_compat.h
 *
 *  Created on: Dec 4, 2022
 *      Author: ionut
 */

#ifndef SRC_STD_CPP_11_COMPAT_H_
#define SRC_STD_CPP_11_COMPAT_H_


namespace std {
	template<typename T, typename... Args>
	std::unique_ptr<T> make_unique(Args&&... args)
	{
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}
}


#endif /* SRC_STD_CPP_11_COMPAT_H_ */
