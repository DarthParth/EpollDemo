/*
 * PingMessage.h
 *
 *  Created on: Nov 28, 2015
 *      Author: parth
 */

#ifndef PINGMESSAGE_H_
#define PINGMESSAGE_H_
#include <string>
#include "Message.hpp"

class PingMessage : public Message {
private:
	std::string _message;
public:
	PingMessage(const std::string& message) : _message(message) {}
	virtual ~PingMessage() {}

	virtual const char *getVerb() const {
		return "PING";
	}

	const std::string& getMessage() {
		return _message;
	}

protected:
	virtual void _toStream(ByteStream& bytes) const {
		//Send some dummy message.
		bytes.insert(_message);
	}
};

class PongMessage : public Message {
private:
	std::string _message;

public:
	PongMessage(const std::string& message) : _message(message) {}
	virtual ~PongMessage() {}

	virtual const char *getVerb() const {
		return "PONG";
	}

protected:
	virtual void _toStream(ByteStream& bytes) const {
		//Send back what we just got.
		bytes.insert(_message);
	}
};

#endif /* PINGMESSAGE_H_ */
