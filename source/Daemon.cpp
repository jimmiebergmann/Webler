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
#include <Log.hpp>
#include <iostream>
#include <winsock2.h>

#define DAEMON_IMP reinterpret_cast<Webler::DaemonImp*>(this->m_pImp)


namespace Webler
{

	// Socket handle
	namespace Socket
	{
		typedef SOCKET Handle;
	}

	// Daemon implementation
	// ...
	class DaemonImp
	{

	public:

		DaemonImp()
		{

		}

		bool ConnectToHost(const std::string & p_PipeName)
		{
			Socket::Handle socketHandle;
			DWORD dwBytes, dwMode;
			HANDLE hPipe;
			WSAPROTOCOL_INFO protInfo;

			// I open in read mode the namedpipe created by the parent process
			hPipe = CreateFile(
				p_PipeName.c_str(),   // pipe name
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
					WEBLER_LOG(Log::Error, "Daemon: Failed to create file: ERROR_PIPE_BUSY");
					return false;
				}

				// if all instances are busy I'll wait 10000 ms
				if (!WaitNamedPipe(p_PipeName.c_str(), 10000))
				{
					WEBLER_LOG(Log::Error, "Pipe is busy: " << static_cast<int>(GetLastError()));
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
				WEBLER_LOG(Log::Error, "Daemon: Invalid socket from host.");
				return false;
			}
			m_SocketHandle = socketHandle;

			WEBLER_LOG(Log::Info, "New daemon running.");

			return 0;

			/*
			// I read the protocol information structure sent me by the parent process
			ReadFile(hPipe, &protInfo, sizeof(protInfo), &dwBytes, NULL);

			// I create a new socket with the structure just read
			sock_client = WSASocket(AF_INET, SOCK_STREAM, 0, &protInfo, 0, WSA_FLAG_OVERLAPPED);

			if (sock_client == INVALID_SOCKET)
			{
				ExitProcess(-1);
			}
			*/
		}

		Socket::Handle m_SocketHandle;

	};



	// Public daemon class
	Daemon::Daemon() :
		m_pImp(reinterpret_cast<void *>(new DaemonImp))
	{

	}

	Daemon::~Daemon()
	{
		if (m_pImp)
		{
			delete m_pImp;
		}
	}

	int Daemon::Boot(int p_ArgumentCount, char ** p_ppArgumentValues, Shared * p_pShared)
	{
		// Load log
		const std::string logPath = GetProgramDirectory() + "/daemons.log";
		Log * pLog = new Log(logPath);
		if (pLog->IsOpen() == false)
		{
			std::cout << "Failed to open server log file: " << logPath << std::endl;
			return 0;
		}
		pLog->m_Imp.SetCurrentLog(pLog);

		// Connec to host process
		const std::string pipeName = p_ppArgumentValues[2];
		if (DAEMON_IMP->ConnectToHost(pipeName) == false)
		{
			return 0;
		}

		Sleep(1000 * 10);

		return 0;
	}

}