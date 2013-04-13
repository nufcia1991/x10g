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
#include <sstream>
#include <sys/types.h>
#include <pwd.h>
#include <gtkmm.h>
#include "mainwindow.h"
#include "settingsdialog.h"
#include "adddialog.h"
#include "brightnessdialog.h"
#include "progresswindow.h"

#define _(str)		Glib::locale_to_utf8(str)
#define FAILURE(reason)	Gtk::MessageDialog(*this, \
			_(std::string("Nie uda³o siê wykonaæ operacji: ") + reason), \
			false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK).run();


namespace X10 
{
	/*
	 * TreeViewColumns
	 */

	MainWindow::TreeViewColumns::TreeViewColumns()
	{
		add(m_state);
		add(m_brightness);
		add(m_devcode);
		add(m_name);
		add(m_device);
	}

	/*
	 * MainWindow
	 */

	MainWindow::MainWindow()
	{
		/*
		 * Budowanie GUI
		 */
		Glib::ustring ui_info = 
			"<ui>"
			"	<menubar name='MenuBar'>"
			"		<menu action='Connection'>"
			"			<menuitem action='ConnectionConnect'/>"
			"			<menuitem action='ConnectionDisconnect'/>"
			"			<separator/>"
			"			<menuitem action='ConnectionSettings'/>"
			"			<separator/>"
			"			<menuitem action='ConnectionQuit'/>"
			"		</menu>"
			" "
			"		<menu action='Devices'>"
			"			<menuitem action='DevicesAdd'/>"
			"			<menuitem action='DevicesRemove'/>"
			"			<menuitem action='DevicesEdit'/>"
			"		</menu>"
			" "
			"		<menu action='Action'>"
			"			<menuitem action='ActionOn'/>"
			"			<menuitem action='ActionOff'/>"
			"			<separator/>"
			"			<menuitem action='ActionLight'/>"
			"			<menuitem action='ActionDim'/>"
			"			<menuitem action='ActionSetBrightness'/>"
			"		</menu>"
			"	</menubar>"
			" "
			"	<toolbar name='ToolBar'>"
			"		<toolitem action='ActionOn'/>"
			"		<toolitem action='ActionOff'/>"
			"		<separator/>"
			"		<toolitem action='ActionSetBrightness'/>"
			"	</toolbar>"
			"</ui>";

		set_title("x10g");
		set_default_size(512, 384);

		add(m_vBox);
		m_refActionGroup = Gtk::ActionGroup::create();

		m_refActionGroup->add(Gtk::Action::create("Connection", _("Po³±czenie")));
		m_refActionGroup->add(Gtk::Action::create("Devices", _("Urz±dzenia")));
		m_refActionGroup->add(Gtk::Action::create("Action", _("Operacje")));

		m_refActionGroup->add(Gtk::Action::create("ConnectionConnect", Gtk::Stock::CONNECT, 
			_("Po³±cz z kontrolerem"), _("£±czy z kontrolerem X10")),
			sigc::mem_fun(*this, &X10::MainWindow::on_ConnectionConnect_click));

		m_refActionGroup->add(Gtk::Action::create("ConnectionDisconnect", Gtk::Stock::DISCONNECT,
			_("Roz³±cz z kontrolerem"), _("Roz³±cza z kontrolerem X10")),
			sigc::mem_fun(*this, &X10::MainWindow::on_ConnectionDisconnect_click));

		m_refActionGroup->add(Gtk::Action::create("ConnectionSettings", Gtk::Stock::PROPERTIES,
			_("Ustawienia..."), _("Uruchamia panel konfiguracyjny")),
			sigc::mem_fun(*this, &X10::MainWindow::on_ConnectionSettings_click));

		m_refActionGroup->add(Gtk::Action::create("ConnectionQuit", Gtk::Stock::QUIT,
			_("Zakoñcz"), _("Koñczy pracê programu")),
			sigc::mem_fun(*this, &X10::MainWindow::on_ConnectionQuit_click));

		m_refActionGroup->add(Gtk::Action::create("DevicesAdd", Gtk::Stock::ADD,
			_("Dodaj urz±dzenie..."), _("Dodaje urz±dzenie X10")),
			sigc::mem_fun(*this, &X10::MainWindow::on_DevicesAdd_click));

		m_refActionGroup->add(Gtk::Action::create("DevicesRemove", Gtk::Stock::DELETE,
			_("Usuñ urz±dzenie"), _("Usuwa urz±dzenie X10")),
			sigc::mem_fun(*this, &X10::MainWindow::on_DevicesRemove_click));

		m_refActionGroup->add(Gtk::Action::create("DevicesEdit", Gtk::Stock::EDIT,
			_("Edytuj urz±dzenie"), _("Edytuje skonfigurowane urz±dzenie X10")),
			sigc::mem_fun(*this, &X10::MainWindow::on_DevicesEdit_click));

		m_refActionGroup->add(Gtk::Action::create("ActionOn", Gtk::Stock::YES,
			_("Zapal"), _("Zapala lampê")),
			sigc::mem_fun(*this, &X10::MainWindow::on_ActionOn_click));

		m_refActionGroup->add(Gtk::Action::create("ActionOff", Gtk::Stock::NO,
			_("Zga¶"), _("Gasi lampê")),
			sigc::mem_fun(*this, &X10::MainWindow::on_ActionOff_click));

		m_refActionGroup->add(Gtk::Action::create("ActionLight", Gtk::Stock::GO_UP,
			_("Ja¶niej"), _("Zwiêksza jasno¶æ lampy")),
			sigc::mem_fun(*this, &X10::MainWindow::on_ActionLight_click));

		m_refActionGroup->add(Gtk::Action::create("ActionDim", Gtk::Stock::GO_DOWN,
			_("Ciemniej"), _("Zmniejsza jasno¶æ lampy")),
			sigc::mem_fun(*this, &X10::MainWindow::on_ActionDim_click));

		m_refActionGroup->add(Gtk::Action::create("ActionSetBrightness", Gtk::Stock::EXECUTE,
			_("Ustaw jasno¶æ"), _("Ustawia dowolnie wybran± jasno¶æ lampy")),
			sigc::mem_fun(*this, &X10::MainWindow::on_ActionSetBrightness_click));

		m_refUIManager = Gtk::UIManager::create();
		m_refUIManager->insert_action_group(m_refActionGroup);
		m_refUIManager->add_ui_from_string(ui_info);
		m_vBox.pack_start(*(m_refUIManager->get_widget("/MenuBar")), Gtk::PACK_SHRINK);
		m_vBox.pack_start(*(m_refUIManager->get_widget("/ToolBar")), Gtk::PACK_SHRINK);
		m_vBox.pack_start(m_treeView, Gtk::PACK_EXPAND_WIDGET);

		m_refTreeModel = Gtk::ListStore::create(m_columns);
		m_treeView.set_model(m_refTreeModel);
		m_treeView.append_column("Stan", m_columns.m_state);
		m_treeView.append_column(_("Jasno¶æ"), m_columns.m_brightness);
		m_treeView.append_column("ID", m_columns.m_devcode);
		m_treeView.append_column("Nazwa", m_columns.m_name);

		show_all_children();

		/*
		 * X10 i konfiguracja
		 */
		struct passwd *pwent = getpwuid(getuid());
		m_config.set_context(&m_context);

		try {
			m_config.load(std::string(pwent->pw_dir) + "/.x10g.xml");
		} catch(std::exception &e) { }

		build_treeview();
	}

	MainWindow::~MainWindow()
	{
		try {
			m_config.sync();
		} catch(std::exception &e) {
			FAILURE(_(std::string("Nie uda³o siê zapisaæ pliku konfiguracyjnego: ") +
				e.what()));
		}
	}

	void MainWindow::on_ConnectionConnect_click()
	{
		m_progress.show_message(_("£±czenie z kontrolerem..."));
	
		m_context.set_maxretry(m_config.get_config_value_as_int("maxretry"));
		m_context.set_housecode(X10Housecode(m_config.get_config_value_as_string("housecode")[0]));

		try {
			m_context.open(m_config.get_config_value_as_string("device"));
		} catch(std::exception &e) {
			m_progress.hide();
			Gtk::MessageDialog(*this, 
				_(std::string("Nie uda³o po³±czyæ siê z kontrolerem: ") + e.what())
				, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK).run();
		}

		m_progress.hide();
	}

	void MainWindow::on_ConnectionDisconnect_click()
	{
		m_context.close();
	}

	void MainWindow::on_ConnectionSettings_click()
	{
		SettingsDialog settings(
			m_config.get_config_value_as_string("device"),
			m_config.get_config_value_as_string("housecode"),
			m_config.get_config_value_as_int("maxretry")
		);
		
		if (settings.run() == Gtk::RESPONSE_OK) {
			m_config.set_config_value("device", settings.get_device_value());
			m_config.set_config_value("housecode", settings.get_housecode_value());
			m_config.set_config_value("maxretry", settings.get_maxretry_value());
		}
	}

	void MainWindow::on_ConnectionQuit_click()
	{
		hide();
	}

	void MainWindow::on_DevicesAdd_click()
	{
		AddDialog add(NewDialog);
		if (add.run() == Gtk::RESPONSE_OK) {
			X10Device *d = new X10Device(m_context, add.get_devcode(), add.get_name());
			m_config.get_device_list().push_back(d);
			add_device(d);
		}
	}

	void MainWindow::on_DevicesRemove_click()
	{
		Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_treeView.get_selection();
		Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();

		if (iter) {
			remove_device((X10Device*)(*iter)[m_columns.m_device]);
			m_refTreeModel->erase(iter);
		}
	}

	void MainWindow::on_DevicesEdit_click()
	{
		Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_treeView.get_selection();
		Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
	
		if (iter) {
			AddDialog edit(
				EditDialog, 
				((Glib::ustring)(*iter)[m_columns.m_name]), 
				((X10Device*)(*iter)[m_columns.m_device])->get_devcode()
			);
			
			X10Device *d = ((X10Device*)(*iter)[m_columns.m_device]);
		
			if (edit.run() == Gtk::RESPONSE_OK) {
				d->set_devcode(edit.get_devcode());
				update_device(d);
			}
		}
	}

	void MainWindow::on_ActionOn_click()
	{
		Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_treeView.get_selection();
		Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();


		if (iter) {
			X10Device *d = ((*iter)[m_columns.m_device]);
			
			m_progress.show_message(_("Proszê czekaæ..."));
			try {
				d->set_on();
			} catch(std::exception &e) {
				FAILURE(e.what());
			}
			update_device(d);
			m_progress.hide();
		}
	}

	void MainWindow::on_ActionOff_click()
	{
		Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_treeView.get_selection();
		Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();

		if (iter) {
			X10Device *d = ((*iter)[m_columns.m_device]);

			m_progress.show_message(_("Proszê czekaæ..."));

			try {
				d->set_off();
			} catch(std::exception &e) {
				FAILURE(e.what());
			}

			update_device(d);
			m_progress.hide();
		}
	}

	void MainWindow::on_ActionLight_click()
	{
		Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_treeView.get_selection();
		Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();

		if (iter) {
			X10Device *d = ((*iter)[m_columns.m_device]);

			try {
				d->set_light();
			} catch(std::exception &e) {
				FAILURE(e.what());
			}

			update_device(d);
		}
	}

	void MainWindow::on_ActionDim_click()
	{
		Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_treeView.get_selection();
		Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();

		if (iter) {
			X10Device *d = ((*iter)[m_columns.m_device]);

			try {
				d->set_dim();
			} catch(std::exception &e) {
				FAILURE(e.what());
			}

			update_device(d);
		}
	}

	void MainWindow::on_ActionSetBrightness_click()
	{
		Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_treeView.get_selection();
		Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();

		if (iter) {
			X10Device *d = ((*iter)[m_columns.m_device]);
			BrightnessDialog brightness(d->get_brightness());
			brightness.run();

			try {
				d->set_brightness(brightness.get_brightness());
			} catch(std::exception &e) {
				FAILURE(e.what());
			}

			update_device(d);
		}
	}

	void MainWindow::build_treeview()
	{
		std::vector<X10Device *> &devices = m_config.get_device_list();
		std::vector<X10Device *>::iterator iter = devices.begin();

		m_refTreeModel->clear();
		
		while(iter != devices.end()) {
			X10Device *d = (*iter);

			Gtk::TreeModel::Row row = *(m_refTreeModel->append());
			row[m_columns.m_state] = d->is_on() ? "ON" : "OFF";
			row[m_columns.m_brightness] = d->get_brightness();
			row[m_columns.m_devcode] = d->get_devcode().as_int();
			row[m_columns.m_name] = d->get_name();
			row[m_columns.m_device] = d;
			iter++;
		}
	}

	void MainWindow::add_device(X10Device *device)
	{
		Gtk::TreeModel::Row row = *(m_refTreeModel->append());
		row[m_columns.m_state] = device->is_on() ? "ON" : "OFF";
		row[m_columns.m_brightness] = device->get_brightness();
		row[m_columns.m_devcode] = device->get_devcode().as_int();
		row[m_columns.m_name] = device->get_name();
		row[m_columns.m_device] = device;
	}

	void MainWindow::remove_device(X10Device *device)
	{

	}

	void MainWindow::update_device(X10Device *device)
	{
		Gtk::TreeModel::Children children = m_refTreeModel->children();
		Gtk::TreeModel::Children::iterator iter;

		for (iter = children.begin(); iter != children.end(); iter++) {
			Gtk::TreeModel::Row row = *iter;

			if (row[m_columns.m_device] != device)
				continue;

			row[m_columns.m_state] = device->is_on() ? "ON" : "OFF";
			row[m_columns.m_brightness] = device->get_brightness();
			row[m_columns.m_devcode] = device->get_devcode().as_int();
			row[m_columns.m_name] = device->get_name();
			return;
		}
	}

}
