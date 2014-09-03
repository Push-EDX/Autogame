#include "autogame.h"
#include "ui_autogame.h"

AutOGame::AutOGame(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AutOGame)
{
    ui->setupUi(this);
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
