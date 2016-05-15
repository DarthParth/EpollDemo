/*
 * RequestExecutor.h
 *
 *  Created on: Dec 6, 2015
 *      Author: parth
 */

#ifndef REQUESTEXECUTOR_H_
#define REQUESTEXECUTOR_H_

#include "CommandProcessor.hpp"
#include "WorkflowException.hpp"
#include "CommandDispatcherListener.hpp"

using procptr_t = std::shared_ptr<CommandProcessorBase>;


class RequestExecutor {
public:
	RequestExecutor(CommandDispatcherListener& listener) : _listener(listener) {}
	virtual ~RequestExecutor() {}
	void addCommandProcessor(const std::string& name, procptr_t processor) {
		_processors[name] = processor;
	}

	void onRequest(byteptr_t requestBytes, int source) {
		const auto& command = requestBytes->next<std::string>();
		auto processor = _processors[command];
		if (nullptr == processor) {
			throw WorkflowException { "Unknown command" };
		}
		_onRequest(processor, std::move(requestBytes), source);
	}

	void onResponse(byteptr_t responseBytes, int source) {
		_listener.onResponse(std::move(responseBytes), source);
	}


protected:
	virtual void _onRequest(procptr_t processor, byteptr_t requestBytes, int source) {
		auto response = processor->processStream(*requestBytes.get());
		onResponse(std::move(response), source);
	}

private:
	std::map<std::string, procptr_t > _processors;
	CommandDispatcherListener& _listener;
};

#endif /* REQUESTEXECUTOR_H_ */
