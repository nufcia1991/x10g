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

#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include "x10.h"
#include "config.h"
#include "progresswindow.h"

namespace X10 
{

	class MainWindow : public Gtk::Window
	{	
	public:
		MainWindow();
		~MainWindow();

	protected:
		virtual void on_ConnectionConnect_click();
		virtual void on_ConnectionDisconnect_click();
		virtual void on_ConnectionSettings_click();
		virtual void on_ConnectionQuit_click();
		virtual void on_DevicesAdd_click();
		virtual void on_DevicesRemove_click();
		virtual void on_DevicesEdit_click();
		virtual void on_ActionOn_click();
		virtual void on_ActionOff_click();
		virtual void on_ActionLight_click();
		virtual void on_ActionDim_click();
		virtual void on_ActionSetBrightness_click();

		void build_treeview();
		void add_device(X10Device *device);
		void remove_device(X10Device *device);
		void update_device(X10Device *device);

		class TreeViewColumns : public Gtk::TreeModel::ColumnRecord
		{
		public:
			TreeViewColumns();

			Gtk::TreeModelColumn<Glib::ustring> m_state;
			Gtk::TreeModelColumn<int> m_brightness;
			Gtk::TreeModelColumn<int> m_devcode;
			Gtk::TreeModelColumn<Glib::ustring> m_name;
			Gtk::TreeModelColumn<X10Device *> m_device;
		};

		ProgressWindow m_progress;
		ConfigManager m_config;
		X10Context m_context;

		Gtk::VBox m_vBox;

		Gtk::TreeView m_treeView;
		TreeViewColumns m_columns;

		Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
		Glib::RefPtr<Gtk::UIManager> m_refUIManager;
		Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
	};
}

#endif
