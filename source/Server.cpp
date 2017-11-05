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
#include <Log.hpp>
//#include <Private/ServerImp.hpp>
#include <Utility/Semaphore.hpp>
#include <Utility/ThreadValue.hpp>
#include <csignal>
#include <set>
#include <list>
#include <iostream>
#include <sstream>
#include <Log.hpp>
#include <exception>
#include <WinSock2.h>

// Static functions / variables
static Webler::Server * g_pServerInstance = nullptr;
static Webler::Utility::ThreadValue<unsigned int> g_DaemonProcessCounter;
static void SignalHandlerFunction(int signal);

#define SERVER_IMP reinterpret_cast<Webler::ServerImp*>(this->m_pImp)
#define SERVER_IMP_FROM(Server) reinterpret_cast<Webler::ServerImp*>(Server->m_pImp)
#define SETTINGS_IMP reinterpret_cast<SettingsImp*>(this->m_pImp)
#define SERVER_SETTINGS_IMP reinterpret_cast<Webler::SettingsImp*>(reinterpret_cast<Webler::ServerImp*>(this->m_pImp)->pSettings->m_pImp)

namespace Webler
{
	// Forward declarations
	class Listener;
	class SettingsImp;


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

		typedef std::set<Listener *> ListenerSet;
		Utility::Semaphore	ExitSemaphore;
		Server::Settings *  pSettings;
		ListenerSet			Listeners;
		unsigned int		MaxExecutionTime;

	};




	// Daemon process class
	class DaemonProcess
	{

	public:

		DaemonProcess() :
			m_ThreadCreated(false),
			m_Started(false),
			m_ProcessHandle(0)
		{
		}

		DaemonProcess::~DaemonProcess()
		{
			Terminate();
		}

		bool Create(const std::string & p_Program, Socket::Handle & p_SocketHandle)
		{
			m_Thread = std::thread([this, p_Program, p_SocketHandle]()
			{
				BOOL ret = 0;
				HANDLE hPipe;
				DWORD dwBytes;
				PROCESS_INFORMATION piProc;
				STARTUPINFO siStartInfo;
				WSAPROTOCOL_INFO protInfo;
				OVERLAPPED ol = { 0,0,0,0,NULL };

				DWORD processId = GetCurrentProcessId();
				std::stringstream ss;
				ss << processId << "_" << GetNextProcessId();
				std::string pipeName = "\\\\.\\pipe\\Webler_shared_mem_" + ss.str();

				// I create a named pipe for communication with the spawned process
				const DWORD BUFSIZE = 256;
				const DWORD PIPE_TIMEOUT_CONNECT = 3000;

				hPipe = CreateNamedPipe(
					pipeName.c_str(),				// pipe name
					PIPE_ACCESS_DUPLEX |			// read/write access
					FILE_FLAG_OVERLAPPED,
					PIPE_TYPE_BYTE |				// message type pipe
					PIPE_READMODE_BYTE |			// message-read mode
					PIPE_WAIT,						// blocking mode
					PIPE_UNLIMITED_INSTANCES,		// max. instances
					BUFSIZE,						// output buffer size
					BUFSIZE,						// input buffer size
					PIPE_TIMEOUT_CONNECT,			// client time-out
					NULL);							// default security attribute

				if (hPipe == INVALID_HANDLE_VALUE)
				{
					WEBLER_LOG(Log::Error, "Failed to create named pipe: " << pipeName << ". " << static_cast<int>(GetLastError()) );
					return;
				}

				// I create a new process(daemon)
				GetStartupInfo(&siStartInfo);
				const std::string programArgs = p_Program + " -daemon " + pipeName;

				ret = CreateProcess(p_Program.c_str(), const_cast<LPSTR>(programArgs.c_str()),
					NULL, NULL, // security attributes process/thread
					TRUE,       // inherit handle
					0, // fdwCreate
					NULL, // lpvEnvironment 
					".", // lpszCurDir
					&siStartInfo, // lpsiStartInfo 
					&piProc);

				if (ret == 0)
				{
					WEBLER_LOG(Log::Error, "Failed to create daemon process: " << pipeName << ". " << static_cast<int>(GetLastError()) );
					return;
				}

				// Notify daemon process as started.
				m_ProcessHandle.Set(piProc.hProcess);
				m_Started.Set(true);
				
				// I duplicate the socket
				/*ret = WSADuplicateSocket(p_SocketHandle, piProc.dwProcessId, &protInfo);
				if (ret == SOCKET_ERROR)
				{
				std::cout << "Failed to duplicate socket. " << GetLastError() << std::endl;
				return false;
				}*/
					
				ol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

				// I connect to the named pipe...
				ret = ConnectNamedPipe(hPipe, &ol);
				if (ret == 0)
				{
					switch (GetLastError())
					{
					case ERROR_PIPE_CONNECTED:
						ret = TRUE;
						break;

					case ERROR_IO_PENDING:
						if (WaitForSingleObject(ol.hEvent, PIPE_TIMEOUT_CONNECT) == WAIT_OBJECT_0)
						{
							DWORD dwIgnore;
							ret = GetOverlappedResult(hPipe, &ol, &dwIgnore, FALSE);
						}
						else
						{
							CancelIo(hPipe);
						}
						break;
					}
				}

				CloseHandle(ol.hEvent);

				if (ret == 0)
				{
					WEBLER_LOG(Log::Error, "Failed to connect named pipe. : " << pipeName << " - " << static_cast<int>(GetLastError()) );
					return;
				}


				// I write the socket descriptor to the named pipe
				if (WriteFile(hPipe, &p_SocketHandle, sizeof(p_SocketHandle), &dwBytes, NULL) == 0)
				{
					WEBLER_LOG(Log::Error, "Failed to write socket handle to shared memory. " << static_cast<int>(GetLastError()) );
					return;
				}

				// I write the protocol information structure to the named pipe
				/*if (WriteFile(hPipe, &protInfo, sizeof(protInfo), &dwBytes, NULL) == 0)
				{
				std::cout << "Failed to write prot info to shared memory. " << GetLastError() << std::endl;
				return false;
				}*/
	

				// Close pipe handle.
				CloseHandle(hPipe);

			});

			m_ThreadCreated = true;
			return true;
		}

		void Terminate()
		{
			if (m_ThreadCreated)
			{
				if (m_Started.Get())
				{
					TerminateProcess(m_ProcessHandle.Get(), 0);
				}
			
				m_Thread.join();
			}
			m_Started.Set(false);
			m_ProcessHandle.Set(false);
			m_ThreadCreated = false;
		}

		bool IsStarted()
		{
			return m_Started.Get();
		}

		HANDLE GetProcessHandle()
		{
			return m_ProcessHandle.Get();
		}

	private:

		unsigned int GetNextProcessId()
		{
			std::lock_guard<std::mutex> lock(g_DaemonProcessCounter.Mutex);
			return (g_DaemonProcessCounter.Value += 1);
		}

		std::thread						m_Thread;
		bool							m_ThreadCreated;
		Socket::Handle					m_SocketHandle;
		Utility::Semaphore				m_StartingSemaphore;
		Utility::ThreadValue<bool>		m_Started;
		Utility::ThreadValue<HANDLE>	m_ProcessHandle;

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
		}

		bool Start(const unsigned short p_Port)
		{
			// Create socket
			if ((m_Handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
			{
				WEBLER_LOG(Log::Error, "Failed to create socket. Error code : " << std::to_string(WSAGetLastError()) );
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
				WEBLER_LOG(Log::Error, "Failed to bind socket. Error code: " + std::to_string(WSAGetLastError()) );
				return false;
			}

			// Listen
			if (listen(m_Handle, SOMAXCONN) != 0)
			{
				closesocket(m_Handle);
				WEBLER_LOG(Log::Error, "Failed to listen on port " + std::to_string(m_Port) + ". Error code: " + std::to_string(WSAGetLastError()) );
				return false;
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
								WEBLER_LOG(Log::Error, "Listen socket of port " + std::to_string(m_Port) +
									" closed unexpectedly. Error code: " + std::to_string(WSAGetLastError()));
								return;
							}
						}
						// Peer reseted connection.
						if (error == WSAECONNRESET)
						{
							continue;
						}

						// Undefined error.
						delete pAcceptHandle;
						WEBLER_LOG(Log::Error, "Failed to accept connection. Error code: " + std::to_string(WSAGetLastError()));
						return;
					}

					WEBLER_LOG(Log::Info, "Client connected.");

					// Create a daemon
					DaemonProcess * pDaemonProcess = new DaemonProcess();
					if (pDaemonProcess->Create(Server::GetProgramPath(), *pAcceptHandle) == false)
					{
						throw new std::runtime_error("Failed to create daemon process.");
					}

					// Finish accept loop by adding daemon process.
					m_DaemonProcesses.push_back(pDaemonProcess);
					pAcceptHandle = nullptr;
				}
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

			// Cleanup daemon processes
			for (auto it = m_DaemonProcesses.begin(); it != m_DaemonProcesses.end(); it++)
			{
				delete *it;
			}
		}

		const unsigned short GetPort() const
		{
			return m_Port;
		}

	private:

		typedef std::list<DaemonProcess *> DaemonDaemonProcessList;

		Socket::Handle					m_Handle;
		std::thread						m_Thread;
		DaemonDaemonProcessList			m_DaemonProcesses;
		Utility::ThreadValue<bool>		m_Running;
		unsigned short					m_Port;

	};



	// Server settings implementation
	class SettingsImp
	{

	public:

		SettingsImp() :
			pLog(nullptr),
			MaxExecutionTime(30)
		{

		}

		Log *						pLog;
		std::set<unsigned short>	ListenPorts;
		unsigned int				MaxExecutionTime;

	};




	// Public server settings class
	Server::Settings::~Settings()
	{
		delete m_pImp;
	}

	Server::Settings & Server::Settings::Listen(const unsigned short p_Port)
	{
		auto it = SETTINGS_IMP->ListenPorts.find(p_Port);
		if (it != SETTINGS_IMP->ListenPorts.end())
		{
			return *this;
		}

		SETTINGS_IMP->ListenPorts.insert(p_Port);
		return *this;
	}

	Server::Settings & Server::Settings::CustomLog(Log * p_pLog)
	{
		if (p_pLog == nullptr)
		{
			return *this;
		}

		if (SETTINGS_IMP->pLog)
		{
			delete SETTINGS_IMP->pLog;
		}

		SETTINGS_IMP->pLog = p_pLog;
		return *this;
	}

	Server::Settings & Server::Settings::MaxExecutionTime(const unsigned int p_Seconds)
	{
		SETTINGS_IMP->MaxExecutionTime = p_Seconds;
		return *this;
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
		Log * pLog = pSettingsImp->pLog;
		if (pLog == nullptr)
		{
			const std::string logPath = GetProgramDirectory() + "/server.log";
			pLog = new Log(logPath);
			if (pLog->IsOpen() == false)
			{
				std::cout << "Failed to open server log file: " <<  logPath << std::endl;
				return 0;
			}
		}
		pLog->m_Imp.SetCurrentLog(pLog);

		// Set maximum execution time
		SERVER_IMP->MaxExecutionTime = pSettingsImp->MaxExecutionTime;

		// Start to listen on ports in settings
		for (auto it = pSettingsImp->ListenPorts.begin(); it != pSettingsImp->ListenPorts.end(); it++)
		{
			const unsigned int port = *it;
			Listener * pListener = new Listener;
			if (pListener->Start(port) == false)
			{
				std::cout << Log::Error << "Errror: Failed to listen on port " << port << ".";
				WEBLER_LOG(Log::Error, "Failed to listen on port " << port << ".");
				return 0;
			}

			SERVER_IMP->Listeners.insert(pListener);
		}

		WEBLER_LOG(Webler::Log::Info, "Server running." );

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
	// Stop server
	if (g_pServerInstance == nullptr)
	{
		SERVER_IMP_FROM(g_pServerInstance)->Stop();
		g_pServerInstance = nullptr;
	}

	// Close the log
	if (Webler::Log::GetCurrentLog())
	{
		WEBLER_LOG(Webler::Log::Info, "Server stopped.");
		delete Webler::Log::GetCurrentLog();
	}
}