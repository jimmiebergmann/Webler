#include <Request.hpp>

namespace Webler
{

	Request::Request()
	{

	}

	Request::~Request()
	{

	}

	void Request::SetRouteParameter(const std::string p_Parameter, const std::string p_Value)
	{
		m_RouteParamters[p_Parameter] = p_Value;
	}

	const std::string & Request::GetRouteParameter(const std::string p_Parameter) const
	{
		auto it = m_RouteParamters.find(p_Parameter);
		if (it != m_RouteParamters.end())
		{
			return it->second;
		}

		return "";
	}

}