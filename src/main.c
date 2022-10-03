#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>


static int conf_server_sock(const u_short port)
{
    struct sockaddr_in se_addr;
    int se_fd;

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

    return se_fd;
}

static void conf_client_sock(const u_short port, 
                             const char *se_addr)
{
    struct sockaddr_in se_addr;

}

int main(int argc, char *argv[])
{


}
