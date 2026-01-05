#include "common.h"
// #include <fcntl.h>


void clientThread(int clientSocket);

int main()
{
    std::cout << "Start server, pid = " << getpid() <<"\n";
    if (!TrySetLock()) {
        return 1;
    }

    int serverSocket = socket(AF_UNIX, SOCK_STREAM, 0); // kprobe:__sys_socket
    if (serverSocket < 0)
    {
        perror("Error open socket");
        return 1;
    }

    // struct flock lock;
    // std::cout << fcntl(serverSocket, F_GETFL, &lock) << "\n";
    // std::cout << "type=" << lock.l_type << ", whence=" << lock.l_whence << ", start=" << lock.l_start
    //           << ", len=" << lock.l_len << ", pid=" << lock.l_pid << "\n";

    unlink(socketPath);

    sockaddr_un address;
    memset((char*)&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, socketPath, sizeof(address.sun_path) - 1);
    address.sun_path[sizeof(address.sun_path) - 1] = 0;
    
    std::cout << "address.sun_family = " << address.sun_family << "\n";
    std::cout << "address.sun_path = " << address.sun_path << ";\n";

    // tracepoint:syscalls:sys_enter_bind
    // kprobe:unix_bind
    if (bind(serverSocket, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        perror("bind");
        return 1;
    }

    if (listen(serverSocket, 64) < 0)   // kprobe:unix_listen
    {
        perror("listen");
        return 1;
    }

    std::cout << "Success\n";

    for (;;)
    {
        std::cout << "Start accept" << std::endl;
        struct sockaddr_in address;
        socklen_t addressLength = sizeof(address);
        // tracepoint:syscalls:sys_enter_accept
        // kprobe:unix_accept
        int clientSocket = accept(serverSocket, (struct sockaddr*)&address, &addressLength);
        if (clientSocket < 0)
        {
            perror("accept");
            continue;
        }
        std::cout << "accept from: " << address.sin_family << ";" << address.sin_port << ";" << address.sin_addr.s_addr << "\n";

        // std::thread([this, clientSocket] { clientThread(clientSocket); }).detach();
        clientThread(clientSocket);
        // break;

        close(clientSocket);
    }


    return 0;
}

void clientThread(int clientSocket)
{
    std::cout << "Start clientThread, clientSocket = " << clientSocket << "\n";
    std::vector<char> buffer;
    if (!RecvBuffer(clientSocket, buffer))
    {
        std::cerr << "Error recv" << std::endl;
        return;
    }
    std::cout << "Recv: " << buffer.size() << std::endl;

    snprintf(buffer.data(), buffer.size(), "Hello from server, pid=%d", getpid());
    if (!SendBuffer(clientSocket, buffer))
    {
        std::cerr << "Error send buffer" << std::endl;
    }
    std::cout << "Sent: " << buffer.data() << std::endl;
}