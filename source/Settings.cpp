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
#include <Listener.hpp>
#include <Server.hpp>
#include <Private/ServerImp.hpp>

#define IMP reinterpret_cast<ListenerImp*>(this->m_pImp)
#define SERVER_IMP reinterpret_cast<ListenerImp*>(this->m_pImp)

namespace Webler
{

	// Listener implementation
	class ListenerImp
	{

	public:

		ListenerImp(Server * p_pServer) :
			pServer(p_pServer)
		{

		}

		// Functions

		// Variables
		Server * pServer;

	};


	// Public listener class
	Listener::~Listener()
	{
		delete m_pImp;
	}

	bool Listener::Listen(const unsigned short p_Port)
	{
		return SERVER_IMP_FROM(IMP->pServer)->Listen(p_Port);
	}

	Listener::Listener()
	{

	}
	
	Listener::Listener(const Listener & p_Listener)
	{

	}
	
	Listener::Listener(Server * p_Server) :
		m_pImp(reinterpret_cast<void *>(new ListenerImp(p_Server)))
	{

	}

}*/