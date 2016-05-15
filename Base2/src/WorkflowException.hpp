/*
 * StreamCorruptException.h
 *
 *  Created on: Dec 8, 2014
 *      Author: parth
 */

#ifndef STREAMCORRUPTEXCEPTION_H_
#define STREAMCORRUPTEXCEPTION_H_
#include <stdexcept>
#include <string>

class WorkflowException : public std::exception {
public:
	explicit WorkflowException(const std::string& what) : _what(what) {}
	virtual ~WorkflowException() noexcept {}
	virtual const char* what() const noexcept { return _what.c_str(); }
private:
	std::string _what;

};

#endif /* STREAMCORRUPTEXCEPTION_H_ */
