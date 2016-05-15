/*
 * HelloMessage.h
 *
 *  Created on: Jan 4, 2015
 *      Author: parth
 */

#ifndef HELLOMESSAGE_H_
#define HELLOMESSAGE_H_
#include <set>
#include <string>
#include "Message.hpp"

class HelloMessage : public Message {
private:
	std::string _source;
	std::set<std::string> _newPeers;
public:
	virtual ~HelloMessage() {}
	HelloMessage(std::string source, const std::set<std::string>& peers) :
			_source(source),
			_newPeers(peers) {}

	const std::string& getSource() const { return _source; }
	const std::set<std::string>& getNewPeers() const { return _newPeers; }
	virtual const char* getVerb() const { return "HELLO"; }
protected:
	virtual void _toStream(ByteStream& stream) const {
		stream.insert(_source);
		stream.insert(_newPeers.size());
		for(std::string peer : _newPeers) {
			stream.insert(peer);
		}
	}
};


#endif /* HELLOMESSAGE_H_ */
