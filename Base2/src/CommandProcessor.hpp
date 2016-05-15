#ifndef COMMANDPROCESSOR_H_
#define COMMANDPROCESSOR_H_


#include "WorkflowException.hpp"
#include "ByteStream.hpp"
#include "Message.hpp"


class CommandProcessorBase {
public:
	virtual ~CommandProcessorBase() {}
	byteptr_t processStream(const ByteStream& requestBytes) {
		return _processStream(requestBytes);
	}

protected:
	virtual byteptr_t _processStream(const ByteStream& requestBytes) = 0;
};


template<class RequestType>
class CommandProcessor : public CommandProcessorBase {
public:
	virtual ~CommandProcessor() {}

protected:
	virtual reqptr_t<RequestType> _toRequest(const ByteStream& stream) const = 0;
	virtual messageptr_t _processMessage(reqptr_t<RequestType> request) = 0;

	byteptr_t _processStream(const ByteStream& requestBytes) {
		auto req = _toRequest(requestBytes);
		auto res = _processMessage(req);
		return res->toStream();
	}
};


#endif /* COMMANDPROCESSOR_H_ */
