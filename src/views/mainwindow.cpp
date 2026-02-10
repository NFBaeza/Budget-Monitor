#include "views/mainwindow.h"
#include "./ui_mainwindow.h"
#include "widgets/dashboardwidget.h"
#include "widgets/savingwidget.h"
#include "dialogs/monthselectordialog.h"

extern const QString user_id = "68e6ff6b-9189-48be-a84b-03a5448e0b8b";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(500, 300);
    connect(ui->CurrentMonthButton, &QPushButton::clicked, this, &MainWindow::onCurrentMonthPressed);
    connect(ui->SavingViewButton, &QPushButton::clicked, this, &MainWindow::onSavingPressed);
    connect(ui->pastMonthsButton, &QPushButton::clicked, this, &MainWindow::onPastMonthsButtonPressed);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onPastMonthsButtonPressed() {
    MonthSelectorDialog *dialog = new MonthSelectorDialog(this);
    connect(dialog, &MonthSelectorDialog::month_clicked, this, [this](QDate date) {
        MonthView *monthView = new MonthView(date, this);
        connect(monthView, &MonthView::backbutton_was_pressed, this, &MainWindow::showMainView);
        setFixedSize(1080, 650);
        setCentralWidget(monthView);
    });
    connect(dialog, &MonthSelectorDialog::close_was_pressed, this, &MainWindow::showMainView);
    setCentralWidget(dialog);
}

void MainWindow::onSavingPressed() {
    SavingView *dashboard = new SavingView(this);
    connect(dashboard, &SavingView::backbutton_was_pressed, this, &MainWindow::showMainView);
    setFixedSize(1080, 650);
    setCentralWidget(dashboard);
}

void MainWindow::onCurrentMonthPressed() {
    MonthView *dashboard = new MonthView(QDate::currentDate(), this);
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
    connect(ui->SavingViewButton, &QPushButton::clicked, this, &MainWindow::onSavingPressed);
    connect(ui->pastMonthsButton, &QPushButton::clicked, this, &MainWindow::onPastMonthsButtonPressed);}