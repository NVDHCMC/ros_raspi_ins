#include "ipc_communication.hpp"

namespace RTOS {
	ipc_com::ipc_com(int svport, int clport) {
        this->server_port = svport;
        this->client_port = clport;
        this->ipc_socket = socket(AF_RTIPC, SOCK_DGRAM, IPCPROTO_XDDP);
        int ret;
        
        if (this->ipc_socket < 0) {
            perror("socket");
        }
        
        this->stream_size = 1024;
        
        ret = setsockopt(this->ipc_socket, SOL_XDDP, XDDP_BUFSZ, &this->stream_size, sizeof(this->stream_size));
        
        memset(&this->server_addr, 0, sizeof(this->server_addr));
        this->server_addr.sipc_family = AF_RTIPC;
        this->server_addr.sipc_port = this->server_port;
        
        ret = bind(this->ipc_socket, (struct sockaddr *)&this->server_addr, sizeof(this->server_addr));
        
        if (ret) {
            perror("bind");
        }
	}

	ipc_com::~ipc_com() {
		close(this->ipc_socket);
	}

	void ipc_com::send(char * msg, int len) {
		int ret;

        //ret = sendto(this->ipc_socket, msg, len, 0, (struct sockaddr *)&this->server_addr, sizeof(this->server_addr));
        ret = sendto(this->ipc_socket, msg, len, 0, NULL, 0);
        
        if (ret < 0) {
        	perror("send to.");
        }
	}
}