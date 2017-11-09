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

#include <Response.hpp>
#include <Request.hpp>
#include <vector>
#include <functional>

/**
* \breif Webler namespace scope.
*
*/
namespace Webler
{

	class Server;

	/**
	* \breif Router classed used for routing paths.
	*
	*/
	class Router
	{

	public:

		friend class Server;
		friend class Daemon;

		/**
		* \breif Single route class.
		*
		*/
		class Route
		{

		public:

			friend class Server;
			friend class Daemon;
			friend class Router;
			friend class RouterImp;

			/**
			* \breif Set maximum execution time of route.
			*		 Server will terminate daemon process if MaxExecutionTime of Server exceeds.
			*		 Default: 30
			*
			* \param p_Seconds Execution time in seconds. 0 = infinite of time.
			*
			*/
			Route & MaxExecutionTime(const unsigned int p_Seconds);

		private:

			/**
			* \breif Constructor.
			*
			*/
			Route();

			/**
			* \breif Copy constructor.
			*
			*/
			Route(const Route & p_Route);

			/**
			* \breif Destructor.
			*
			*/
			~Route();

			void * m_pImp;  //< Implementation
		};

		/**
		* \breif Routed callback function.
		*
		*/
		typedef std::function<void(Request &, Response &)> CallbackFunction;

		/**
		* \breif Route path
		*
		* \param[in]	p_Method	Incasesensitive method name to route.
		* \param[in]	p_Path		Route path to given callback function.
		*							Specify wildcard arguments in curly brackets.
		*							Example: /customer/{name}
		* \param[in]	p_Callback	Callback function for request.
		*
		*/
		Router::Route & Add(const std::string & p_Method, const std::string & p_Path, CallbackFunction p_Callback);

		/**
		* \breif Find matching route by given url.
		*
		*/
		Router::Route & Find(const std::string & p_Path, std::vector<std::string> & p_Wildcards) const;

		/**
		* \breif Invalid route to compare result from Get function with.
		*
		*/
		Router::Route & InvalidRoute() const;
		
	private:

		/**
		* \breif Constructor.
		*
		*/
		Router();

		/**
		* \breif Copy constructor.
		*
		*/
		Router(const Router & p_Router);

		/**
		* \breif Destructor.
		*
		*/
		~Router();

		void * m_pImp; //< Implementation

	};

}