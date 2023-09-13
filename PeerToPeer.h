/*
Absolute Simple Peer-To-Peer (ASPTP) library is an incredibly simple library for C++ programming,
to exchange data between computers, with no main-server or middleman required, via peer-to-peer protocol.
The method used with this library is through UDP hole-punching, which basically temporarily makes an exception
in your network's firewall and opens a specified port and socket, in-order to communicate directly to another system.
I wrote this library as a frustrated programmer who couldn't find any already made, super bare-bones simple library for
P2P connections and communication. So, I made one myself and kept it as SUPER simple as I could. I've commented everything
I possibly could and explained everything in the simplest form to where a beginner programmer can understand.
If you have any questions whatsoever, feel free to contact me.


Email: brdane@gmail.com
Instagram: @spankedcheese
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
#include <vector>

#include <time.h>

#include <sys/types.h>
#include <stdlib.h>

#include<winsock2.h>
#include<ws2tcpip.h>
#include<wininet.h>
#include <sys/stat.h>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"wininet.lib")

struct client
{
	int host;
	short port;
};

struct client server;

bool bConnected = false;


WSADATA wsa;
struct sockaddr_in si_me, si_other;
SOCKET s = 0;
int slen;

#define BUFLEN 4096
char buf[BUFLEN];

char* lastSent;

std::string outputboi;

std::string MyIP()
{

	HINTERNET net = InternetOpen("IP retriever", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	//InternetSetOption(net, INTERNET_OPTION_CONNECT_TIMEOUT, (DWORD)2000, sizeof( (DWORD)2000) );

	HINTERNET conn = InternetOpenUrl(net, "http://myexternalip.com/raw", NULL, 0, INTERNET_FLAG_RELOAD, 0);

	char buffer[BUFLEN];
	DWORD read;

	InternetReadFile(conn, buffer, sizeof(buffer) / sizeof(buffer[0]), &read);
	InternetCloseHandle(net);

	return std::string(buffer, read);

}

//A pointer to the socket for the P2P connection, if you're connected.
int GetSocket()
{
	return s;
}

//Let's you know if you are connected to a socket or not.
bool Connected()
{
	return bConnected;
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

void Disconnect()
{
	if (bConnected)
	{
		closesocket(s);
		WSACleanup();
		bConnected = false;
	}
}

//Connects to a port of your choosing to create the hole-punch.
bool Connect(int ip_port)
{
	//Disconnect from the last socket before we connect to another.
	if (bConnected)
		Disconnect();

	if (!bConnected)
	{
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			outputboi = "WSA cannot start.";
			return false;
		}

		if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		{
			s = 0;
			outputboi = "Cannot make socket.";
			return false;
		}
		else
		{
			memset((char*)&si_me, 0, sizeof(si_me));
			si_me.sin_family = AF_INET;
			si_me.sin_port = htons(ip_port);
			si_me.sin_addr.s_addr = htonl(INADDR_ANY);

			if (bind(s, (sockaddr*)&si_me, sizeof(si_me)) < 0)
			{
				s = 0;
				outputboi = "Cannot bind socket";
				return false;
			}
			slen = sizeof(si_me);
		}
	}
	bConnected = true;
	return true;
}

// check if a given string is a numeric string or not
bool isNumber(const std::string& str)
{
	// `std::find_first_not_of` searches the string for the first character
	// that does not match any of the characters specified in its arguments
	return !str.empty() &&
		(str.find_first_not_of("[0123456789]") == std::string::npos);
}

// Function to split string `str` using a given delimiter
std::vector<std::string> split(const std::string& str, char delim)
{
	auto i = 0;
	std::vector<std::string> list;

	auto pos = str.find(delim);

	while (pos != std::string::npos)
	{
		list.push_back(str.substr(i, pos - i));
		i = ++pos;
		pos = str.find(delim, pos);
	}

	list.push_back(str.substr(i, str.length()));

	return list;
}

// Function to validate an IP address
bool validateIP(std::string ip)
{
	// split the string into tokens
	std::vector<std::string> list = split(ip, '.');

	// if the token size is not equal to four
	if (list.size() != 4)
		return false;

	return true;
}

//Send data to an IP of your choice.
bool SendToIp(const char* server_ip = "", const char* message = "")
{
	char broadcast = '1';

	if (!bConnected)
		return false;

	if (!validateIP(server_ip))
		return false;

	if (message == "")
		return false;

	if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
	{
		closesocket(s);
		return false;
	}


	server_ip = (char*)(LPCTSTR)server_ip;

	memset((char*)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = si_me.sin_port;

	if (server_ip != "255.255.255.255")
		si_other.sin_addr.s_addr = inet_addr(server_ip);
	else
		si_other.sin_addr.s_addr = INADDR_BROADCAST;

	server.host = si_other.sin_addr.s_addr;
	server.port = si_other.sin_port;

	if (sendto(s, message, sizeof(message), 0, (struct sockaddr*)(&si_other), slen) == -1)
		return false;
	else
	{
		lastSent = (char*)message;
		return true;
	}
}

//Returns true if you receive data in your connected socket, also returns the data that you received.
bool received(char*& msg)
{
	if (recvfrom(GetSocket(), buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen) != -1)
	{
		if (msg == lastSent)
			return false;

		msg = buf;
		return true;
	}
	else
		return false;
}

//This is SendToIP(), but it sends a message to the last IP it received a message from.
//Therefore, this is literal replying.
bool Reply(char* message)
{

	if (!bConnected)
		return false;

	int slen = sizeof(si_other);

	if (sendto(s, message, strlen(message), 0, (struct sockaddr*)(&si_other), slen) == -1)
		return false;
	else
		return true;
}

//Disconnect from the socket that you're connected to.



//Quick and easy function to get rid of garbage text in a string.
//This basically NULLs any character that is not on a standard keyboard.
//(any character with an ascii code not between 32 and 126)
const char* NormalizeString(char* in)
{

	for (int i = 0; i < strlen(in); i++)
	{
		if ((in[i] < 31) || (in[i] > 126))
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

//Same as received(), but this has the ability to timeout.
bool received_timeout(char*& msg, long sec, long usec)
{
	fd_set readfds, masterfds;
	struct timeval timeout = { sec,usec };

	if (!Connected())
		return false;

	FD_ZERO(&masterfds);
	FD_SET(GetSocket(), &masterfds);

	memcpy(&readfds, &masterfds, sizeof(fd_set));

	if (select(GetSocket(), &readfds, NULL, NULL, &timeout) < 0)
		return false;

	if (FD_ISSET(GetSocket(), &readfds))
	{
		if (recvfrom(GetSocket(), buf, 4096, 0, (struct sockaddr*)&si_other, &slen) != 1)
		{
			if (msg == lastSent)
				return false;

			msg = buf;
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

std::string NewTag(std::string tagLabel, std::string contents)
{
	return "<" + tagLabel + ">" + contents + "</" + tagLabel + ">";
}

std::string GetTag(std::string tagName, std::string data)
{
	std::string mustfindthis = "<" + tagName + ">";
	std::string andthis = "</" + tagName + ">";

	int fl = data.find(mustfindthis);
	int sl = data.find(andthis);

	if ((fl == -1) || (sl == -1))
		return "";

	return data.substr(fl + mustfindthis.length(), sl - (fl + mustfindthis.length()));
}

bool fileexists(std::string name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

bool writefile(std::string name, std::string contents)
{
	if ( (name == "") || (contents == "") )
		return false;

	if (fileexists(name))
		DeleteFile(name.c_str());

	std::ofstream myfile;

	if (myfile.good())
	{
		myfile.open(name, std::ofstream::out);
		myfile.write(contents.c_str(), contents.length());
		myfile.close();
		return true;
	}
	else
		return false;
}