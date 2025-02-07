#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define TEST_SOCKET_PATH "/tmp/test_select_%d.sock"
#define WRITERS_NUMBER 3

struct timeval time_start;

double get_time() {
    struct timeval time_end;
    gettimeofday(&time_end, NULL);
    return (time_end.tv_sec - time_start.tv_sec) + (time_end.tv_usec - time_start.tv_usec) * 1e-6;
}

struct ThreadSocketWriterInfo {
    int index;
    char socket_path[256];
    int pipes[2];
    int time_shift;
    int time_sleep;
};

int thread_socket_writer(void* args) {
    struct ThreadSocketWriterInfo* info = (struct ThreadSocketWriterInfo*)args;
    printf("%10.6f %sstart thread socket writer %d%s\n", get_time(), ANSI_COLOR_MAGENTA, info->index, ANSI_COLOR_RESET);
    
    // Create socket
    int fd_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd_socket < 0) {
        perror("error create socket");
        return 1;
    }

    // Bind socket to file + listen
    struct sockaddr_un address;
    memset((char*)&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, info->socket_path, sizeof(address.sun_path) - 1);
    address.sun_path[sizeof(address.sun_path) - 1] = 0;
    unlink(info->socket_path);
    if (bind(fd_socket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        printf("error bind socket on %s, %d: %s\n", info->socket_path, errno, strerror(errno));
        return 1;
    } else if (listen(fd_socket, 64) < 0) {
        printf("error listen socket on %s, %d: %s\n", info->socket_path, errno, strerror(errno));
        return 1;
    }

    // Accept (only one client!)
    struct sockaddr_in6 address_in;
	socklen_t addressLength = sizeof(address_in);
	memset((char*)&address_in, 0, sizeof(address_in));
	int clientSocket = accept(fd_socket, (struct sockaddr*)&address_in, &addressLength);
	if (clientSocket < 0)
	{
		printf("error accept socket on %s, %d: %s\n", info->socket_path, errno, strerror(errno));
		return 1;
	}

    // Send messages
    sleep(info->time_shift);
    for (int index = 0; index < 1000; index++) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "message %d", index + 1);
        if (send(clientSocket, buffer, strlen(buffer), MSG_NOSIGNAL) <= 0) {
            printf("error send message from thread %d, %d: %s\n", info->index, errno, strerror(errno));
            break;
        }

        // timeout or wait signal from pipe
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(info->pipes[0], &rfds);

        struct timeval tv;
        tv.tv_sec = info->time_sleep;
        tv.tv_usec = 0;
        
        int retval = select(info->pipes[0] + 1, &rfds, NULL, NULL, &tv);
        if (retval < 0) {
            printf("error select in thread %d, %d: %s\n", info->index, errno, strerror(errno));
            return 1;
        } else if (FD_ISSET(info->pipes[0], &rfds)) {
            printf("%10.6f %sreceived signal for stopping in writer %d%s\n", get_time(), ANSI_COLOR_MAGENTA, index + 1, ANSI_COLOR_RESET);
            break;
        }
    }

    close(info->pipes[0]);
    close(clientSocket);
    close(fd_socket);
    
    printf("%10.6f %sexit thread socket writer %d%s\n", get_time(), ANSI_COLOR_MAGENTA, info->index, ANSI_COLOR_RESET);
    return 0;
}

int create_socket_for_read(const char* socket_path) {
    int fd_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd_socket < 0)
	{
		perror("error create socket");
		return -1;
	}

    struct sockaddr_un server_addr;
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, socket_path, sizeof(server_addr.sun_path) - 1);
	if (connect(fd_socket,
		    (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
		printf("error connect to socket %s, %d: %s\n", server_addr.sun_path, errno, strerror(errno));
		return -1;
	}

    return fd_socket;
}

int thread_reader(void* args) {
    struct ThreadSocketWriterInfo* info_writers = (struct ThreadSocketWriterInfo*)args;
    printf("%10.6f %sstart thread reader%s\n", get_time(), ANSI_COLOR_CYAN, ANSI_COLOR_RESET);

    int writers_sockets[WRITERS_NUMBER];
    for (int index = 0; index < WRITERS_NUMBER; index++) {
        writers_sockets[index] = create_socket_for_read(info_writers[index].socket_path);
        if (writers_sockets[index] == -1) {
            return 1;
        }
    }

    int writers_total = WRITERS_NUMBER;
    while (writers_total > 0) {
        int max_sd = 0;
        fd_set rfds;
        FD_ZERO(&rfds);
        for (int index = 0; index < WRITERS_NUMBER; index++) {
            if (writers_sockets[index] != -1) {
                FD_SET(writers_sockets[index], &rfds);
                if (max_sd < writers_sockets[index]) {
                    max_sd = writers_sockets[index];
                }
            }
        }

        int retval = select(max_sd + 1, &rfds, NULL, NULL, NULL);
        if (retval < 0) {
            printf("Error select %d: %s\n", errno, strerror(errno));
            return 1;
        }

        // printf("%10.6f select on:", get_time());
        // for (int index = 0; index < WRITERS_NUMBER; index++) {
        //     if (writers_sockets[index] != -1 && FD_ISSET(writers_sockets[index], &rfds)) {
        //         printf(" %d", index + 1);
        //     }
        // }
        // printf("\n");

        for (int index = 0; index < WRITERS_NUMBER; index++) {
            if (writers_sockets[index] != -1 && FD_ISSET(writers_sockets[index], &rfds)) {
                char buffer[256];
                int fd_socket = writers_sockets[index];
                ssize_t size = read(fd_socket, buffer, sizeof(buffer) - 1);
                if (size == 0) {
                    writers_sockets[index] = -1;
                    writers_total--;
                    printf("%10.6f %sthread reader - closed writer socket %d%s\n", get_time(), ANSI_COLOR_CYAN, index + 1, ANSI_COLOR_RESET);
                    continue;
                } else if (size < 0) {
                    printf("%10.6f %serror read from socket %s, %d: %s%s\n", get_time(), ANSI_COLOR_RED, info_writers[index].socket_path, errno, strerror(errno), ANSI_COLOR_RESET);
                    return 1;
                }
                buffer[size] = 0;
                printf("%10.6f %d -> \"%s\"\n", get_time(), index + 1, buffer);

            }
        }
    }
    printf("%10.6f %sexit thread reader%s\n", get_time(), ANSI_COLOR_CYAN, ANSI_COLOR_RESET);

    return 0;
}

int main() {
    // get start time
    gettimeofday(&time_start, NULL);

    struct ThreadSocketWriterInfo info_writers[WRITERS_NUMBER];
    thrd_t threads[WRITERS_NUMBER + 1];

    // Run writers threads
    for (int index = 0; index < WRITERS_NUMBER; index++) {
        info_writers[index].index = index + 1;
        snprintf(info_writers[index].socket_path, sizeof(info_writers[index].socket_path), TEST_SOCKET_PATH, index + 1);
        info_writers[index].time_shift = 1 + index;
        info_writers[index].time_sleep = 3 + (index * index % 6);
        if (pipe(info_writers[index].pipes) < 0) {
            printf("error create pipes for thread %d, %d: %s\n", index + 1, errno, strerror(errno));
            return 1;
        }
        thrd_create(&threads[index], thread_socket_writer, &info_writers[index]);
    }

    // Run reader threads
    sleep(1);
    thrd_create(&threads[WRITERS_NUMBER], thread_reader, info_writers);

    // Send signals to pipes for stop
    for (int index = 0; index < WRITERS_NUMBER; index++) {
        sleep(15);
        unsigned char buf;
        printf("%10.6f %ssend signal for writer %d%s\n", get_time(), ANSI_COLOR_GREEN, index + 1, ANSI_COLOR_RESET);
        if (write(info_writers[index].pipes[1], &buf, sizeof(buf)) <= 0) {
            printf("error write to pipe %d: %s\n", errno, strerror(errno));
            return 1;
        }
        close(info_writers[index].pipes[1]);
    }

    // Wait finish threads
    for (int index = 0; index < WRITERS_NUMBER; index++) {
        thrd_join(threads[index], NULL);
        printf("%10.6f %sjoin finished on writer %d%s\n", get_time(), ANSI_COLOR_GREEN, index + 1, ANSI_COLOR_RESET);
    }
    thrd_join(threads[WRITERS_NUMBER], NULL);
    printf("%10.6f %sjoin finished on reader%s\n", get_time(), ANSI_COLOR_GREEN, ANSI_COLOR_RESET);

    return 0;
}
