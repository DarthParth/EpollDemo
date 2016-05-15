/*
 * Parser.h
 *
 *  Created on: Dec 8, 2014
 *      Author: parth
 */

#ifndef COMMANDDISPATCHER_H_
#define COMMANDDISPATCHER_H_
#include <memory>
#include <cstring>
#include <map>

#include "ByteStream.hpp"
#include "WorkflowException.hpp"
#include "Message.hpp"
#include "CommandProcessor.hpp"
#include "RequestExecutor.hpp"
#include "CommandDispatcherListener.hpp"

class CommandDispatcher {
public:

	void hydrate(const char* buffer, unsigned int buffSize, int peerSocket) {
		if(buffSize == 0) {
			return;
		}

		auto stream = _byteStreams[peerSocket];
		if(nullptr == stream) {
			stream = new ByteStream {};
			_byteStreams[peerSocket] = stream;
		}
		_hydrate(buffer, buffSize, stream, peerSocket);
	}

	void _hydrate(const char* buffer, unsigned int buffSize, ByteStream* stream, int peerSocket) {
		auto end = endMarker(buffer, buffSize);

		if(end) {
			size_t bytesProcessed = (end - buffer);
			stream->insert(ByteStream::Raw { buffer, bytesProcessed });
			_byteStreams.erase(peerSocket);
			_executor.onRequest(byteptr_t(stream), peerSocket);
			hydrate(end + 4, buffSize - bytesProcessed - 4, peerSocket);
		} else {
			stream->insert(ByteStream::Raw {buffer, buffSize });
		}
	}

	const char *endMarker(const char* buffer, unsigned int buffSize) {
		auto start = buffer;
		for(unsigned int i = 0; i < (buffSize - 3); i++) {
			if(start[0] == ' ' && (start + 1)[0] == 'E' && (start + 2)[0] == 'N' && (start + 3)[0] == 'D') {
				return start;
			}
			start += 1;
		}
		return nullptr;
	}

	CommandDispatcher(RequestExecutor& executor) : _executor(executor) {}
	virtual ~CommandDispatcher() {}
private:
	RequestExecutor& _executor;
	std::map<int, ByteStream*> _byteStreams;
};

#endif /* COMMANDDISPATCHER_H_ */
