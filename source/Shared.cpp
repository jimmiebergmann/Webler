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

#include <Shared.hpp>
#include <string>
#include <Windows.h>

#define IMP reinterpret_cast<SharedImp*>(this->m_pImp)

static bool g_GlobalInitialized = false;
static std::string g_ProgramDirectory = "";
static std::string g_ProgramPath = "";

namespace Webler
{

	// Shared implementation
	// ...


	// public shared class
	Shared::Shared()
	{
		if (g_GlobalInitialized == false)
		{
			// Get program path
			int pathLength = FILENAME_MAX;
			char path[FILENAME_MAX + 1];

			const int finalPathLength = GetModuleFileName(NULL, path, pathLength);
			if (finalPathLength == 0)
			{
				throw new std::runtime_error("Failed to get executable path.");
			}
			path[finalPathLength] = 0;

			// Get program directory
			g_ProgramPath = path;
			int dirPos = -1;
			for (int i = g_ProgramPath.size() - 1; i >= 0; i--)
			{
				if (g_ProgramPath[i] == '/' || g_ProgramPath[i] == '\\')
				{
					dirPos = i;
					break;
				}
			}
			if (dirPos < 0)
			{
				throw new std::runtime_error("Failed to get executable directory.");
				return;
			}
			g_ProgramDirectory = g_ProgramPath.substr(0, dirPos);

			g_GlobalInitialized = true;
		}
	}

	Shared::~Shared()
	{
	}

	void Shared::RequestError(Request & p_Request, Response & p_Response)
	{
		p_Response << "Error - " << p_Response.GetCode();
	}

	const std::string & Shared::GetProgramPath()
	{
		return g_ProgramPath;
	}

	const std::string & Shared::GetProgramDirectory()
	{
		return g_ProgramDirectory;
	}

}