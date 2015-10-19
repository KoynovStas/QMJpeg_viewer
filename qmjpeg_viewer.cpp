/*
 * qmjpeg_viewer.cpp
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


#include "qmjpeg_viewer.h"





QMJpegViewer::QMJpegViewer(QObject *parent) :
    QObject(parent),

    //private
    _qlabel(NULL)
{
    qRegisterMetaType<QMJpegViewer::MJpegViewerError>("QMJpegViewer::MJpegViewerError");

    set_socket_size(4*1024*1024);
}



QMJpegViewer::~QMJpegViewer()
{
    disconnect_from_host();
    set_qlabel(NULL);
}



void QMJpegViewer::connect_to_host(const QString &host_name, quint16 host_port)
{
    QObject::connect(&_tcp_socket, SIGNAL(connected()), this, SIGNAL(connected()));


    QObject::connect(&_tcp_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));


    QObject::connect(&_tcp_socket, SIGNAL(error(QAbstractSocket::SocketError)),
                     this, SLOT(proxy_socket_error()));


    _tcp_socket.connectToHost(host_name, host_port);
}



void QMJpegViewer::disconnect_from_host()
{
    //disconnect all signals and slots for _tcp_socket
    QObject::disconnect(&_tcp_socket, 0, 0, 0);

    _tcp_socket.close();

    emit disconnected();
}



int QMJpegViewer::send_request(const QByteArray &request)
{
    if( request.length() == 0 )
        return -1;


    return _tcp_socket.write((char *)request.constData(), request.length());
}



void QMJpegViewer::set_qlabel(QLabel *qlabel)
{
    _qlabel_mutex.lock();
    _qlabel = qlabel;
    _qlabel_mutex.unlock();
}
