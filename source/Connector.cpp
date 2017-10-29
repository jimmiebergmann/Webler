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

#include <Connector.hpp>
#include <Webler.hpp>
#include <exception>
#include <string>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#define BUF_SIZE 256


namespace Webler
{


	Connector::Connector(Server * p_pServer, const unsigned short p_Port) :
		m_pServer(p_pServer),
		m_Port(p_Port),
		m_Handle(0)
	{
		// Create socket
		if ((m_Handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
		{
			throw std::string("Cannot create the socket.");
			//GetLastError();
		}

		// Bind
		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = htonl(0);
		service.sin_port = htons(static_cast<USHORT>(p_Port));
			
		if (bind(m_Handle, reinterpret_cast<const sockaddr *>(&service), sizeof(service)) != 0)
		{
			throw std::string("Cannot bind the socket.");
			//GetLastError();
			closesocket(m_Handle);
		}

		// Listen
		if (listen(m_Handle, SOMAXCONN) != 0)
		{
			throw std::string("Cannot listen.");
			//GetLastError();
			closesocket(m_Handle);
		}

		m_Running.Set(true);

		// Start thread
		m_Thread = std::thread([this]()
		{
			Socket::Handle * pAcceptHandle = nullptr;

			while (m_Running.Get() == true)
			{
				// Create a new accept handle if needed.
				if (pAcceptHandle == nullptr)
				{
					pAcceptHandle = new Socket::Handle;
				}
				
				// Accept incoming connection.
				if ((*pAcceptHandle = accept(m_Handle, NULL, NULL)) == -1)
				{
					std::cout << "FAILED TO ACCEPT!" << std::endl;
					
					// Cleanup accept handle if needed.
					if (pAcceptHandle)
					{
						delete pAcceptHandle;
					}
					return;
				}

				std::cout << "Client connected!" << std::endl;

				// Create a daemon
				Daemon * pDaemon = new Daemon(Daemon::HostType);
				if (pDaemon->Create(m_pServer->GetProgramPath(), *pAcceptHandle) == false)
				{
					std::cout << "Failed to create daemon!" << std::endl;
					return;
				}

				m_Daemons.push_back(pDaemon);

				//ThreadData * pThreadData = new ThreadData;
				//pThreadData->Finished.Value = false;

				// Create new daemon thread
				/*pThreadData->Thread = std::thread([this, pAcceptHandle, pThreadData]()
				{
					// Create daemon
					Daemon daemon(Daemon::HostType);
					if (daemon.Create(m_pServer->GetProgramPath(), *pAcceptHandle) == false)
					{
						std::cout << "Failed to create daemon!" << std::endl;
						return;
					}

					// Join daemon
					if (daemon.Join() == false)
					{
						std::cout << "Failed to join daemon." << std::endl;
					}

					std::cout << "Joined daemon." << std::endl;

					pThreadData->Finished.Set(true);
				});*/

				//m_DaemonThreads.push_back(pThreadData);

				// Set accept handle to nullptr, will force another allocation in next loop.
				pAcceptHandle = nullptr;
			}

		});


	}

	Connector::~Connector()
	{
		// Wait for listening thread to stop
		m_Running.Set(false);
		closesocket(m_Handle);
		m_Thread.join();

		// Clean up daemon threads
		for (auto it = m_Daemons.begin(); it != m_Daemons.end(); it++)
		{
			// Terminate daemon process if needed.
			if ((*it)->IsFinished() == false)
			{
				(*it)->Terminate();
			}

			// Join thrread
			(*it)->Join();
			
			delete *it;
		}

	}

	unsigned short Connector::GetPort()
	{
		return m_Port;
	}

}