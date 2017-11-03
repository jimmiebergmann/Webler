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


/*#include <Router.hpp>
#include <Response.hpp>
#include <Request.hpp>
#include <Utility\Semaphore.hpp>
#include <Utility\ThreadValue.hpp>
#include <Connector.hpp>
#include <Http.hpp>
#include <exception>
#include <thread>
#include <set>
#include <type_traits>*/
//#include <signal.h>
/*#include <csignal>
#include <winsock2.h>
*/
/**
* \breif Forward declarations.
*
*/
/*int main(int argc, char ** argv);
void WeblerSignalHandler(int signal);
*/

/**
* \breif Webler namespace scope.
*
*/
/*namespace Webler
{*/
	/**
	 * \breif Web server main class.
	 *
	 */
/*	class Server
	{

	public:
*/

		/**
		* \breif Default constructor
		*
		*/
//		Server();

		/**
		* \breif Destructor
		*
		*/
//		virtual ~Server();

		/**
		* \breif Application defined function.
		*		 Called once by the Webler framework at server startup.
		*
		*/
//		virtual void Host() = 0;

		/**
		* \breif Application defined function.
		*		 Called once by the Webler framework at daemon start.
		*
		* \param p_Router Request router.
		*
		*/
//		virtual void Route(Router & p_Router) = 0;

		/**
		* \breif Application defined function.
		*		 Called once by the Webler framework at server startup.
		*
		* \param p_Router Request router.
		*
		*/
//		virtual void RequestError(Request & p_Request, Response & p_Response);

		/**
		* \breif Start listening on given port.
		*
		* \param[in]	p_Port	Port to listening on.
		*
		* \see Mute
		*
		*/
//		void Listen(const unsigned short p_Port);

		/**
		* \breif Stop listening on given port.
		*
		* \param[in]	p_Port	Port to stop listening on.
		*
		* \see Listen
		*
		*/
//		void Mute(const unsigned short p_Port);

		/**
		* \breif Stop the Webler server. The application will terminate.
		*
		*/
//		int Stop();

		/**
		* \breif Friend classes and functions
		*
		*/
/*		friend class Connector;
		friend class Router;							//< Friend router class.
		friend int ::main(int argc, char ** argv);		//< Friend main function in order to call Start from main.
		friend void ::WeblerSignalHandler(int signal);	//< Friend signlar handler function.
		template <typename T> friend static int Boot(int argc, char ** argv, Server * p_pWeblerClass);

	private:
		*/
		// Http class
	/*	class HttpParser
		{

		public:

			HttpParser();

			Http::eCode AppendData(const char * p_pData, const unsigned int p_DataSize);

		private:

			enum eState
			{
				ParseRequestLine,
				ParseHeader,
				ParseBody
			};

			eState m_State;

		};

		enum eType
		{
			HostType,
			DaemonType
		};

		/**
		* \breif Functions executed by main(argc, argv), starts the Webler server.
		*
		*/
/*		template <typename T> 
		static int Boot(int argc, char ** argv, Server * p_pWeblerClass);

		int Start(int argc, char ** argv);
		int RunDaemon(int argc, char ** argv);
		const std::string & GetProgramPath() const;


		// Private typedefs
		typedef std::set<Connector *> ConnectorSet;

		// Private variables
		eType						m_Type;
		ConnectorSet				m_Connectors;
		Utility::Semaphore			m_ExitSemaphore;
		Utility::ThreadValue<bool>	m_Started;
		std::string					m_ProgramPath;*/

	/*};

	#include <Webler.inl>

}
*/

#include <Server.hpp>
#include <Daemon.hpp>
#include <Shared.hpp>

/**
* \breif	This macro must be called ONCE in the application
*			and will start the server.
*
*/

#define WeblerStart(WeblerServer, WeblerDaemon, WeblerShared)\
	\
	int main(int argc, char ** argv)\
	{\
		WeblerShared * pPrivate_weblerSharedClass = new WeblerShared;\
		if(argc == 3 && strcmp("-daemon", argv[1]) == 0)\
		{\
			WeblerDaemon * pPrivate_weblerDaemonClass = new WeblerDaemon;\
			return pPrivate_weblerDaemonClass->Boot(argc, argv, pPrivate_weblerSharedClass);\
		}\
		else\
		{\
			WeblerServer * pPrivate_weblerServerClass = new WeblerServer;\
			return pPrivate_weblerServerClass->Boot(argc, argv, pPrivate_weblerSharedClass);\
		}\
		\
		return 0;\
	}



/*

std::signal(SIGABRT, private_WeblerSignalHandler);\
std::signal(SIGTERM, private_WeblerSignalHandler);\
std::signal(SIGBREAK, private_WeblerSignalHandler);\

*/
/*#define WeblerStart(WeblerServerClass)\
	static WeblerServerClass weblerServerClass;\
	\
	void WeblerSignalHandler(int signal)\
	{\
		weblerServerClass.Stop();\
	}\
	\
	int main(int argc, char ** argv)\
	{\
		if(std::is_base_of<Webler::Server, WeblerServerClass>::value == false)\
		{\
			throw std::string("Webler::Server is not base class of application class.");\
		}\
		\
		WSADATA wsaData;\
		if (WSAStartup(MAKEWORD(2, 2), &wsaData))\
		{\
			throw std::string("Failed to initialize winsock."); \
		}\
		\
		signal(SIGABRT, WeblerSignalHandler);\
		signal(SIGTERM, WeblerSignalHandler);\
		signal(SIGBREAK, WeblerSignalHandler);\
		return weblerServerClass.Start(argc, argv);\
	}*/
