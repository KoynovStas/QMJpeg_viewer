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
#include <QTcpSocket>
#include <QByteArray>





class QMJpegViewer : public QObject
{
    Q_OBJECT


    public:


        explicit QMJpegViewer(QObject *parent = 0);
                ~QMJpegViewer();


        void  connect_to_host(const QString &host_name, quint16 host_port);
        void  disconnect_from_host();

        int   send_request(const QByteArray &request);

        void  set_qlabel(QLabel *qlabel);



    signals:

        void connected();
        void disconnected();



    private:

        QMutex      _qlabel_mutex;
        QLabel     *_qlabel;
        QTcpSocket  _tcp_socket;
};





#endif // QMJPEG_VIEWER_H
