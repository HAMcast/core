/******************************************************************************\
 *           ___        __                                                    *
 *          /\_ \    __/\ \                                                   *
 *          \//\ \  /\_\ \ \____    ___   _____   _____      __               *
 *            \ \ \ \/\ \ \ '__`\  /'___\/\ '__`\/\ '__`\  /'__`\             *
 *             \_\ \_\ \ \ \ \L\ \/\ \__/\ \ \L\ \ \ \L\ \/\ \L\.\_           *
 *             /\____\\ \_\ \_,__/\ \____\\ \ ,__/\ \ ,__/\ \__/.\_\          *
 *             \/____/ \/_/\/___/  \/____/ \ \ \/  \ \ \/  \/__/\/_/          *
 *                                          \ \_\   \ \_\                     *
 *                                           \/_/    \/_/                     *
 *                                                                            *
 * Copyright (C) 2011, 2012                                                   *
 * Dominik Charousset <dominik.charousset@haw-hamburg.de>                     *
 *                                                                            *
 * This file is part of libcppa.                                              *
 * libcppa is free software: you can redistribute it and/or modify it under   *
 * the terms of the GNU Lesser General Public License as published by the     *
 * Free Software Foundation, either version 3 of the License                  *
 * or (at your option) any later version.                                     *
 *                                                                            *
 * libcppa is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                       *
 * See the GNU Lesser General Public License for more details.                *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with libcppa. If not, see <http://www.gnu.org/licenses/>.            *
\******************************************************************************/


#include <utility>
#include <fstream>
#include <iostream>

#include <regex>

#include "hamcast/util/config_map.hpp"

using namespace std;

namespace hamcast { namespace util {

namespace {

typedef config_map::container_type container_type;

typedef void (*regex_handler)(std::smatch&, container_type&, string&);
typedef pair<std::regex, regex_handler> rx_handler_pair;
typedef vector<rx_handler_pair> rx_handler_t;

void push_back(rx_handler_t& v, const std::regex& rx, regex_handler rxh)
{
    v.push_back(rx_handler_pair(rx, rxh));
}

bool matches(rx_handler_t& vec, const string& line,
             container_type& ini_map, string& section)
{
    std::smatch sm;
    for (rx_handler_t::const_iterator i(vec.begin()); i != vec.end(); ++i)
    {
        if (std::regex_match(line, sm, i->first))
        {
            (i->second)(sm, ini_map, section);
            return true;
        }
    }
    return false;
}

void new_section(std::smatch& sm, container_type& cm, string& sec)
{
    string sec_name = sm[1];

    if (cm.find(sec_name) != cm.end())
    {
        string err;
        err  = "section ";
        err += sec_name;
        err += " already defined";
        throw runtime_error(err);
    }
    sec = sec_name;
    // insert sec_name to the map
    cm[sec_name];
}

void add_key_value_pair(std::smatch& sm, container_type& cm, string& sec)
{
    if (sec.empty())
    {
        throw runtime_error("key value pair outside section");
    }
    string key = sm[1];
    string value = sm[2];
    map<string,string>& kv_map = cm[sec];
    if (kv_map.find(key) != kv_map.end())
    {
        string err;
        err  = "key ";
        err += key;
        err += " already defined";
        throw runtime_error(err);
    }
    kv_map[key] = value;
}

} // namespace <anonymous>

void config_map::read_ini(const string& filename)
{
    // stores current section
    string section;
    // INI line handler
    rx_handler_t rx_handler;
    // INI section
    push_back(rx_handler,
              std::regex("^\\[([-a-zA-Z0-9\\._]+)\\]\\s*(?:;.*)?$"),
              new_section);
    // INI value
    push_back(rx_handler,
              std::regex("^([-a-zA-Z0-9\\._]+)\\s*=\\s*([-a-zA-Z0-9\\._/]+)\\s*(?:;.*)?$"),
              add_key_value_pair);
    // INI string value
    push_back(rx_handler,
              std::regex("^([-a-zA-Z0-9\\._]+)\\s*=\\s*\"([^\"]+)\"\\s*(?:;.*)?$"),
              add_key_value_pair);
    int line_count = 0;
    string line;
    ifstream input_stream(filename.c_str());
    if (!(input_stream.is_open() && input_stream.good()))
    {
        throw runtime_error("Config file " + filename + " not found");
    }
    while (getline(input_stream, line))
    {
        ++line_count;
        if (line.empty())
        {
            // ignore empty lines
        }
        else if (line.at(0) == ';')
        {
            // ignore comment lines
        }
        else if (!matches(rx_handler, line, m_data, section))
        {
            cerr << "Could not parse line " << line_count
                 << " of middleware.ini (skipped)" << endl;
        }
    }
}

} } // namespace hamcast::util
