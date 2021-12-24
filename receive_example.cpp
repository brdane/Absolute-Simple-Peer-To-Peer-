
#include "resource.h"
#include "stdafx.h"
#include "PeerToPeer.h"


int main(int argc, char* argv[])
{
	char* buf;

	Connect(6969);

	if (Connected())
		printf("Connection Established.\n");

	while(1)
	{

		if (received(buf))
		{
			printf("%s: %s\n",inet_ntoa(ServerNetwork().sin_addr), NormalizeString(buf));
			SendToIp(inet_ntoa(ServerNetwork().sin_addr), NormalizeString(buf));
		}
	}

	return 0;
}