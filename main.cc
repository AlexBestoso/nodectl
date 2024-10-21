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
int argCount = 0;

bool validateArgs(int argc, char *argv[]){
	if(argc == 1){
		return false;
	}
	command = argv[1];
	argCount = 0;
	for(int i=2; i<argc; i++){
		command_args += argv[i];
		command_args += "\t";
		argCount++;
	}

	if(command != "mode" && command != "help"){
		return false;
	}
	return true;
}

std::string getCommandArg(int idx){
	std::string ret = "";
	int xdi = 0;
	for(int i=0; i<command_args.length() && xdi < argCount; i++){
		if(xdi == idx && command_args[i] != '\t'){
			ret += command_args[i];
		}else if(xdi == idx && command_args[i] == '\t'){
			return ret;
		}else if(command_args[i] == '\t'){
			xdi++;
		}
	}
	return ret;
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

void modeHelp(void){
	printf("MODE command description:\n\tChange palnode relay settings.\n\n\t./nodectl mode <disable|repeat|serial> [/dev/usbDevice]\n\n");
	printf("disable : This mode disables the relay so it stops emitting messages.\n");
	printf("repeat : This mode causes the node to emit received messages via the LoRa frequency of the node device.\n");
	printf("serial : This mode causes the node to emit the message over serial USB.\n         This allows for the palrouter software and custom code to route messages by other means.\n");
}

void showHelp(void){
	printf("Usage : ./nodectl [command] [command args]\n\n");
	printf("Available commands :\n\tmode\n\thelp\n\n");
	if(argCount != 1){
		printf("For more help, use the help command like so\n\t./nodectl help [available command]\n");
		exit(EXIT_FAILURE);
	}
	std::string arg = getCommandArg(0);
		
	if(arg == "help"){
		printf("HELP command description:\n\tProvide a description and usage of available commands.\n");
	}else if(arg == "mode"){
		modeHelp();
	}

	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){
	if(!validateArgs(argc, argv)){
		showHelp();
		exit(EXIT_FAILURE);
	}

	if(command == "help"){
		showHelp();
		exit(EXIT_FAILURE);
	}else if(command == "mode"){
		if(argCount != 2){
			modeHelp();
			exit(EXIT_FAILURE);
		}
		targetPort = getCommandArg(1);
		command_args = getCommandArg(0);
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
