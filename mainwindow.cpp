#include "mainwindow.h"
#include "ui_mainwindow.h"





MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mjpeg.set_qlabel(ui->mjpeg_label);


    QObject::connect(ui->connect_button, SIGNAL(clicked()),
                     this,  SLOT(connect_btn_clicked()));
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
