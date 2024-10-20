#include <stdio.h>
#include <string>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define DEBUG_MODE 1

/*
 * functions for bash initation
*/
std::string targetPort = "";
std::string command = "";
std::string command_args = "";

bool validateArgs(int argc, char *argv[]){
	if(argc <= 2){
		return false;
	}
	targetPort = argv[1];
	command = argv[2];
	for(int i=3; i<argc; i++){
		if(i == argc-1){
			command_args += argv[3];
		}else{
			command_args += argv[i];
			command_args += " ";
		}
	}
	return true;
}

int openConnection(void){
	int ret = open(targetPort.c_str(), O_RDWR);
	if(!ret || errno){
		#ifdef DEBUG_MODE
			perror("open");
		#endif
		close(ret);
		return -1;
	}	
	return ret;
}

void sendCommand(int fd){
	std::string out = "CMD " + command + " " + command_args;
	printf("Executing %s\n", out.c_str());
	
	if(write(fd, out.c_str(), out.length()) != out.length()){
		printf("command failed.\n");
	}else{
		printf("SUCCESS\n");
	}
}

void recvResponse(int fd){
	char res[10000] = {0};
	int amt = 0;
	if((amt = read(fd, res, 10000)) > 0){
		printf("CMDRESP: ");
		for(int i=0; i<amt; i++){
			printf("%c", res[i]);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[]){
	if(!validateArgs(argc, argv)){
		#ifdef DEBUG_MODE
			printf("validateArgs: invalid arguments.\n");
		#endif
		exit(EXIT_FAILURE);
	}
	
	#ifdef DEBUG_MODE
		 printf("Connecting to port '%s'\n", targetPort.c_str());
	#endif

	int fd = openConnection();
	if(fd <= 0){
		#ifdef DEBUG_MODE
			printf("Are you authorized?\n");
		#endif
		exit(EXIT_FAILURE);
	}
	#ifdef DEBUG_MODE
	else{
		printf("Serial connection opened...\n");
	}
	#endif
	/* end Initialization */
	
	sendCommand(fd);
	//recvResponse(fd);

	/* End Program */
	close(fd);
	exit(EXIT_SUCCESS);
}
