/******************************************************************************\
 *  _   ___     ____  __               _                                      *
 * | | | \ \___/ /  \/  | ___ __ _ ___| |_                                    *
 * | |_| |\     /| |\/| |/ __/ _` / __| __|                                   *
 * |  _  | \ - / | |  | | (_| (_| \__ \ |_                                    *
 * |_| |_|  \_/  |_|  |_|\___\__,_|___/\__|                                   *
 *                                                                            *
 * This file is part of the HAMcast project.                                  *
 *                                                                            *
 * HAMcast is free software: you can redistribute it and/or modify            *
 * it under the terms of the GNU Lesser General Public License as published   *
 * by the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                        *
 *                                                                            *
 * HAMcast is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                       *
 * See the GNU Lesser General Public License for more details.                *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with HAMcast. If not, see <http://www.gnu.org/licenses/>.            *
 *                                                                            *
 * Contact: HAMcast support <hamcast-support@informatik.haw-hamburg.de>       *
\******************************************************************************/

#include <fstream>
#include <sstream>
#include <regex>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#include "hamcast/config.hpp"
#include "hamcast/hamcast_logging.h"
#include "hamcast/ipc/middleware_configuration.hpp"

namespace {

bool fileExists(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

void create_path_if_needed(const char* path)
{
    if (!fileExists(path))
	{
        mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}
}

void create_file_if_needed(const char* filename)
{
    if (!fileExists(filename))
	{
		std::ofstream file;
		file.open(filename);
		file.close();
	}
}

} // namespace <anonymous>

namespace hamcast { namespace ipc {

middleware_configuration::middleware_configuration()
	: m_port(0), m_str_port("0"), m_pid(0)
{
}

middleware_configuration::middleware_configuration(std::uint16_t port)
	: m_port(port)
{
    m_str_port = std::to_string(port);
#	ifdef WIN_32
#	else
	m_pid = getpid();
#	endif
}

bool middleware_configuration::write_to(const char* filename)
{
	std::ostringstream string_config;
	string_config << "middleware.port = " << m_str_port << "\n";
	string_config << "middleware.pid = " << m_pid << "\n";
	std::ofstream file;
	file.open(filename);
	if (file.good())
	{
		file << string_config.str();
		file.close();
		return true;
	}
	return false;
}

bool middleware_configuration::write()
{
	std::string fname = meeting_point;
	fname += config_filename;
	return write_to(fname.c_str());
}

bool middleware_configuration::read()
{
	std::string fname = meeting_point;
	fname += config_filename;
	return read_from(fname.c_str());
}

bool middleware_configuration::read_from(const char* filename)
{
	// restore hardcodet defaults
	m_port = 0;
	m_str_port = "0";
    std::smatch sm;
	std::ifstream input;
	std::string line;
	input.open(filename);
    std::regex port_regex("^middleware\\.port = ([1-9][0-9]*)$",
                          std::regex_constants::extended | std::regex_constants::nosubs);
    std::regex pid_regex("^middleware\\.pid = ([1-9][0-9]*)$",
                         std::regex_constants::extended | std::regex_constants::nosubs);
	bool port_given = false;
	bool pid_given = false;
	int line_count = 0;
	while (std::getline(input, line))
	{
		++line_count;
		if (line.empty())
		{
			// ignore empty lines
		}
        else if (std::regex_match(line, sm, port_regex))
		{
			if (port_given)
			{
				HC_LOG_ERROR("Multiple ports given in config file");
				return false;
			}
			// group 1 contains the port
			std::string captured = sm[1];
			try
			{
                m_port = static_cast<std::uint16_t>(std::stoi(captured));
			}
            catch (std::exception& e)
			{
				HC_LOG_ERROR("Invalid port given: " << e.what());
				return false;
			}
			port_given = true;
		}
        else if (std::regex_match(line, sm, pid_regex))
		{
			if (pid_given)
			{
				HC_LOG_ERROR("Multiple process ids given in config file");
				return false;
			}
			// group 1 contains the process id
			std::string captured = sm[1];
			try
			{
                m_pid = static_cast<process_id>(std::stoi(captured));
			}
            catch (std::exception& e)
			{
				HC_LOG_ERROR("Invalid port given: " << e.what());
				return false;
			}
			pid_given = true;
		}
	}
	if (!port_given)
	{
		HC_LOG_ERROR("No port given in config file.");
	}
	if (!pid_given)
	{
		HC_LOG_ERROR("No process id given in config file.");
	}
	return port_given && pid_given;
}

} } // namespace hamcast::ipc
