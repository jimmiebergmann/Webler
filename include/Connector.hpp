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

#pragma once
/*
#include <Utility/Semaphore.hpp>
#include <Utility/ThreadValue.hpp>
#include <Daemon.hpp>
#include <thread>
#include <mutex>
#include <list>
#include <Socket/Handle.hpp>

namespace Webler
{

	class Server;

	class Connector
	{

	public:

		Connector(Server * p_pServer, unsigned short p_Port);

		~Connector();

		unsigned short GetPort();

	private:

		typedef std::list<Daemon *> DaemonList;

		Server *									m_pServer;
		Socket::Handle								m_Handle;
		std::thread									m_Thread;
		DaemonList									m_Daemons;
		Utility::ThreadValue<bool>					m_Running;
		unsigned short								m_Port;

	};

}*/