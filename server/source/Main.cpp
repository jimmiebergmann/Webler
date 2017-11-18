#include <Server.hpp>
#include <exception>
#include <iostream>
#include <csignal>

static Webler::Server server;

int main()
{
	/*
	// Memory leak detection on windows.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	*/

	// Load server settings.
	Webler::Server::Settings settings({80, 88}, "Daemon.exe", 10899);

	// Start server
	try
	{
		server.Start(settings);
	}
	catch (const std::exception & e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		return -1;
	}

	std::cout << "Started Webler server." << std::endl;
	std::cout << "Configurations:" << std::endl;
	std::cout << "--------------------------" << std::endl;
	std::cout << "Listener ports: ";
	auto listenerPorts = settings.ListeningPorts();
	size_t portLoop = 0;
	for (auto it = listenerPorts.begin(); it != listenerPorts.end(); it++)
	{
		portLoop++;
		std::cout << *it;
		if (portLoop < listenerPorts.size())
		{
			std::cout << ", ";
		}
	}
	std::cout << std::endl;
	std::cout << "Daemon listener port: " << settings.GetDaemonPort() << std::endl;
	std::cout << "--------------------------" << std::endl;

	std::cin.get();
	server.Stop();

	//server.Finish();
	return 0;
}