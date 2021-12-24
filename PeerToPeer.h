/*
Absolute Simple Peer-To-Peer(ASPTP) v1.0

Brenden Dane, 12/23/2021


I wrote this library as a frustrated programmer who couldn't find
any already made, super bare-bones simple library for P2P connections
and communication. So, I made one myself and kept it as SUPER simple
as I could. I've commented everything I possibly could and explained
everything in the simplest for to where a beginner programmer can
understand. If you have any questions whatsoever, feel free to contact
me.

Email: brdane@gmail.com
Instagram: @spankedcheese


If you'd like to send some pizza money my way, I'd appreciate it.. Thank you.

PayPal: brdane@gmail.com
Bitcoin: 172aAiHgmnSZexoieWw97qm9Ztpn8Gna5C
Litecoin: LKVqoLLaWhTbCskDJzH87PMeXuw8f3gFT9
Ethereum: 0xd28621824c85084ef8694e06e31C8590aaf4b5c8

*/



//I probably don't need all of these libraries, but better to be safe than sorry.
#include <string>
#include <fstream>

#include<stdio.h>
#include<string.h>
#include <iostream>
#include <fstream>

#include <time.h>

#include <sys/types.h>
#include <stdlib.h>

#include<winsock2.h>
#include<ws2tcpip.h>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

//A simple structure for our client information.
struct client
{
    int host;
    short port;
};


struct client server;

bool bConnected; //Used by Connect(), I can call connect(), but not this.

WSADATA wsa; //Basic structure that contains socket information.
struct sockaddr_in si_me, si_other; //Pointers to both your ip address and the last ip you received data from.

int s; //Our socket.

int slen; //Length of si_other.

#define BUFLEN 4096 //The size of each message that we receive. Feel free to change depending on your application.

char buf[BUFLEN]; //A char array that our received message/data goes into.

//Let's you know if you are connected to a socket or not.
bool Connected()
{
	return bConnected;
}

//A pointer to the socket for the P2P connection, if you're connected.
int GetSocket()
{
	return s;
}

//Your network information, including your computer's IP.
sockaddr_in MyNetwork()
{
 return si_me;
}

//The network information of the last person you received data from.
sockaddr_in ServerNetwork()
{
 return si_other;
}

//Connects to a port of your choosing to create the hole-punch.
void Connect(unsigned short ip_port)
{
	//If we are already connected, then don't try to connect again.
	if (bConnected)
		return;

	
	if (!bConnected)
	{
		if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
		{
		}
		
		//Create our socket.
		if ( (s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		{
		}
		else
		{
			memset((char *) &si_me, 0, sizeof(si_me));
			si_me.sin_family = AF_INET;
			si_me.sin_port = htons(ip_port); // This is not really necessary, we can also use 0 (any ip_port)
			si_me.sin_addr.s_addr = htonl(INADDR_ANY);

			//If we can't bind and activate our socket, error out and exit.
			if( bind(s ,(struct sockaddr *)&si_me , sizeof(si_me)) == SOCKET_ERROR)
			{
				printf("Bind failed with error code : %d" , WSAGetLastError());
				return;
			}
			
			//Obtain the size of the pointer to the other ip.
			slen = sizeof(si_other);
			
			//Set out connected bool to true.
			bConnected = true;
		}
	}
}

//Send data to an IP of your choice. 
bool SendToIp(char* server_ip, const char* message)
{

	//If we don't have an active connection, don't go any further.
	if (!bConnected)
		return false;

	//Convert the server_ip parameter.
	server_ip = (char *)(LPCTSTR)server_ip;

	//Setup our si_other pointer and put in our server_ip
	//parameter.
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = si_me.sin_port;
	si_other.sin_addr.s_addr = inet_addr(server_ip);
	server.host = si_other.sin_addr.s_addr;
	server.port = si_other.sin_port;

	//If the message doesn't go through, return false.
	if (sendto(s, message, strlen(message), 0, (struct sockaddr*)(&si_other), slen) == -1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//Returns true if you receive data in your connected socket, also returns the data that you received.
bool received(char* &msg)
{
	//Check to see if received something on our socket, if so, point our buf variable to it
	//and set fill it with BUFLEN. Also, set si_other with information on who sent the data
	//we received.
	if (recvfrom(GetSocket(), buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) != -1)
	{
		//Fetch the data we received to the msg parameter.
		msg = buf;
		
		//return true to let the programmer know that something was received.
		return true; 
	}
else
	{	//Otherwise, return false.
		return false;
	}
}

//This is SendToIP(), but it sends a message to the last IP it received a message from.
//Therefore, this is literally replying.
bool Reply(char* message)
{

	if (!bConnected)
		return false;
	
	int slen = sizeof(si_other);

	if (sendto(s, message, strlen(message), 0, (struct sockaddr*)(&si_other), slen) == -1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//Disconnect from the socket that you're connected to.
void Disconnect()
{
	if (bConnected)
	{
		closesocket(s);
		WSACleanup();
	}
}



//Quick and easy function to get rid of garbage text in a string.
//This basically NULLs any character that is not on a standard keyboard.
//(any character with an ascii code not between 32 and 126)
const char* NormalizeString(char* in)
{

	for (int i=0;i<strlen(in);i++)
		{
			if ( (in[i]  < 31) || (in[i] > 126) ) 
				in[i] = 0;
		}
	return in;

}

//Used to compare two NormalizeString()'s.
bool SameThing(const char* in1, const char* in2)
{
	std::string i1 = in1;
	std::string i2 = in2;

	return (i1.compare(i2) == 0);
}

//Let's you know if a specified file exists.
bool fileexists (const std::string& name) 
{
    ifstream f(name.c_str());
    return f.good();
}
