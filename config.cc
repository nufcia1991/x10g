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

#include <iostream>
#include <vector>
#include <libxml++/libxml++.h>
#include "strconv.h"
#include "config.h"

namespace X10
{

	/*
	 * ConfigManager
	 */

	ConfigManager::ConfigManager()
	{
	       set_defaults();	
	}

	ConfigManager::ConfigManager(const std::string &xmlfile, X10Context *ctx) :
		m_xml_path(xmlfile)
	{
		set_defaults();
		m_context = ctx;
		load(m_xml_path);
	}

	ConfigManager::~ConfigManager()
	{ }

	void ConfigManager::load(const std::string &xmlfile)
	{
		xmlpp::Node *rootnode;
		xmlpp::NodeSet config_nodes;
		xmlpp::NodeSet devices_nodes;
		m_xml_path = xmlfile;

		if (!m_context)
			throw ConfigContextException();

		m_parser.set_substitute_entities();
		m_parser.parse_file(m_xml_path);

		rootnode = dynamic_cast<xmlpp::Node*>(m_parser.get_document()->get_root_node());
		config_nodes = rootnode->find("/x10g/config/entry");
		devices_nodes = rootnode->find("/x10g/devices/device");

		std::vector<xmlpp::Node*>::iterator config_iter = config_nodes.begin();
		while (config_iter != config_nodes.end()) {
			xmlpp::Element *config_item = dynamic_cast<xmlpp::Element*>(*config_iter++);
			m_config[config_item->get_attribute("name")->get_value()] =
			       	config_item->get_attribute("value")->get_value();
		}

		std::vector<xmlpp::Node*>::iterator devices_iter = devices_nodes.begin();
		while (devices_iter != devices_nodes.end()) {
			xmlpp::Element *devices_item = dynamic_cast<xmlpp::Element*>(*devices_iter++);
			X10Device *device = new X10Device(devices_item->get_attribute("name")->get_value());
			device->set_devcode(from_string<int>(devices_item->get_attribute("devcode")->get_value()));
			device->set_brightness(from_string<int>(devices_item->get_attribute("brightness")->get_value()));
			devices_item->get_attribute("state")->get_value() == "on" ? device->set_on() : device->set_off();
			device->set_context(*m_context);
			m_devices.push_back(device);
		}

	}

	void ConfigManager::sync()
	{
		xmlpp::Document document;
		xmlpp::Node *rootnode;
		xmlpp::Node *config_node;
		xmlpp::Node *devices_node;
		
		rootnode = dynamic_cast<xmlpp::Node*>(document.create_root_node("x10g"));
		config_node = dynamic_cast<xmlpp::Node*>(rootnode->add_child("config"));
		devices_node = dynamic_cast<xmlpp::Node*>(rootnode->add_child("devices"));
	
		std::map<std::string, std::string>::iterator config_iter = m_config.begin();
		while (config_iter != m_config.end()) {
			xmlpp::Element *config_item = config_node->add_child("entry");
			config_item->set_attribute("name", config_iter->first);
			config_item->set_attribute("value", config_iter->second);
			
			config_iter++;
		}
	
		std::vector<X10Device *>::iterator devices_iter = m_devices.begin();
		while (devices_iter != m_devices.end()) {
			xmlpp::Element *devices_item = devices_node->add_child("device");
			devices_item->set_attribute("name", (*devices_iter)->get_name());
			devices_item->set_attribute("devcode", to_string<int>((*devices_iter)->get_devcode().as_int()));
			devices_item->set_attribute("state", (*devices_iter)->is_on() ? "on" : "off");
			devices_item->set_attribute("brightness", to_string<int>((*devices_iter)->get_brightness()));

			devices_iter++;
		}

		document.write_to_file_formatted(m_xml_path);
	}

	X10Context *ConfigManager::get_context()
	{ return m_context; }

	void ConfigManager::set_context(X10Context *ctx)
	{ m_context = ctx; }

	int ConfigManager::get_config_value_as_int(const std::string &name)
	{ return from_string<int>(m_config[name]); }
	
	const std::string &ConfigManager::get_config_value_as_string(const std::string &name) 
	{ return m_config[name]; }
	
	void ConfigManager::set_config_value(const std::string &name, int value) 
	{ m_config[name] = to_string<int>(value); }	
	
	void ConfigManager::set_config_value(const std::string &name, const std::string &value) 
	{ m_config[name] = value; }
	
	std::vector<X10Device *> &ConfigManager::get_device_list() 
	{ return m_devices; }

	void ConfigManager::set_defaults()
	{
		m_config["device"] = "/dev/ttyUSB0";
		m_config["housecode"] = "A";
		m_config["maxretry"] = "4";
	}
}
