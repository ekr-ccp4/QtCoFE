// $Id: qtx_utils.cpp $
// =================================================================
//  This code is distributed under the terms and conditions of the
//  CCP4 Program Suite Licence Agreement as 'Part 2' (Annex 2)
//  software. A copy of the CCP4 licence can be obtained by writing
//  to CCP4, Research Complex at Harwell, Rutherford Appleton
//  Laboratory, Didcot OX11 0FA, UK, or from
//  http://www.ccp4.ac.uk/ccp4license.php.
// =================================================================
//
//    09.01.13   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  qtx_utils <implementation>
//       ~~~~~~~~~
//  **** Project :  QtLibs
//       ~~~~~~~~~
//  **** Classes :
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2012-2013
//
// =================================================================
//

#include <QFileInfo>
#include <QDirIterator>

#include "qtx/qtx_utils.h"

bool qtx::removeDir ( const QString & path )  {
QDirIterator dit ( path,QDir::Files | QDir::Dirs | QDir::NoDot |
                   QDir::NoDotDot | QDir::Hidden | QDir::System,
                   QDirIterator::Subdirectories );
QDir         dir ( path );
QFileInfo    fInfo;
QString      itemPath;

  while (dit.hasNext())
    if (!dit.next().isEmpty())  {
      fInfo    = dit.fileInfo();
      itemPath = fInfo.absoluteFilePath();
      if (fInfo.isDir())  removeDir ( itemPath );
                    else  QFile::remove ( itemPath );
    }

  return dir.rmdir ( path );

}

/// If destination exists and is a directory, the file is copied
/// into that directory. Otherwise, file "source" is copied with
/// new name "dest".
int qtx::copyFile ( const QString & source, const QString & dest )  {

}
