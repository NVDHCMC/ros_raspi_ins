
#ifndef IPC_COMMUNICATION_HPP
#define IPC_COMMUNICATION_HPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <rtdm/ipc.h>

namespace RTOS {
	class ipc_com
	{
	public:
		ipc_com(int svport, int clport);
		~ipc_com();
		void send(char * msg, int len);
	
	private:
		struct sockaddr_ipc server_addr;
		struct sockaddr_ipc client_addr;
		int client_port;
		int server_port;
		int ipc_socket;
		size_t stream_size;
	};
}

#endif
