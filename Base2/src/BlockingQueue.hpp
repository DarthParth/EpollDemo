/*
 * BlockingQueue.h
 *
 *  Created on: Nov 17, 2013
 *      Author: parth
 */

#ifndef BLOCKINGQUEUE_H_
#define BLOCKINGQUEUE_H_
#include <queue>
#include <functional>


class BlockingQueue {
public:
	BlockingQueue(int depth) {
		pthread_mutex_init(&_mutex, NULL);
		pthread_cond_init(&_queueFullCondition, NULL);
		pthread_cond_init(&_queueSlotAvailableCondition, NULL);
	}

	virtual ~BlockingQueue() {
		pthread_mutex_destroy(&_mutex);
		pthread_cond_destroy(&_queueFullCondition);
		pthread_cond_destroy(&_queueSlotAvailableCondition);
	}

	void put(std::function<void()> element) {
		pthread_mutex_lock(&_mutex);
		_q.push(element);
		pthread_cond_broadcast(&_queueSlotAvailableCondition);
		pthread_mutex_unlock(&_mutex);
	}

	std::function<void()> take() {
		pthread_mutex_lock(&_mutex);
		while (_q.empty())
			pthread_cond_wait(&_queueSlotAvailableCondition, &_mutex);
		std::function<void()> element = _q.front();
		_q.pop();
		pthread_mutex_unlock(&_mutex);
		return element;
	}
private:
	std::queue<std::function<void()>> _q;
	pthread_mutex_t _mutex;
	pthread_cond_t _queueFullCondition;
	pthread_cond_t _queueSlotAvailableCondition;
};

#endif /* BLOCKINGQUEUE_H_ */
