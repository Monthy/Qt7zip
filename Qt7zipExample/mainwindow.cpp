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
#include <QDirIterator>

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
	is_load_7zlib = z_file->isLoad7zLib();

	if (is_load_7zlib)
		 ui->lb_info->setText(tr("Libreria de 7-Zip cargada correctamente."));
	else
		 ui->lb_info->setText(tr("No se ha podido cargar la libreria de 7-Zip."));
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

void MainWindow::on_btn_abrir_clicked()
{
	if (is_load_7zlib)
	{
		if (z_file->open(ui->txt_file->text(), ui->txt_pass->text()))
		{
			QList<QTreeWidgetItem *> items;

			z_listInfo = z_file->entryListInfo();
			const int total = z_file->getNumEntries();

			ui->lb_info->setText(tr("Total de archivos") +": "+ QString::number(total));
			ui->z_progress->setRange(0, total);
			ui->z_progress->setValue(0);

			for (int i = 0; i < total; ++i)
			{
				ui->lb_items->setText(QString::number(total) +" "+ tr("de") +" "+ QString::number(i + 1));
				ui->z_progress->setValue(i + 1);

				QTreeWidgetItem *item = new QTreeWidgetItem;
				item->setCheckState(col_index, Qt::Unchecked);
				item->setText(col_index    , QString::number(i)                       );
				item->setText(col_name     , z_listInfo.at(i).name                    );
				item->setText(col_path     , z_listInfo[i].path                       );
				item->setText(col_size     , QString::number(z_listInfo[i].size)      );
				item->setText(col_packsize , QString::number(z_listInfo[i].packsize)  );
				item->setText(col_crc32    , z_listInfo[i].crc32                      );
				item->setText(col_encrypted, z_listInfo[i].encrypted ? "*" : ""       );
				item->setText(col_isdir    , z_listInfo[i].isDir ? tr("si") : tr("no"));
				items << item;
			}

			ui->tw_list_files->clear();
			ui->tw_list_files->addTopLevelItems(items);
		} else
			ui->lb_info->setText("No se ha podido abrir el archivo.");
	}
}

void MainWindow::on_btn_descomprimir_selected_clicked()
{
	if (is_load_7zlib)
	{
		if (z_file->isOpen())
		{
			QDir dir;
			QString directorio = QFileDialog::getExistingDirectory(this, tr("Selecciona un directorio"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

			if (!directorio.isEmpty() && dir.exists(directorio))
			{
				QList<szEntryExtract> listEntry;
				const int count = ui->tw_list_files->topLevelItemCount();
				for (int i = 0; i < count; ++i)
				{
					QTreeWidgetItem *item = ui->tw_list_files->topLevelItem(i);
					if (item->checkState(col_index) == Qt::Checked)
					{
						szEntryExtract entry;
							entry.index   = item->text(col_index).toInt();
							entry.fileOut = item->text(col_path);
						listEntry << entry;
					}
				}

				z_file->extract(listEntry, directorio);
			}
		} else
			ui->lb_info->setText("El archivo no esta abierto.");
	}
}

void MainWindow::on_btn_descomprimir_all_clicked()
{
	if (is_load_7zlib)
	{
		if (z_file->isOpen())
		{
			QDir dir;
			QString directorio = QFileDialog::getExistingDirectory(this, tr("Selecciona un directorio"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

			if (!directorio.isEmpty() && dir.exists(directorio))
			{
				z_file->extract(directorio);
			}
		} else
			ui->lb_info->setText("El archivo no esta abierto.");
	}
}

void MainWindow::on_btn_comprimir_clicked()
{
	if (is_load_7zlib)
	{
		QStringList lista_archivo = QFileDialog::getOpenFileNames(this, tr("Selecciona un archivo"), QDir::homePath(), tr("Todos los archivo") +" (*)", 0);

		const int size = lista_archivo.size();
		if (size > 0)
		{
			QStringList filters;
			filters << "7-Zip - (*.7z)" << "Zip - (*.zip)" << "Tar - (*.tar)";

			QString selectedFilter = filters.at(0);
			QString archivo = QFileDialog::getSaveFileName(this, tr("Guardar como"), QDir::homePath(), filters.join(";;"), &selectedFilter);
			if (!archivo.isEmpty() && filters.contains(selectedFilter))
			{
				int index = filters.indexOf(selectedFilter);
				switch (index)
				{
					default:
					case 0:
						z_file->create(CFormat7z, lista_archivo, archivo);
					break;
					case 1:
						z_file->create(CFormatZip, lista_archivo, archivo);
					break;
					case 2:
						z_file->create(CFormatTar, lista_archivo, archivo);
					break;
				}
			}
		}
	} else
		ui->lb_info->setText("La libreria de 7zip no estan cargadas.");
}

void MainWindow::on_btn_comprimir_directorio_clicked()
{
	if (is_load_7zlib)
	{
		QDir dir;
		QString directorio = QFileDialog::getExistingDirectory(this, tr("Selecciona un directorio"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

		if (!directorio.isEmpty() && dir.exists(directorio))
		{
			szEntryCompress entry;
			QList<szEntryCompress> lista_archivo;

			QDirIterator it(directorio, QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
			while (it.hasNext())
			{
				QString entry_it = it.next();
				entry.fileIn  = entry_it;
				entry.fileOut = entry_it.remove(directorio +"/");
				lista_archivo << entry;
			}

			QStringList filters;
			filters << "7-Zip - (*.7z)" << "Zip - (*.zip)" << "Tar - (*.tar)";

			QString selectedFilter = filters.at(0);
			QString archivo = QFileDialog::getSaveFileName(this, tr("Guardar como"), QDir::homePath(), filters.join(";;"), &selectedFilter);
			if (!archivo.isEmpty() && filters.contains(selectedFilter))
			{
				int index = filters.indexOf(selectedFilter);
				switch (index)
				{
					default:
					case 0:
						z_file->create(CFormat7z, lista_archivo, archivo);
					break;
					case 1:
						z_file->create(CFormatZip, lista_archivo, archivo);
					break;
					case 2:
						z_file->create(CFormatTar, lista_archivo, archivo);
					break;
				}
			}
		}
	}
}
