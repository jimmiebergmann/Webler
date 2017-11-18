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


#define WEBLER_LOG(type, message)\
	Webler::Log::Mutex.lock();\
	*Webler::Log::GetCurrentLog() << static_cast<Webler::Log::eType>(type);\
	*Webler::Log::GetCurrentLog() << message;\
	Webler::Log::GetCurrentLog()->Flush();\
	Webler::Log::Mutex.unlock();

#include <mutex>
#include <string>

/**
* \breif Webler namespace scope.
*
*/
namespace Webler
{

	/**
	* \breif Log class.
	*
	*/
	class Log
	{

	public:

		// Friend classes
		friend class ServerImp;
		friend class Daemon;

		enum eType
		{
			Info,
			Warning,
			Error
		};

		/**
		* \breif Default constructor
		*
		*/
		Log();

		/**
		* \breif Destructor
		*
		*/
		virtual ~Log();

		/**
		* \breif Check if log is open.
		*		 Overloading and implementation is required for custom logs.
		*
		*/
		virtual bool IsOpen();

		/**
		* \breif Flush log
		*
		*/
		virtual void Flush();

		/**
		* \breif Logging operators
		*
		*/
		virtual Log & operator << (const eType & p_Type);

		virtual Log & operator << (const std::string & p_String);

		virtual Log & operator << (const int p_Integer);

		virtual Log & operator << (const unsigned int p_Integer);

		virtual Log & operator << (const float p_Float);

		
		static Log * GetCurrentLog();

		static std::mutex Mutex;

	private:

		Log(const std::string & p_Filename);

		class Implementation
		{

		public:

			void SetCurrentLog(Log * p_pLog);

		};

		Implementation m_Imp;

	};

}
