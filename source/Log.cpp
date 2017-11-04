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
#include <fstream>
#include <chrono>

static std::fstream g_File;
static std::string g_TypeStrings[3] =
	{"Info", "Warning", "Error"};

namespace Webler
{

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

	Log & Log::operator << (const eType & p_Type)
	{
		if (g_File.is_open() == false)
		{
			return *this;
		}

		g_File.flush();

		g_File << "\n";
		g_File << g_TypeStrings[p_Type] << ": ";


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

	Log::Log(const std::string & p_Filename)
	{
		g_File.open(p_Filename.c_str(), std::fstream::app);
		
		if (g_File.is_open())
		{
			// Get log file size
			g_File.seekp(0, std::fstream::end);
			int fileSize = g_File.tellg();
			g_File.seekp(0, std::fstream::beg);

			if (fileSize != 0)
			{
				g_File << "\n";
			}

			g_File << "Started log at ...\n";
			g_File.flush();
		}
	}

}