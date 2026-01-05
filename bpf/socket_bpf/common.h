#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <vector>

#define socketPath "test.sock"
#define socketLock "test.lock"

static bool recvAll(int clientSocket, char* buffer, uint64_t size)
{
	uint64_t totalRecv = 0;
	while (totalRecv < size)
	{
		int ret = recv(clientSocket, buffer + totalRecv, size - totalRecv, MSG_NOSIGNAL);
		if (ret <= 0)
		{
			return false;
		}

		totalRecv += ret;
	}

	return true;
}

static bool sendAll(int clientSocket, char* buffer, uint64_t size)
{
	uint64_t totalSend = 0;
	while (totalSend < size)
	{
		int ret = send(clientSocket, buffer + totalSend, size - totalSend, MSG_NOSIGNAL);
		if (ret <= 0)
		{
			return false;
		}

		totalSend += ret;
	}

	return true;
}

bool RecvBuffer(int clientSocket, std::vector<char>& buffer)
{
	uint64_t messageSize;
    if (!recvAll(clientSocket, (char*)&messageSize, sizeof(messageSize)))
    {
        std::cerr << "Error recvAll messageSize\n";
        return false;
    }

    buffer.resize(messageSize);
    if (!recvAll(clientSocket, buffer.data(), buffer.size()))
    {
        std::cerr << "Error recvAll buffer\n";
        return false;
    }

    std::cout << "RecvBuffer:: From clientSocket = " << clientSocket << ", messageSize = " << messageSize << ": " << buffer.data() << ";" << std::endl;
    return true;
}

bool SendBuffer(int clientSocket, std::vector<char>& buffer)
{
	uint64_t messageSize = buffer.size();
    if (!sendAll(clientSocket, (char*)&messageSize, sizeof(messageSize)))
    {
        std::cerr << "Error sendAll messageSize\n";
        return false;
    }

    if (!sendAll(clientSocket, buffer.data(), buffer.size()))
    {
        std::cerr << "Error sendAll buffer\n";
        return false;
    }

    return true;
}

bool TrySetLock() {
    // open lock file
    int lock_fd = open(socketLock, O_RDONLY | O_CREAT, 0600);
    if (lock_fd == -1) {
        std::cerr << "Error open lock file " << socketLock << ": " << strerror(errno) << "\n";
        return false;
    }

    // try to acquire lock
    int ret = flock(lock_fd, LOCK_EX | LOCK_NB);
    if (ret != 0) {
        std::cerr << "Error set lock on file " << socketLock << ": " << strerror(errno) << "\n";
        return false;
    }
    
    return true;
}
