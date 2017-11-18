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

#include <Address.hpp>
#include <exception>
#include <WinSock2.h>


/**
* \breif Webler namespace scope.
*
*/
namespace Webler
{

	const Address & Address::LocalHostIpv4 = { 127, 0, 0, 1 };

	Address::Address(const eType p_Type) :
		m_Type(p_Type)
	{
		memset(m_Bytes, 0, 16);
	}

	Address::Address(const Address & p_Address) :
		m_Type(p_Address.m_Type)
	{
		memcpy(m_Bytes, p_Address.m_Bytes, 16);
	}

	Address::Address(const unsigned int p_Address) :
		m_Type(Ipv4)
	{
		memcpy(m_Bytes, &p_Address, 4);
	}

	Address::Address(const unsigned char * p_pSourceBytes, const unsigned int p_ByteCount) :
		m_Type(Ipv6)
	{
		if (p_ByteCount > 16)
		{
			std::exception("Overflow: p_ByteCount is larger than 16.");
		}

		memcpy(m_Bytes, p_pSourceBytes, p_ByteCount);
	}

	Address::Address(const std::initializer_list<unsigned char> & p_Bytes)
	{
		size_t counter = 0;
		for (auto it = p_Bytes.begin(); it != p_Bytes.end() && counter < 16; it++, counter++)
		{
			m_Bytes[counter] = *it;
		}

	}

	Address & Address::GetByName(const std::string & p_Name)
	{
		throw std::exception("GetByName() is not yet implemented.");
		return *this;
	}

	const unsigned char Address::GetType() const
	{
		return m_Type;
	}

	unsigned char Address::GetByte(const unsigned int p_ByteIndex) const
	{
		if (p_ByteIndex > 16)
		{
			std::exception("Overflow: p_ByteIndex is larger than 16.");
		}

		return m_Bytes[p_ByteIndex];
	}

	void Address::GetBytes(unsigned char * p_DestinationBytes, const unsigned int p_ByteCount) const
	{
		if (p_ByteCount > 16)
		{
			std::exception("Overflow: p_ByteCount is larger than 16.");
		}

		memcpy(p_DestinationBytes, m_Bytes, p_ByteCount);
	}

	void Address::SetByte(const unsigned char p_Byte, const unsigned int p_ByteIndex)
	{
		if (p_ByteIndex >= 16)
		{
			std::exception("Overflow: p_ByteIndex is larger than 16.");
		}

		m_Bytes[p_ByteIndex] = p_Byte;
	}

	void Address::SetBytes(const unsigned char * p_SourceBytes, const unsigned int p_ByteCount)
	{
		if (p_ByteCount >= 16)
		{
			std::exception("Overflow: p_ByteCount is larger than 16.");
		}

		memcpy(m_Bytes, p_SourceBytes, p_ByteCount);
	}


}
