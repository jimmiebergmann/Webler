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

#include <Router.hpp>
#include <Webler.hpp>
#include <map>
#include <set>
#include <sstream>

#define ROUTER_IMP reinterpret_cast<Webler::RouterImp*>(this->m_pImp)
static const std::string g_MethodStrings[2] = {"Get", "Post"};

namespace Webler
{

	// Static declarations
	Router::Route Router::InvalidRoute;


	// Router node struct
	struct RouterNode
	{
		enum eType
		{
			File,
			Directory
		};

		RouterNode(const eType p_Type) :
			Type(p_Type),
			IsWildcard(false)
		{

		}

		std::map<std::string, RouterNode *>	Childs;
		eType								Type;
		bool								IsWildcard;
	};


	// Router implementation
	class RouterImp
	{

	public:

		enum eMethod
		{
			Get,
			Post
		};

		RouterImp() :
			m_StartNode(RouterNode::Directory)
		{

		}

		~RouterImp()
		{

		}

		Router::Route & DoRouting(const eMethod p_Method, const std::string & p_Path, Router::CallbackFunction p_Callback)
		{
			std::stringstream pathStream(p_Path);
			std::string segment;
			std::vector<std::string> seglist;
			std::set<std::string> wildcards;

			while (std::getline(pathStream, segment, '/'))
			{
				// Is the last part empty?
				if (segment.size() == 0 && pathStream.eof() == false)
				{
					seglist.push_back("/");
					continue;
				}

				if (segment.size())
				{
					// Handle wildcard
					auto wildcardStart = segment.find('{');
					if (wildcardStart != std::string::npos)
					{
						if (wildcardStart != 0)
						{
							WEBLER_LOG(Log::Error, "Invalid characters before wildcard start. Method: " << g_MethodStrings[p_Method] << ". Path: " << p_Path);
							throw std::runtime_error("Wildcard not starting after \"/\".");
						}

						auto wildcardEnd = segment.find('}');
						if (wildcardStart == std::string::npos)
						{
							WEBLER_LOG(Log::Error, "Wildcard is not ending. Method: " << g_MethodStrings[p_Method] << ". Path: " << p_Path);
							throw std::runtime_error("Wildcard is not ending.");
						}
						if (wildcardEnd != segment.size() - 1)
						{
							WEBLER_LOG(Log::Error, "Invalid characters after wildcard end. Method: " << g_MethodStrings[p_Method] << ". Path: " << p_Path);
							throw std::runtime_error("Invalid characters after wildcard end.");
						}
						const std::string wildcard = segment.substr(1, segment.size() - 2);
						if (wildcards.find(wildcard) != wildcards.end())
						{
							WEBLER_LOG(Log::Error, "Wildcard name \"" << wildcard << "\" is routed multiple times. Method: " << g_MethodStrings[p_Method] << ". Path: " << p_Path);
							throw std::runtime_error("Wildcard name \"" + wildcard + "\" is routed multiple times");
						}
						wildcards.insert(wildcard);

						seglist.push_back(wildcard);

						int a = 0;
					}
					else
					{
						seglist.push_back(segment);
					}

					// Do we have more to load?
					if (pathStream.eof() == false)
					{
						seglist.push_back("/");
						continue;
					}
					
					// End of file? Done?
					break;
				}
			}


			return Router::InvalidRoute;
		}

		RouterNode m_StartNode;

	};


	// Public router class
	Router::Route & Router::Route::MaxExecutionTime(const unsigned int p_Seconds)
	{
		return *this;
	}

	Router::Route & Router::Get(const std::string & p_Route, CallbackFunction p_Callback)
	{
		return ROUTER_IMP->DoRouting(RouterImp::Get, p_Route, p_Callback);
	}

	Router::Route & Router::Post(const std::string & p_Route, CallbackFunction p_Callback)
	{
		return ROUTER_IMP->DoRouting(RouterImp::Post, p_Route, p_Callback);
	}

	Router::Route & Router::Find(const std::string & p_Path, std::vector<std::string> & p_Wildcards) const
	{
		return Router::InvalidRoute;
	}

	Router::Router() :
		m_pImp(reinterpret_cast<void *>(new RouterImp))
	{

	}

	Router::~Router()
	{
		if (m_pImp)
		{
			delete m_pImp;
		}
	}

}