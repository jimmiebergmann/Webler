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

#include <set>
#include <string>
#include <initializer_list>


/**
* \breif Webler namespace scope.
*
*/
namespace Webler
{

	/**
	* \breif Server class.
	*
	*/
	class Address
	{

	public:

		enum eType
		{
			Ipv4,
			Ipv6
		};


		Address(const eType p_Type = Ipv4);
		Address(const Address & p_Address);
		Address(const unsigned int p_Address);
		Address(const unsigned char * p_pSourceBytes, const unsigned int p_ByteCount);
		Address(const std::initializer_list<unsigned char> & p_Bytes);

		Address & GetByName(const std::string & p_Name);
		const unsigned char GetType() const;
		unsigned char GetByte(const unsigned int p_ByteIndex) const;
		void GetBytes(unsigned char * p_DestinationBytes, const unsigned int p_ByteCount) const;
		void SetByte(const unsigned char p_Byte, const unsigned int p_ByteIndex);
		void SetBytes(const unsigned char * p_SourceBytes, const unsigned int p_ByteCount);

		static const Address & LocalHostIpv4;

	private:

		eType			m_Type;
		unsigned char	m_Bytes[16];

	};

}