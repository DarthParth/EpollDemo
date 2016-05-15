//============================================================================
// Name        : epoll_test_serv.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <fstream>
#include <exception>
#include "CommandDispatcher.hpp"
#include "HelloCommandProcessor.hpp"
#include "PingProcessor.hpp"
#include "ThreadPoolRequestExecutor.hpp"

using namespace std;


class DummyListener : public CommandDispatcherListener {
public:
	void onResponse(byteptr_t bytes, int peerSocket) override {
		auto length = bytes->size();
		auto rawBytes = bytes->rawBytes(length);
		send(peerSocket, (void *)rawBytes, length, 0);
	}
};

int make_socket_non_blocking(int sfd) {
	int flags {}, s {};

	flags = fcntl(sfd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl");
		return -1;
	}

	flags |= O_NONBLOCK;
	s = fcntl(sfd, F_SETFL, flags);
	if (s == -1) {
		perror("fcntl");
		return -1;
	}

	return 0;
}

int createSocket(int port) {
	int sock_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock_fd == 0)
		exit(1);

	sockaddr_in addr {};
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = port;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int bind_res = bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr));
	if(bind_res < 0)
		exit(1);
	if(make_socket_non_blocking(sock_fd))
		exit(1);
	listen(sock_fd, 1);
	return sock_fd;
}


int main(int arc, char** argv) {
	DummyListener listener {};
	ThreadPoolRequestExecutor executor {listener};
	CommandDispatcher dispatcher {executor};
	executor.addCommandProcessor("HELLO", std::make_shared<HelloCommandProcessor>("localhost"));
	executor.addCommandProcessor("PING", std::make_shared<PingProcessor>());

	if(arc < 1) {
		cout << "Port not given" << endl;
		exit(-1);
	}
	int port = atoi(argv[1]);
	cout << port << endl;
	int socket = createSocket(port);
	epoll_event event {};
	epoll_event *events = new epoll_event[1000];

	event.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;
	event.data.fd = socket;
	int epfd = epoll_create(1000);

	if(epoll_ctl(epfd, EPOLL_CTL_ADD, socket, &event))
		exit(1);

	int totalbytes { 0 };
	while(1) {
		int n = epoll_wait(epfd, events, 1000, -1);
		cout << "Event received" << endl;
		if(n < 0)
			exit(1);
		for (int i = 0; i < n; i++) {
			epoll_event ev = events[i];
			//There is an incoming connection
			if (ev.data.fd == socket) {
				cout << "Incoming connection" << endl;
				sockaddr addr {};
				unsigned int len {};
				int client_socket = accept(socket, (struct sockaddr *) &addr, &len);
				if(make_socket_non_blocking(client_socket))
					exit(1);
                event.data.fd = client_socket;
                event.events = EPOLLIN | EPOLLET;
                if(epoll_ctl (epfd, EPOLL_CTL_ADD, client_socket, &event))
                	exit(1);
				//There is incoming data
			} else {
				char buffer[128] {};
				while(1) {
					try {
						int bytesRead = recv(ev.data.fd, buffer, 128, 0);
						if(bytesRead <= 0)
							break;
						dispatcher.hydrate(buffer, bytesRead, ev.data.fd);
						totalbytes += bytesRead;
					} catch(const exception& e) {
						cout << e.what() << endl;
					}
				}
				cout << "Total bytes = " << totalbytes << endl;
			}
		}
	}
	return 0;
}
