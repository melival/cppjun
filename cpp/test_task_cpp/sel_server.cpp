#include <iostream>
#include <set>
#include <algorithm>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

//#include "TZinfo.h"
#include "TZinfo.hpp"

#include "set_socket_nblock.h"


/*class server {
private:
	int MasterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
/*
int optval = 1;
setsockopt(MasterSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
*/
/*
public:
	server() {
		struct sockaddr_in SockAddr;
		SockAddr.sin_family = AF_INET;
		SockAddr.sin_port = htons(12345);
		SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		bind(MasterSocket, (struct sockaddr *) (&SockAddr), sizeof(SockAddr));
		set_nonblock(MasterSocket);

		listen(MasterSocket, SOMAXCONN);

	}

};

class sessions {
private:
public:
	std::set<int> sockets;

	sessions() {}
};
*/

std::string create_response_ctime(std::string z, int o){
    using namespace std;
    string result;
    time_t t = time(0) + o;
    result += ctime(&t);
    result.insert(result.size() - 5, z + " ");
    return result;
}


int main(int argc, char ** argv){

	TZinfo ti;
	std::cout << "Loading timezones... ";
	if (ti.load_base("tzones.txt") < 0) {
		std::cout << "Open info file error." << std::endl;
		return 1;
	} else if (ti.get_count() > 0) {
		std::cout << ti.get_count() << " items loaded." << std::endl;
	} else {
		std::cout << "Timezones in file not found." << std::endl;
	}

	int MasterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	std::set<int> SlaveSockets;

	struct sockaddr_in SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(12345);
	SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(MasterSocket, (struct sockaddr *)(&SockAddr), sizeof(SockAddr));

	set_nonblock(MasterSocket);
	listen(MasterSocket, SOMAXCONN);
	
	while(true){
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
					for(int i = 0; i < RecvSize; i++) 
						if(Buffer[i] < 'A' || Buffer[i] > 'Z')
							Buffer[i] = '\0';
						
					std::string msg(Buffer);
					std::cout << msg << std::endl;
					if (ti.find_zone(msg) > 0) {
						msg = create_response_ctime(msg, ti.get_offset(msg));
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

	return 0;
}
