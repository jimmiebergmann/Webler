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

#include <Request.hpp>
#include <Response.hpp>
#include <Shared.hpp>
#include <Log.hpp>


// Forward declarations
int main(int argc, char ** argv);
void SignalHandlerFunction(int signal);

/**
* \breif Webler namespace scope.
*
*/
namespace Webler
{

	/**
	* \breif Web server main class.
	*
	*/
	class Server : public Shared
	{

	public:
	
		/**
		* \breif Server settings class
		*
		*/
		class Settings
		{

		public:

			friend class Server; //< Forward declaration and friendship

			/**
			* \breif Destructor
			*
			*/
			~Settings();

			/**
			* \breif Start listening on given port.
			*
			* \param[in]	p_Port	Port to listening on.
			*
			* \see Mute
			*
			*/
			Server::Settings & Listen(const unsigned short p_Port);

			/**
			* \breif Set given log class.
			*		 Default loggning will write to the server executable directory.
			*
			* \param[in]	p_pLog	Pointer to log class.
			*
			*/
			Server::Settings & CustomLog(Log * p_pLog);

			/**
			* \breif Set maximum execution time of daemon process.
			*		 Default: 30
			*
			* \param p_Seconds Execution time in seconds. 0 = infinite of time.
			*
			*/
			Server::Settings & MaxExecutionTime(const unsigned int p_Seconds);

		private:

			/**
			* \breif Default constructor
			*
			*/
			Settings();

			/**
			* \breif Copy constructor
			*
			*/
			Settings(const Settings & p_Settings);

			void * m_pImp; //< Implementation

		};


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
		virtual void Start(Settings & p_Settings) = 0;


		// Friend functions and classes
		friend class Settings;
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
