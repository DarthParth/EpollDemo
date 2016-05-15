/*
 * PeerTracker.h
 *
 *  Created on: Dec 13, 2014
 *      Author: parth
 */

#ifndef PEERTRACKER_H_
#define PEERTRACKER_H_
#include <string>
#include <set>

class PeerTracker {
public:
	void update(const std::set<std::string> &incoming) {
		std::set<std::string> diff {};
		_discoveredPeers.insert(incoming.begin(), incoming.end());
	}

	const std::set<std::string> &getPeers() {
		return _discoveredPeers;
	}

	static PeerTracker& instance() {
		if(!_instance) {
			_instance = new PeerTracker {};
		}
		return *_instance;
	}

	PeerTracker() {}
	~PeerTracker() {}
private:
	static PeerTracker *_instance;
	std::set<std::string> _discoveredPeers;
};

#endif /* PEERTRACKER_H_ */
