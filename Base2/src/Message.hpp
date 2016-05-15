/*
 * Message.h
 *
 *  Created on: Apr 29, 2015
 *      Author: parth
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_
#include <memory>

//forward declaration
class Message;

//typedefs
typedef std::unique_ptr<ByteStream> byteptr_t;

template<class RequestType>
using reqptr_t = std::shared_ptr<RequestType>;

typedef std::unique_ptr<Message> messageptr_t;

class Message {
public:
	virtual ~Message() {}

	virtual byteptr_t toStream() const {
		ByteStream *stream = new ByteStream {};
		stream->insert(getVerb()); //Verb
		_toStream(*stream); //The actual data
		stream->insert(ByteStream::Raw { " END", 4 }); // Delimiter + end marker;
		return byteptr_t {stream};
	}

	virtual const char* getVerb() const = 0;

protected:
	virtual void _toStream(ByteStream& stream) const = 0;
};

#endif /* MESSAGE_H_ */
