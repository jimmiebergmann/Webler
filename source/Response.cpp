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

#include <Response.hpp>

namespace Webler
{
	static char g_TempBuffer32[32];



	Http::eCode Response::GetCode() const
	{
		return m_Code;
	}

	void Response::SetCode(const Http::eCode p_Code)
	{
		m_Code = p_Code;
	}

	Response & Response::operator << (const std::string & p_String)
	{
		m_Response += p_String;
		return  *this;
	}

	Response & Response::operator << (const int & p_Integer)
	{
		_itoa_s(p_Integer, g_TempBuffer32, 10);
		m_Response += g_TempBuffer32;
		return  *this;
	}


	Response::Response()
	{
	}

	Response::Response(const Response & p_Response)
	{
	}

	Response::Response(Server * p_Server)
	{
	}

	Response::~Response()
	{
	}

}