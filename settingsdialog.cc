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
#include <gtkmm.h>
#include "settingsdialog.h"

#define _(str)          Glib::locale_to_utf8(str)

namespace X10
{
	SettingsDialog::SettingsDialog(const std::string &device_path, const std::string &housecode, int maxretry) :
		m_table(3, 2, false),
		m_device_path_label(_("Ścieżka do urządzenia X10:")),
		m_housecode_label(_("Housecode:")),
		m_maxretry_label(_("Liczba powtórzeń transmisji:"))
	{
		set_title("Ustawienia");
		set_default_size(300, 100);

		m_device_path.get_entry()->set_text(device_path);
		m_housecode.get_entry()->set_text(housecode);
		m_maxretry.set_value(maxretry);

		m_table.attach(m_device_path_label, 0, 1, 0, 1, Gtk::EXPAND | Gtk::FILL, Gtk::EXPAND);
		m_table.attach(m_device_path, 1, 2, 0, 1, Gtk::EXPAND | Gtk::FILL, Gtk::EXPAND);
		m_table.attach(m_housecode_label, 0, 1, 1, 2, Gtk::EXPAND | Gtk::FILL, Gtk::EXPAND);
		m_table.attach(m_housecode, 1, 2, 1, 2, Gtk::EXPAND | Gtk::FILL, Gtk::EXPAND);
		m_table.attach(m_maxretry_label, 0, 1, 2, 3, Gtk::EXPAND | Gtk::FILL, Gtk::EXPAND);
		m_table.attach(m_maxretry, 1, 2, 2, 3, Gtk::EXPAND | Gtk::FILL, Gtk::EXPAND);

		m_maxretry.set_increments(1, 1);
		m_maxretry.set_range(1, 10);

		get_vbox()->pack_start(m_table, Gtk::PACK_EXPAND_WIDGET);

		add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
		add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		
		show_all_children();
	}

	SettingsDialog::~SettingsDialog()
	{ }

	const std::string SettingsDialog::get_device_value() const
	{ return m_device_path.get_entry()->get_text(); }

	const std::string SettingsDialog::get_housecode_value() const
	{ return m_housecode.get_entry()->get_text(); }

	int SettingsDialog::get_maxretry_value() const
	{ return m_maxretry.get_value_as_int(); }

}
