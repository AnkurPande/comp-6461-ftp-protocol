// CLIENT TCP PROGRAM
char* getmessage(char *);

/* send and receive codes between client and server */
/* This is your basic WINSOCK shell */

#include <winsock2.h>
#include <ws2tcpip.h>
#include <winsock.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <windows.h>
#include <winsock.h>
#include "dirent.h"
using namespace std;
#pragma comment (lib,"WS2_32.lib") 

//user defined port number
#define REQUEST_PORT 0x7070;
int port = REQUEST_PORT;

//socket data types
SOCKET s;
SOCKADDR_IN sa;         // filled by bind
SOCKADDR_IN sa_in;      // fill with server info, IP, port

//buffer data types
char szbuffer[128];
char *buffer;

int ibufferlen = 0;
int ibytessent;
int ibytesrecv = 0;

//host data types
HOSTENT *hp;
HOSTENT *rp;

char localhost[11], remotehost[11];

//other

HANDLE test;
DWORD dwtest;

//reference for used structures

/*
* Host structure
struct  hostent
{
char    FAR * h_name;             official name of host *
char    FAR * FAR * h_aliases;    alias list *
short   h_addrtype;               host address type *
short   h_length;                 length of address *
char    FAR * FAR * h_addr_list;  list of addresses *
#define h_addr  h_addr_list[0]            address, for backward compat *
};

* Socket address structure
struct sockaddr_in
{
short   sin_family;
u_short sin_port;
struct  in_addr sin_addr;
char    sin_zero[8];
};
*/

int listFiles(char a[]){

	DIR *dir;
	struct dirent *ent;
	char clientDir[] = "C:\\Users\\Ankurp\\Documents\\Visual Studio 2013\\Projects\\Assignment1\\Assignment1";
	char serverDir[] = "C:\\Users\\Ankurp\\Documents\\Visual Studio 2013\\Projects\\Assignment1Server\\Assignment1Server";
	int i = -2;
	if (a[0] == 'P'){
		cout << "\nClient Directory";
		cout << "\n======================";
		dir = opendir(clientDir);
	}
	else if(a[0] == 'G'){
		cout << "\nServer Directory\n";
		cout << "\n======================";
		dir = opendir(serverDir);
	}
	
	if ((dir) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			cout<<"\n"<<i<<"."<<ent->d_name;
			i++;
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		perror("");
		return EXIT_FAILURE;
	}
	
	return 0;
}

int main(void)
{
	WSADATA wsadata;
	try
	{
		if (WSAStartup(0x0202, &wsadata) != 0)
		{
			cout << "Error in starting WSAStartup()" << endl;
		}
		else
		{
			buffer = "WSAStartup was successful\n";
			WriteFile(test, buffer, sizeof(buffer), &dwtest, NULL);

			/* Display the wsadata structure */
			cout << endl
				<< "wsadata.wVersion " << wsadata.wVersion << endl
				<< "wsadata.wHighVersion " << wsadata.wHighVersion << endl
				<< "wsadata.szDescription " << wsadata.szDescription << endl
				<< "wsadata.szSystemStatus " << wsadata.szSystemStatus << endl
				<< "wsadata.iMaxSockets " << wsadata.iMaxSockets << endl
				<< "wsadata.iMaxUdpDg " << wsadata.iMaxUdpDg << endl;
		}

		//Display name of local host.

		gethostname(localhost, 10);
		cout << "Local host name is \"" << localhost << "\"" << endl;

		if ((hp = gethostbyname(localhost)) == NULL)
			throw "gethostbyname failed\n";

		//Ask for name of remote server

		cout << "please enter your remote server name :" << flush;
		cin >> remotehost;
		cout << "Remote host name is: \"" << remotehost << "\"" << endl;

		if ((rp = gethostbyname(remotehost)) == NULL)
			throw "remote gethostbyname failed\n";

		//Create the socket
		if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
			throw "Socket failed\n";
		/* For UDP protocol replace SOCK_STREAM with SOCK_DGRAM */

		//Specify server address for client to connect to server.
		memset(&sa_in, 0, sizeof(sa_in));
		memcpy(&sa_in.sin_addr, rp->h_addr, rp->h_length);
		sa_in.sin_family = rp->h_addrtype;
		sa_in.sin_port = htons(port);

		//Display the host machine internet address

		cout << "ftp_tcp starting on host: " << localhost << endl;
		cout << "Connecting to remote host:";
		cout << inet_ntoa(sa_in.sin_addr) << endl;

		//Connect Client to the server
		if (connect(s, (LPSOCKADDR)&sa_in, sizeof(sa_in)) == SOCKET_ERROR)
			throw "connect failed\n";
		else
			cout << "Connection Established" << endl;

		/* Have an open connection, so, server is

		- waiting for the client request message
		- don't forget to append <carriage return>
		- <line feed> characters after the send buffer to indicate end-of file */

		//append client message to szbuffer + send.

		
		char fileName[20] = { '\0' };
		char direction[10] = { '\0' };
		
		cout << "Enter G to request a file from Server" << endl;
		cout << "Enter P to send a file to Server" << endl;
		cout << "Enter your choice: ";
		cin >> direction;
		listFiles(direction);
		cout << "\nEnter the file name :" << fileName;
		cin >> fileName;

		send(s, direction, 10, 0);
		if (direction[0] == 'G')
		{
			ibytessent = 0;
			ibufferlen = strlen(fileName);
			ibytessent = send(s, fileName, 20, 0);
			if (ibytessent == SOCKET_ERROR)
				throw "Send failed\n";
			else
				cout << "Message to server: " << szbuffer;

			char filecontent[100];
			ofstream myFile(fileName, ios::out | ios::binary);
			ibytesrecv = 0;
			while ((ibytesrecv = recv(s, filecontent, sizeof(filecontent), 0))>0)
			{
				myFile.write(filecontent, ibytesrecv);
				memset(filecontent, 0, sizeof(filecontent));
				cout << "Receiving...." << endl;
			}
			cout << "File received completely." << endl;
			myFile.close();
			system("PAUSE");
		}
		else if (direction[0] == 'P')
		{
			send(s, fileName, 20, 0);
			ifstream myfile;
			char filecontent[100];
			int something = 0;
			myfile.open(fileName, ios::in | ios::binary);

			if (myfile.is_open())
			{
				while (!myfile.eof())
				{
					myfile.read(filecontent, sizeof(filecontent));
					cout << "Sending...." << endl;
					if ((ibytessent = send(s, filecontent, sizeof(filecontent), 0)) == SOCKET_ERROR)
					{
						throw "error in send in server program while sendinf data \n";
					}
					else
					{
						memset(filecontent, 0, sizeof (filecontent));
					}
				}
				sprintf(szbuffer, "\r\n");
				ibufferlen = strlen(szbuffer);
				if ((ibytessent = send(s, szbuffer, ibufferlen, 0)) == SOCKET_ERROR)
					throw "error in send in server program while sendinf data \n";
			}
			cout << "File transferred completely." << endl;
			system("PAUSE");
		}
	}// try loop

	//Display any needed error response.

	catch (char *str) { cerr << str << ":" << dec << WSAGetLastError() << endl; }

	//close the socket.

	closesocket(s);
	/* When done uninstall winsock.dll (WSACleanup()) and exit */

	WSACleanup();
	return(0);
}
