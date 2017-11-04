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
#include <Private/ServerImp.hpp>
#include <Utility/Semaphore.hpp>
#include <csignal>
#include <exception>
#include <WinSock2.h>

// Static pointer to server class.
static Webler::Server * g_pServerInstance = nullptr;
static void SignalHandlerFunction(int signal);


namespace Webler
{

	// Public server class
	Server::Server() :
		m_pImp(reinterpret_cast<void *>(new Private::ServerImp()))
	{

	}

	Server::~Server()
	{
		if (m_pImp)
		{
			delete m_pImp;
		}
	}

	void Server::RequestError(Request & p_Request, Response & p_Response)
	{
		p_Response << "Error - " << p_Response.GetCode();
	}

	int Server::Boot(int p_ArgumentCount, char ** p_ppArgumentValues, Shared * p_pShared)
	{
		// Set program path.
		SERVER_IMP->ProgramPath = p_ppArgumentValues[0];

		// Add current server instance.
		if (g_pServerInstance != nullptr)
		{
			throw new std::runtime_error("Server is already booted. Only one instance of Server is allowed.");
		}
		g_pServerInstance = this;

		// Initialize Windows sockets 2.2.
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			throw std::string("Failed to initialize winsock.");
		}

		// Bind signals
		std::signal(SIGABRT, SignalHandlerFunction);
		std::signal(SIGTERM, SignalHandlerFunction);
		std::signal(SIGBREAK, SignalHandlerFunction);

		// Run user definied Start function.
		Start(*(SERVER_IMP->pListener = new Listener(this)));

		// Wait for Stop implementation call.
		SERVER_IMP->ExitSemaphore.Wait();
		return 0;
	}

}


// Static implementations
static void SignalHandlerFunction(int signal)
{
	// Signals are bound in Boot function.

	if (g_pServerInstance == nullptr)
	{
		return;
	}
	SERVER_IMP_FROM(g_pServerInstance)->Stop();
	g_pServerInstance = nullptr;
}