// $Id: qtx_xmlutils.cpp $
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
//  **** Module  :  qtx_xmlutils <implementation>
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

#include <stdlib.h>

#include <QFile>
#include <QStringList>

#ifdef QT_XMLPATTERNS_LIB
#include <QXmlQuery>
#endif

#include "qtx/qtx_xmlutils.h"

int qtx::readXMLFile ( QString fileName, QDomDocument & doc )  {
QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly))
    return XMLU_CantOpenFileToRead;

  if (!doc.setContent(&file))  {
    file.close();
    return XMLU_WrongFormat;
  }

  file.close();

  return XMLU_Ok;

}



QDomElement qtx::addXMLElement ( QDomDocument & doc, QString tag )  {
QDomElement element = doc.createElement ( tag );
  doc.appendChild ( element );
  return element;
}

QDomElement qtx::addXMLElement ( QDomDocument & doc,
                                 QDomElement & element, QString tag )  {
QDomElement category = doc.createElement ( tag );
  element.appendChild ( category );
  return category;
}

void qtx::addXMLInteger ( QDomDocument & doc, QDomElement & element,
                          QString tag, int value )  {
QDomElement category = doc.createElement ( tag );
  category.appendChild ( doc.createTextNode(QString("%1").arg(value)) );
  element.appendChild ( category );
}

void qtx::addXMLString ( QDomDocument & doc, QDomElement & element,
                         QString tag, QString value )  {
QDomElement category = doc.createElement ( tag );
  category.appendChild ( doc.createTextNode(value) );
  element.appendChild ( category );
}

void qtx::addXMLString ( QDomDocument & doc, QDomElement & element,
                         QString tag, QString attr, QString attrValue,
                         QString value )  {
QDomElement category = doc.createElement ( tag );
  category.setAttribute ( attr,attrValue );
  category.appendChild  ( doc.createTextNode(value) );
  element.appendChild   ( category );
}

void qtx::addXMLBoolean ( QDomDocument & doc, QDomElement & element,
                          QString tag, bool value )  {
QDomElement category = doc.createElement ( tag );
  if (value)  category.appendChild ( doc.createTextNode("yes") );
        else  category.appendChild ( doc.createTextNode("no")  );
  element.appendChild ( category );
}


int qtx::readXMLIntAttr ( const QDomElement & node, QString attr,
                          int defValue, bool & ok )  {
int   value;
char *p1;
char *p2;
bool  B = false;

  p1 = node.attribute(attr,"").toAscii().data();
  if (p1[0])  {
    value = (int)float(strtod(p1,&p2)+0.49999);
    B     = (p2!=p1);
  }

  if (B)  return value;
  ok = B;
  return defValue;

}


int qtx::readXMLIntAttr ( const QDomElement & node, QString attr,
                          const QStringList & values,
                          int defValue )  {
// Reads an attribute of a tag as a match to enumerator.
// \example: <node attr='value1'>
int v = values.indexOf(node.attribute(attr,"").toLower());
  if (v<0)  return defValue;
  return v;
}


int qtx::readXMLIntAttr ( const QDomElement & node, QString tag,
                          QString attr, int defValue, bool & ok )  {
QDomElement category = node.firstChildElement(tag);
  if (!category.isNull())
    return readXMLIntAttr ( category,attr,defValue,ok );
  ok = false;
  return defValue;
}

qreal qtx::readXMLRealAttr ( const QDomElement & node, QString attr,
                             qreal defValue, bool & ok )  {
qreal  value;
char  *p1;
char  *p2;
bool   B = false;

  p1 = node.attribute(attr,"").toAscii().data();
  if (p1[0])  {
    value = strtod ( p1,&p2 );
    B     = (p2!=p1);
  }

  if (B)  return value;
  ok = B;
  return defValue;

}

qreal qtx::readXMLRealAttr ( const QDomElement & node, QString tag,
                             QString attr, qreal defValue, bool & ok ) {
QDomElement category = node.firstChildElement(tag);
  if (!category.isNull())
    return readXMLRealAttr ( category,attr,defValue,ok );
  ok = false;
  return defValue;
}


QString qtx::readXMLStrAttr ( const QDomElement & node, QString tag,
                              QString attr, QString defValue,
                              bool & ok )  {
QDomElement category = node.firstChildElement(tag);
  if (!category.isNull())
    return category.attribute ( attr,defValue );
  ok = false;
  return QString();
}

int qtx::readXMLInteger ( const QDomNode & node, QString tag,
                          int defValue, bool & ok )  {
QDomNode category = node.firstChildElement(tag);
int      value    = defValue;
//char    *p1;
//char    *p2;
bool     B = false;
  if (!category.isNull())  {
    category = category.firstChild();
    /*
    if ((!category.isNull()) && category.isText())  {
      p1    = category.nodeValue().trimmed().toAscii().data();
      value = (int)float(strtod(p1,&p2)+0.49999);
      B     = (p2!=p1);
    }
    */
    value = category.nodeValue().trimmed().toInt(&B);
  }
  if (B)  return value;
  ok = B;
  return defValue;
}

qreal qtx::readXMLReal ( const QDomNode & node, QString tag,
                         qreal defValue, bool & ok )  {
QDomNode category = node.firstChildElement(tag);
qreal    value;
char    *p1;
char    *p2;
bool     B = false;
  if (!category.isNull())  {
    category = category.firstChild();
    if ((!category.isNull()) && category.isText())  {
      p1    = category.nodeValue().trimmed().toAscii().data();
      value = strtod ( p1,&p2 );
      B     = (p2!=p1);
    }
//      value = category.nodeValue().toDouble(&B);
  }
  if (B)  return value;
  ok = B;
  return defValue;
}

QString qtx::readXMLString ( const QDomNode & node, QString defValue,
                             bool & ok )  {
  if (!node.isNull())  {
    QDomNode category = node.firstChild();
    if ((!category.isNull()) && category.isText())
      return category.nodeValue();
  }
  ok = false;
  return defValue;
}


QString qtx::readXMLString ( const QDomNode & node, QString tag,
                             QString defValue, bool & ok )  {
QDomNode category = node.firstChildElement(tag);
  if (!category.isNull())  {
    category = category.firstChild();
    if ((!category.isNull()) && category.isText())
      return category.nodeValue();
    else if (category.isNull())
      return QString("");
  }
  ok = false;
  return defValue;
}


bool qtx::readXMLBoolean ( const QDomNode & node, QString tag,
                           bool defValue, bool & ok )  {
QDomNode category = node.firstChildElement(tag);
  if (!category.isNull())  {
    category = category.firstChild();
    if ((!category.isNull()) && category.isText())
      return (category.nodeValue()=="yes");
  }
  ok = false;
  return defValue;
}


qreal qtx::getReal ( QString S, qreal defValue, bool & ok )  {
char *p1;
char *p2;
qreal v;
  p1 = S.trimmed().toAscii().data();
  v  = strtod ( p1,&p2 );
  if (p2==p1)  {
    ok = false;
    return defValue;
  }
  return v;
}


#ifdef QT_XMLPATTERNS_LIB
int qtx::selectDoc ( const QDomDocument & xml_in,
                     const QString & select,
                     QDomDocument & xml_out )  {
QXmlQuery  query;
QString    res;

   xml_out.clear();

   query.setFocus ( xml_in.toString() );
   query.setQuery ( select );

   if (query.isValid())  {

     query.evaluateTo ( &res );
     xml_out.setContent ( "<doc>"+res+"</doc>" );

     if (!res.isEmpty())  return 0;
     return 1;

   }

   return -1;

}
#endif

QString qtx::getHTMLString ( QString S )  {
  return S.replace ( "{","<" ).replace ( "}",">" );
}

