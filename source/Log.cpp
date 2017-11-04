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

#include <Log.hpp>
#include <Utility/Time.hpp>
#include <fstream>
#include <chrono>


// Static variables
static Webler::Log * g_pCurrentLog = nullptr;
static std::fstream g_File;
static std::string g_TypeStrings[3] =
	{"Info:    ", "Warning: ", "Error:   "};


namespace Webler
{

	// Static declarations of Log class.
	std::mutex Log::Mutex;


	// Log class.
	Log::Log()
	{

	}

	Log::~Log()
	{
		if (g_File.is_open())
		{
			g_File.close();
		}
	}

	bool Log::IsOpen()
	{
		return g_File.is_open();
	}

	void Log::Flush()
	{
		g_File.flush();
	}

	Log * Log::GetCurrentLog()
	{
		return g_pCurrentLog;
	}

	Log & Log::operator << (const eType & p_Type)
	{
		if (g_File.is_open() == false)
		{
			return *this;
		}

		g_File.flush();

		g_File << "\n";
		g_File << Utility::Time::Now().GetTimeString() << " - " << g_TypeStrings[p_Type];

		return *this;
	}

	Log & Log::operator << (const std::string & p_String)
	{
		if (g_File.is_open() == false)
		{
			return *this;
		}

		g_File << p_String;
		
		return *this;
	}

	Log & Log::operator << (const int p_Integer)
	{
		if (g_File.is_open() == false)
		{
			return *this;
		}

		g_File << p_Integer;

		return *this;
	}

	Log & Log::operator << (const unsigned int p_Integer)
	{
		if (g_File.is_open() == false)
		{
			return *this;
		}

		g_File << p_Integer;

		return *this;
	}

	Log & Log::operator << (const float p_Float)
	{
		if (g_File.is_open() == false)
		{
			return *this;
		}

		g_File << p_Float;

		return *this;
	}

	Log * GetCurrentLog()
	{
		return g_pCurrentLog;
	}

	Log::Log(const std::string & p_Filename)
	{
		// Check if log file exists.
		bool newLog = false;
		std::ifstream readLog(p_Filename);
		if (readLog.is_open() == false)
		{
			newLog = true;
		}
		else
		{
			readLog.close();
		}

		// Open log
		g_File.open(p_Filename.c_str(), std::fstream::app);
		
		if (g_File.is_open() && newLog)
		{
			g_File << "Webler log created at YYYY-MM-DD:HH:MM:SS.\n";
			g_File << "---------------------------------------------------------\n";
			g_File.flush();
		}
	}

	void Log::Implementation::SetCurrentLog(Log * p_pLog)
	{
		g_pCurrentLog = p_pLog;
	}

}