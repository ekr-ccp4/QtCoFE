// $Id: qtx_dnlmanager.h $
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
//  **** Module  :  qtx_dnlmanager <interface>
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

#ifndef qtx_DOWNLOADMANAGER_H
#define qtx_DOWNLOADMANAGER_H

#include <QFile>
#include <QObject>
#include <QQueue>
#include <QTime>
#include <QUrl>
#include <QNetworkAccessManager>

QT_BEGIN_NAMESPACE
class QCryptographicHash;
QT_END_NAMESPACE

namespace qtx  {

  struct DownloadData  {

    enum {Starting,WriteError,Progress,Finished,Failed,Succeeded,
          Cancelled,Unknown};

    int     status;  // one of the above constants
    int     filesDownloaded;
    int     filesTotal;
    qint64  bytesReceived;
    qint64  bytesTotal;
    double  speed;
    int     networkError; // NetworkError code
    QString url;
    QString fileName;
    QString error;
    QString md5;         // valid only in the end
    bool    proxyError;  // true if proxy settings may be the reason

    void    init();
    void    setError ( QNetworkReply *download, bool overwrite=false );
    QString makeErrorMessage ( const QString & pgmName );
    void    makeErrorMessage ( QWidget *parent, const QString & pgmName );

  };


  class DownloadManager : public QObject {
  Q_OBJECT

    public:

      enum { DOWNLOAD_File,DOWNLOAD_Buffer };

      DownloadManager ( QObject *parent = 0 );
      ~DownloadManager();

      static void    detectProxy(); // automatic attempt, no guarantees
      static QString getProgressString ( double speed,
                                         qint64 bytesRemained=-1 );

      void setDestination ( int dest );
      int  getDestination () { return destination; }

      void setOutputDir ( QString dir    );
      void setOverwrite ( bool overwrite );

      void download ( const QUrl &url );
      void download ( const QStringList &urlList );

      QByteArray & getBuffer() { return buffer; }

      void cancelDownload();

    protected:
      QString saveFileName ( const QUrl &url );
      QString getMD5();  // after download finishes

    signals:
      void finished ();
      void progress ( qtx::DownloadData dnlData );

    private slots:
      void startNextDownload();
      void downloadProgress ( qint64 bytesReceived, qint64 bytesTotal );
      void downloadFinished ();
      void downloadReadyRead();

      void proxyAuthenticationRequired ( const QNetworkProxy & proxy,
                                       QAuthenticator * authenticator );
      void authenticationRequired      ( QNetworkReply * reply,
                                       QAuthenticator * authenticator );

    private:
      QNetworkAccessManager manager;
      QCryptographicHash   *checkSum;
      QQueue<QUrl>          downloadQueue;
      QNetworkReply        *currentDownload;
      QFile                 output;
      QTime                 downloadTime;
      QString               outDir;
      QByteArray            buffer;
      int                   destination;
      DownloadData          downloadData;
      bool                  allowOverwrite;

      int downloadedCount;
      int totalCount;

  };

}

#endif  // qtx_DOWNLOADMANAGER_H
