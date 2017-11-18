/*#include <Webler.hpp>
using namespace Webler;

class MyServer : public Server
{

	virtual void Start(Server::Settings & settings)
	{
		settings.Listen(80).Listen(88).MaxExecutionTime(30);
	}

	virtual void Route(Router & p_Router)
	{
		p_Router.Add("GET", "/Customer/jimmie/animals", [](Request & req, Response & resp)
		{
			if (req.GetWildcard("name") == "jimmie")
			{
				resp << "Jimmie loves cats!";
				return;
			}
			resp << "Unkown name, try \"jimmie\".";
		}).MaxExecutionTime(10).Incasesensitive();

		p_Router.Add("GET", "/Customer/{name}/animals", [](Request & req, Response & resp)
		{
			if (req.GetWildcard("name") == "jimmie")
			{
				resp << "Jimmie loves cars!";
				return;
			}
			resp << "Unkown name, try \"jimmie\".";
		}).MaxExecutionTime(20);
	}

	virtual void RequestError(Request & req, Response & resp)
	{
		resp << "An error occured: " << resp.GetCode();
	}

};


WeblerStart(MyServer)
*/


int main()
{

	return 0;
}