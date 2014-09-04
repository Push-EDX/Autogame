#include "loading.h"
#include "ui_loading.h"
#include <QMovie>

Loading::Loading(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Loading)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    QMovie *movie = new QMovie(":/ajax-loader.gif");
    ui->loadingLabel->setMovie(movie);
    movie->start();
}

Loading::~Loading()
{
    delete ui;
}
