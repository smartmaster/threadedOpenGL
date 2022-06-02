#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ThreadedOpenglWindowApp.h"
#include <QWindow>

class SmlThreadedOpenglMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    SmlThreadedOpenglMainWindow(QWidget *parent = Q_NULLPTR);


public slots:
    void OnPushButtonTestOthoClicked();

private:
    Ui::ThreadedOpenglWindowAppClass ui;
    QWindow* _glwin{ nullptr };
    QWidget* _cont{ nullptr };
};
