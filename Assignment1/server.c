// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>
#include <pwd.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    struct passwd* pwd;
    char *hello = "Hello from server";
   char *name = "nobody";
   int val;
   pid_t current_pid;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
	&opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if (bind(server_fd, (struct sockaddr *)&address,
	sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    //Solution Assignment1 Server should be running in sudo mode for this to work
    // Forking a child process
    current_pid=fork(); // creates a child process
    if(current_pid == 0)
    {
	//In child 
	//Getting nobody id in pwd structure 
	pwd = getpwnam(name);
	if (pwd == NULL) {
	        printf("Cannot find UID for name %s\n", name); 
		return 0;
	}
	//Setting uid to nobody
	val = setuid((long)pwd->pw_uid);
	if(val ==-1){
	printf("Error dropping privileges\n");
	return 0;
	}
    valread = read(new_socket, buffer, 1024);
    printf("Read %d bytes: %s\n", valread, buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    }
    else if(current_pid > 0)
    {
	//In Parent
	wait(NULL);
	printf("Back to parent...\n");
	
    }
    else
    {
    	printf("Fork Failed\n"); 
	_exit(2);
    }
    return 0;
}