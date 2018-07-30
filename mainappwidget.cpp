#include "mainappwidget.h"
#include "ui_mainappwidget.h"

MainAppWidget::MainAppWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainAppWidget)
{
    ui->setupUi(this);
}

MainAppWidget::~MainAppWidget()
{
    delete ui;
}
