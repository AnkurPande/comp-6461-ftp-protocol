// SERVER TCP PROGRAM

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <sys/stat.h>
using namespace std;

#define REQUEST_PORT 0x7070
int port = REQUEST_PORT;

SOCKET s;
SOCKET s1;
SOCKADDR_IN sa;   // filled by bind
SOCKADDR_IN sa1;   // fill with server info, IP, port not used

union
{
	struct sockaddr generic;
	struct sockaddr_in ca_in;
}ca;

int caLen = sizeof(ca); //calen;
char szbuffer[500];
char *buffer;
int ibufferlen;
int ibytesrecv;
int ibytessent;

char localhost[11];

HOSTENT *hp;

//wait variables

int nsa1;
int r, infds = 1, outfds = 0;
struct timeval timeout;
const struct timeval *tp = &timeout;

fd_set readfds;

//others

HANDLE test;
DWORD dwtest;

DWORD WINAPI clientThread(LPVOID lpParam)
{
	SOCKET currentClient = (SOCKET)lpParam;
	char action[10];
	char filename[20] = { '\0' };
	//Fill in szbuffer from accepted request.
	if ((ibytesrecv = recv(currentClient, action, 10, 0)) == SOCKET_ERROR)
		throw "Receive error in server program\n";
	cout << "This is message from client: " << action << endl;

	//Check whether client requested for the Get operation.(Download from server)
	if (action[0] == 'G')
	{
		recv(currentClient, filename, 20, 0);
		ifstream myfile;
		char filecontent[100];

		myfile.open(filename, ios::in | ios::binary);
		if (myfile.is_open())
		{
			while (!myfile.eof())
			{
				myfile.read(filecontent, sizeof(filecontent));
				//cout << sizeof(filecontent) << endl;
				if ((ibytessent = send(currentClient, filecontent, sizeof(filecontent), 0)) == SOCKET_ERROR)
				{
					throw "error in send in server program while sending data \n";
				}
				else
				{
					memset(filecontent, 0, sizeof (filecontent));
				}
			}
			sprintf(szbuffer, "\r\n");
			ibufferlen = strlen(szbuffer);
			if ((ibytessent = send(currentClient, szbuffer, ibufferlen, 0)) == SOCKET_ERROR)
				throw "error in send in server program while sending data \n";
			else
			{
				cout << "File transferred Successfully";
			}
		}
	}
	//Check whether client requested for a Put operation.(Upload to server)
	if (action[0] == 'P')
	{
		recv(currentClient, filename, 20, 0);
		char filecontent[100];
		int ibytesrecv = 0;

		ofstream myFile(filename, ios::out | ios::binary);
		while ((ibytesrecv = recv(currentClient, filecontent, sizeof(filecontent), 0))>0)
		{
			myFile.write(filecontent, ibytesrecv);
			memset(filecontent, 0, sizeof(filecontent));
		}
		cout << "File is received completely." << endl;
		myFile.close();
	}
	closesocket(currentClient);
	ExitThread(0);
}

int main(void)
{

	DWORD thread;
	WSADATA wsadata;
	try
	{
		if (WSAStartup(0x0202, &wsadata) != 0)
		{
			cout << "Error in starting WSAStartup()\n"<<WSAGetLastError;
		}
		else
		{
			buffer = "WSAStartup was suuccessful\n";
			WriteFile(test, buffer, sizeof(buffer), &dwtest, NULL);

			/* display the wsadata structure */
			cout << endl
				<< "wsadata.wVersion " << wsadata.wVersion << endl
				<< "wsadata.wHighVersion " << wsadata.wHighVersion << endl
				<< "wsadata.szDescription " << wsadata.szDescription << endl
				<< "wsadata.szSystemStatus " << wsadata.szSystemStatus << endl
				<< "wsadata.iMaxSockets " << wsadata.iMaxSockets << endl
				<< "wsadata.iMaxUdpDg " << wsadata.iMaxUdpDg << endl;
		}

		//Display info of local host
		gethostname(localhost, 10);
		cout << "hostname: " << localhost << endl;

		if ((hp = gethostbyname(localhost)) == NULL)
		{
			cout << "gethostbyname() cannot get local host info?"
				<< WSAGetLastError() << endl;
			exit(1);
		}

		//Create the server socket
		if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
			throw "can't initialize socket";
		// For UDP protocol replace SOCK_STREAM with SOCK_DGRAM

		//Fill-in Server Port and Address info.
		sa.sin_family = AF_INET;
		sa.sin_port = htons(port);
		sa.sin_addr.s_addr = htonl(INADDR_ANY);

		//Bind the server port

		if (bind(s, (LPSOCKADDR)&sa, sizeof(sa)) == SOCKET_ERROR)
			throw "can't bind the socket";
		cout << "Bind was successful" << endl;

		//Successfull bind, now listen for client requests.
		if (listen(s, 10) == SOCKET_ERROR)
			throw "couldn't  set up listen on socket";
		else cout << "Listen was successful" << endl;

		FD_ZERO(&readfds);

		while (1)
		{
			FD_SET(s, &readfds);  //always check the listener
			if (!(outfds = select(infds, &readfds, NULL, NULL, tp))) {}
			else if (outfds == SOCKET_ERROR) throw "failure in Select";
			else if (FD_ISSET(s, &readfds))  cout << "got a connection request" << endl;

			//Found a connection request, try to accept.
			if ((s1 = accept(s, &ca.generic, &caLen)) == INVALID_SOCKET)
				throw "Couldn't accept connection\n";

			//Connection request accepted.
			cout << endl << "accepted connection from " << inet_ntoa(ca.ca_in.sin_addr) << ":"
				<< hex << htons(ca.ca_in.sin_port) << endl;

			//create thread and pass the client and return back to accept new client connection.
			// For every new client request a new dedicated thread is created for the client.
			
			CreateThread(NULL, 0, clientThread, (LPVOID)s1, 0, &thread);

		}//while loop

	} //try loop

	//Display needed error message.

	catch (char* str) { cerr << str << WSAGetLastError() << endl; }

	//close server socket
	closesocket(s);

	/* When done uninstall winsock.dll (WSACleanup()) and exit */
	WSACleanup();
	return 0;
}
