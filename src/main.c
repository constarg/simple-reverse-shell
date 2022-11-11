#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define MSG_CHUNK 0x200       // Buffer size.


static int sendall(int sock_fd, const char *msg)
{
    // TODO - send the messaue by broke it into sagments.   
    size_t msg_size = strlen(msg);
    size_t bytes_left = msg_size; // bytes left.
    size_t chunk_size = 0;

    int pos = 0;
    // First send how many bytes does the reciever expect.
    if (send(sock_fd, &msg_size, 
        sizeof(msg_size), 0) == -1) return -1;
    // Send the data.
    while (bytes_left) {
         chunk_size = (MSG_CHUNK < bytes_left)? MSG_CHUNK : bytes_left;
         chunk_size = send(sock_fd, msg + pos, chunk_size, 0);
         if (chunk_size == -1) return -1;
         // caculate the left bytes and the current position.
         bytes_left -= chunk_size;
         pos += chunk_size;
    }

    return 0;
}

static int recvall(char **dst, int sock_fd)
{
    size_t s_expect = 0; // expected size.
    size_t s_recieved = 0; // the size that we have recieved.
    size_t chunk_size = MSG_CHUNK; // how many bytes to read each call.

    // Get the length of the command.
    if (recv(sock_fd, &s_expect, sizeof(s_expect),
             0) == -1) return -1;

    // Initialize buffer.
    *dst = (char *) malloc(sizeof(char) * 
                           (s_expect + 1));
    if (*dst == NULL) return -1;

    while (s_expect) {
        chunk_size = (MSG_CHUNK < s_expect)? MSG_CHUNK : s_expect;
        chunk_size = recv(sock_fd, *dst + s_recieved, chunk_size, 0);
        if (chunk_size == -1) return -1;
        s_expect -= chunk_size;
        s_recieved += chunk_size;
    }
    (*dst)[s_recieved] = '\0'; // terminate the string. 

    return 0;
}

static int run_server(const u_short port)
{
    struct sockaddr_in se_addr;
    int se_addr_l = sizeof(se_addr);
    int se_fd; // the server socket.
    int cl_fd; // the client socket.

    // initialize memory for se_addr.
    memset(&se_addr, 0x0, sizeof(se_addr));
    // Get discriptor for the socket.
    se_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (se_fd == -1) return -1;

    se_addr.sin_family      = AF_INET;
    se_addr.sin_addr.s_addr = INADDR_ANY;
    se_addr.sin_port        = htons(port);

    // bind the address and the socket.
    if (bind(se_fd, (struct sockaddr *)&se_addr,
             sizeof(se_addr)) == -1) return -1;

    // listen for connection.
    char *command = NULL;
    int is_conn_alive = EPIPE;
    while (1) {
        if (listen(se_fd, 3) == -1) continue;

        cl_fd = accept(se_fd, (struct sockaddr *) &se_addr,
                       &se_addr_l);
        if (cl_fd == -1) continue;

        // check if the client is dead.
        while (is_conn_alive != -1) {
            // if for some reason we can't recieve packets, close connection.
            if (recvall(&command, cl_fd) == -1) break; // TODO - instead of break close connection.

            is_conn_alive = send(cl_fd, &is_conn_alive,
                                 sizeof(is_conn_alive), 0);
        }

        if (recvall(&command, cl_fd) == -1) 
        {
            printf("Error\n");
            return -1;
        }
        // TODO - recieve the requested command.
        // TODO - send the results.
    }

    close(cl_fd);
    shutdown(se_fd, SHUT_RDWR);
    return 0;
}

char *get_user_input(FILE *stream, 
                     size_t init_size)
{
    char *input = NULL;
    int curr_ch;
    size_t curr_len = 0;

    // Allocate space for init_size characters.
    input = (char *) malloc(sizeof(char) * init_size);
    if (input == NULL) return NULL;

    while ((curr_ch = fgetc(stream)) != EOF 
			&& curr_ch != '\n') {
        input[curr_len++] = curr_ch;
        if (curr_len == init_size) {
            input = (char *) realloc(input, sizeof(char) * 
                                    (init_size += 10));
            if (input == NULL) return NULL;
        }
    }
    input[curr_len++] = '\0';

    // Decreace the memory to the exac size of string.
    return (char *) realloc(input, sizeof(char) * curr_len);
}

static int run_client(const u_short port, 
                      const char *se_addr)
{
    struct sockaddr_in se_sock_addr;
    int cl_fd; // the client socket.
    int se_fd; // the server socket.

    // initialize memory for se_addr.
    memset(&se_sock_addr, 0x0, sizeof(se_sock_addr));
    cl_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (cl_fd == -1) return -1;

    se_sock_addr.sin_family = AF_INET;
    se_sock_addr.sin_port   = htons(port);

    if (inet_pton(AF_INET, se_addr, 
                  &se_sock_addr.sin_addr) == -1) return -1;

    // connect to the server.
    if (connect(cl_fd, (struct sockaddr *)& se_sock_addr,
                        sizeof(se_sock_addr)) == -1) return -1;

    char *input = NULL;

    // simulate a shell like prompt.
    while (1) {
        printf("-> ");
        input = get_user_input(stdin, 10);
        if (input == NULL) return -1;
        printf("\n");

        if (sendall(cl_fd, input) == 0)
        {
            printf("Command sended\n");
        }
        // Send a request to the server to execute the command.
        // TODO - send the data.
        // recieve the output.

        // TODO - recieve from the server the results of the command.
        free(input);
    }

    close(cl_fd);
    return 0;
}

int main(int argc, char *argv[])
{
    u_short port; 

    if (argv[1] == NULL) {
        printf("No option.\n");
    } else if (!strcmp(argv[1], "server")) {
        int se_fd;

        if (!(argv[2] == NULL)) {
            port = atoi(argv[2]);
            if (port == 0 || port < 6000) {
                printf("The port must be an number greater than 6000\n");
                return -1;
            }
        } else {
            printf("Please specify a port.\n");
        }
        // get the socket.
        if (run_server(port) == -1) {
            printf("Failed to start server.\n");
            return -1;
        }

    } else if (!strcmp(argv[1], "client")) {
        int cl_fd;
        char *ip;

        if (!(argv[2] == NULL)) {
            ip = argv[2];

            if (!(argv[3] == NULL)) {
                port = atoi(argv[3]);
                if (port == 0 || port < 6000) {
                    printf("The port must be an number greater than 6000\n");
                    return -1;
                }
            } else {
                printf("Please specify a port.\n");
            }
        } else {
            printf("Please specify an ip address.\n");
            return -1;
        }

        // get the socket.
        if (run_client(port, ip) == -1) {
            printf("Failed to start client.\n");
            return -1;
        }
    } else {
        printf("Unrecognized option.\n");
    }

    return -1;
}
