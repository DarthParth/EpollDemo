/*
 * ThreadPool.h
 *
 *  Created on: Nov 17, 2013
 *      Author: parth
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <vector>
#include <functional>
#include <memory>
#include <iostream>

#include "BlockingQueue.hpp"
#include "Thread.hpp"


template<class ReturnType>
class FutureResult {
private:
	ReturnType _result;
	pthread_mutex_t _mutex;
	pthread_cond_t _condition;
	bool _resultReady;
public:
	FutureResult() :
			_resultReady(false) {
		pthread_mutex_init(&_mutex, NULL);
		pthread_cond_init(&_condition, NULL);
	}

	inline ReturnType get() {
		pthread_mutex_lock(&_mutex);
		while (!_resultReady)
			pthread_cond_wait(&_condition, &_mutex);
		pthread_mutex_unlock(&_mutex);
		return _result;
	}

	inline void set(ReturnType result) {
		pthread_mutex_lock(&_mutex);
		_result = result;
		_resultReady = true;
		pthread_cond_signal(&_condition);
		pthread_mutex_unlock(&_mutex);
	}
};

template<class ReturnType>
class Future {
public:
	Future(std::shared_ptr<FutureResult<ReturnType> > futureResult) :
			_futurResult(futureResult) {
	}
	ReturnType get() {
		return _futurResult->get();
	}
private:
	std::shared_ptr<FutureResult<ReturnType> > _futurResult;

};

class ThreadPool {
public:
	ThreadPool(int cores, int queueDepth) :
			_blockingQueue(queueDepth) {
		for (int core = 1; core <= cores; core++) {
			Worker *worker = new Worker { _blockingQueue };
			_workers.push_back(worker);
			worker->start();
		}
	}
	virtual ~ThreadPool() {
		for (std::vector<Worker *>::iterator iter = _workers.begin();
				iter != _workers.end(); iter++) {
			delete *iter;
		}
		_workers.clear();
	}

	template<class ReturnType, class FuncType>
	inline Future<ReturnType> submit(std::function<FuncType> command) {

		std::shared_ptr<FutureResult<ReturnType> > futureResult =
				std::make_shared<FutureResult<ReturnType> >();

		//Lambda function that will run our main command and give the result to the FutureResult instance
		std::function<void()> executor = [futureResult, command] () mutable {
			ReturnType result = command();
			futureResult->set(result);
		};
		Future<ReturnType> future(futureResult);
		_blockingQueue.put(executor);
		return future;
	}

	inline void submit(std::function<void()> command) {
		_blockingQueue.put(command);
	}

private:
	BlockingQueue _blockingQueue;

	class Worker: public Thread {
	public:
		Worker(BlockingQueue &blockingQueue) :
				Thread(), _blockingQueue(blockingQueue) {
		}

		virtual ~Worker() {}

		virtual void run() {
			while (true) {
				std::function<void()> executor = _blockingQueue.take();
				executor();
			}
		}
	private:
		BlockingQueue &_blockingQueue;
	};
	std::vector<Worker *> _workers;
};

#endif /* THREADPOOL_H_ */
