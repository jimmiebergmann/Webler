#include <Webler.hpp>
#include <iostream>

class Server : public Webler::Server
{

public:

	virtual void Host()
	{
		Listen(80);
	}

	virtual void Route(Webler::Router & p_Router)
	{
		p_Router.Get("/Customer/{name}", [](Webler::Request & req, Webler::Response & resp)
		{
			if (req.GetRouteParameter("name") == "jimmie")
			{
				resp << "Jimmie loves cats!";
			}

			resp << "Unkown name, try \"jimmie\"";
		});

		p_Router.Post("/Customer/{name}", [](Webler::Request & req, Webler::Response & resp)
		{
			const std::string & name = req.GetRouteParameter("name");
			resp << "Created customer \"" << name << "\"";
			const std::string & loves = req.GetHeaderField("loves");
			if (loves.length())
			{
				resp << ", who loves " << loves << ".";
			}
		});
	}

private:

	
};

WeblerStart(Server);



