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

#include <Listener.hpp>
#include <Request.hpp>
#include <Response.hpp>


// Forward declarations
int main(int argc, char ** argv);
void SignalHandlerFunction(int signal);

/**
* \breif Webler namespace scope.
*
*/
namespace Webler
{

	class Shared; //< Shared forward declaration.


	/**
	* \breif Web server main class.
	*
	*/
	class Server
	{

	public:
	
		/**
		* \breif Default constructor
		*
		*/
		Server();

		/**
		* \breif Destructor
		*
		*/
		virtual ~Server();

		/**
		* \breif Application defined function.
		*		 Called once by the Webler framework at server startup.
		*
		* \param p_Listener Listener handler used for listening for incoming connections.
		*
		* \see Listener
		*
		*/
		virtual void Start(Listener & p_Listener) = 0;

		/**
		* \breif Handle function for request errors.
		*		 Overrides the RequestError function in Webler::Shared if defined.
		*
		*/
		virtual void RequestError(Request & p_Request, Response & p_Response);

		// Friend functions and classes
		friend class Listener;
		friend int ::main(int argc, char ** argv);			//< Friend main function to let WeblerStart call Boot function.
		friend void ::SignalHandlerFunction(int signal);	//< Friend signal handler function.

	private:

		/**
		* \breif Bootup function for WeblerStart macro.
		*
		*/
		int Boot(int p_ArgumentCount, char ** p_ppArgumentValues, Shared * p_pShared);

		void * m_pImp; //< Implementation

	};

}
