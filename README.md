# Qt7zip
Este es un wrapper en Qt C++ para acceder a la API de 7-zip usando 7z.dll o 7z.so. El código se basa en gran medida en el SDK de LZMA.


## Ejemplos de uso del wrapper

A continuación se muestran algunos ejemplos, que muestran cómo usar algunas de las características principales del wrapper:

Para probar el ejemplo de consola correctamente se debe copiar **ejemplo.7z** junto al ejecutable para hacer las pruebas de lectura y extracción correctamente o indicar otro archivo en el código.


### Lectura de metadatos de archivo.

```cpp
#include <QDebug>
#include <QDir>

#include "qt7zip.h"

...

QString stDirApp = QDir::currentPath() +"/";

Qt7zip *z_file = new Qt7zip();
bool is_load_7zlib = z_file->isLoad7zLib();

if (is_load_7zlib)
{
	qDebug() << "Librería de 7-Zip cargada correctamente.";

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
} else {
	qDebug() << "No se ha podido cargar la librería de 7-Zip.";
}
```

### Extracción de archivos.

```cpp
#include <QDebug>
#include <QDir>

#include "qt7zip.h"

...

QString stDirApp = QDir::currentPath() +"/";

Qt7zip *z_file = new Qt7zip();
bool is_load_7zlib = z_file->isLoad7zLib();

if (is_load_7zlib)
{
	qDebug() << "Librería de 7-Zip cargada correctamente.";

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

		// Extracción de múltiples archivos.
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
} else {
	qDebug() << "No se ha podido cargar la librería de 7-Zip.";
}
```

### Comprimir archivos en un archivo.

```cpp
#include <QDebug>
#include <QDir>
#include <QDirIterator>

#include "qt7zip.h"

...

QString stDirApp = QDir::currentPath() +"/";

Qt7zip *z_file = new Qt7zip();
bool is_load_7zlib = z_file->isLoad7zLib();

if (is_load_7zlib)
{
	qDebug() << "Librería de 7-Zip cargada correctamente.";

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
	qDebug() << "No se ha podido cargar la librería de 7-Zip.";
}
```

## Requisitos
* Qt7zip
	* En sistema UNIX/LINUX
		* Instalar p7zip v16.02
	* En Windows
		* Instalar 7zip v19.00
		* Copiar 7z.dll junto al ejecutable de tu aplicación

## Compilarlo
1. Tener Qt 4/5 con MinGW
2. Clonar el repositorio

```
qmake Qt7zip.pro
mingw32-make.exe
```

## Agradecimientos
- A los desarrolladores de [Qt](https://www.qt.io/) por facilitar el desarrollo multiplataforma.
- A los desarrolladores de [7-zip](https://www.7-zip.org/) y [P7ZIP](http://p7zip.sourceforge.net/) por el genial compresor/descompresor.
- A los desarrolladores de [YACReader](https://github.com/YACReader) por copiar parte de su wrapper descaradamente :)

