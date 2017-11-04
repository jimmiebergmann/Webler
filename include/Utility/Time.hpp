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

namespace Webler
{

	namespace Utility
	{

		class Time
		{

		public:

			/**
			* \breif Get current time.
			*
			*/
			static Time Now();

			/**
			* \breif Get time as string, in format: YYYY-MM-DD:HH:MM:SS:vvvv
			*
			*/
			std::string GetTimeString();

		private:

			Time();

			unsigned int	m_Year;
			unsigned int	m_Month;
			unsigned int	m_Day;
			unsigned int	m_Hour;
			unsigned int	m_Minute;
			unsigned int	m_Second;
			unsigned int	m_Milliseconds;

		};

	}

}