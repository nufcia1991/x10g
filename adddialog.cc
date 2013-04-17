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

#include <gtkmm.h>
#include "x10.h"
#include "adddialog.h"

#define _(str)          Glib::locale_to_utf8(str)

namespace X10 {
	
	/*
	 * AddDialog
	 */

	AddDialog::AddDialog(AddDialogType type, const std::string &name, const X10Devcode &devcode) :
		m_table(2, 2, false),
		m_name_label(_("Nazwa urządzenia:")),
		m_devcode_label(_("Kod urządzenia:"))
	{
		set_title("Dodaj urządzenie");
		set_default_size(200, 50);

		m_table.attach(m_name_label, 0, 1, 0, 1);
		m_table.attach(m_name, 1, 2, 0, 1);
		m_table.attach(m_devcode_label, 0, 1, 1, 2);
		m_table.attach(m_devcode, 1, 2, 1, 2);

		get_vbox()->pack_start(m_table);

		add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
		add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

		m_devcode.set_increments(1, 1);
		m_devcode.set_range(1, 16);

		if (type == EditDialog) {
			m_name.set_text(name);
			m_name.set_editable(false);
			m_devcode.set_value(devcode.as_int());
		}

		show_all_children();
	}

	AddDialog::~AddDialog()
	{
	}

	const std::string AddDialog::get_name()
	{
		return m_name.get_text();
	}

	const X10Devcode AddDialog::get_devcode()
	{
		return m_devcode.get_value_as_int();
	}
}

