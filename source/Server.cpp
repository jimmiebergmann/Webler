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

#include <Server.hpp>
//#include <Private/ServerImp.hpp>
#include <Utility/Semaphore.hpp>
#include <Utility/ThreadValue.hpp>
#include <csignal>
#include <set>
#include <iostream>
#include <Log.hpp>
#include <exception>
#include <WinSock2.h>

// Static pointer to server class.
static Webler::Server * g_pServerInstance = nullptr;
static Webler::Log * g_pLog = nullptr;
static void SignalHandlerFunction(int signal);

#define SERVER_IMP reinterpret_cast<Webler::ServerImp*>(this->m_pImp)
#define SERVER_IMP_FROM(Server) reinterpret_cast<Webler::ServerImp*>(Server->m_pImp)
#define SETTINGS_IMP reinterpret_cast<SettingsImp*>(this->m_pImp)
#define SERVER_SETTINGS_IMP reinterpret_cast<Webler::SettingsImp*>(reinterpret_cast<Webler::ServerImp*>(this->m_pImp)->pSettings->m_pImp)
#define WEBLER_LOG (*g_pLog)

namespace Webler
{
	// Forward declarations
	class Listener;


	// Socket handle
	namespace Socket
	{
		typedef SOCKET Handle;
	}


	// Server settings implementation
	class ServerImp
	{

	public:

		ServerImp() :
			pSettings(nullptr)
		{

		}

		~ServerImp()
		{
			for (auto it = Listeners.begin(); it != Listeners.end(); it++)
			{
				delete *it;
			}
		}

		void Stop()
		{

		}

		bool Listen(const unsigned short p_Port)
		{

			return false;
		}

		typedef std::set<Listener *> ListenerSet;

		Utility::Semaphore	ExitSemaphore;
		Server::Settings *  pSettings;
		ListenerSet			Listeners;
		//std::string		ProgramPath;


	};


	// Listener class
	class Listener
	{

	public:

		Listener() :
			m_Port(0),
			m_Handle(0),
			m_Running(false)
		{
			/*// Create socket
			if ((m_Handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
			{
				throw new std::runtime_error("Failed to create socket. Error code: " + std::to_string(WSAGetLastError()));
			}

			// Bind
			sockaddr_in service;
			service.sin_family = AF_INET;
			service.sin_addr.s_addr = htonl(0);
			service.sin_port = htons(static_cast<USHORT>(p_Port));

			if (bind(m_Handle, reinterpret_cast<const sockaddr *>(&service), sizeof(service)) != 0)
			{
				closesocket(m_Handle);
				throw new std::runtime_error("Failed to bind socket. Error code: " + std::to_string(WSAGetLastError()));
			}

			// Listen
			if (listen(m_Handle, SOMAXCONN) != 0)
			{
				closesocket(m_Handle);
				throw new std::runtime_error("Failed to listen on port " + std::to_string(m_Port) + ". Error code: " + std::to_string(WSAGetLastError()));
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
					if ((*pAcceptHandle = accept(m_Handle, NULL, NULL)) == INVALID_SOCKET)
					{
						const int error = WSAGetLastError();

						// Socket has been deleted, is it supposed to?
						if (error == WSAENOTSOCK)
						{
							delete pAcceptHandle;

							if (m_Running.Get() == true)
							{
								throw new std::runtime_error("Listen socket of port " + std::to_string(m_Port) +
									" closed unexpectedly. Error code: " + std::to_string(WSAGetLastError()));
							}

							return;
						}
						// Peer reseted connection.
						if (error == WSAECONNRESET)
						{
							continue;
						}

						// Undefined error.
						delete pAcceptHandle;
						throw new std::runtime_error("Failed to accept connection. Error code: " + std::to_string(WSAGetLastError()));
					}

					std::cout << "Client connected!" << std::endl;

					// Create a daemon
					DaemonProcess * pDaemonProcess = new DaemonProcess();
					if (pDaemonProcess->Create(pServerImp->ProgramPath, *pAcceptHandle) == false)
					{
						throw new std::runtime_error("Failed to create daemon process.");
					}

					// Finish accept loop by adding daemon process.
					m_DaemonProcesses.push_back(pDaemonProcess);
					pAcceptHandle = nullptr;
				}
			});*/
		}

		bool Start(const unsigned short p_Port)
		{
			
			// Create socket
			if ((m_Handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
			{
				WEBLER_LOG << Log::Error << "Failed to create socket. Error code : " + std::to_string(WSAGetLastError());
				return false;
			}

			m_Port = p_Port;

			// Bind
			sockaddr_in service;
			service.sin_family = AF_INET;
			service.sin_addr.s_addr = htonl(0);
			service.sin_port = htons(static_cast<USHORT>(p_Port));

			if (bind(m_Handle, reinterpret_cast<const sockaddr *>(&service), sizeof(service)) != 0)
			{
				closesocket(m_Handle);
				WEBLER_LOG << Log::Error << "Failed to bind socket. Error code: " + std::to_string(WSAGetLastError());
				return false;
			}

			// Listen
			if (listen(m_Handle, SOMAXCONN) != 0)
			{
				closesocket(m_Handle);
				WEBLER_LOG << Log::Error << "Failed to listen on port " + std::to_string(m_Port) + ". Error code: " + std::to_string(WSAGetLastError());
				return false;
			}

			m_Running.Set(true);

			// Start thread
			m_Thread = std::thread([this]()
			{
				Socket::Handle * pAcceptHandle = nullptr;
			});

			return true;
		}

		~Listener()
		{
			if (m_Running.Get())
			{
				m_Running.Set(false);
				m_Thread.join();
			}
		}

		const unsigned short GetPort() const
		{
			return m_Port;
		}

	private:

		//typedef std::list<DaemonProcess *> DaemonDaemonProcessList;

		Socket::Handle					m_Handle;
		std::thread						m_Thread;
		//DaemonDaemonProcessList			m_DaemonProcesses;
		Utility::ThreadValue<bool>		m_Running;
		unsigned short					m_Port;

	};



	// Server settings implementation
	class SettingsImp
	{

	public:

		SettingsImp() :
			pLog(nullptr)
		{

		}

		Log *						pLog;
		std::set<unsigned short>	ListenPorts;

	};




	// Public server settings class
	Server::Settings::~Settings()
	{
		delete m_pImp;
	}

	bool Server::Settings::Listen(const unsigned short p_Port)
	{
		auto it = SETTINGS_IMP->ListenPorts.find(p_Port);
		if (it != SETTINGS_IMP->ListenPorts.end())
		{
			return false;
		}

		SETTINGS_IMP->ListenPorts.insert(p_Port);
		return true;
	}

	bool Server::Settings::CustomLog(Log * p_pLog)
	{
		if (p_pLog == nullptr)
		{
			return false;
		}

		if (SETTINGS_IMP->pLog)
		{
			delete SETTINGS_IMP->pLog;
		}

		SETTINGS_IMP->pLog = p_pLog;
		return true;
	}

	Server::Settings::Settings() :
		m_pImp(reinterpret_cast<void *>(new SettingsImp))
	{
	}

	Server::Settings::Settings(const Settings & p_Settings)
	{
	}


	// Public server class
	Server::Server() :
		m_pImp(reinterpret_cast<void *>(new ServerImp))
	{

	}

	Server::~Server()
	{
		if (m_pImp)
		{
			delete m_pImp;
		}
	}

	int Server::Boot(int p_ArgumentCount, char ** p_ppArgumentValues, Shared * p_pShared)
	{
		// Add current server instance.
		if (g_pServerInstance != nullptr)
		{
			std::cout << "Server is already booted. Only one instance of Server is allowed." << std::endl;
			return 0;
		}
		g_pServerInstance = this;

		// Initialize Windows sockets 2.2.
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			std::cout << "Failed to initialize winsock." << std::endl;
			return 0;
		}

		// Create settings
		SERVER_IMP->pSettings = new Settings;
		SettingsImp * pSettingsImp = SERVER_SETTINGS_IMP;

		// Run user definied Start function.
		Start(*SERVER_IMP->pSettings);


		// Set log class
		g_pLog = pSettingsImp->pLog;
		if (g_pLog == nullptr)
		{
			/// ADD PROGRAM DIR PATH.
			const std::string logPath = GetProgramDirectory() + "/server.log";
			g_pLog = new Log(logPath);
			if (g_pLog->IsOpen() == false)
			{
				std::cout << "Failed to open server log file: " <<  logPath << std::endl;
				return 0;
			}
		}

		// Start to listen on ports in settings
		for (auto it = pSettingsImp->ListenPorts.begin(); it != pSettingsImp->ListenPorts.end(); it++)
		{
			const unsigned int port = *it;
			Listener * pListener = new Listener;
			if (pListener->Start(port) == false)
			{
				std::cout << Log::Error << "Failed to listen on port " << port << ".";
				WEBLER_LOG << Log::Error << "Failed to listen on port " << port << ".";
				return 0;
			}

			SERVER_IMP->Listeners.insert(pListener);
		}

		WEBLER_LOG << Webler::Log::Info << "Server running.";

		// Bind signals
		std::signal(SIGABRT, SignalHandlerFunction);
		std::signal(SIGTERM, SignalHandlerFunction);
		std::signal(SIGBREAK, SignalHandlerFunction);

		// Wait for Stop implementation call.
		SERVER_IMP->ExitSemaphore.Wait();
		return 0;
	}

}


// Static implementations
// Signals are bound in Boot function.
static void SignalHandlerFunction(int signal)
{
	if (g_pLog)
	{
		WEBLER_LOG << Webler::Log::Info << "Stopping server.";
		delete g_pLog;
	}


	/*if (g_pServerInstance == nullptr)
	{
		return;
	}
	SERVER_IMP_FROM(g_pServerInstance)->Stop();
	g_pServerInstance = nullptr;*/
}