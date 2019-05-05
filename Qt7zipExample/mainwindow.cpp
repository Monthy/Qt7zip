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
#include <QInputDialog>
#include <QDirIterator>
#include <QTextCodec>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ext_z << "*.7z" << "*.zip" << "*.tar" << "*.rar";
	ext_comic << "*.cb7" << "*.cbz" << "*.cbt" << "*.cbr";
	ext_img << ".jpg" << ".jpeg" << ".png" << ".gif" << ".tiff" << ".tif" << ".bmp";
	ext_txt << ".xml" << ".txt" << ".ini" << ".conf" << ".cpp" << ".h";


	// Configuración del tw_list_files
	ui->tw_list_files->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
	ui->tw_list_files->header()->setSectionsMovable(false);
#else
	ui->tw_list_files->header()->setMovable(false);
#endif

	z_file = new Qt7zip();
	is_load_7zlib = z_file->isLoad7zLib();

	connect(z_file, SIGNAL(passwordRequired()), this, SLOT(passwordRequired()));

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

void MainWindow::seleccioar_archivo()
{
	if (is_load_7zlib)
	{
		QString archivo = ui->txt_file->text();

	//	if (archivo.isEmpty() || !QFile::exists(archivo))
	//	{
			archivo = QFileDialog::getOpenFileName(this, tr("Selecciona un archivo"), QDir::homePath(),
						tr("Archivos soportados") +" - ("+ ext_z.join(" ") +" "+ ext_comic.join(" ") +");;"+
						tr("Archivos comprimidos") +" - ("+ ext_z.join(" ") +");;"+
						tr("Comic") +" - ("+ ext_comic.join(" ") +");;"+
						tr("Todos los archivo") +" (*)", 0);
	//	}

		if (!archivo.isEmpty() && QFile::exists(archivo))
		{
			ui->txt_file->setText(archivo);

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
}

void MainWindow::descomprimir_raw(bool extract_all)
{
	if (is_load_7zlib)
	{
		if (z_file->isOpen())
		{
			QList<int> list_select_extract_raw;
			const int count = ui->tw_list_files->topLevelItemCount();
			for (int i = 0; i < count; ++i)
			{
				QTreeWidgetItem *item = ui->tw_list_files->topLevelItem(i);
				if (extract_all || item->checkState(col_index) == Qt::Checked)
				{
					list_select_extract_raw << item->text(col_index).toInt();
				}
			}

			z_list_raw.clear();
			z_list_raw = z_file->extractRaw(list_select_extract_raw);

			QList<QTreeWidgetItem *> itemsRaw;
			const int countRaw = z_list_raw.size();
			for (int i = 0; i < countRaw; ++i)
			{
				QTreeWidgetItem *itemRaw = new QTreeWidgetItem;
				itemRaw->setText(col_index, QString::number(i));
				itemRaw->setText(col_name, "Item raw "+ QString::number(i));
				itemsRaw << itemRaw;
			}

			ui->tw_list_files_raw->clear();
			ui->tw_list_files_raw->addTopLevelItems(itemsRaw);
		} else
			ui->lb_info->setText("El archivo no esta abierto.");
	}
}

QString MainWindow::leerDataRaw(QByteArray data) const
{
	QTextCodec::ConverterState state;
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");

	const QString text = codec->toUnicode(data.constData(), data.size(), &state);
	if (state.invalidChars > 0)
	{
		// Not a UTF-8 text - using system default locale
		QTextCodec *codec = QTextCodec::codecForLocale();
		if (!codec)
			return text;

		return codec->toUnicode(data);
	}

	return text;
}

void MainWindow::passwordRequired()
{
	bool ok = false;
	QString text_pass = QInputDialog::getText(this, tr("Archivos cifrados encontrados") +"...", tr("Puede que necesite una contraseña") +"\n"+ tr("Contraseña") +":", QLineEdit::Password, "", &ok);
	if (ok && !text_pass.isEmpty())
	{
		ui->txt_pass->setText(text_pass);
		z_file->setPassword(text_pass);
	}
}

void MainWindow::on_btn_abrir_clicked()
{
	seleccioar_archivo();
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

void MainWindow::on_btn_descomprimir_selected_raw_clicked()
{
	if (is_load_7zlib)
	{
		if (z_file->isOpen())
		{
			descomprimir_raw();
		} else
			ui->lb_info->setText("El archivo no esta abierto.");
	}
}

void MainWindow::on_btn_descomprimir_all_raw_clicked()
{
	if (is_load_7zlib)
	{
		if (z_file->isOpen())
		{
			descomprimir_raw(true);
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

void MainWindow::on_tw_list_files_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		if (is_load_7zlib)
		{
			if (z_file->isOpen())
			{
				QFileInfo f_info(item->text(col_name));
				QString ext = "."+ f_info.suffix().toLower();

				int index = item->text(col_index).toInt();
				QByteArray data;

				bool esImagen = ext_img.contains(ext, Qt::CaseInsensitive);
				bool esTxt    = ext_txt.contains(ext, Qt::CaseInsensitive);

				if (esImagen || esTxt)
				{
					if (z_file->extractRaw(index, data))
					{
						if (esImagen)
						{
							QPixmap px;
							px.loadFromData(data);
							ui->lb_pixmap->setPixmap(px);
						}

						if (esTxt)
						{
							ui->txt_info->setPlainText(leerDataRaw(data));

					//		QTextStream z_txt_in(data);
					//		z_txt_in.setAutoDetectUnicode(true);
					//	//	z_txt_in.setCodec(QTextCodec::codecForName("ISO 8859-1")); //ANSI
					//	//	z_txt_in.setCodec(QTextCodec::codecForName("UTF-8"));
					//		ui->txt_info->setPlainText(z_txt_in.readAll());
						}
					}
				}
			}
		}
	}
}

void MainWindow::on_tw_list_files_raw_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item && column > -1)
	{
		if (!z_list_raw.isEmpty())
		{
			int index = item->text(col_index).toInt();
			QByteArray data = z_list_raw.at(index);

			QPixmap px;
			if (px.loadFromData(data))
				ui->lb_pixmap->setPixmap(px);
			else
				ui->txt_info->setPlainText(leerDataRaw(data));
		}
	}
}
