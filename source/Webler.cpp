/*

MIT License

Copyright (c) 2017 Jimmie Bergmann

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <Webler.hpp>
#include <Daemon.hpp>
#include <chrono>
#include <iostream>
#include <Windows.h>


namespace Webler
{

	Server::Server()
	{
		std::cout << __TIMESTAMP__ << std::endl;
	}

	Server::~Server()
	{
	}

	void Server::Listen(const unsigned short p_Port)
	{
		if (m_Type != HostType)
		{
			return;
		}

		// Check if we already are listening on this port
		for (auto it = m_Connectors.begin(); it != m_Connectors.end(); it++)
		{
			if ((*it)->GetPort() == p_Port)
			{
				return;
			}
		}

		Connector * pConnector = new Connector(this, p_Port);
		m_Connectors.insert(pConnector);
	}

	void Server::Mute(const unsigned short p_Port)
	{
		if (m_Type != HostType)
		{
			return;
		}

		for (auto it = m_Connectors.begin(); it != m_Connectors.end(); it++)
		{
			if ((*it)->GetPort() == p_Port)
			{
				delete (*it);
				m_Connectors.erase(it);
				return;
			}
		}
	}

	int Server::Stop()
	{
		if (m_Started.Get() == false)
		{
			return 0;
		}

		m_ExitSemaphore.Notify();
		return 0;
	}

	int Server::Start(int argc, char ** argv)
	{
		// Daemon launch
		if (m_Type == DaemonType)
		{
			m_Started.Set(true);
			return RunDaemon(argc, argv);
		}

		// Get program path
		m_ProgramPath = argv[0];

		// Call user defined setup function.
		Router router(this);
		Route(router);

		// Flag the server as running and wait for it to exit.
		m_Started.Set(true);
		m_ExitSemaphore.Wait();
		return 0;
	}

	int Server::RunDaemon(int argc, char ** argv)
	{
		std::cout << "Daemon: Running." << std::endl;

		Daemon daemon(Daemon::DaemonType);
		if(daemon.Load(argc, argv) == false)
		{
			std::cout << "Daemon: Failed to load daemon." << std::endl;
			return 0;
		}

		Socket::Handle socketHandle = daemon.GetSocketHandle();
		if (socketHandle == 0)
		{
			std::cout << "Daemon: Invalid socket handle." << std::endl;
			return 0;
		}

		// Use the handle here..
		std::cout << "Daemon: Can now use the socket handle: " << socketHandle << std::endl;
		
		if (send(socketHandle, "test", 4, 0) != 4)
		{
			std::cout << "Daemon: Failed to send data!" << std::endl;
		}

		std::cout << "Daemon: Sent data." << std::endl;

		
		return 0;
	}

	const std::string & Server::GetProgramPath() const
	{
		return m_ProgramPath;
	}

}
