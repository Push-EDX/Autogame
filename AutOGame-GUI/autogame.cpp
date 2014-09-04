#include "autogame.h"
#include "ui_autogame.h"
#include "loading.h"
#include "worker.h"

#include <QMessageBox>
#include <QThread>
#include <QTimer>

#include "Autogame.hpp"


AutOGame::AutOGame(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AutOGame)
{
    ui->setupUi(this);

    loading = new Loading(this);

    QStringList list=(QStringList()<<"Argentina"<<"Brazil"<<"Danmark"<<"Deutschland"<<"España"<<"France"<<"Hrvatska"<<"Italia"<<"Magyarország"<<"México"<<"Nederland"<<"Norge"<<"Polska"<<"Portugal"<<"Romania"<<"Slovenija"<<"Slovensko"<<"Suomi"<<"Sverige"<<"Türkiye"<<"USA"<<"United Kingdom"<<"Ceská Republika"<<"Greece"<<"Rusia"<<"Taiwan"<<"Japan");
    ui->locale->addItems(list);
    ui->locale->setCurrentText("España");
    ui->universe->setValidator(new QIntValidator(0, 999, this));
}

AutOGame::~AutOGame()
{
    delete ui;
}

void AutOGame::on_closeWindow_clicked()
{
    this->close();
}

void AutOGame::mouseMoveEvent(QMouseEvent* event)
{
    if( event->buttons().testFlag(Qt::LeftButton) && mMoving)
    {
        this->move(this->pos() + (event->pos() - mLastMousePosition));
    }
}

void AutOGame::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        mMoving = false;
    }
}


void AutOGame::on_pushButton_2_pressed()
{
    mMoving = true;
    mLastMousePosition = mapFromGlobal(QCursor::pos());
}

void AutOGame::on_loginSend_clicked()
{
    loading->show();

    QStringList list=(QStringList()<<"ar"<<"br"<<"dk"<<"de"<<"es"<<"fr"<<"hr"<<"it"<<"hu"<<"mx"<<"ne"<<"no"<<"pl"<<"pt"<<"ro"<<"si"<<"sk"<<"fi"<<"se"<<"tr"<<"us"<<"en"<<"cz"<<"gr"<<"ru"<<"tw"<<"jp");
    int index = ui->locale->currentIndex();

    int result = autogame::Session(0);
    if (result == LIBRARY_OK)
    {
        result = autogame::Init(list[index].toStdString().c_str(), ui->universe->text().toInt());
        if (result == LIBRARY_OK)
        {
            result = autogame::Login(ui->loginUsername->text().toStdString().c_str(),ui->loginPassword->text().toStdString().c_str());
            if (result == LIBRARY_OK)
            {
                ui->widgedPage->setCurrentWidget(ui->Supply);

                QThread* thread = new QThread;
                Worker* worker = new Worker();
                worker->moveToThread(thread);
                connect(thread, SIGNAL(started()), worker, SLOT(process()));
                thread->start();
            }
        }
    }

    loading->hide();

    if (result == ERROR_LOGIN_ERROR)
    {
        QMessageBox::warning(this, "Error", "Invalid location/universe combination.", QMessageBox::Ok, QMessageBox::NoButton);
    }
    else if (result == ERROR_INCORRECT_LOGIN)
    {
        QMessageBox::warning(this, "Error", "Wrong username or password.", QMessageBox::Ok, QMessageBox::NoButton);
    }
    else if (result == LIBRARY_ERROR)
    {
        QMessageBox::warning(this, "Error", "Internal error.", QMessageBox::Ok, QMessageBox::NoButton);
    }
}
