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

#include <Address.hpp>
#include <set>
#include <string>
#include <initializer_list>


/**
* \breif Webler namespace scope.
*
*/
namespace Webler
{

	/**
	* \breif Server class.
	*
	*/
	class Server
	{

	public:

		/**
		* \breif Server settings class.
		*
		*/
		class Settings
		{

		public:

			// Friend classes
			friend class Server;

			// Typedefs
			typedef std::set<unsigned short> Ports;

			/**
			* \breif Constructor
			*
			*/
			Settings(const std::initializer_list<unsigned short> & p_ListenPorts = {},
				const std::string & p_DaemonProcess = "Daemon.exe",
				const unsigned short p_DaemonPort = 0);

			/**
			* \breif Destructor
			*
			*/
			~Settings();

			/**
			* \breif Listen on given port.
			*
			*/
			void Listen(const unsigned short p_Port);

			/**
			* \breif Do not listen on given port
			*
			*/
			void Mute(const unsigned short p_Port);

			/**
			* \breif Get set of listenting ports.
			*
			*/
			const Ports & ListeningPorts() const;

			/**
			* \breif Get daemon listener port
			*
			*/
			void SetDaemonPort(const unsigned short p_Port);

			/**
			* \breif Get daemon listener port
			*
			*/
			unsigned short GetDaemonPort() const;

			/**
			* \breif Set path for daemon executable.
			*
			*/
			void SetDaemonProcess(const std::string & p_DaemonPath);

			/**
			* \breif Get path for daemon executable.
			*
			*/
			const std::string & GetDaemonProcess() const;

		private:

			void * m_pImp; ///< Implementation

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
		~Server();

		/**
		* \breif Start the server.
		*
		*/
		void Start(Settings & p_Settings);

		/**
		* \breif Stop the server.
		*
		*/
		void Stop();

		/**
		* \breif Wait for server to stop.
		*
		*/
		void Finish();

	private:

		/**
		* \breif Copy constructor
		*
		*/
		Server(const Server & p_Server);

		/**
		* \breif Assign operator
		*
		*/
		Server & operator = (const Server & p_Server);

		void * m_pImp;

	};

}