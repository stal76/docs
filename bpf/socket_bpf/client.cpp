#include "common.h"

int main()
{
	int clientSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (clientSocket == -1)
	{
		perror("socket");
		return 1;
	}

	sockaddr_un address;
	memset((char*)&address, 0, sizeof(address));
	address.sun_family = AF_UNIX;
	strncpy(address.sun_path, socketPath, sizeof(address.sun_path) - 1);
	address.sun_path[sizeof(address.sun_path) - 1] = 0;

	int ret = connect(clientSocket, (struct sockaddr*)&address, sizeof(address));
	if (ret == -1)
	{
		perror("connect");
		return 1;
	}

	
	std::vector<char> buffer(100);
	snprintf(buffer.data(), buffer.size(), "Hello from client");

	if (!SendBuffer(clientSocket, buffer))
	{
		perror("send");
		return 1;
	}

	std::cout << "Success send\n";
	
	std::vector<char> buffer_read;
	if (!RecvBuffer(clientSocket, buffer_read))
	{
		perror("recv");
		return 1;
	}

	std::cout << "Success recv: " << buffer_read.size() << "\n";
	std::cout << buffer_read.data() << "\n";

	return 0;
}
