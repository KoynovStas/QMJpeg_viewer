/*
 * qmjpeg_viewer.h
 *
 *
 * Copyright (c) 2015, Koynov Stas - skojnov@yandex.ru
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1 Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  2 Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  3 Neither the name of the <organization> nor the
 *    names of its contributors may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef QMJPEG_VIEWER_H
#define QMJPEG_VIEWER_H


#include <QObject>
#include <QMutex>
#include <QLabel>
#include <QRegExp>
#include <QPixmap>
#include <QDateTime>
#include <QTcpSocket>
#include <QByteArray>
#include <QMetaType>





class QMJpegViewer : public QObject
{
    Q_OBJECT
    Q_ENUMS(MJpegViewerError)


    public:

        enum MJpegViewerError
        {
            InnerSocketError,   //for detail see socket_error()
            BrokenHeader,
            MaxJpegSize,
            CantFindJpegOffset,
            CantLoadJpeg,

            UnknownError = -1
        };


        explicit QMJpegViewer(QObject *parent = 0);
                ~QMJpegViewer();


        quint32 fps_max_frames;
        Qt::AspectRatioMode aspect_ratio; //for *_qlabel see _refresh_qlabel()


        void  connect_to_host(const QString &host_name, quint16 host_port);
        void  disconnect_from_host();

        int   send_request(const QByteArray &request);

        void  set_qlabel(QLabel *qlabel);
        float get_fps() { return _fps; }

        quint32 set_max_mjpeg_header_size(quint32 size);
        quint32 get_max_mjpeg_header_size() { return _max_mjpeg_header_size; }

        quint32 set_max_jpeg_size(quint32 size);
        quint32 get_max_jpeg_size() { return _max_jpeg_size; }

        void  set_socket_size(qint32 size) { _tcp_socket.setReadBufferSize(size);}

        QAbstractSocket::SocketError socket_error() {return _tcp_socket.error();}
        QAbstractSocket::SocketState state() {return _tcp_socket.state();}



    signals:

        void connected();
        void disconnected();
        void updated_fps(float fps);
        void error(QMJpegViewer::MJpegViewerError);



    protected slots:

        void proxy_socket_error(){ emit error(InnerSocketError); }
        void ReadyRead_state_1();
        void ReadyRead_state_2();



    private:

        QMutex      _qlabel_mutex;
        QLabel     *_qlabel;
        QPixmap     _pixmap;
        QTcpSocket  _tcp_socket;
        QByteArray  _buf;

        quint32     _max_mjpeg_header_size;
        quint32     _max_jpeg_size;

        qint32      _jpeg_size;
        qint32      _jpeg_offset;

        QRegExp     _rx_jpeg_len;  //for find Content-Length:
        QRegExp     _rx_rnrn;      //for find \r\n\r\n

        float       _fps;
        quint32     _fps_num_frames;
        QTime       _fps_time_start;


        void  _calculate_fps();
        void  _refresh_qlabel();
};

Q_DECLARE_METATYPE(QMJpegViewer::MJpegViewerError)





#endif // QMJPEG_VIEWER_H
