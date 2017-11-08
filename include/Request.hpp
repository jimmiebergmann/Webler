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

#include <string>
#include <map>

namespace Webler
{

	class Daemon; //< Forward declaration

	class Request
	{

	public:

		void SetRouteParameter(const std::string p_Parameter, const std::string p_Value);
		const std::string & GetWildcard(const std::string & p_Name = "") const;
		const std::string & GetHeaderField(const std::string & p_Field) const;

	private:

		friend class Daemon;

		/**
		* \breif Default constructor
		*
		*/
		Request();

		/**
		* \breif Copy constructor
		*
		*/
		Request(const Request & p_Request);

		/**
		* \breif Initialization constructor
		*
		*/
		Request(Daemon * p_pDaemon);

		/**
		* \breif Destructor
		*
		*/
		~Request();



		std::map<std::string, std::string> m_RouteWildcards;
		std::map<std::string, std::string> m_HeaderFields;

	};

}