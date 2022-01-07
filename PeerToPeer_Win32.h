/*
Absolute Simple Peer-To-Peer (ASPTP) library is an incredibly simple library for C++ programming, 
to exchange data between computers, with no main-server or middleman required, via peer-to-peer protocol.
The method used with this library is through UDP hole-punching, which basically temporarily makes an exception 
in your network's firewall and opens a specified port and socket, in-order to communicate directly to another system.

I wrote this library as a frustrated programmer who couldn't find any already made, super bare-bones simple library for
P2P connections and communication. So, I made one myself and kept it as SUPER simple as I could. I've commented everything 
I possibly could and explained everything in the simplest form to where a beginner programmer can understand.

If you have any questions whatsoever, feel free to contact me.

Email: brdane@gmail.com Instagram: @spankedcheese

PayPal: brdane@gmail.com

Bitcoin: 172aAiHgmnSZexoieWw97qm9Ztpn8Gna5C

Litecoin: LKVqoLLaWhTbCskDJzH87PMeXuw8f3gFT9

Ethereum: 0xd28621824c85084ef8694e06e31C8590aaf4b5c8

*/



//I probably don't need all of this, but better to be safe than sorry.
#include <string>
#include <fstream>

#include<stdio.h>
#include<string.h>
#include <iostream>
#include <fstream>
#include "stdstring.h"


#include <time.h>

#include <sys/types.h>
#include <stdlib.h>

#include<winsock2.h>
#include<ws2tcpip.h>
#include <sys/stat.h>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

struct client
{
    int host;
    short port;
};

struct client server;

bool bConnected;

WSADATA wsa;
struct sockaddr_in si_me, si_other;
int s, slen;

#define BUFLEN 4096
char buf[BUFLEN];

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
bool Connect(unsigned short ip_port)
{
	if (bConnected)
		return false;

	if (!bConnected)
	{
		if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
		{
		}

		if ( (s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		{
		}
		else
		{
			memset((char *) &si_me, 0, sizeof(si_me));
			si_me.sin_family = AF_INET;
			si_me.sin_port = htons(ip_port); // This is not really necessary, we can also use 0 (any ip_port)
			si_me.sin_addr.s_addr = htonl(INADDR_ANY);

			if( bind(s ,(struct sockaddr *)&si_me , sizeof(si_me)) == SOCKET_ERROR)
			{
				printf("Bind failed with error code : %d" , WSAGetLastError());
				return false;
			}

			slen = sizeof(si_other);
			bConnected = true;
		}
	}
	return true;
}

//Send data to an IP of your choice. 
bool SendToIp(char* server_ip, const char* message)
{

	if (!bConnected)
		return false;

	server_ip = (char *)(LPCTSTR)server_ip;

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = si_me.sin_port;
	si_other.sin_addr.s_addr = inet_addr(server_ip);
	server.host = si_other.sin_addr.s_addr;
	server.port = si_other.sin_port;

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
	if (recvfrom(GetSocket(), buf, 4096, 0, (struct sockaddr *) &si_other, &slen) != -1)
	{

		msg = buf;
		return true;
	}
else
	{
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
bool fileexists (CStdString name) 
{
    struct stat buffer;
    return (stat (name, &buffer) ==0);
}

//Load an entire file into a string.
std::string loadfile(CStdString name) 
{
	std::string contents;
	
    if (fileexists(name))
	{
		ifstream input(name);
		
		for (std::string line; getline(input, line); )
		{
		 contents += line;	
		}
	}
    return contents;
}

//Quick-and-easy file-writing function. Only writes a new file,
//and will not work if the file name specified already exists.
bool writefile(CStdString name, CStdString contents)
{
	if (name == "")
		return false;

	if (contents == "")
		return false;

	if (fileexists(name))
		return false;

	ofstream MyFile(name);
	MyFile << contents;
	MyFile.close();
	return true;
}

//Quick-and-easy function that writes to the end of a file. Only
//writes to files that already exist.
bool appendfile(CStdString name, CStdString contents)
{
	if (name == "")
		return false;

	if (contents == "")
		return false;

	if (!fileexists(name))
		return false;

	ofstream MyFile(name);
	MyFile << contents;
	MyFile.close();
	return true;
}

//Quick-and-easy function that searches for text in a file.
//returns true if it finds some. Super freaking simple.
bool foundinfile(CStdString name, CStdString contents)
{
	if (name == "")
		return false;

	if (contents == "")
		return false;

	if (!fileexists(name))
		return false;

	std::string searching = loadfile(name);

	return (searching.find(contents) != string::npos );

}


//Loads a file (configfile, if it can find it) and looks for a keyword (value)
//that has '=' after it, and reads whatever text comes after the '='. If
//it finds anything, it will set what it finds to the result variable and 
//return true to let the user know that it indeed found something.
//This is usually used for INI configuration files, but the format can be
//anything.
bool getconfig(CStdString configfile, CStdString value, CStdString &result ) 
{
	std::string contents;

	bool bResult = false;
	
    if (fileexists(configfile))
	{
		ifstream input(configfile);
		
		for (std::string line; getline(input, line);)
		{
		 if (line.substr(0,value.GetLength()) == (LPCSTR)value)
		 {
			 result = line.substr(value.GetLength()+1,line.length() - value.GetLength()-1).c_str();

			 if (result.GetLength() > 0)
				bResult = true;
		 }
		}
	}
    return bResult;
}

//A useful CStdString to Unsigned Short converting function that I whipped together.
//Used this for fetching port numbers from strings.
unsigned short cstous(CStdString in)
{
	return (unsigned short)strtol(in.GetBuffer(in.GetLength()),NULL,10);
}


//Same as received(), but this has the ability to timeout.
bool received_timeout(char* &msg, long sec, long usec)
{
   fd_set readfds, masterfds;
   struct timeval timeout={sec,usec};
 
   if (!Connected())
	   return false;

   FD_ZERO(&masterfds);
   FD_SET(GetSocket(), &masterfds);

   memcpy(&readfds, &masterfds, sizeof(fd_set));

   if (select(GetSocket()+1, &readfds, NULL, NULL, &timeout) < 0)
   {
     return false;
   }

   if (FD_ISSET(GetSocket(), &readfds))
   {
		if (recvfrom(GetSocket(), buf, 4096, 0, (struct sockaddr *) &si_other, &slen) != 1)
		{
			msg = buf;
			return true;
		}
	 else
		 return false;
   }
	else
		return false;
}

CStdString EpochSeconds()
{
	CStdString f;
	time_t result = time(NULL);
    asctime(localtime(&result));

	char temp[20];
	long ld = result;
	sprintf(temp,"%ld",ld);

	f += temp;

	return f;

	
}

//-Added EpochSeconds(), returns the current time in Epoch format, as a CString.
//-Added Basic file-handling functions that return true or false if they successfully processed.
//-File-handling functions are loadfile(), writefile(), appendfile() and foundinfile().
//-Now featuring a Command Prompt-friendly version of this library, PeerToPeer_Win32.h, that uses 
// CStdString as a substitute for CString, which is used in MFC Applications. I did that because
// I have had trouble trying to load CStrings into console applications.
//-Added an example MFC program that does Peer-to-Peer chat, used port 6969 as an example.
// Type in a username, and IP to send a message to and the message itself.
// When you receive a message, you don't need to type in an IP, the program will send your message
// to the last IP you received a message to.
