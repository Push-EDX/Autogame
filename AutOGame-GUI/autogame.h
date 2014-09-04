#ifndef AUTOGAME_H
#define AUTOGAME_H

#include <QMainWindow>
#include <QMouseEvent>
#include "loading.h"

namespace Ui {
class AutOGame;
}

class AutOGame : public QMainWindow
{
    Q_OBJECT

public:
    explicit AutOGame(QWidget *parent = 0);
    ~AutOGame();

private slots:
    void on_closeWindow_clicked();

    void on_pushButton_2_pressed();

    void on_loginSend_clicked();

private:
    Ui::AutOGame *ui;

    Loading* loading;

    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    QPoint mLastMousePosition;
    bool mMoving;
};

#endif // AUTOGAME_H
