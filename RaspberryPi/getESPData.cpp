// Server side of a socket connection, waits for ESP to "ping" it to start motion detection
// Resource: https://www.geeksforgeeks.org/socket-programming-cc/
// https://stackoverflow.com/questions/17703721/how-to-execute-another-exe-from-a-c-program-in-windows

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#define PORT 4422

//Global stuff to keep track of PIDs 
//1 is for camera process
int PID1 = 0;
//2 is for detection process
//int PID2 = 0;


int main(int argc, char const* argv[])
{
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = { 0 };

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { //IPV4 using TCP
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
    //printf("Socket Created!\n");

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

    //printf("Socket attached\n");
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr*)&address,
			sizeof(address))
		< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
    //printf("Binded!\n");
    if (listen(server_fd, 3) < 0) { //Listen for a connection
        perror("listen");
        exit(EXIT_FAILURE);
    }
    //printf("Before loop\n");

    //Some weird nesting stuff because we want the camera to run in the background of this program
    while(1){
        //printf("Waiting for acceptance...\n");
        if ((new_socket
            = accept(server_fd, (struct sockaddr*)&address, //Wait and accept connection
                    (socklen_t*)&addrlen))
            < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        //printf("Accepted\n");
        valread = read(new_socket, buffer, 1024);
	    //printf("Buffer: %s.\n",buffer);
        if(!strcmp(buffer, "Stop"))
            break;
        if(!strcmp(buffer, "Start"))
        {
        //Doing some jank double forkness over here
   	        printf("Forking\n");
            PID1 = fork();
            switch(PID1)
            {
                case -1: //error
                    perror("fork");
                    exit(1);
                case 0: //child process
                    printf("child made...\n");
                    execl("./dist/rpi_IP_camera","rpi_IP_camera", (char*)NULL);
                    perror("execl"); //Function doesn't return if it runs correctly
                    exit(1);
                //Else, its the parent process
                default:
		            printf("Parent continuing\n");
                    //Below is when I was trying to run the detection model on the PI
                    //Ran out of time trying to get this to work, Future work
                    //system("python3 useModel.py"); //This returns after the call, not when the program is done
                    /*PID2 = fork();
                    switch(PID2)
                    {
                        case -1: //error
                            perror("fork2");
                            exit(1);
                        case 0: //child process
                            printf("parent/child made...\n");
                            //CHANGE THIS WITH NEW EXECUTIBLE
                            execl("./dist/useModel","useModel", (char*)NULL);
                            perror("execl2"); //Function doesn't return if it runs correctly
                            exit(1);
                        //Else, its the parent/parent process
                        default:
                            printf("parent/parent continuing\n");
                    }*/
                    //break;//Should leave switch
            }
        }
        if(!strcmp(buffer, "Off"))
        {  //Only *ultimate* parent process should get here
            printf("Killing processes..\n");
            //kill(PID,SIGTERM);
            std::string cmd = "sudo kill ";
            //std::string cmd2 = cmd;
            cmd.append(std::to_string(PID1));
            //cmd2.append(std::to_string(PID2));
            //system(cmd2.c_str());
            //This command *should* end the detection should the kill command above fail
            system(cmd.c_str());
        }

        send(new_socket, "Done", 40, 0); //Resume polling on ESP
        ////printf("Hello message sent\n");
        // closing the connected socket
        close(new_socket);
    }//stop is shutdown command from ESP

	// closing the listening socket
	shutdown(server_fd, SHUT_RDWR);
	return 0;
}
