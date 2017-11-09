#include <Request.hpp>

namespace Webler
{

	static const std::string g_EmptyString = "";

	void Request::SetRouteParameter(const std::string p_Parameter, const std::string p_Value)
	{
		m_RouteWildcards[p_Parameter] = p_Value;
	}

	const std::string & Request::GetWildcard(const std::string & p_Name) const
	{
		auto it = m_RouteWildcards.find(p_Name);
		if (it != m_RouteWildcards.end())
		{
			return it->second;
		}

		return g_EmptyString;
	}

	const std::string & Request::GetHeaderField(const std::string & p_Field) const
	{
		auto it = m_HeaderFields.find(p_Field);
		if (it != m_HeaderFields.end())
		{
			return it->second;
		}

		return g_EmptyString;
	}

	Request::Request()
	{
	}

	Request::Request(const Request & p_Request)
	{
	}

	Request::Request(Daemon * p_pDaemon)
	{
	}

	Request::~Request()
	{
	}

}