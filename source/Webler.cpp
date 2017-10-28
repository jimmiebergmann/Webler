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
#include <chrono>
#include <iostream>
#include <Windows.h>


namespace Webler
{

	Server::Server()
	{
	}

	Server::~Server()
	{
	}

	void Server::Listen(const unsigned short p_Port)
	{

	}

	void Server::Mute(const unsigned short p_Port)
	{

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
		// Call user defined setup function.
		Router router(this);
		Setup(router);

		// Flag the server as running and wait for it to exit.
		m_Started.Set(true);
		m_ExitSemaphore.Wait();
		return 0;
	}

}
