#include "views/mainwindow.h"
#include "widgets/dashboardwidget.h"
#include "./ui_mainwindow.h" // Archivo generado automáticamente por CMake
#include <QMessageBox>
#include <QSqlTableModel>
#include <QLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "dialogs/formdialog.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->go, &QPushButton::clicked, this, &MainWindow::alPresionarBotonGo);
}

MainWindow::~MainWindow()
{
    delete ui; // Liberamos la memoria de la interfaz
}

void MainWindow::alPresionarBotonGo() {
    MonthView *dashboard = new MonthView(this);

    connect(dashboard, &MonthView::backbutton_was_pressed, this, &MainWindow::showMainView);
    setCentralWidget(dashboard);
}

void MainWindow::showMainView() {
    QWidget *oldWidget = centralWidget();
    if (oldWidget) {
        oldWidget->deleteLater();
    }

    QWidget *newCentralWidget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(newCentralWidget);

    QPushButton *goButton = new QPushButton("Go", newCentralWidget);
    layout->addWidget(goButton);

    // Reconectar el botón
    connect(goButton, &QPushButton::clicked, this, &MainWindow::alPresionarBotonGo);

    setCentralWidget(newCentralWidget);
}