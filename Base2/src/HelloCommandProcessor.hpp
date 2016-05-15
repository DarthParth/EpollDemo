/*
 * HelloCommandProcessor.h
 *
 *  Created on: May 25, 2015
 *      Author: parth
 */

#ifndef HELLOCOMMANDPROCESSOR_H_
#define HELLOCOMMANDPROCESSOR_H_

#include <stddef.h>
#include <algorithm>
#include <iterator>
#include <memory>
#include <set>
#include <string>
#include <utility>

#include "ByteStream.hpp"
#include "CommandDispatcher.hpp"
#include "HelloMessage.hpp"
#include "PeerTracker.h"

class HelloCommandProcessor : public CommandProcessor<HelloMessage> {
public:

	HelloCommandProcessor(std::string me) : _me(me) {}
	virtual ~HelloCommandProcessor() {}

protected:
	reqptr_t<HelloMessage> _toRequest(const ByteStream& requestBytes) const override {
		std::string source = requestBytes.next<std::string>();
		std::set<std::string> newPeers {};
		int numPeers = requestBytes.next<size_t>();
		for(int i = 1; i <= numPeers; i++) {
			newPeers.insert(requestBytes.next<std::string>());
		}

		/* We do not want to have ourselves as a peer */
		newPeers.erase(_me);
		return std::make_shared<HelloMessage>(source, newPeers);
	}

	messageptr_t _processMessage(reqptr_t<HelloMessage> request) override {
		PeerTracker &tracker = PeerTracker::instance();
		auto incomingPeers = request->getNewPeers();
		incomingPeers.insert(request->getSource());
		auto myPeers = tracker.getPeers();

		std::set<std::string> peersToSend {};
		std::set_difference(myPeers.begin(),
				myPeers.end(),
				incomingPeers.begin(),
				incomingPeers.end(),
				std::inserter(peersToSend, peersToSend.end()));

		tracker.update(incomingPeers);
		return std::unique_ptr<HelloMessage>(new HelloMessage {_me, peersToSend});
	}
private:
	std::string _me;
};


#endif /* HELLOCOMMANDPROCESSOR_H_ */
