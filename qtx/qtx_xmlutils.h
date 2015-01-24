// $Id: qtx_xmlutils.h $
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
//  **** Module  :  qtx_xmlutils <interface>
//       ~~~~~~~~~
//  **** Project :  QtLibs
//       ~~~~~~~~~
//  **** Functions: qtx::readXMLFile
//       ~~~~~~~~~  qtx::addXMLElement
//                  qtx::addXMLInteger
//                  qtx::addXMLString
//                  qtx::addXMLBoolean
//                  qtx::readXMLIntAttr
//                  qtx::readXMLRealAttr
//                  qtx::readXMLStrAttr
//                  qtx::readXMLInteger
//                  qtx::readXMLReal
//                  qtx::readXMLString
//                  qtx::readXMLBoolean
//                  qtx::getReal
//                  qtx::selectDoc
//                  qtx::getHTMLString
//
//  (C) E. Krissinel 2012-2013
//
// =================================================================
//

#ifndef qtx_XMLUTILS_H
#define qtx_XMLUTILS_H

#include <QDomDocument>

namespace qtx {

  /// Return codes
  #define XMLU_Ok                    0
  #define XMLU_CantOpenFileToRead    1
  #define XMLU_WrongFormat           2

  int readXMLFile ( QString fileName, QDomDocument & doc );

  QDomElement addXMLElement ( QDomDocument & doc, QString tag );
  QDomElement addXMLElement ( QDomDocument & doc, QDomElement & element,
                              QString tag );

  void addXMLInteger ( QDomDocument & doc, QDomElement & element,
                       QString tag, int value );

  void addXMLString  ( QDomDocument & doc, QDomElement & element,
                       QString tag, QString value );

  void addXMLString  ( QDomDocument & doc, QDomElement & element,
                       QString tag, QString attr, QString attrValue,
                       QString value );

  void addXMLBoolean ( QDomDocument & doc, QDomElement & element,
                       QString tag, bool value );


  /// Reads an attribute of a tag as an integer.
  /// \example: <node attr='1'>
  int     readXMLIntAttr ( const QDomElement & node, QString attr,
                           int defValue, bool & ok );

  /// Reads an attribute of a tag as a match to enumerator.
  /// \example: <node attr='value1'>
  int     readXMLIntAttr ( const QDomElement & node, QString attr,
                           const QStringList & values,
                           int defValue );

  /// Reads an attribute of given category of the node as an integer.
  /// \example: <node><tag attr="12345">any text</tag></node>
  int     readXMLIntAttr ( const QDomElement & node, QString tag,
                           QString attr, int defValue, bool & ok );

  /// Reads an attribute of a tag as a real number.
  /// \example: <node attr='1.1'>
  qreal   readXMLRealAttr ( const QDomElement & node, QString attr,
                           qreal defValue, bool & ok );

  /// Reads an attribute of given category of the node as a real number.
  /// \example: <node><tag attr="1.2345">any text</tag></node>
  qreal   readXMLRealAttr ( const QDomElement & node, QString tag,
                            QString attr, qreal defValue, bool & ok );

  /// Reads an attribute of given category of the node as a string.
  /// \example: <node><tag attr="integer">12345</tag></node>
  QString readXMLStrAttr ( const QDomElement & node, QString tag,
                           QString attr, QString defValue,
                           bool & ok );

  /// Reads an integer number in given category of the node.
  /// \example: <node><tag>12345</tag></node>
  int     readXMLInteger ( const QDomNode & node, QString tag,
                           int defValue, bool & ok );

  /// Reads a real number in given category of the node.
  /// \example: <node><tag>3.1415265</tag></node>
  qreal   readXMLReal    ( const QDomNode & node, QString tag,
                           qreal defValue, bool & ok );

  /// \brief Reads a string in given node.
  /// \example: <node>Any string</node>
  QString readXMLString  ( const QDomNode & node, QString defValue,
                           bool & ok );

  /// \brief Reads a string in given category of the node.
  /// \example: <node><tag>Any string</tag></node>
  QString readXMLString  ( const QDomNode & node, QString tag,
                           QString defValue, bool & ok );

  /// Reads a boolean in given category of the node.
  /// \example: <node><tag>Yes</tag></node>
  bool    readXMLBoolean ( const QDomNode & node, QString tag,
                           bool defValue, bool & ok );

  qreal  getReal ( QString S, qreal defValue, bool & ok );

#ifdef QT_XMLPATTERNS_LIB
  /// Selects a sub-document.
  /// \return:  0: success
  /// \return:  1: valid query but empty selection
  /// \return: -1: invalid query
  int  selectDoc ( const QDomDocument & xml_in, const QString & select,
                   QDomDocument & xml_out );
#endif

  extern QString getHTMLString  ( QString S );

}

#endif // qtx_XMLUTILS_H
