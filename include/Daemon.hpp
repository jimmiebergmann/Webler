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

#include <Router.hpp>


// Forward declarations
int main(int argc, char ** argv);


/**
* \breif Webler namespace scope.
*
*/
namespace Webler
{

	class Shared; //< Shared forward declaration.


	/**
	* \breif Daemon class.
	*
	*/
	class Daemon
	{

	public:

		/**
		* \breif Default constructor
		*
		*/
		Daemon();

		/**
		* \breif Destructor
		*
		*/
		virtual ~Daemon();

		/**
		* \breif Application defined function.
		*		 Called once by the Webler framework at daemon start.
		*
		* \param p_Router Request router.
		*
		*/
		virtual void Start(Webler::Router & p_Router) = 0;

		/**
		* \breif Handle function for request errors.
		*		 Overrides the RequestError function in Webler::Shared if defined.
		*
		*/
		virtual void RequestError(Request & p_Request, Response & p_Response);

		// Friend main function to let WeblerStart call Boot function.
		friend int ::main(int argc, char ** argv);

	private:

		/**
		* \breif Bootup function for WeblerStart macro.
		*
		*/
		int Boot(int p_ArgumentCount, char ** p_ArgumentValues, Shared * p_pShared);

		void * m_pImp; //< Implementation

	};

}
