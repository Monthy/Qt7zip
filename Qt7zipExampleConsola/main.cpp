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

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>

#include "qt7zip.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	QString stDirApp = QDir::currentPath() +"/";

	Qt7zip *z_file = new Qt7zip();
	bool is_load_7zlib = z_file->isLoad7zLib();

	if (is_load_7zlib)
	{
		qDebug() << "Libreria de 7-Zip cargada correctamente.";

		// Lectura de metadatos de archivo.
		if (z_file->open(stDirApp +"ejemplo.7z"))
		{
			qDebug() << "Total archivos: " << z_file->getNumEntries();

			QList<szEntryInfo> z_listInfo = z_file->entryListInfo();

			qDebug() << "Listado de archivos del comprimido.";
			const int total = z_file->getNumEntries();
			for (int i = 0; i < total; ++i)
			{
				qDebug() << "index    : " << z_listInfo.at(i).id;
				qDebug() << "name     : " << z_listInfo.at(i).name;
				qDebug() << "path     : " << z_listInfo.at(i).path;
				qDebug() << "size     : " << z_listInfo.at(i).size;
				qDebug() << "packsize : " << z_listInfo.at(i).packsize;
				qDebug() << "crc32    : " << z_listInfo.at(i).crc32;
				qDebug() << "encrypted: " << (z_listInfo.at(i).encrypted ? "*" : "");
				qDebug() << "isdir    : " << (z_listInfo.at(i).isDir ? "si" : "no");
				qDebug() << "---------------------" << endl;
			}
		} else {
			qDebug() << "No se ha podido abrir el archivo.";
		}

		// Extracción de archivos.
		if (z_file->open(stDirApp +"ejemplo.7z"))
		{
			qDebug() << "Extracción de archivos del comprimido.";

			// Extracción de un archivo solo.
			z_file->extract("LICENSE.txt", stDirApp +"temp/");

			// Extracción de un archivo cambiando el nombre de salida.
			z_file->extract("LICENSE.txt", stDirApp +"temp/", "licencia.txt");

			// Extracción de un archivo a un buffer QByteArray.
			QByteArray data;
			z_file->extractRaw("README.txt", data);
			qDebug() << "Contenido del buffer QByteArray ---------------------";
			qDebug() << QString(data);
			qDebug() << "-----------------------------------------------------" << endl;

			// Extracción de un archivo cifrado con un password.
			z_file->setPassword("12345");
			z_file->extract("encrypted.txt", stDirApp +"temp/");

			// Extracción de mutiples archivos.
			QStringList z_list = z_file->entryList();
			QList<szEntryInfo> z_listInfo = z_file->entryListInfo();
			QList<szEntryExtract> listEntry;
			szEntryExtract entry;
				entry.index   = z_list.indexOf("README.txt"); // El index debe ser del entryList y no el interno del comprimido.
				entry.fileOut = z_listInfo.at(entry.index).path;
			listEntry << entry;
				entry.index   = z_list.indexOf("imagen/qt-logo.png");
				entry.fileOut = z_listInfo.at(entry.index).path;
			listEntry << entry;

			z_file->extract(listEntry, stDirApp +"temp/");

			// Extracción completa del archivo comprimido.
			z_file->setPassword("12345");
			z_file->extract(stDirApp +"temp/todo/");
		} else {
			qDebug() << "No se ha podido abrir el archivo.";
		}

		// Creando un archivo comprimido.
		qDebug() << "Creando un archivo comprimido.";

		QString directorio = stDirApp +"temp/todo/";

		qDebug() << "Destino: " << stDirApp +"temp/files_compress.7z";

		if (!QFile::exists(stDirApp +"temp/files_compress.7z"))
		{
			szEntryCompress entry;
			QList<szEntryCompress> lista_archivo;
			QDirIterator it(directorio, QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
			while (it.hasNext())
			{
				QString entry_it = it.next();
				entry.fileIn  = entry_it;
				entry.fileOut = entry_it.remove(directorio);
				lista_archivo << entry;
			}

			z_file->create(CFormat7z, lista_archivo, stDirApp +"temp/files_compress.7z");
		} else {
			qDebug() << "No se ha podido crear el comprimido porque ya existe.";
		}
	} else {
		qDebug() << "No se ha podido cargar la libreria de 7-Zip.";
	}

	return app.exec(); //and we run the application
}
