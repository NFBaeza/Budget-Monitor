#include "views/mainwindow.h"
#include "./ui_mainwindow.h"
#include "widgets/dashboardwidget.h"
#include "widgets/savingwidget.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(500, 230);
    connect(ui->CurrentMonthButton, &QPushButton::clicked, this, &MainWindow::onCurrentMonthPressed);
    connect(ui->SavingsViewButton, &QPushButton::clicked, this, &MainWindow::onSavingPressed);
}

MainWindow::~MainWindow()
{
    delete ui; // Liberamos la memoria de la interfaz
}

void MainWindow::onSavingPressed() {
    QDate currentDate = QDate::currentDate();
    SavingView *dashboard = new SavingView(currentDate, this);
    connect(dashboard, &SavingView::backbutton_was_pressed, this, &MainWindow::showMainView);
    setFixedSize(1080, 650);
    setCentralWidget(dashboard);
}

void MainWindow::onCurrentMonthPressed() {
    QDate currentDate = QDate::currentDate();
    MonthView *dashboard = new MonthView(currentDate, this);
    connect(dashboard, &MonthView::backbutton_was_pressed, this, &MainWindow::showMainView);
    setFixedSize(1080, 650);
    setCentralWidget(dashboard);
}

void MainWindow::showMainView() {
    QWidget *oldWidget = centralWidget();
    if (oldWidget) {
        oldWidget->deleteLater();
    }

    // Restore the UI from the .ui file
    ui->setupUi(this);

    // Restore the original window size
    setFixedSize(500, 230);

    // Reconnect the button signals
    connect(ui->CurrentMonthButton, &QPushButton::clicked, this, &MainWindow::onCurrentMonthPressed);
    connect(ui->SavingsViewButton, &QPushButton::clicked, this, &MainWindow::onSavingPressed);
}