#include <Request.hpp>

#define REQUEST_IMP reinterpret_cast<Webler::RequestImp*>(this->m_pImp)

static const std::string g_EmptyString = "";

namespace Webler
{

	// Request implementation
	class RequestImp
	{

	public:

		RequestImp()
		{

		}

		std::map<std::string, std::string> Wildcards;
		std::map<std::string, std::string> HeaderFields;

	};

	// Public request class
	void Request::SetWildcard(const std::string p_Name, const std::string p_Value)
	{
		REQUEST_IMP->Wildcards[p_Name] = p_Value;
	}

	const std::string & Request::GetWildcard(const std::string & p_Name) const
	{
		auto it = REQUEST_IMP->Wildcards.find(p_Name);
		if (it != REQUEST_IMP->Wildcards.end())
		{
			return it->second;
		}

		return g_EmptyString;
	}

	const std::string & Request::GetHeaderField(const std::string & p_Field) const
	{
		auto it = REQUEST_IMP->HeaderFields.find(p_Field);
		if (it != REQUEST_IMP->HeaderFields.end())
		{
			return it->second;
		}

		return g_EmptyString;
	}

	Request::Request()
	{
	}

	Request::Request(const Request & p_Request) :
		m_pImp(reinterpret_cast<void *>(new RequestImp))
	{
	}

	Request::~Request()
	{
		if (m_pImp)
		{
			delete m_pImp;
		}
	}

}