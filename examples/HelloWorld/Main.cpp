#include <Webler.hpp>
#include <iostream>

class ServerImp : public Webler::Server
{

	virtual void Start(Webler::Server::Settings & p_Settings)
	{
		p_Settings.Listen(80).Listen(88).MaxExecutionTime(5);
	}

};

class DaemonImp : public Webler::Daemon
{

	virtual void Start(Webler::Router & p_Router)
	{
		p_Router.Get("/Customer/{name}", [](Webler::Request & req, Webler::Response & resp)
		{
			if (req.GetRouteParameter("name") == "jimmie")
			{
				resp << "Jimmie loves cats!";
				return;
			}
			resp << "Unkown name, try \"jimmie\".";
		});

		p_Router.Post("/Customer/{name}", [](Webler::Request & req, Webler::Response & resp)
		{
			auto name = req.GetRouteParameter("name");
			resp << "Created customer \"" << name << "\"";
			auto loves = req.GetHeaderField("loves");
			if (loves.length())
			{
				resp << ", who loves " << loves << ".";
			}
		}).MaxExecutionTime(3);
	}
	
};

class SharedImp : public Webler::Shared
{

	virtual void RequestError(Webler::Request & req, Webler::Response & resp)
	{
		resp << "An error occured: " << resp.GetCode();
	}

};

WeblerStart(ServerImp, DaemonImp, SharedImp)



