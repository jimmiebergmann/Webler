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
#include <functional>

/**
* \breif Webler namespace scope.
*
*/
namespace Webler
{

	class Server;

	class Router
	{

	public:

		class Route
		{

		public:

			/**
			* \breif Set maximum execution time of route.
			*		 Server will terminate daemon process if MaxExecutionTime of Server exceeds.
			*		 Default: 30
			*
			* \param p_Seconds Execution time in seconds. 0 = infinite of time.
			*
			*/
			Route & MaxExecutionTime(const unsigned int p_Seconds);

		};


		friend class Server;

		typedef std::function<void(Request &, Response &)> CallbackFunction;

		/**
		* \breif Route GET request
		*
		* \param[in]	p_Route	Route to given callback function.
		*						Specify wildcard arguments in curly brackets.
		*						Example: /customer/{name}
		* \param[in]	p_Callback Callback function for request.
		*
		*/
		Router::Route & Get(const std::string & p_Route, CallbackFunction p_Callback);

		/**
		* \breif Route POST request
		*
		* \param[in]	p_Route	Route to given callback function.
		*						Specify wildcard arguments in curly brackets.
		*						Example: /customer/{name}
		* \param[in]	p_Callback Callback function for request.
		*
		*/
		Router::Route & Post(const std::string & p_Route, CallbackFunction p_Callback);

	private:

		Router(Server * p_Server);
		~Router();

	};

}