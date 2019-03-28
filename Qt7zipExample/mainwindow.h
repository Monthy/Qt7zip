/*
 *
 * Qt7zip
 *
 * This file is part of Qt7zip is a C++ wrapper for accessing the 7-zip API
 * Copyright (C) 2019 Pedro A. Garcia Rosado Aka Monthy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see http://www.gnu.org/licenses
 * or write to the Free Software Foundation,Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301  USA
 *
 *
**/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>

#include "qt7zip.h"

enum ColTwList {
	col_index = 0,
	col_name = 1,
	col_path = 2,
	col_size = 3,
	col_packsize = 4,
	col_crc32 = 5,
	col_encrypted = 6,
	col_isdir = 7
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow *ui;

	Qt7zip *z_file;

	QList<szEntryInfo> z_listInfo;
	bool is_load_7zlib;

private slots:
	void on_btn_seleccioar_archivo_clicked();
	void on_btn_abrir_clicked();
	void on_btn_descomprimir_selected_clicked();
	void on_btn_descomprimir_all_clicked();
	void on_btn_comprimir_clicked();

};

#endif // MAINWINDOW_H
