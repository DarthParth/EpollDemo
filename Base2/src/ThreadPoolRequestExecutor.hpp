/*
 * ThreadPoolRequestExecutor.h
 *
 *  Created on: Dec 6, 2015
 *      Author: parth
 */

#ifndef THREADPOOLREQUESTEXECUTOR_H_
#define THREADPOOLREQUESTEXECUTOR_H_

#include "ThreadPool.hpp"
#include "RequestExecutor.hpp"

class ThreadPoolRequestExecutor : public RequestExecutor {
public:
	ThreadPoolRequestExecutor(CommandDispatcherListener& listener) :
		RequestExecutor(listener),
		_pool(new ThreadPool {15, 100}) {

	}
	virtual ~ThreadPoolRequestExecutor() {}

protected:
	virtual void _onRequest(procptr_t processor, byteptr_t requestBytes, int source) {

		//The main thread no longer owns it, passes on ownership to worker threads.
		auto rawRequestBytes = requestBytes.release();

		auto job = [this, rawRequestBytes, processor, source] () {
			auto response = processor->processStream(*(rawRequestBytes));
			onResponse(std::move(response), source);
			delete rawRequestBytes;
		};
		_pool->submit(job);
	}

private:
	ThreadPool *_pool;
};

#endif /* THREADPOOLREQUESTEXECUTOR_H_ */
