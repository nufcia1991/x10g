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

#include <string>
#include <gtkmm.h>
#include "progresswindow.h"

#define _(str)		Glib::locale_to_utf8(str)

namespace X10
{
	/*
	 * ProgressWindow
	 */

	ProgressWindow::ProgressWindow(const std::string &message) :
		Gtk::Window(Gtk::WINDOW_TOPLEVEL),
		m_label(message)
	{
		if (message == "")
			m_label.set_text(_("Proszê czekaæ..."));

		set_title(_("Proszê czekaæ..."));
		set_default_size(200, 75);

		add(m_label);
		set_modal(true);
		show_all_children();
		realize();
	}

	ProgressWindow::~ProgressWindow()
	{
	}

	void ProgressWindow::show_message(const std::string &message)
	{
		m_label.set_text(message);

		set_position(Gtk::WIN_POS_CENTER_ALWAYS);
		get_window()->show();
		show();
		m_label.show();
	}
}
