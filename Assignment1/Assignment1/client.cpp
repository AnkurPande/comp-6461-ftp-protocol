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
#include <string>
#include <windows.h>
#include<vector>
#include "Shlwapi.h"
using namespace std;
#pragma comment (lib,"WS2_32.lib") 
#pragma comment (lib,"Shlwapi.lib") 

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

void list(string s){

	WIN32_FIND_DATA file_data;
	HANDLE hFile;
	vector<string> files;

	string dir = s;
	cout << dir;

	hFile = FindFirstFile((dir + "/*").c_str(), &file_data);

	cout << file_data.cFileName;
	do{
		string fileName = file_data.cFileName;
		files.push_back(fileName);
	} while ((FindNextFile(hFile, &file_data)) != 0);

	cout << endl
		<< "========================" << endl;
	for (auto & i : files){
		cout << i << endl;
	}
	cout << endl
		<< "========================" << endl;
}

void deleteFile(string s)
{
	char  filename[150] = { "\0" };
	cout << "Enter the file name : ";
	cin >> filename;

	string s1 = s;
	string s2 = filename;
	s1.append("\\");
	s1.append(s2);
	s1.copy(filename, 150);
	cout << filename << endl;

	if (remove(filename) != 0)
		perror("\nError deleting file.\n");
	else
		puts("\nFile successfully deleted.\n");

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

		// Client and server directory path.
		string clientDir = "C:\\Users\\Ankurp\\Documents\\Visual Studio 2013\\Projects\\Assignment1Client\\Assignment1";
		string serverDir = "C:\\Users\\Ankurp\\Documents\\Visual Studio 2013\\Projects\\Assignment1Server\\Assignment1Server";

		char action[10] = { "\0" };
		cout << endl
			<< "Press 'G' for Get operation" << endl
			<< "Press 'P' for Put operation" << endl
			<< "Press 'L' for List opeartion" << endl
			<< "Press 'D' for Delete the file" << endl
			<< "Press 'E' to exit" << endl;


		cin >> action;
		send(s, action, 10, 0);//send to server the action chosen by client.


		switch (action[0])
		{
		case 'G':
		{       //List the Files and Directories in the server directory.
					list(serverDir);

					char fileName[20] = { '\0' };
					char fullpath[100] = { "\0" };
					cout << "\nEnter the file name :" << fileName;
					cin >> fileName;
					string s1 = serverDir;
					string s2 = fileName;
					s1.append("\\");
					s1.append(s2);
					s1.copy(fullpath, 100);
					char * LPStr1;
					LPStr1 = fullpath;
					int fileExist = 0;
					fileExist = PathFileExists(LPStr1);
					if (fileExist == 0){
						perror("\n File not Found ");
						break;
					}
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
		}
			break;
		case 'P':
		{       //List the Files and Directories in the client directory.
					list(clientDir);

					char fileName[20] = { '\0' };
					cout << "\nEnter the file name :" << fileName;
					cin >> fileName;

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
							something = myfile.gcount();
						
							cout << "Sending...." << endl;
			
							
							if ((ibytessent = send(s, filecontent, something, 0)) == SOCKET_ERROR)
							{
								perror("\n Error in send in server program while sending data ");
							}
							else
							{
								memset(filecontent, 0, sizeof (filecontent));
							}
						}
					/*	sprintf(szbuffer, "\r\n");
						ibufferlen = strlen(szbuffer);
						if ((ibytessent = send(s, szbuffer, ibufferlen, 0)) == SOCKET_ERROR)
							perror("\nError in send in server program while sending data ");
						*/
					}
					else
					{
						perror("\nFile not found");
						break;
					}
					cout << "\nFile transferred completely.\n" << endl;
				
		}
			break;

		case 'L':
		{
					cout << endl
						<< "Press 'S' for Server Directory : " << endl
						<< "Press 'C' for Client Directory : " << endl;
					char choice[10] = { "\0" };
					cin >> choice;
					if (choice[0] == 'S'){
						perror("\nOpening Server Directory");
						list(serverDir);
					}
					else if (choice[0] == 'C'){
						cout << "\nOpening Client Directory\n";
						list(clientDir);
					}

		}
			break;
		case 'D':
		{
					cout << endl
						<< "Press 'S' for Server Directory : " << endl
						<< "Press 'C' for Client Directory : " << endl;
					char choice[10] = { "\0" };
					cin >> choice;
					if (choice[0] == 'S'){
						cout << "\nOpening Server Directory\n";

						list(serverDir);

						deleteFile(serverDir);
					}
					else if (choice[0] == 'C'){
						cout << "\nOpening Client Directory\n";

						list(clientDir);

						deleteFile(clientDir);
					}
		}
			break;

		default:
			break;
		}//switch close
		system("PAUSE");
	}// try loop

	//Display any needed error response.

	catch (char *str) { cerr << str << ":" << dec << WSAGetLastError() << endl; }

	//close the socket.

	closesocket(s);
	/* When done uninstall winsock.dll (WSACleanup()) and exit */

	WSACleanup();
	return(0);
}