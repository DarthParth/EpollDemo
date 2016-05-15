/*
 * CommandDispatcherListener.h
 *
 *  Created on: Dec 6, 2015
 *      Author: parth
 */

#ifndef COMMANDDISPATCHERLISTENER_H_
#define COMMANDDISPATCHERLISTENER_H_



class CommandDispatcherListener {
public:
	virtual ~CommandDispatcherListener() {}
	virtual void onResponse(byteptr_t responseBytes, int peerSocket) = 0;
};


#endif /* COMMANDDISPATCHERLISTENER_H_ */
