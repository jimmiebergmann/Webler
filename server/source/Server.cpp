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
#include <exception>
#include <mutex>
#include <thread>
#include <vector>
#include <queue>
#include <map>
#include <fstream>
#include <iostream>
#include <chrono>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>


#include <stdlib.h>
#include <crtdbg.h>

#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)


#define SERVER_SETTINGS_IMP reinterpret_cast<Webler::ServerSettingsImp*>(this->m_pImp)
#define SERVER_IMP reinterpret_cast<Webler::ServerImp*>(this->m_pImp)
#define SERVER_IMP_FROM(ServerImp) reinterpret_cast<Webler::ServerImp*>(ServerImp->m_pImp)

/**
* \breif Webler namespace scope.
*
*/
namespace Webler
{
	// Typedefs and forward declarations
	typedef SOCKET SocketHandle;
	class Listener;
	class Process;


	/******************************************************
	* Helper functions
	*
	******************************************************/

	// Sempahore class
	class Semaphore
	{

	public:

		Semaphore() :
			m_Count(0)
		{
		}

		void Notify()
		{
			std::unique_lock<decltype(m_Mutex)> lock(m_Mutex);
			++m_Count;
			m_Condition.notify_one();
		}

		void NotifyAll()
		{
			std::unique_lock<decltype(m_Mutex)> lock(m_Mutex);
			m_Count = 0;
			m_Condition.notify_all();
		}

		void Wait()
		{
			std::unique_lock<decltype(m_Mutex)> lock(m_Mutex);
			while (!m_Count)
			{
				m_Condition.wait(lock);
			}
			--m_Count;
		}

		bool TryWait()
		{
			std::unique_lock<decltype(m_Mutex)> lock(m_Mutex);
			if (m_Count)
			{
				--m_Count;
				return true;
			}

			return false;
		}

		bool WaitFor(const unsigned int p_Milliseconds)
		{
			std::unique_lock<decltype(m_Mutex)> lock(m_Mutex);
			if (!m_Count)
			{
				if (m_Condition.wait_for(lock, std::chrono::milliseconds(p_Milliseconds)) == std::cv_status::no_timeout)
				{
					--m_Count;
					return true;
				}
			}

			return false;
		}

	private:

		std::mutex				m_Mutex;
		std::condition_variable m_Condition;
		unsigned long			m_Count;

	};


	// Thread value class
	template<typename T>
	class ThreadValue
	{

	public:

		ThreadValue()
		{
		}

		ThreadValue(const T & p_Value) :
			Value(p_Value)
		{
		}

		ThreadValue<T> & operator = (const T & p_Value)
		{
			std::lock_guard<std::mutex> lock(Mutex);
			Value = p_Value;
			return *this;
		}

		const T operator ()()
		{
			std::lock_guard<std::mutex> lock(Mutex);
			return Value;
		}

		ThreadValue<T> & operator ++ ()
		{
			std::lock_guard<std::mutex> lock(Mutex);
			Value++;
			return *this;
		}

		std::mutex	Mutex;
		T			Value;

	};


	/******************************************************
	* Private declarations of implementation classes.
	*
	******************************************************/

	// Hidden server implementations
	class ServerImp
	{

	public:

		ServerImp();
		~ServerImp();
		void Stop();
		void CreateListeners();
		void Listen();
		void StartDaemonListener();
		Process * GetIdleProcess();

		void NotifyProcessTermination(Process * p_pProcess);

		void SendInternalError(SocketHandle * p_pSockethandle);


		// Typedefs
		typedef std::set<Listener*> Listeners;

		// Daemon listener
		class DaemonListener
		{

		public:

			DaemonListener();
			~DaemonListener();

			std::thread					m_Thread;
			ThreadValue<bool>			m_Started;
			SocketHandle				m_SocketHandle;
			unsigned short				m_DaemonPort;
		};

		typedef std::queue<Process*> ProcessQueue;
		typedef std::map<unsigned int, Process*> ProcessMap;

		// Variables
		ThreadValue<bool>			m_Started;
		Server::Settings			m_Settings;
		Semaphore					m_StopSemaphore;
		Listeners					m_Listeners;
		ThreadValue<ProcessMap>		m_SpawnedProcesses;
		ThreadValue<ProcessQueue>	m_IdleProcesses;
		ThreadValue<ProcessQueue>	m_WorkingProcesses;
		DaemonListener *			m_pDaemonListener;

	};


	// Listener class handling each listening port.
	class Listener
	{

	public:

		Listener(ServerImp * p_pServerImp);
		~Listener();
		void Create(const unsigned short p_Port);
		void Start();

	private:

		typedef std::set<std::thread *> TheadSet;

		void HandleConnection(SocketHandle * p_pSocketHandle);

		ServerImp *					m_pServerImp;
		ThreadValue<bool>			m_Created;
		ThreadValue<bool>			m_Started;
		unsigned short				m_Port;
		SocketHandle				m_SocketHandle;
		std::thread					m_Thread;
		ThreadValue<TheadSet>		m_ConnectionThreads;


	};


	// Process class
	class Process
	{
		
	public:

		enum eState
		{
			Idle,
			Working,
			Finished
		};

		Process(SocketHandle * p_pSocketHandle, unsigned int p_ProcessId, ServerImp * p_pServerImp);
		~Process();
		
		eState GetState() const;
		void SetState(const eState & p_State);
		SocketHandle * GetSocketHandle();
		unsigned int GetProcessId() const;

		static unsigned int GetProcessIdFromSocket(const SocketHandle & p_SocketHandle);
		void Handshake();

	private:

		eState				m_State;
		SocketHandle *		m_pSocketHandle;
		unsigned int		m_ProcessId;
		HANDLE				m_ProcessHandle;
		ServerImp *			m_pServerImp;
		std::thread			m_Thread;
		ThreadValue<bool>	m_ClosingThread;

	};


	/******************************************************
	* Private declarations of implementation classes.
	*
	******************************************************/

	// Server implementation
	ServerImp::ServerImp() :
		m_Started(false),
		m_pDaemonListener(nullptr)
	{

	}

	ServerImp::~ServerImp()
	{
		Stop();
	}

	void ServerImp::Stop()
	{
		m_Started = false;

		if (m_pDaemonListener)
		{
			delete m_pDaemonListener;
			m_pDaemonListener = nullptr;
		}

		for (auto it = m_Listeners.begin(); it != m_Listeners.end(); it++)
		{
			delete *it;
		}
		m_Listeners.clear();

		while (m_IdleProcesses.Value.size())
		{
			Process * pProcess = m_IdleProcesses.Value.front();
			m_IdleProcesses.Value.pop();
			delete pProcess;
		}

		while (m_WorkingProcesses.Value.size())
		{
			Process * pProcess = m_WorkingProcesses.Value.front();
			m_WorkingProcesses.Value.pop();
			delete pProcess;
		}
	}

	void ServerImp::CreateListeners()
	{
		// Go throgh all listening ports
		auto ports = m_Settings.ListenPorts;

		Listener * pListener = nullptr;
		bool error = false;
		for (auto it = ports.begin(); it != ports.end(); it++)
		{
			const unsigned short port = *it;
			try
			{
				pListener = new Listener(this);
				pListener->Create(port);
				m_Listeners.insert(pListener);
			}
			catch (...)
			{
				// Clean up listeners
				delete pListener;
				for (auto it = m_Listeners.begin(); it != m_Listeners.end(); it++)
				{
					delete *it;
				}
				m_Listeners.clear();
				
				// Rethrow exception
				throw;
			}
		}
	}

	void ServerImp::Listen()
	{
		for(auto it = m_Listeners.begin(); it != m_Listeners.end(); it++)
		{
			(*it)->Start();
		}
	}

	void ServerImp::StartDaemonListener()
	{
		if (m_pDaemonListener)
		{
			throw std::exception("Daemon listener is already running.");
		}
		m_pDaemonListener = new DaemonListener;

		int error = 0;
		unsigned short daemonPort = m_Settings.DaemonPort;

		// Create socket
		if ((m_pDaemonListener->m_SocketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
		{
			throw std::exception(std::string("Failed to create daemon socket. Error code: " + std::to_string(WSAGetLastError())).c_str());
		}

		// Bind
		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		service.sin_port = htons(static_cast<USHORT>(daemonPort));

		if ((error = bind(m_pDaemonListener->m_SocketHandle, reinterpret_cast<const sockaddr *>(&service), sizeof(service))) != 0)
		{
			const int errorCode = WSAGetLastError();
			delete m_pDaemonListener;
			m_pDaemonListener = nullptr;
			throw std::exception(std::string("Failed to bind socket. Error code: " + std::to_string(errorCode)).c_str());
		}

		// Get the daemon listener port if port is randomized.
		if (daemonPort == 0)
		{
			struct sockaddr_in sin;
			int addrlen = sizeof(sin);
			if (getsockname(m_pDaemonListener->m_SocketHandle, (struct sockaddr *)&sin, &addrlen) == 0 &&
				sin.sin_family == AF_INET &&
				addrlen == sizeof(sin))
			{
				daemonPort = ntohs(sin.sin_port);
			}
			else
			{
				const int errorCode = WSAGetLastError();
				throw std::exception(std::string("Failed to retrive daemon port. Error code: " + std::to_string(errorCode)).c_str());
			}
		}
		m_pDaemonListener->m_DaemonPort = daemonPort;

		// Listen
		if ((error = listen(m_pDaemonListener->m_SocketHandle, SOMAXCONN)) != 0)
		{
			const int errorCode = WSAGetLastError();
			delete m_pDaemonListener;
			m_pDaemonListener = nullptr;
			throw std::exception(std::string("Failed to listen on port. Error code: " + std::to_string(errorCode)).c_str());
		}

		// Start thread
		m_pDaemonListener->m_Thread = std::thread([this]()
		{
			SocketHandle * pAcceptHandle = nullptr;
			m_pDaemonListener->m_Started = true;

			while (m_pDaemonListener->m_Started())
			{
				// Create a new accept handle if needed.
				if (pAcceptHandle == nullptr)
				{
					pAcceptHandle = new SocketHandle;
				}

				// Accept incoming connection.
				if ((*pAcceptHandle = accept(m_pDaemonListener->m_SocketHandle, NULL, NULL)) == INVALID_SOCKET)
				{
					const int error = WSAGetLastError();

					// Socket has been deleted, is it supposed to?
					if (error == WSAENOTSOCK)
					{
						delete pAcceptHandle;
						return;
					}
					// Peer reseted connection.
					if (error == WSAECONNRESET)
					{
						continue;
					}

					// Undefined error.

					/*
					LOG ERROR HERE PLEASE
					*/
					continue;
				}

				// Handle daemon connection.
				// Find daemon PID
				DWORD daemonPid = Process::GetProcessIdFromSocket(*pAcceptHandle);
				if (daemonPid == 0)
				{
					closesocket(*pAcceptHandle);
					continue;
				}

				// Check if daemon is spawned by server.
				{
					std::lock_guard<std::mutex> spawnLock(m_SpawnedProcesses.Mutex);

					auto spawnIt = m_SpawnedProcesses.Value.find(daemonPid);
					if (spawnIt != m_SpawnedProcesses.Value.end())
					{
						std::lock_guard<std::mutex> idleLock(m_IdleProcesses.Mutex);

						Process * pProcess = spawnIt->second;
						m_SpawnedProcesses.Value.erase(spawnIt);
						m_IdleProcesses.Value.push(pProcess);

						// Reset socket for next loop
						pAcceptHandle = nullptr;
						continue;
					}

				}

				// Daemon is not spawned by server, add it to idle queue.
				// System spawned daemons are mainly used for debugging purposes.
				// The user can run the daemon process via a debugger, but still attach it to the server.
				// Great, isn't it?!
				Process * pProcess = nullptr;
				try
				{
					pProcess = new Process(pAcceptHandle, static_cast<unsigned int>(daemonPid), this);
				}
				catch (const std::exception e)
				{
					std::cout << e.what() << std::endl;
					closesocket(*pAcceptHandle);
					continue;
				}

				// Add process to idle queue.
				{
					std::lock_guard<std::mutex> idleLock(m_IdleProcesses.Mutex);
					m_IdleProcesses.Value.push(pProcess);
				}

				// Reset socket for next loop
				pAcceptHandle = nullptr;
			}

			if (pAcceptHandle)
			{
				delete pAcceptHandle;
			}

		});
	}

	Process * ServerImp::GetIdleProcess()
	{
		Process * pProcess = nullptr;

		{
			std::lock_guard<std::mutex> idleLock(m_IdleProcesses.Mutex);
			
			if (m_IdleProcesses.Value.size())
			{
				pProcess = m_IdleProcesses.Value.front();
				m_IdleProcesses.Value.pop();
				
				{
					std::lock_guard<std::mutex> workingLock(m_WorkingProcesses.Mutex);

					m_WorkingProcesses.Value.push(pProcess);
				}

			}
		}

		if (pProcess != nullptr)
		{
			return pProcess;
		}

		// Spawn new process, do some attemps, if failing, send internal server error.

		return nullptr;
	}

	void ServerImp::NotifyProcessTermination(Process * p_pProcess)
	{

	}

	void ServerImp::SendInternalError(SocketHandle * p_pSockethandle)
	{
		const std::string error = "error!";
		send(*p_pSockethandle, error.c_str(), error.size(), 0);
	}

	ServerImp::DaemonListener::DaemonListener() :
		m_Started(false),
		m_SocketHandle(0),
		m_DaemonPort(0)
	{
	}

	ServerImp::DaemonListener::~DaemonListener()
	{
		if (m_SocketHandle)
		{
			closesocket(m_SocketHandle);
		}
		if (m_Started())
		{
			m_Started = false;
			m_Thread.join();
		}
	}

	
	// Process implementation
	Process::Process(SocketHandle * p_pSocketHandle, unsigned int p_ProcessId, ServerImp * p_pServerImp) :
		m_pSocketHandle(p_pSocketHandle),
		m_ProcessId(p_ProcessId),
		m_pServerImp(p_pServerImp),
		m_State(Idle)
	{
		m_ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_ProcessId);
		if (m_ProcessHandle == NULL)
		{
			throw std::exception(std::string("Failed to get process handle of pid " + std::to_string(m_ProcessId)).c_str());
		}

		// Run thread to check if process terminate.
		m_Thread = std::thread([this]()
		{
			DWORD status = 0;
			while (m_ClosingThread() == false)
			{
				WaitForSingleObject(m_ProcessHandle, INFINITE);

				// Notify server about termination. 
				m_pServerImp->NotifyProcessTermination(this);
			}
		});
	}

	Process::~Process()
	{
		m_ClosingThread = true;
		TerminateProcess(m_ProcessHandle, 0);
		CloseHandle(m_ProcessHandle);
		closesocket(*m_pSocketHandle);
		m_Thread.join();
		delete m_pSocketHandle;
	}

	Process::eState Process::GetState() const
	{
		return m_State;
	}

	void Process::SetState(const eState & p_State)
	{
		m_State = p_State;
	}

	SocketHandle * Process::GetSocketHandle()
	{
		return m_pSocketHandle;
	}

	unsigned int Process::GetProcessId() const
	{
		return m_ProcessId;
	}

	unsigned int Process::GetProcessIdFromSocket(const SocketHandle & p_SocketHandle)
	{
		unsigned int pid = 0;

		sockaddr_in ServerAddr = { 0 };
		int ServerAddrSize = sizeof(ServerAddr);

		sockaddr_in ClientAddr = { 0 };
		int ClientAddrSize = sizeof(ClientAddr);

		if ((getsockname(p_SocketHandle, (sockaddr*)&ServerAddr, &ServerAddrSize) == 0) &&
			(getpeername(p_SocketHandle, (sockaddr*)&ClientAddr, &ClientAddrSize) == 0))
		{
			PMIB_TCPTABLE2 TcpTable = NULL;
			ULONG TcpTableSize = 0;
			ULONG result;

			do
			{
				result = GetTcpTable2(TcpTable, &TcpTableSize, TRUE);
				if (result != ERROR_INSUFFICIENT_BUFFER)
					break;

				LocalFree(TcpTable);
				TcpTable = (PMIB_TCPTABLE2)LocalAlloc(LMEM_FIXED, TcpTableSize);
			} while (TcpTable != NULL);

			if (result == NO_ERROR)
			{
				for (DWORD dw = 0; dw < TcpTable->dwNumEntries; ++dw)
				{
					PMIB_TCPROW2 row = &(TcpTable->table[dw]);

					if ((row->dwState == MIB_TCP_STATE_ESTAB) &&
						(row->dwLocalAddr == ClientAddr.sin_addr.s_addr) &&
						((row->dwLocalPort & 0xFFFF) == ClientAddr.sin_port) &&
						(row->dwRemoteAddr == ServerAddr.sin_addr.s_addr) &&
						((row->dwRemotePort & 0xFFFF) == ServerAddr.sin_port))
					{
						pid = static_cast<unsigned int>(row->dwOwningPid);
						break;
					}
				}
			}

			LocalFree(TcpTable);
		}

		return pid;
	}

	void Process::Handshake()
	{

	}


	// Listener implementation
	Listener::Listener(ServerImp * p_pServerImp) :
		m_pServerImp(p_pServerImp),
		m_Created(false),
		m_Started(false),
		m_Port(0),
		m_SocketHandle(0)
	{
		if (m_pServerImp == nullptr)
		{
			throw std::exception("m_pServerImp is nullptr.");
		}
	}

	Listener::~Listener()
	{
		if (m_SocketHandle)
		{
			closesocket(m_SocketHandle);
		}
		if (m_Started())
		{
			m_Started = false;
			m_Thread.join();
		}
	}

	void Listener::Create(const unsigned short p_Port)
	{
		int error = 0;

		if (m_Created())
		{
			throw std::exception("Listener already created.");
		}

		// Create socket
		if ((m_SocketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
		{
			throw std::exception(std::string("Failed to create socket. Error code: " + std::to_string(WSAGetLastError())).c_str());
		}

		// Bind
		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = htonl(0);
		service.sin_port = htons(static_cast<USHORT>(p_Port));

		if ((error = bind(m_SocketHandle, reinterpret_cast<const sockaddr *>(&service), sizeof(service))) != 0)
		{
			const int errorCode = WSAGetLastError();
			closesocket(m_SocketHandle);
			throw std::exception(std::string("Failed to bind socket. Error code: " + std::to_string(errorCode)).c_str());
		}

		// Listen
		if ((error = listen(m_SocketHandle, SOMAXCONN)) != 0)
		{
			const int errorCode = WSAGetLastError();
			closesocket(m_SocketHandle);
			throw std::exception(std::string("Failed to listen on port. Error code: " + std::to_string(errorCode)).c_str());
		}
	}

	void Listener::Start()
	{
		m_Started = true;

		m_Thread = std::thread([this]()
		{
			SocketHandle * pAcceptHandle = nullptr;

			while (m_Started())
			{
				// Create a new accept handle if needed.
				if (pAcceptHandle == nullptr)
				{
					pAcceptHandle = new SocketHandle;
				}

				// Accept incoming connection.
				if ((*pAcceptHandle = accept(m_SocketHandle, NULL, NULL)) == INVALID_SOCKET)
				{
					const int error = WSAGetLastError();

					// Socket has been deleted, is it supposed to?
					if (error == WSAENOTSOCK)
					{
						delete pAcceptHandle;
						return;
					}
					// Peer reseted connection.
					if (error == WSAECONNRESET)
					{
						continue;
					}

					// Undefined error.

					/*
					 LOG ERROR HERE PLEASE
					*/
					continue;
				}

				HandleConnection(pAcceptHandle);
				
				// Reset socket for next loop
				pAcceptHandle = nullptr;
			}

			if (pAcceptHandle)
			{
				delete pAcceptHandle;
			}
		});
	}

	void Listener::HandleConnection(SocketHandle * p_pSocketHandle)
	{
		std::thread * pThread = new std::thread([this, &p_pSocketHandle]()
		{
			Process * pProcess = m_pServerImp->GetIdleProcess();
			if (pProcess == nullptr)
			{
				m_pServerImp->SendInternalError(p_pSocketHandle);
				closesocket(*p_pSocketHandle);
				delete p_pSocketHandle;
				return;
			}

			pProcess->Handshake();
		});

		{
			std::lock_guard<std::mutex> lock(m_ConnectionThreads.Mutex);
			m_ConnectionThreads.Value.insert(pThread);
		}
	}


	/******************************************************
	* Public implementations
	*
	******************************************************/

	// Public server settings implementations
	Server::Settings::Settings()
	{
		
	}

	bool Server::Settings::LoadFromFile(const std::string & p_Filename)
	{
		std::ifstream file(p_Filename.c_str());
		if(file.is_open() == false)
		{
			return false;
		}

		// Read each line
		std::string line = "";
		int loops = 1;
		while (std::getline(file, line))
		{
			if (line.size() == 0)
			{
				continue;
			}

			auto start = line.find_first_not_of(' ');
			auto end = line.find_last_not_of(' ');
			if (start != std::string::npos && end != std::string::npos)
			{
				line = line.substr(start, (end - start) + 1);
			}
			else
			{
				continue;
			}

			if (line.size() == 0)
			{
				continue;
			}

			start = line.find_first_not_of('\t');
			end = line.find_last_not_of('\t');
			if (start != std::string::npos && end != std::string::npos)
			{
				line = line.substr(start, (end - start) + 1);
			}
			else
			{
				continue;
			}

			if (line.size() == 0)
			{
				continue;
			}

			// Get first word
			auto wordend = line.find_first_of(' ');
			if (wordend == std::string::npos)
			{
				throw std::exception(std::string("Setting \"" + line + "\" missing value.").c_str());
			}
			std::string setting = line.substr(0, wordend);

			if (setting == "listen")
			{

			}
			else if (setting == "daemon")
			{

			}
			else if (setting == "daemonport")
			{

			}
			else
			{
				throw std::exception(std::string("Unknown setting: \"" + setting + "\".").c_str());
			}
		}

		file.close();
		return true;
	}

	bool Server::Settings::LoadFromFile(std::initializer_list<std::string> p_Filenames)
	{
		bool status = false;

		for (auto it = p_Filenames.begin(); it != p_Filenames.end(); it++)
		{
			if (LoadFromFile(*it) == true)
			{
				return true;
			}
		}

		return false;
	}


	// Public server implementations
	Server::Server() :
		m_pImp(reinterpret_cast<void*>(new ServerImp))
	{
	}

	Server::~Server()
	{
		Stop();

		if (m_pImp)
		{
			delete m_pImp;
		}
	}

	void Server::Start(Settings & p_Settings)
	{
		auto pImp = SERVER_IMP;
		pImp->m_Settings = p_Settings;

		// Initialize Windows sockets 2.2.
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			throw std::exception("Failed to initialize winsock.");
		}

		pImp->m_Started = true;

		try
		{
			// Create listener sockets
			pImp->CreateListeners();
			pImp->StartDaemonListener();
			pImp->Listen();
		}
		catch (...)
		{
			pImp->m_Started = false;
			throw;
		}

		// Set new settings.
		p_Settings.DaemonPort = pImp->m_pDaemonListener->m_DaemonPort;
	}

	void Server::Stop()
	{
		SERVER_IMP->Stop();
		SERVER_IMP->m_StopSemaphore.Notify();
	}

	void Server::Finish()
	{
		// Wait until closed.
		SERVER_IMP->m_StopSemaphore.Wait();
	}

	Server::Server(const Server & p_Server)
	{
		throw std::exception("Cannot not use copy constructor.");
	}

	Server & Server::operator = (const Server & p_Server)
	{
		throw std::exception("Cannot not use assign operator.");
		return *this;
	}

}