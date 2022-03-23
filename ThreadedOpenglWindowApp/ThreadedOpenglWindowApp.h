#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ThreadedOpenglWindowApp.h"
#include <QWindow>

class ThreadedOpenglWindowApp : public QMainWindow
{
    Q_OBJECT

public:
    ThreadedOpenglWindowApp(QWidget *parent = Q_NULLPTR);

private:
    Ui::ThreadedOpenglWindowAppClass ui;
    QWindow* _glwin{ nullptr };
    QWidget* _cont{ nullptr };
};
