/*
 * Copyright (c) 2011 Agnieszka Tabaka <nufcia@gmail.com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <vector>
#include <string>
#include <libxml++/libxml++.h>
#include "x10.h"

namespace X10 
{

	class ConfigContextException : public std::runtime_error
	{
	public:
		ConfigContextException() : std::runtime_error("X10 context not set!") { }
	};

	class ConfigManager
	{
	public:
		ConfigManager();
		ConfigManager(const std::string &xmlfile, X10Context *ctx);
		~ConfigManager();

		void load(const std::string &xmlfile);
		void sync();

		X10Context *get_context();
		void set_context(X10Context *ctx);
		int get_config_value_as_int(const std::string &name);
		const std::string &get_config_value_as_string(const std::string &name);
		void set_config_value(const std::string &name, int value);
		void set_config_value(const std::string &name, const std::string &value);

		std::vector<X10Device *> &get_device_list();

	private:
		void set_defaults();

		X10Context *m_context;
		std::string m_xml_path;
		xmlpp::DomParser m_parser;
		std::map<std::string, std::string> m_config;
		std::vector<X10Device *> m_devices;
	};
}

#endif
