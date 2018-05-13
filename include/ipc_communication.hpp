
#ifndef IPC_COMMUNICATION_HPP
#define IPC_COMMUNICATION_HPP

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
		int socket;
	};
}

#endif
