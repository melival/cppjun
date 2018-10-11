#include <iostream>
#include <set>
#include <algorithm>
#include <thread>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#include "TZinfo.hpp"
#include "set_socket_nblock.h"
#include "create_response.hpp"

const int MasterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
const int default_port_number = 12345;
const std::string default_tz_if_path="tzones.txt";
static bool server_on_air = true;


std::set<int> SlaveSockets;
TZinfo ti;

bool load_timezones_base(std::string tz_if_path=default_tz_if_path);
void init_listener(const int port_number=default_port_number);
void main_loop();
void server_cli();
void command_interpreter(std::string);

int main(int argc, char ** argv){
	//Timezone base loading...
	std::cout << "Timezone server v0.1." << std::endl;
	std::cout << "Type 'man' for manual." << std::endl;

	if(!load_timezones_base()) { return 1; }

	//Setting up the server
	if(argc == 2 && isdigit(argv[1][0])) {
		init_listener(atoi(argv[1]));
	} else {
		init_listener();
	}

	std::thread (main_loop).detach();

	std::thread server_cli_thread(server_cli);
	server_cli_thread.join();

	return 0;
}

void server_cli() {
	std::string server_command;
	while (server_on_air) {
		std::cin >> server_command;
		command_interpreter(server_command);
	}
}

void command_interpreter(std::string cmd) {
	if ( cmd == "quit" ) { 
		server_on_air = false; 
	} else if ( cmd == "man" || cmd == "help" ) {
		std::cout << "for quit type 'quit'" << std::endl;
	}
}

bool load_timezones_base(std::string tz_if_path) {
	std::cout << "Loading timezones... ";

	while( ti.load_base(tz_if_path) <= 0 ) {
		if (!server_on_air) return false;
		std::cout << "Can't load information about timezones offsets."
		<< std::endl << "Please specify path and name for infofile:";
		std::cin >> tz_if_path;
		command_interpreter(tz_if_path);
	}

	std::cout << ti.get_count() << " items loaded." << std::endl;
	return true;
}

void init_listener(const int port_number) {

	std::cout << "open server on " 
			<< ( (port_number == default_port_number)?"default":"custom" )
			<< " port number: " << port_number << std::endl;

	struct sockaddr_in SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(port_number);
	SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(MasterSocket, (struct sockaddr *)(&SockAddr), sizeof(SockAddr));

	set_nonblock(MasterSocket);
	listen(MasterSocket, SOMAXCONN);
}

void main_loop() {
	while(server_on_air){
		fd_set Set;
		FD_ZERO(&Set);
		FD_SET(MasterSocket, &Set);
		for(auto ss = SlaveSockets.begin();	ss != SlaveSockets.end(); ss++) {
			FD_SET(*ss, &Set);
		}

		int last_socket = std::max(MasterSocket,
				*std::max_element(SlaveSockets.begin(),
				SlaveSockets.end()));

		select(last_socket+1, &Set, NULL, NULL, NULL);

		for(auto ss = SlaveSockets.begin(); ss != SlaveSockets.end(); ss++) {
			if(FD_ISSET(*ss, &Set)) {
				static char Buffer[1024];
				int RecvSize = recv(*ss, Buffer, 1024, MSG_NOSIGNAL);
				if((RecvSize == 0) && (errno != EAGAIN)) {
					shutdown(*ss, SHUT_RDWR);
					close(*ss);
					SlaveSockets.erase(ss);
				} else if(RecvSize != 0){
					//cleaning from request nonALPHABETH chars 
					for(int i = 0; i < RecvSize; i++)
						if(Buffer[i] < 'A' || Buffer[i] > 'Z')
							Buffer[i] = '\0';
					std::string msg(Buffer);
					//std::cout << msg << std::endl; //logging incoming messages
					if (ti.find_zone(msg) > 0) {
						msg = tztime_cr::make(msg, ti.get_offset(msg));
					} else {
						msg = "Unknown timezone\n";
					}
					std::copy(msg.begin(), msg.end(), Buffer);
					send(*ss, Buffer, msg.size(), MSG_NOSIGNAL);
				}
			}
		}
		if(FD_ISSET(MasterSocket, &Set)) {
			int SlaveSocket = accept(MasterSocket, 0, 0);
			set_nonblock(SlaveSocket);
			SlaveSockets.insert(SlaveSocket);
		}
	}
}
