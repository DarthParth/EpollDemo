/*
 * ThreadException.h
 *
 *  Created on: Oct 23, 2013
 *      Author: parth
 */

#ifndef THREADEXCEPTION_H_
#define THREADEXCEPTION_H_

#include <string>
#include <iostream>
#include <exception>

class ThreadException : public std::exception {
public:
	ThreadException(std::string message) : _message( message ) {}
	friend std::ostream& operator<<(std::ostream& out, ThreadException& exception);
	virtual ~ThreadException() noexcept {};
	virtual const char* what() const noexcept { return _message.c_str(); }
private:
	std::string _message;

};

#endif /* THREADEXCEPTION_H_ */
