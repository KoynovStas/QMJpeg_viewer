#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qmjpeg_viewer.h"





namespace Ui
{
    class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:

        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();


    public slots:

        void connect_btn_clicked();
        void connect_to_dev();
        void disconnect_from_dev();

        void send_request();
        void set_ratio();
        void set_fps_frames();

        void updated_fps(float new_fps);
        void change_status();

        void process_error(QMJpegViewer::MJpegViewerError error);


    private:

        Ui::MainWindow *ui;

        QMJpegViewer    mjpeg;
};



#endif // MAINWINDOW_H
