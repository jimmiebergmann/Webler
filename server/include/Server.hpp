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

			/**
			* \breif Constructor
			*
			*/
			Settings();

			/**
			* \breif Load from file.
			*
			* \param p_Filenames First try the first filename, if invalid, go on with next one.
			*
			* \throw std::exception on error.
			*
			* \return false if filename is invalid, else true.
			*
			*/
			bool LoadFromFile(const std::string & p_Filename);
			bool LoadFromFile(std::initializer_list<std::string> p_Filenames);

			std::set<unsigned short>	ListenPorts;
			unsigned short				DaemonPort;
			std::string					DaemonExecutable;

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