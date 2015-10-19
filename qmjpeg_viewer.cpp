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

    //public
    fps_max_frames(1000),
    aspect_ratio(Qt::KeepAspectRatio),

    //private
    _qlabel(NULL),
    _max_mjpeg_header_size(1024),
    _max_jpeg_size(1024*1024),

    _rx_jpeg_len("Content-Length: (\\d+)"),
    _rx_rnrn("(\\r\\n\\r\\n)"),

    _fps(0),
    _fps_num_frames(0),
    _fps_time_start(QTime::currentTime())
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


    QObject::connect(&_tcp_socket, SIGNAL(readyRead()), this, SLOT(ReadyRead_state_1()));


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



quint32 QMJpegViewer::set_max_mjpeg_header_size(quint32 size)
{
    if( size < 1024 )
        size = 1024;

    _max_mjpeg_header_size = size;

    return _max_mjpeg_header_size;
}



quint32 QMJpegViewer::set_max_jpeg_size(quint32 size)
{
    if( size < 1024 )
        size = 1024;

    _max_jpeg_size = size;

    return _max_jpeg_size;
}



void QMJpegViewer::ReadyRead_state_1()
{
    if( _tcp_socket.bytesAvailable() < _max_mjpeg_header_size)
        return;


    _buf = _tcp_socket.read(_max_mjpeg_header_size);


    int pos = _rx_jpeg_len.indexIn(_buf);

    if( pos == -1 ) //broken header
    {
        emit error(BrokenHeader);
        return;
    }


    _jpeg_size = _rx_jpeg_len.cap(1).toInt(0, 10);

    if( _jpeg_size >= (qint32)_max_jpeg_size )
    {
        emit error(MaxJpegSize);
        return;
    }


    //find Binary JPEG offset
    _jpeg_offset = _rx_rnrn.indexIn(_buf, pos);

    if( _jpeg_offset == -1 ) //broken header
    {
        emit error(CantFindJpegOffset);
        return;
    }

    _jpeg_offset += _rx_rnrn.matchedLength();


    //next state
    QObject::disconnect(&_tcp_socket, SIGNAL(readyRead()), 0, 0);
    QObject::connect(&_tcp_socket, SIGNAL(readyRead()), this, SLOT(ReadyRead_state_2()));
}



void QMJpegViewer::ReadyRead_state_2()
{
    if( _tcp_socket.bytesAvailable() < _jpeg_size)
        return;


    _buf = _buf.mid(_jpeg_offset);


    //(_max_jpeg_header_size - _jpeg_offset) - the size of which is already reading
    _buf += _tcp_socket.read(_jpeg_size - (_max_mjpeg_header_size - _jpeg_offset));


    if(_pixmap.loadFromData((const uchar *)_buf.constData(), _jpeg_size, "JPEG"))
    {
        _refresh_qlabel();
    }
    else
    {
        emit error(CantLoadJpeg);
    }


    //next state
    QObject::disconnect(&_tcp_socket, SIGNAL(readyRead()), 0, 0);
    QObject::connect(&_tcp_socket, SIGNAL(readyRead()), this, SLOT(ReadyRead_state_1()));
}



void QMJpegViewer::_calculate_fps()
{
    if( !fps_max_frames || (_fps_num_frames++ < fps_max_frames) )
        return;

    _fps = fps_max_frames / (float)_fps_time_start.secsTo(QTime::currentTime());

    _fps_time_start = QTime::currentTime();
    _fps_num_frames = 0;

    emit updated_fps(_fps);
}



void QMJpegViewer::_refresh_qlabel()
{
    if(!_qlabel)
        return;


    _qlabel_mutex.lock();

    if( _qlabel )
    {
        _qlabel->setPixmap(_pixmap.scaled(_qlabel->size(), aspect_ratio));
    }

    _qlabel_mutex.unlock();


    _calculate_fps();
}
