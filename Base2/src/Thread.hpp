/*
 * Thread.h
 *
 *  Created on: Oct 23, 2013
 *      Author: parth
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>
#include "ThreadException.hpp"
#include <iostream>

void *_run(void *t);

class Thread {
public:
	Thread() : _threadID(0) {}
	virtual ~Thread() {}
	friend void *_run(void *thread);

	int start() {
		int errorCode = pthread_create(&_threadID, NULL, _run, (void *)this);
		if(errorCode) {
			throw ThreadException { "Cannot create thread, failed with error " + errorCode };
		}
		return _threadID;
	}

	pthread_t getID() { return _threadID; }

protected:
	virtual void run() = 0;
private:
	pthread_t _threadID;
};

inline void *_run(void *t) {
	Thread *thread = (Thread *)t;
	thread->run();
	pthread_exit(0);
}

#endif /* THREAD_H_ */
