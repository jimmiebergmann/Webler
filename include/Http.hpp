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

namespace Webler
{

	namespace Http
	{

		enum eCode
		{
			NoCode = 0,

			// Informational codes
			Continue = 100, ///< The client should send the remainder of the request
			SwitchingProtocols = 101, ///< Server will change protocols

			// Successful codes
			Ok = 200,	///< The request has succeeded.
			Created = 201,	///< A new resource has been created.
			Accepted = 202,	///< The request has been accepted, but not compleded yet.
			NonAuthoritative = 203,	///< The content is a copy from a third party server.
			NoContent = 204,	///< Fulfilled request, but no body was sent.
			ResetContent = 205,	///< Fulfilled request, but used agent should reset the document view.
			PartialContent = 206,	///< The server has fulfilled the partial GET request.

			// Redirection codes
			MultipleChoices = 300,	///< ?
			MovedPermanently = 301,	///< Requested resource has been assigned a new parmanent URI.
			Found = 302,	///< Requested resource resides temporarily under a different URI.
			SeeOther = 303,	///< Response to the request can be found under a different URI.
			NotModified = 304,	///< "Access" is allowed, and the requested document(GET) has not been modified.
			UseProxy = 305,	///< Requested resource must be accessed through the given proxy("Location" field).
			Unused = 306,	///< Removed status code from previous specification version.
			TemporaryRedirect = 307,	///< Requested resource resides temporarily under a different URI.

			// Client error codes
			BadRequest = 400,	///< The request could not be understood by the server because of syntax errors.
			Unauthorized = 401,	///< The request requires user authentication.
			PaymentRequired = 402,	///< Reserved for future use.
			Forbidden = 403,	///< The server understood the request, but refused it.
			NotFound = 404,	///< The server could not find anything mathing the request-URI.
			MethodNotAllowed = 405,	///< The method specified in the "Request-Line" is not allowed.
			NotAcceptable = 406,	///< ?
			ProxyAuthentication = 407,	///< (Almost like 401) Client must authenticate itself with the proxy.
			RequestTimeout = 408,	///< Client did not send a request fast enough.
			Conflict = 409,	///< Request could not be completed due to a conflict with the current statue of the resource.
			Gone = 410,	///< Requested resource is no longer available.
			LengthRequired = 411,	///< Server requires the "Content-Length" field, it's missing in the request.
			PreconditionFailed = 412,	///< Server did not receive a response from it's upstream fast enough.
			RequestEntityTooLarge = 413,	///<
			RequestUriTooLong = 414,	///< The requested URI was too long.
			UnsupportedMediaType = 415,	///<
			RangeNotSatisfiable = 416,	///<
			ExpectationFailed = 417,	///<

			// Server error codes
			InternalServerError = 500,	///< The server could not fulfill the request.
			NotImplemented = 501,	///< The server does not support the functionality required to fulfill the request.
			BadGateway = 502,	///< The server received an invalid response from upstream server.
			ServiceUnavailable = 503,	///< The server is current unable to handle the request due to overloading or maintenance.
			GatewayTimeout = 504,	///< The server timeout from it's upstream.
			VersionNotSupported = 505	///< The server does not support the specified http protocol version.
		};

	}
}