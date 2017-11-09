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
#include <list>
#include <sstream>
#include <algorithm>

#define ROUTER_IMP reinterpret_cast<Webler::RouterImp*>(this->m_pImp)
#define ROUTE_IMP reinterpret_cast<Webler::RouteImp*>(this->m_pImp)
#define ROUTE_IMP_FROM(Route) reinterpret_cast<Webler::RouteImp*>(Route->m_pImp)

namespace Webler
{

	// Route implementation
	class RouteImp
	{

	public:

		Router::CallbackFunction Callback;
		unsigned int MaxExecutionTime;

	};


	// Router node struct
	struct RouterNode
	{
		RouterNode(	RouterNode * p_pParent = nullptr) :
			pParent(p_pParent),
			pWildcard(nullptr),
			pRoute(nullptr)
		{
		}

		~RouterNode()
		{
			if (pWildcard)
			{
				delete pWildcard;
			}
		}

		std::map<std::string, RouterNode *>	Childs;
		RouterNode *						pParent;
		std::string *						pWildcard;
		Router::Route *						pRoute;
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

		RouterImp()
		{

		}

		~RouterImp()
		{
			// Clear up all m_RouterNodes...
		}

		Router::Route & Add(const std::string & p_Method, const std::string & p_Path, Router::CallbackFunction p_Callback)
		{
			struct PathPart
			{
				PathPart(	const std::string & p_Part,
							const bool p_IsWildcard = false) :
					Part(new std::string(p_Part)),
					IsWildcard(p_IsWildcard)
				{
				}

				~PathPart()
				{
					delete Part;
				}

				std::string * Part;
				bool		IsWildcard;

			};

			Router::Route * pRoute = nullptr;

			// Parse path
			std::stringstream pathStream(p_Path);
			std::string pathPart;
			std::list<std::unique_ptr<PathPart>> partList;
			std::set<std::string> wildcards;

			while (std::getline(pathStream, pathPart, '/'))
			{
				// Is the current part empty?
				if (pathPart.size() == 0 && pathStream.eof() == false)
				{
					partList.push_back(std::unique_ptr<PathPart>( new PathPart("/")));
					continue;
				}

				if (pathPart.size())
				{
					// Handle wildcard
					auto wildcardStart = pathPart.find('{');
					if (wildcardStart != std::string::npos)
					{
						if (wildcardStart != 0)
						{
							WEBLER_LOG(Log::Error, "Invalid characters before wildcard start. Method: " << p_Method << ". Path: " << p_Path);
							throw std::runtime_error("Wildcard not starting after \"/\".");
						}

						auto wildcardEnd = pathPart.find('}');
						if (wildcardStart == std::string::npos)
						{
							WEBLER_LOG(Log::Error, "Wildcard is not ending. Method: " << p_Method << ". Path: " << p_Path);
							throw std::runtime_error("Wildcard is not ending.");
						}
						if (wildcardEnd != pathPart.size() - 1)
						{
							WEBLER_LOG(Log::Error, "Invalid characters after wildcard end. Method: " << p_Method << ". Path: " << p_Path);
							throw std::runtime_error("Invalid characters after wildcard end.");
						}
						const std::string wildcard = pathPart.substr(1, pathPart.size() - 2);
						if (wildcards.find(wildcard) != wildcards.end())
						{
							WEBLER_LOG(Log::Error, "Wildcard name \"" << wildcard << "\" is routed multiple times. Method: " << p_Method << ". Path: " << p_Path);
							throw std::runtime_error("Wildcard name \"" + wildcard + "\" is routed multiple times");
						}
						wildcards.insert(wildcard);

						partList.push_back(std::unique_ptr<PathPart>(new PathPart(wildcard, true)));

						int a = 0;
					}
					else
					{
						partList.push_back(std::unique_ptr<PathPart>(new PathPart(pathPart)));
					}

					// Do we have more to load?
					if (pathStream.eof() == false)
					{
						partList.push_back(std::unique_ptr<PathPart>(new PathPart("/")));
						continue;
					}
					
					// End of file? Done?
					break;
				}
			}


			// Check if method exists
			std::string method = p_Method;
			std::transform(method.begin(), method.end(), method.begin(), ::tolower);
			auto routerNodeIt = m_RouterNodes.find(method);
			RouterNode * pCurrentRouterNode = nullptr;
			if (routerNodeIt == m_RouterNodes.end())
			{
				pCurrentRouterNode = new RouterNode;
				m_RouterNodes[method] = pCurrentRouterNode;
			}
			else
			{
				pCurrentRouterNode = routerNodeIt->second;
			}

			// Go through the segment list and create routing.
			
			if (partList.size() == 0)
			{
				// Is the full path already routed?
				if (pCurrentRouterNode->pRoute)
				{
					WEBLER_LOG(Log::Error, "Path is already routed: Method: " << p_Method << ". Path: " << p_Path);
					throw std::runtime_error("Path is already routed.");
				}

				pRoute = new Router::Route;
				ROUTE_IMP_FROM(pRoute)->MaxExecutionTime = 30;
				ROUTE_IMP_FROM(pRoute)->Callback = p_Callback;
				pCurrentRouterNode->pRoute = pRoute;
			}

			for (auto it = partList.begin(); it != partList.end(); it++)
			{
				const std::string & part = *(*it)->Part;

				// Child already exists?
				auto childIt = pCurrentRouterNode->Childs.find(part);
				if (childIt != pCurrentRouterNode->Childs.end())
				{
					auto itCopy = it;
					if ((++itCopy) == partList.end())
					{
						WEBLER_LOG(Log::Error, "Path is already routed. Method: " << p_Method << ". Path: " << p_Path);
						throw std::runtime_error("Path is already routed.");
					}

					pCurrentRouterNode = childIt->second;
					continue;
				}

				// Is wildcard?
				if ((*it)->IsWildcard)
				{
					// Wildcard already exists for this node?
					if (pCurrentRouterNode->pWildcard)
					{
						if (*pCurrentRouterNode->pWildcard != part)
						{
							WEBLER_LOG(Log::Error, "Wilcard name collision in route: \"" << part << "\". Method: " << p_Method << ". Path: " << p_Path);
							throw std::runtime_error("Wilcard name collision in route : \"" + part);
						}
					}
					else
					{
						pCurrentRouterNode->pWildcard = new std::string(part);
					}
				}
				else
				{
					// New child.
					RouterNode * pNewRouterNode = new RouterNode(pCurrentRouterNode);
					pCurrentRouterNode->Childs[part] = pNewRouterNode;
					pCurrentRouterNode = pNewRouterNode;
				}
				
				// Last part in list?
				auto itCopy = it;
				if ((++itCopy) == partList.end())
				{
					// Is the full path already routed?
					if (pCurrentRouterNode->pRoute)
					{
						WEBLER_LOG(Log::Error, "Path is already routed: Method: " << p_Method << ". Path: " << p_Path);
						throw std::runtime_error("Path is already routed.");
					}

					pRoute = new Router::Route;
					ROUTE_IMP_FROM(pRoute)->MaxExecutionTime = 30;
					ROUTE_IMP_FROM(pRoute)->Callback = p_Callback;
					pCurrentRouterNode->pRoute = pRoute;
				}
			}

			// Is route nullptr for some reason?
			if (pRoute == nullptr)
			{
				WEBLER_LOG(Log::Error, "Failed to each end of route because of unknown reason: Method: " << p_Method << ". Path: " << p_Path);
				throw std::runtime_error("Failed to each end of route because of unknown reason.");
			}

			return *pRoute;
		}

		Router::Route & Find(const std::string & p_Path, std::vector<std::string> & p_Wildcards)
		{
			p_Wildcards.clear();

			// Split path in parts.
			std::stringstream pathStream(p_Path);
			std::string pathPart;
			std::list<std::string> partList;

			while (std::getline(pathStream, pathPart, '/'))
			{
				// Is the current part empty?
				if (pathPart.size() == 0 && pathStream.eof() == false)
				{
					partList.push_back("/");
					continue;
				}
				if (pathPart.size())
				{
					partList.push_back(pathPart);
					if (pathStream.eof() == false)
					{
						partList.push_back("/");
					}
				}
			}

			return m_InvalidRoute;
		}

		// Variables
		typedef std::map<std::string, RouterNode*> RouterNodeMap;
		RouterNodeMap m_RouterNodes;
		Router::Route m_InvalidRoute;

	};


	// Public route class
	Router::Route & Router::Route::MaxExecutionTime(const unsigned int p_Seconds)
	{
		ROUTE_IMP->MaxExecutionTime = p_Seconds;
		return *this;
	}

	Router::Route::Route() :
		m_pImp(reinterpret_cast<void *>(new RouteImp))
	{

	}

	Router::Route::Route(const Route & p_Route)
	{
		throw std::runtime_error("Not allowed to copy route class.");
	}

	Router::Route::~Route()
	{
		if (m_pImp)
		{
			delete m_pImp;
		}
	}


	// Public router class
	Router::Route & Router::Add(const std::string & p_Method, const std::string & p_Path, CallbackFunction p_Callback)
	{
		return ROUTER_IMP->Add(p_Method, p_Path, p_Callback);
	}

	Router::Route & Router::Find(const std::string & p_Path, std::vector<std::string> & p_Wildcards) const
	{
		return ROUTER_IMP->Find(p_Path, p_Wildcards);
	}

	Router::Route & Router::InvalidRoute() const
	{
		return ROUTER_IMP->m_InvalidRoute;
	}

	Router::Router() :
		m_pImp(reinterpret_cast<void *>(new RouterImp))
	{
	}

	Router::Router(const Router & p_Router)
	{
		throw std::runtime_error("Not allowed to copy router class.");
	}

	Router::~Router()
	{
		if (m_pImp)
		{
			delete m_pImp;
		}
	}

}