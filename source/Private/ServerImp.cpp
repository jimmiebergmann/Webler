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
/*
#include <Private/ServerImp.hpp>
#include <Utility/ThreadValue.hpp>
#include <Socket/Handle.hpp>
#include <list>
#include <thread>
#include <string>
#include <sstream>
#include <iostream>


namespace Webler
{

	namespace Private
	{

		// Global variables
		static Utility::ThreadValue<unsigned int> g_DaemonProcessCounter;



		// Daemon process
		class DaemonProcess
		{

		public:

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
						std::cout << "Failed to create named pipe. " << GetLastError() << std::endl;
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
						std::cout << "Failed to create daemon process. " << GetLastError() << std::endl;
						return;
					}

					m_ProcessHandle = piProc.hProcess;

					std::cout << "Created daemon: " << piProc.dwProcessId << std::endl;
*/					
					// I duplicate the socket
					/*ret = WSADuplicateSocket(p_SocketHandle, piProc.dwProcessId, &protInfo);
					if (ret == SOCKET_ERROR)
					{
					std::cout << "Failed to duplicate socket. " << GetLastError() << std::endl;
					return false;
					}*/
/*						
					ol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

					// I connect to the named pipe...
					ret = ConnectNamedPipe(hPipe, &ol);
					if (ret == 0)
					{
						switch (GetLastError())
						{

						case ERROR_PIPE_CONNECTED:
							std::cout << "ERROR_PIPE_CONNECTED" << std::endl;
							ret = TRUE;
							break;

						case ERROR_IO_PENDING:
							std::cout << "Waiting for daemon process connection." << std::endl;
							if (WaitForSingleObject(ol.hEvent, PIPE_TIMEOUT_CONNECT) == WAIT_OBJECT_0)
							{
								std::cout << "WAit.. " << PIPE_TIMEOUT_CONNECT << std::endl;
								DWORD dwIgnore;
								ret = GetOverlappedResult(hPipe, &ol, &dwIgnore, FALSE);
							}
							else
							{
								std::cout << "Cancel IO, no daemon process connected to pipe." << std::endl;
								CancelIo(hPipe);
							}
							break;
						}
					}

					CloseHandle(ol.hEvent);

					if (ret == 0)
					{
						std::cout << "Failed to connect named pipe. : " << pipeName << " - " << GetLastError() << std::endl;
						return;
					}


					// I write the socket descriptor to the named pipe
					if (WriteFile(hPipe, &p_SocketHandle, sizeof(p_SocketHandle), &dwBytes, NULL) == 0)
					{
						std::cout << "Failed to write socket handle to shared memory. " << GetLastError() << std::endl;
						return;
					}
						
*/

					// I write the protocol information structure to the named pipe
					/*if (WriteFile(hPipe, &protInfo, sizeof(protInfo), &dwBytes, NULL) == 0)
					{
					std::cout << "Failed to write prot info to shared memory. " << GetLastError() << std::endl;
					return false;
					}*/

/*				

					// Close pipe handle.
					CloseHandle(hPipe);

				});

				return true;
			}

		private:

			unsigned int GetNextProcessId()
			{
				std::lock_guard<std::mutex> lock(g_DaemonProcessCounter.Mutex);
				return (g_DaemonProcessCounter.Value += 1);
			}

			std::thread	m_Thread;
			Socket::Handle								m_SocketHandle;
			HANDLE										m_ProcessHandle;

		};


		// Connector class
		class Connector
		{

		public:

			Connector(ServerImp * p_pServer, unsigned short p_Port) :
				pServerImp(p_pServer),
				m_Port(p_Port),
				m_Handle(0)
			{
				// Create socket
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
					throw new std::runtime_error("Failed to listen on port " + std::to_string(m_Port) +  ". Error code: " + std::to_string(WSAGetLastError()));
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
				});
			}

			~Connector()
			{

			}

			unsigned short GetPort()
			{
				return m_Port;
			}

		private:

			typedef std::list<DaemonProcess *> DaemonDaemonProcessList;

			ServerImp *						pServerImp;
			Socket::Handle					m_Handle;
			std::thread						m_Thread;
			DaemonDaemonProcessList			m_DaemonProcesses;
			Utility::ThreadValue<bool>		m_Running;
			unsigned short					m_Port;

		};



		// Server implementation
		ServerImp::ServerImp
		:
			pListener(nullptr)
		{

		}

		ServerImp::~ServerImp()
		{
			if (pListener != nullptr)
			{
				delete pListener;
			}

		}

		void ServerImp::Stop()
		{
			ExitSemaphore.Notify();
		}

		bool ServerImp::Listen(const unsigned short p_Port)
		{
			// Check if we already are listening on this port
			for (auto it = Connectors.begin(); it != Connectors.end(); it++)
			{
				if ((*it)->GetPort() == p_Port)
				{
					return false;
				}
			}

			try
			{
				Connector * pConnector = new Connector(this, p_Port);
				Connectors.insert(pConnector);
			}
			catch(std::exception & e)
			{
				throw e;
			}

			return true;
		}

	}

}

*/