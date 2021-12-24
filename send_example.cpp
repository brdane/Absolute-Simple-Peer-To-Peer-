#include "resource.h"
#include "StdAfx.h"
#include "PeerToPeer.h"

int main(int argc, char* argv[])
{
	char *msg;


	Connect(6969);

	if (Connected())
		printf("Connection Established.\n");
	else
		return 1; //Shouldn't continue if we can't open the socket.

	while(1)
	{
		if (SendToIp("74.91.112.5", "Fisters."))
			printf("Sent.\n");
		else
			printf("Failed to Send.\n");

		if (received(msg))
		{
			if (SameThing(NormalizeString(msg),"Fisters."))
			printf("Delivered.\n");
		}
	}

	return 0;

}