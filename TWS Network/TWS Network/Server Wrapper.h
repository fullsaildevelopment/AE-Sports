#pragma once

#include "Server.h"

class ServerWrapper
{
private:
	Server newServer;
	//Client serverClient;
public:
	~ServerWrapper() { 
		//serverClient.stop();
	newServer.stop(); }
	int init(char * address, UINT16 port)
	{
		
		return newServer.init(port);//serverClient.init(address, port);
	}
	int run()
	{
		
		return newServer.update(); //serverClient.run();
	}
	void stop()
	{
		//serverClient.stop();
		newServer.stop();
	}
};