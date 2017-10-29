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

#include <Daemon.hpp>
#include <iostream>
#include <sstream>

namespace Webler
{

	Utility::ThreadValue<unsigned int>	Daemon::m_Identifier(0);

	Daemon::Daemon(const eType p_Type) :
		m_Type(p_Type)
	{

	}

	Daemon::~Daemon()
	{

	}

	bool Daemon::Create(const std::string & p_Program, Socket::Handle & p_SocketHandle)
	{
		if (m_Type != HostType)
		{
			return false;
		}
		
		BOOL ret = 0;
		HANDLE hPipe;
		DWORD dwBytes;
		PROCESS_INFORMATION piProc;
		STARTUPINFO siStartInfo;
		WSAPROTOCOL_INFO protInfo;
		OVERLAPPED ol = {0,0,0,0,NULL};


		DWORD processId = GetCurrentProcessId();
		std::stringstream ss;
		ss << processId << "_" << GetUniqueIdentifier();
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
			return false;
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
			return false;
		}

		std::cout << "Created daemon: " << piProc.dwProcessId << std::endl;

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
		if (ret == 0) {
			switch (GetLastError())
			{

			case ERROR_PIPE_CONNECTED:
				std::cout << "ERROR_PIPE_CONNECTED" << std::endl;
				ret = TRUE;
				break;

			case ERROR_IO_PENDING:
				std::cout << "ERROR_IO_PENDING" << std::endl;
				if (WaitForSingleObject(ol.hEvent, PIPE_TIMEOUT_CONNECT) == WAIT_OBJECT_0)
				{
					std::cout << "WAit.. " << PIPE_TIMEOUT_CONNECT << std::endl;
					DWORD dwIgnore;
					ret = GetOverlappedResult(hPipe, &ol, &dwIgnore, FALSE);
				}
				else
				{
					std::cout << "Cancel IO" << std::endl;
					CancelIo(hPipe);
				}
				break;
			}
		}

		CloseHandle(ol.hEvent);

		if (ret == 0)
		{
			std::cout << "Failed to connect named pipe. : " << pipeName << " - " << GetLastError() << std::endl;
			return false;
		}


		// I write the socket descriptor to the named pipe 
		if (WriteFile(hPipe, &p_SocketHandle, sizeof(p_SocketHandle), &dwBytes, NULL) == 0)
		{
			std::cout << "Failed to write socket handle to shared memory. " << GetLastError() << std::endl;
			return false;
		}

		// I write the protocol information structure to the named pipe
		/*if (WriteFile(hPipe, &protInfo, sizeof(protInfo), &dwBytes, NULL) == 0)
		{
			std::cout << "Failed to write prot info to shared memory. " << GetLastError() << std::endl;
			return false;
		}*/

		CloseHandle(hPipe);


		return true;
	}

	bool Daemon::Load(int p_Argc, char ** p_ppArgv)
	{
		if (m_Type != DaemonType)
		{
			return false;
		}

		Socket::Handle socketHandle;
		DWORD dwBytes, dwMode;
		HANDLE hPipe;
		WSAPROTOCOL_INFO protInfo;
		
		std::string pipeName = p_ppArgv[2];

		// I open in read mode the namedpipe created by the parent process
		hPipe = CreateFile(
			pipeName.c_str(),   // pipe name
			GENERIC_READ |  // read and write access
			GENERIC_WRITE,
			0,              // no sharing
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe
			0,              // default attributes
			NULL);          // no template file

		if (hPipe == INVALID_HANDLE_VALUE)
		{
			if (GetLastError() != ERROR_PIPE_BUSY)
			{
				std::cout << "Daemon: Failed to create file: ERROR_PIPE_BUSY" << std::endl;
				return false;
			}

			// if all instances are busy I'll wait 10000 ms
			if (!WaitNamedPipe(pipeName.c_str(), 10000))
			{
				std::cout << "Pipe is busy: " << GetLastError() << std::endl;
				return false;
			}
		}

		// I set the pipe in read mode
		dwMode = PIPE_READMODE_BYTE;

		// I update the pipe
		SetNamedPipeHandleState(
			hPipe,
			&dwMode,  // new mode
			NULL,
			NULL);

		// I read the client socket sent me by the parent process

		ReadFile(hPipe, &socketHandle, sizeof(socketHandle), &dwBytes, NULL);
		if (socketHandle == 0)
		{
			std::cout << "Daemon: Invalid socket from host." << std::endl;
			return false;
		}


		m_SocketHandle = socketHandle;

		/*
		// I read the protocol information structure sent me by the parent process

		ReadFile(hPipe, &protInfo, sizeof(protInfo), &dwBytes, NULL);

		// I create a new socket with the structure just read

		sock_client = WSASocket(AF_INET, SOCK_STREAM, 0, &protInfo, 0, WSA_FLAG_OVERLAPPED);

		if (sock_client == INVALID_SOCKET) {

			ExitProcess(-1);

		}

		*/


		return true;
	}

	bool Daemon::Join()
	{
		if (m_Type != HostType)
		{
			return false;
		}

		return true;
	}

	Socket::Handle & Daemon::GetSocketHandle()
	{
		return m_SocketHandle;
	}

	unsigned int Daemon::GetUniqueIdentifier()
	{
		std::lock_guard<std::mutex> lock(m_Identifier.Mutex);
		m_Identifier.Value = m_Identifier.Value + 1;
		return m_Identifier.Value;
	}

}