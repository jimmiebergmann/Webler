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

#include <Socket/Handle.hpp>
#include <Utility\ThreadValue.hpp>
#include <string>

namespace Webler
{

	
	class Daemon
	{

	public:

		enum eType
		{
			HostType,
			DaemonType
		};

		Daemon(const eType p_Type);

		~Daemon();

		bool Create(const std::string & p_Program, Socket::Handle & p_SocketHandle);

		bool Load(int p_Argc, char ** p_ppArgv);

		bool Join();

		Socket::Handle & GetSocketHandle();

	private:

		static unsigned int GetUniqueIdentifier();

		eType										m_Type;
		Socket::Handle								m_SocketHandle;
		static Utility::ThreadValue<unsigned int>	m_Identifier;

	};

}