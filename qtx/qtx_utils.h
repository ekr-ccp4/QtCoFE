// $Id: qtx_utils.h $
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
//  **** Module  :  qtx_utils <interface>
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

#ifndef qtx_UTILS_H
#define qtx_UTILS_H

#include <QString>

namespace qtx {

  /// Removes a non-empty directory.
  /// \param   path  path to the directory
  /// \return  true  if successfully removed
  bool removeDir ( const QString & path );

}

#endif // qtx_UTILS_H
