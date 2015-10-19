#include "mainwindow.h"
#include "ui_mainwindow.h"





MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mjpeg.set_qlabel(ui->mjpeg_label);


    QObject::connect(&mjpeg,  SIGNAL(connected()),
                     this, SLOT(change_status()));


    QObject::connect(&mjpeg,  SIGNAL(connected()),
                     this, SLOT(send_request()));


    QObject::connect(&mjpeg,  SIGNAL(disconnected()),
                     this, SLOT(change_status()));


    QObject::connect(ui->connect_button, SIGNAL(clicked()),
                     this,  SLOT(connect_btn_clicked()));


    QObject::connect(ui->ratio_combo_box, SIGNAL(currentIndexChanged(int)),
                     this,  SLOT(set_ratio()));
}



MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::connect_btn_clicked()
{
    if( mjpeg.state() == QAbstractSocket::UnconnectedState )
        connect_to_dev();
    else
        disconnect_from_dev();
}



void MainWindow::connect_to_dev()
{
    ui->connect_button->setEnabled(false);

    mjpeg.connect_to_host(ui->IP_line_edit->text(), ui->port_spin_box->value());
}



void MainWindow::disconnect_from_dev()
{
    ui->connect_button->setEnabled(false);

    mjpeg.disconnect_from_host();
}



void MainWindow::send_request()
{
    mjpeg.send_request(ui->request_edit->text().toLatin1());
}



void MainWindow::set_ratio()
{
    switch(ui->ratio_combo_box->currentIndex())
    {
        case Qt::IgnoreAspectRatio:
            mjpeg.aspect_ratio = Qt::IgnoreAspectRatio;
            break;

        case Qt::KeepAspectRatio:
            mjpeg.aspect_ratio = Qt::KeepAspectRatio;
            break;

        case Qt::KeepAspectRatioByExpanding:
            mjpeg.aspect_ratio = Qt::KeepAspectRatioByExpanding;
            break;

        default:
            mjpeg.aspect_ratio = Qt::KeepAspectRatio;
            break;
    }
}



void MainWindow::change_status()
{

    ui->connect_button->setEnabled(true);


    if( mjpeg.state() == QAbstractSocket::ConnectedState )
    {
        ui->statusBar->showMessage(QString("Connected"));
        ui->connect_button->setText("Disconnect");
        return;
    }


    ui->statusBar->showMessage(QString("Disconnected"));
    ui->connect_button->setText("Connect");
}
