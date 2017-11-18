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

#include <Utility/Time.hpp>
#include <sstream>
#include <Windows.h>

namespace Webler
{

	namespace Utility
	{


		Time Time::Now()
		{
			SYSTEMTIME st;
			GetLocalTime(&st);

			Time time;
			time.m_Year = st.wYear;
			time.m_Month = st.wMonth;
			time.m_Day = st.wDay;
			time.m_Hour = st.wHour;
			time.m_Minute = st.wMinute;
			time.m_Second = st.wSecond;
			time.m_Milliseconds = st.wMilliseconds;

			return time;
		}

		std::string Time::GetTimeString()
		{
			std::stringstream ss;

			// Date
			ss << m_Year << '-';
			if (m_Month < 10)
			{
				ss << '0';
			}
			ss << m_Month << '-';
			if (m_Day < 10)
			{
				ss << '0';
			}
			ss << m_Day << ' ';

			// Time
			if (m_Hour < 10)
			{
				ss << '0';
			}
			ss << m_Hour << ':';
			if (m_Minute < 10)
			{
				ss << '0';
			}
			ss << m_Minute << ':';
			if (m_Second < 10)
			{
				ss << '0';
			}
			ss << m_Second << " ";
			
			// Milliseconds
			unsigned int milliseconds = m_Milliseconds;
			int count = 0;
			while (milliseconds >= 1)
			{
				milliseconds /= 10;
				count++;
			}
			count = (3 - count);
			ss << std::string(count, '0') << m_Milliseconds;
		
			return ss.str();
		}

		// Private
		Time::Time()
		{
		}
		
	}

}