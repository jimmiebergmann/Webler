#include <Webler.hpp>
#include <iostream>

class Server : public Webler::Server
{

public:

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

		Listen(80);
	}
	
};

WeblerStart(Server);



