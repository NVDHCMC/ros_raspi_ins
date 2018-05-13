#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <rtdm/ipc.h>
#include "ipc_communication.hpp"

namespace RTOS {
	ipc_com::ipc_com(int svport, int clport) {
        this->server_port = svport;
        this->client_port = clport;
        this->socket = socket(AF_RTIPC, SOCK_DGRAM, IPCPROTO_IDDP);
        
        if (this->socket < 0) {
            perror("socket");
        }

        this->client_addr.sipc_family = AF_RTIPC;
        this->client_addr.sipc_port = this->client_port;
        ret = bind(this->socket, (struct sockaddr *)&this->client_addr, sizeof(this->client_addr));

        if (ret) {
            perror("bind");
        }
        
        this->server_addr.sipc_family = AF_RTIPC;
        this->server_addr.sipc_port = this->server_port;
	}

	ipc_com::~ipc_com() {
		close(this->socket);
	}

	void ipc_com::send(char * msg, int len) {
		int ret;
        
        ret = sendto(s, msg[n], len, 0, (struct sockaddr *)&this->server_addr, sizeof(this->server_addr));
        
        if (ret < 0) {
        	perror("send to.");
        }
	}
}