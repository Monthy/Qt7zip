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

#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// Configuración del tw_list_files
	ui->tw_list_files->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
	ui->tw_list_files->header()->setSectionsMovable(false);
#else
	ui->tw_list_files->header()->setMovable(false);
#endif

	z_file = new Qt7zip();

}

MainWindow::~MainWindow()
{
	delete z_file;
	delete ui;
}

void MainWindow::on_btn_seleccioar_archivo_clicked()
{
	QString ext_z = "*.7z *.zip *.rar *.tar";
	QString ext_comic = "*.cb7 *.cbz *.cbr *.cbt";

	QString archivo = QFileDialog::getOpenFileName(this, tr("Selecciona un archivo"), QDir::homePath(),
				tr("Archivos soportados") +" - ("+ ext_z +" "+ ext_comic +");;"+
				tr("Archivos comprimidos") +" - ("+ ext_z +");;"+
				tr("Comic") +" - ("+ ext_comic +");;"+
				tr("Todos los archivo") +" (*)", 0);

	if (!archivo.isEmpty() && QFile::exists(archivo))
	{
		ui->txt_file->setText(archivo);
	}
}
