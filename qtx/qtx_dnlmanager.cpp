// $Id: qtx_dnlmanager.cpp $
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
//  **** Module  :  qtx_dnlmanager <implementation>
//       ~~~~~~~~~
//  **** Project :  QtLibs
//       ~~~~~~~~~
//  **** Classes :  qtx::DownloadData
//       ~~~~~~~~~  qtx::DownloadManager
//
//  (C) E. Krissinel 2012-2013
//
// =================================================================
//

#include "qtx_dnlmanager.h"

#include <QFileInfo>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkProxyFactory>
#include <QtNetwork/QAuthenticator>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QTime>
#include <stdio.h>


void qtx::DownloadData::init()  {
  status          = -1;
  filesDownloaded =  0;
  filesTotal      =  0;
  bytesReceived   =  0;
  bytesTotal      =  0;
  speed           = 0.0;
  networkError    = QNetworkReply::NoError;
  proxyError      = false;
  url     .clear ();
  fileName.clear ();
  error   .clear ();
}

void qtx::DownloadData::setError ( QNetworkReply *download,
                                   bool overwrite )  {
  if (download)  {
    if (overwrite || (networkError==QNetworkReply::NoError))
      networkError = download->error();
  }
  proxyError = (networkError==QNetworkReply::TimeoutError)      ||
               (networkError==QNetworkReply::HostNotFoundError) ||
               (networkError==QNetworkReply::ConnectionRefusedError);
}

QString qtx::DownloadData::makeErrorMessage ( const QString & pgmName ) {
  return QString (
     pgmName + " has encountered a network problem:"
     "<h3><i>") + error + QString (
     " </i></h3>"
     "This is likely to be due to an intermittent<br>"
     "unavailability of one of the web-servers.<br><br>"
     "<i>Please try again later on.</i>");
}

void qtx::DownloadData::makeErrorMessage ( QWidget *parent,
                                           const QString & pgmName )  {
  if (parent)
    QMessageBox::critical ( parent,
       "Connection error",makeErrorMessage(pgmName) );
  else
    printf ( " +++ Connection error: %s\n",
             makeErrorMessage(pgmName).toAscii().constData() );
}



qtx::DownloadManager::DownloadManager ( QObject *parent )
                    : QObject(parent),
                      downloadedCount(0), totalCount(0)  {

  allowOverwrite  = true;
  currentDownload = NULL;
  destination     = DownloadManager::DOWNLOAD_File;

  checkSum = new QCryptographicHash ( QCryptographicHash::Md5 );

  downloadData.init();

  connect (
    &manager,
    SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
    this,
    SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
    Qt::DirectConnection );

  connect (
    &manager,
    SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
    this,
    SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)),
    Qt::DirectConnection );

}

qtx::DownloadManager::~DownloadManager()  {
  delete checkSum;
}

void qtx::DownloadManager::setDestination ( int dest )  {
  destination = dest;
}


void qtx::DownloadManager::detectProxy()  {
// automatic attempt, no guarantees
#ifdef Q_OS_LINUX
bool ok;
  // This is definitely required with QT 4.7.X.
  // Expected to work on most linuxes.
  QString var   ( getenv("http_proxy")      );
  QRegExp regex ( "(http://)?(.*):(\\d*)/?" );
//  QMessageBox::information(NULL,"proxy",var);
  int pos = regex.indexIn(var);
  if (pos>-1)  {
    QString host = regex.cap(2);
    if (!host.endsWith(".pac",Qt::CaseInsensitive))  {
      QString user;
      QString password;
      int port = regex.cap(3).toInt(&ok);
      if (ok)  {
        QStringList lst = host.split ( '@',QString::KeepEmptyParts );
        if (lst.size()>1)  {
          host = lst.at(1);
          QStringList lst1 = lst.at(0).split ( ':',
                                           QString::KeepEmptyParts );
          user = lst1.at(0);
          if (lst1.size()>1)
            password = lst1.at(1);
        }
        QNetworkProxy proxy ( QNetworkProxy::HttpProxy,host,port,
                              user,password );
        QNetworkProxy::setApplicationProxy ( proxy );
      }
    }
  }
#else
  // should work fine for Macs and Windows
  QNetworkProxyFactory::setUseSystemConfiguration ( true );
#endif
}

void qtx::DownloadManager::proxyAuthenticationRequired (
                                    const QNetworkProxy & proxy,
                                    QAuthenticator * authenticator )  {
  authenticator->setUser     ( proxy.user()     );
  authenticator->setPassword ( proxy.password() );
}

void qtx::DownloadManager::authenticationRequired (
                                    QNetworkReply * reply,
                                    QAuthenticator * authenticator )  {

  if (reply->manager())  {
    authenticator->setUser     ( reply->manager()->proxy().user()     );
    authenticator->setPassword ( reply->manager()->proxy().password() );
  } else  {
    authenticator->setUser ( QNetworkProxy::applicationProxy().user() );
    authenticator->setPassword (
                         QNetworkProxy::applicationProxy().password() );
  }

}

void qtx::DownloadManager::setOutputDir ( QString dir )  {
  outDir = dir;
  if ((!outDir.isEmpty()) && (!outDir.endsWith('/')))
    outDir = outDir + "/";
}

void qtx::DownloadManager::setOverwrite ( bool overwrite )  {
  allowOverwrite = overwrite;
}

void qtx::DownloadManager::download ( const QStringList &urlList )  {

  foreach (QString url, urlList)
    download ( QUrl::fromEncoded(url.toLocal8Bit()) );

  if (downloadQueue.isEmpty())
    QTimer::singleShot ( 0, this, SIGNAL(finished()) );

}

void qtx::DownloadManager::download ( const QUrl &url )  {

  if (downloadQueue.isEmpty())
    QTimer::singleShot ( 0,this,SLOT(startNextDownload()) );

  downloadQueue.enqueue ( url );
  totalCount++;

}

QString qtx::DownloadManager::saveFileName ( const QUrl &url )  {
QString path     = url.path();
QString basename = QFileInfo(path).fileName();
int     i;

  if (basename.isEmpty())
    basename = "download";

  basename = outDir + basename;

  if (QFile::exists(basename) && (!allowOverwrite))  {
    // already exists, don't overwrite
    i = 0;
    basename += '.';
    while (QFile::exists(basename+QString::number(i)))
      i++;
    basename += QString::number(i);
  }

  return basename;

}

void qtx::DownloadManager::startNextDownload()  {

  if (downloadQueue.isEmpty())  {
    downloadData.status          = DownloadData::Finished;
    downloadData.filesDownloaded = downloadedCount;
    downloadData.filesTotal      = totalCount;
    downloadData.setError ( currentDownload );
//    downloadData.md5.clear();
    emit progress ( downloadData );
    emit finished();
    return;
  }

  QUrl url = downloadQueue.dequeue();

  checkSum->reset();
  downloadData.md5.clear();

  if (destination==DownloadManager::DOWNLOAD_File)  {
    QString filename = saveFileName(url);
    output.setFileName(filename);
    if (!output.open(QIODevice::WriteOnly)) {
      downloadData.status   = DownloadData::WriteError;
      downloadData.fileName = filename;
      downloadData.url      = url.toEncoded();
      downloadData.setError ( currentDownload,true );
      downloadData.error    = output.errorString();
      emit progress ( downloadData );
//      QTimer::singleShot ( 0,this,SLOT(startNextDownload()) );
      return;                 // skip this download
    }
  }

  buffer.clear();

  QNetworkRequest request(url);
  currentDownload = manager.get(request);

  connect ( currentDownload, SIGNAL(downloadProgress(qint64,qint64)),
            SLOT(downloadProgress(qint64,qint64)));

  connect ( currentDownload, SIGNAL(finished()),
            SLOT(downloadFinished()));

  connect ( currentDownload, SIGNAL(readyRead()),
            SLOT(downloadReadyRead()));

  // prepare the output
  downloadData.status = DownloadData::Starting;
  downloadData.url    = url.toEncoded();
  downloadData.setError ( currentDownload,true );
  emit progress ( downloadData );
  downloadTime.start();

}

void qtx::DownloadManager::downloadProgress ( qint64 bytesReceived,
                                              qint64 bytesTotal )  {

  downloadData.status        = DownloadData::Progress;
  downloadData.bytesReceived = bytesReceived;
  downloadData.bytesTotal    = bytesTotal;
  downloadData.speed         = bytesReceived * 1000.0 /
                               downloadTime.elapsed();

  downloadData.setError ( currentDownload );

  emit progress ( downloadData );

  /*
  progressBar.setStatus(bytesReceived, bytesTotal);

  // calculate the download speed
  double speed = bytesReceived * 1000.0 / downloadTime.elapsed();
  QString unit;
  if (speed < 1024) {
    unit = "bytes/sec";
  } else if (speed < 1024*1024) {
    speed /= 1024;
    unit = "kB/s";
  } else {
    speed /= 1024*1024;
    unit = "MB/s";
  }

  progressBar.setMessage(QString::fromLatin1("%1 %2")
                         .arg(speed, 3, 'f', 1).arg(unit));
  progressBar.update();
  */

}

void qtx::DownloadManager::downloadFinished()  {

  if (destination==DownloadManager::DOWNLOAD_File)
    output.close();

  if (currentDownload)  {

    if (currentDownload->error()) {
      // download failed
      downloadData.status = DownloadData::Failed;
      downloadData.error  = currentDownload->errorString();
 //fprintf(stderr, "Failed: %i %s\n",currentDownload->error(),
 //        qPrintable(currentDownload->errorString()));
     } else {
      downloadData.status = DownloadData::Succeeded;
//    printf("Succeeded.\n");
      downloadedCount++;
    }

    downloadData.setError ( currentDownload );

  } else
    downloadData.status = DownloadData::Unknown;

  downloadData.md5 = getMD5();

  emit progress ( downloadData );

  currentDownload->deleteLater();
  currentDownload = NULL;
  startNextDownload();

}

void qtx::DownloadManager::downloadReadyRead()  {
QByteArray chunk = currentDownload->readAll();
  checkSum->addData ( chunk );
  if (destination==DownloadManager::DOWNLOAD_File)  {
    if (output.isOpen())
      output.write  ( chunk );
  } else
    buffer.append ( chunk );
}

/*
void qtx::DownloadManager::downloadReadyRead()  {
  if (destination==DownloadManager::DOWNLOAD_File)  {
    if (output.isOpen())
      output.write ( currentDownload->readAll() );
  } else
    buffer.append ( currentDownload->readAll() );
}
*/

QString qtx::DownloadManager::getMD5()  {
// after download finishes
  return checkSum->result().toHex().toUpper();
}

void qtx::DownloadManager::cancelDownload()  {
  if (currentDownload)  {
    delete currentDownload;
    currentDownload = NULL;
  }
  if (destination==DownloadManager::DOWNLOAD_File)  {
    if (output.isOpen())  {
      output.close();
      output.remove();
    }
  }
  buffer.clear();
  downloadData.status = DownloadData::Cancelled;
  downloadData.md5    = getMD5();
  emit progress ( downloadData );
}


QString qtx::DownloadManager::getProgressString (
                             double speed, qint64 bytesRemained )  {
QString S;
double  q = speed;

  if (q<1024.0) {
    S  = "bytes/sec";
  } else if (q<1024.0*1024.0) {
    q /= 1024;
    S  = "kB/s";
  } else {
    q /= 1024.0*1024.0;
    S  = "MB/s";
  }

  S = QString::fromLatin1("%1 ").arg(q,3,'f',1) + S;

  if ((bytesRemained>=0) && (speed>0.0))  {
    QTime t(0,0,0,0);
    int secs = int(bytesRemained/speed);
    S = "time remained: " + t.addSecs(secs).toString() + " at " + S;
  }

  return S;

}
