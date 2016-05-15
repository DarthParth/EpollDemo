/*
 * PingProcessor.h
 *
 *  Created on: Nov 28, 2015
 *      Author: parth
 */

#ifndef PINGPROCESSOR_H_
#define PINGPROCESSOR_H_
#include "CommandDispatcher.hpp"
#include "PingPong.hpp"
#include <memory>

class PingProcessor : public CommandProcessor<PingMessage>{
public:
	PingProcessor() {}
	virtual ~PingProcessor() {}
protected:
	virtual reqptr_t<PingMessage> _toRequest(const ByteStream& stream) const override {
		const std::string message = stream.next<std::string>();
		reqptr_t<PingMessage> req = std::make_shared<PingMessage>(message);
		return req;
	}

	virtual messageptr_t _processMessage(reqptr_t<PingMessage> request) override {
		const std::string& message = request->getMessage();
		messageptr_t resp = messageptr_t(new PongMessage {message});
		return resp;
	}
};

#endif /* PINGPROCESSOR_H_ */
