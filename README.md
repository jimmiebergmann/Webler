# Webler

Easy to use c++11 web server framework with core features such as routing and json, making Webler perfect for RESTful API servers.

## Requirements
 - C++ 11 compiler
 - No additional dependencies 

## Example
```sh
#include <Webler.hpp>
#include <iostream>

class Server : public Webler::Server
{

public:

	virtual void Host()
	{
		Listen(80);
	}

	virtual void RequestError(Webler::Request & req, Webler::Response & resp)
	{
		resp << "An error occured: " << resp.GetCode();
	}

	virtual void Route(Webler::Router & p_Router)
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
			const std::string & name = req.GetRouteParameter("name");
			resp << "Created customer \"" << name << "\"";
			const std::string & loves = req.GetHeaderField("loves");
			if (loves.length())
			{
				resp << ", who loves " << loves << ".";
			}
		});
	}
	
};

WeblerStart(Server);
```

## License
```sh
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
```
