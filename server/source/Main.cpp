#include <Server.hpp>
#include <exception>
#include <iostream>
#include <csignal>
#include <fstream>
#include <set>
#include <cstdlib>

// Command line settings
static std::string g_ProgramPath = "";
static std::string g_ProgramDir = "";
static std::string g_ProgramName = "";
static std::string g_SettingsFile = "webler.conf";
static std::string g_DaemonFile = "daemon.exe";
static std::set<unsigned short> g_ListenPorts;
static unsigned short g_DaemonListenPort = 0;


static bool FileExists(const std::string & p_Filename)
{
	std::ifstream fin(p_Filename.c_str());
	bool status = fin.is_open();
	fin.close();
	return status;
}

static bool StringToUnsignedShort(const std::string & p_PortString, unsigned short & p_Value)
{
	if (p_PortString.size() > 5)
	{
		return false;
	}

	for (int i = 0; i < p_PortString.size(); i++)
	{
		if (p_PortString[i] < '0' || p_PortString[i] > '9')
		{
			return false;
		}
	}

	int number = std::atoi(p_PortString.c_str());
	if (number <= 0 || number > 65535)
	{
		return false;
	}

	p_Value = number;
	return true;
}


static bool ParseCommandLine(int argc, char ** argv)
{
	if (argc == 0)
	{
		std::cout << "Missing executable path in command line." << std::endl;
		return false;
	}


	// Parse program path/dir/name
	g_ProgramPath = argv[0];
	const size_t programDirPosisions[2] = { g_ProgramPath.find_last_of('/'), g_ProgramPath.find_last_of('\\') };
	if (programDirPosisions[0] == std::string::npos && programDirPosisions[1] == std::string::npos)
	{
		std::cout << "Missing executable directory." << std::endl;
		return false;
	}
	const size_t programDirPos = programDirPosisions[0] != std::string::npos ? programDirPosisions[0] : programDirPosisions[1];
	g_ProgramDir = g_ProgramPath.substr(0, programDirPos);
	g_ProgramName = g_ProgramPath.substr(programDirPos + 1, g_ProgramName.size() - programDirPos);

	// Loop through the command line arguments
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "-settings") == 0)
		{
			if (i + 1 >= argc)
			{
				std::cout << "Failed to parse command \"" << argv[i] << "\": Missing argument." << std::endl;
				return false;
			}

			g_SettingsFile = argv[i + 1];
			i++;
		}
		else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "-daemon") == 0)
		{
			if (i + 1 >= argc)
			{
				std::cout << "Failed to parse command \"" << argv[i] << "\": Missing argument." << std::endl;
				return false;
			}

			g_DaemonFile = argv[i + 1];
			i++;
		}
		else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "-listen") == 0)
		{
			if (i + 1 >= argc)
			{
				std::cout << "Failed to parse command \"" << argv[i] << "\": Missing argument(s)." << std::endl;
				return false;
			}

			int loops = 1;
			// Loop until finding an argument without starting with a "-"
			for (int j = i + 1; j < argc; j++, i++)
			{
				if (argv[j][0] == '-')
				{
					break;
				}

				unsigned short port = 0;
				if (StringToUnsignedShort(argv[i + 1], port) == false)
				{
					std::cout << "Failed to parse command \"" << argv[i] << "\": Argument no " << loops << " is not a valid port number." << std::endl;
					return false;
				}

				g_ListenPorts.insert(port);
				loops++;
			}
		}
		else if (strcmp(argv[i], "-D") == 0 || strcmp(argv[i], "-daemonport") == 0)
		{
			if (i + 1 >= argc)
			{
				std::cout << "Failed to parse command \"" << argv[i] << "\": Missing argument." << std::endl;
				return false;
			}

			if (StringToUnsignedShort(argv[i + 1], g_DaemonListenPort) == false)
			{
				std::cout << "Invalid given daemon listen port." << std::endl;
				return false;
			}

			i++;
		}
		else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-help") == 0)
		{
			std::cout << "Webler server." << std::endl << std::endl;
			std::cout << "Options:" << std::endl << std::endl;
			std::cout << "    -h -help            Display option list." << std::endl;
			std::cout << "    -s -settings        Specify option file path." << std::endl;
			std::cout << "    -d -daemon          Specify daemon executable file." << std::endl;
			std::cout << "    -l -listen          Specify peer listen port(s)." << std::endl;
			std::cout << "    -D -daemonport      Specify daemon listen port." << std::endl;
			return false;
		}
		else
		{
			std::cout << "Failed to parse command \"" << argv[i] << "\": Unknwon command." << std::endl;
			std::cout << "Use command -help for command lind." << std::endl;
			return false;
		}
	}

	return true;
}

int main(int argc, char ** argv)
{
	
	// Memory leak detection on windows.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	// parse command line.
	if (ParseCommandLine(argc, argv) == false)
	{
		return 0;
	}

	// Load server settings.
	Webler::Server::Settings settings;
	try
	{
		settings.LoadFromFile({ g_SettingsFile, g_ProgramDir + "/" + g_SettingsFile});
	}
	catch (const std::exception & e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		return -1;
	}

	// Start server
	Webler::Server server;
	try
	{
		server.Start(settings);
	}
	catch (const std::exception & e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		return -1;
	}

	std::cout << "Started Webler server." << std::endl;
	std::cout << "Configurations:" << std::endl;
	std::cout << "--------------------------" << std::endl;
	std::cout << "Listener ports: ";
	auto listenerPorts = settings.ListenPorts;
	size_t portLoop = 0;
	for (auto it = listenerPorts.begin(); it != listenerPorts.end(); it++)
	{
		portLoop++;
		std::cout << *it;
		if (portLoop < listenerPorts.size())
		{
			std::cout << ", ";
		}
	}
	std::cout << std::endl;
	std::cout << "Daemon listener port: " << settings.DaemonPort << std::endl;
	std::cout << "--------------------------" << std::endl;

	server.Finish();
	return 0;
}