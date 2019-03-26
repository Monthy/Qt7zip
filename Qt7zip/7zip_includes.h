#ifndef _7ZIP_INCLUDES_H
#define _7ZIP_INCLUDES_H

#include <QDebug>
#include <QString>

#include "StdAfx.h"

#include <stdio.h>

#include "CPP/Common/Defs.h"
#include "CPP/Common/MyInitGuid.h"
#include "CPP/Common/MyWindows.h"
#include "CPP/Common/IntToString.h"
#include "CPP/Common/StringConvert.h"

#include "CPP/Windows/DLL.h"
#include "CPP/Windows/FileDir.h"
#include "CPP/Windows/FileFind.h"
#include "CPP/Windows/FileName.h"
#include "CPP/Windows/NtCheck.h"
#include "CPP/Windows/PropVariant.h"
#include "CPP/Windows/PropVariantConv.h"

#include "CPP/7zip/Common/FileStreams.h"
#include "CPP/7zip/Common/StreamObjects.h"
#include "CPP/7zip/Archive/IArchive.h"
#include "CPP/7zip/IPassword.h"
#include "CPP/7zip/ICoder.h"

#include "C/Alloc.h"
#include "C/7zVersion.h"

#ifdef _WIN32
HINSTANCE g_hInstance = 0;
#endif

using namespace NWindows;
using namespace NFile;
using namespace NDir;

static void PrintString(const QString &s)
{
	Q_UNUSED(s);
//	qDebug() << s;
}

static void PrintError(const QString &s, const QString &name = "")
{
//	Q_UNUSED(s);
//	Q_UNUSED(name);
	qDebug() << "Error: " << s;
	if (!name.isEmpty())
		qDebug() << name;
}

static void PrintNewLine()
{
//	qDebug() << "";
}

static HRESULT IsArchiveItemProp(IInArchive *archive, UInt32 index, PROPID propID, bool &result)
{
	NCOM::CPropVariant prop;
	RINOK(archive->GetProperty(index, propID, &prop));
	if (prop.vt == VT_BOOL)
		result = VARIANT_BOOLToBool(prop.boolVal);
	else if (prop.vt == VT_EMPTY)
		result = false;
	else
		return E_FAIL;
	return S_OK;
}

static HRESULT IsArchiveItemFolder(IInArchive *archive, UInt32 index, bool &result)
{
	return IsArchiveItemProp(archive, index, kpidIsDir, result);
}

static const wchar_t *kEmptyFileAlias = L"[Content]";

#endif // _7ZIP_INCLUDES_H
