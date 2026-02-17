#include "views/mainwindow.h"
#include "./ui_mainwindow.h"
#include "widgets/dashboardwidget.h"
#include "widgets/savingwidget.h"
#include "widgets/monthreportwidget.h"
#include "dialogs/monthselectordialog.h"
#include "dialogs/usersettingsdialog.h"

extern QString user_id;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    setMinimumSize(380, 300);

    connect(ui->CurrentMonthButton, &QPushButton::clicked, this, &MainWindow::onCurrentMonthPressed);
    connect(ui->SavingViewButton, &QPushButton::clicked, this, &MainWindow::onSavingPressed);
    connect(ui->userSettingsButton, &QPushButton::clicked, this, &MainWindow::onUserSettingsPressed);
    connect(ui->pastMonthsButton, &QPushButton::clicked, this, &MainWindow::onPastMonthsButtonPressed);

    UserSettingsDialog::tryAutoLogin(m_networkManager, [this](bool success) {
        if (success) {
            qDebug() << "Session restored from saved token";
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onUserSettingsPressed() {
    UserSettingsDialog *dialog = new UserSettingsDialog(this);
    connect(dialog, &UserSettingsDialog::accepted, this, &MainWindow::showMainView);
    connect(dialog, &UserSettingsDialog::rejected, this, &MainWindow::showMainView);
    connect(dialog, &UserSettingsDialog::backToMain, this, &MainWindow::showMainView);
    setCentralWidget(dialog);
}

void MainWindow::onPastMonthsButtonPressed() {
    if(!user_id.isEmpty()){
        MonthSelectorDialog *dialog = new MonthSelectorDialog(this);
        connect(dialog, &MonthSelectorDialog::month_clicked, this, [this](QDate date) {
            MonthView *monthView = new MonthView(date, this);
            connect(monthView, &MonthView::backbutton_was_pressed, this, &MainWindow::showMainView);
            setMinimumSize(1080, 650);
            setCentralWidget(monthView);
        });
        connect(dialog, &MonthSelectorDialog::close_was_pressed, this, &MainWindow::showMainView);
        setCentralWidget(dialog);
    }else{
        QMessageBox::warning(this, "Error", "Please login first");
        onUserSettingsPressed();
    } 
}

void MainWindow::onSavingPressed() {
    if(!user_id.isEmpty()){
        SavingView *dashboard = new SavingView(this);
        connect(dashboard, &SavingView::backbutton_was_pressed, this, &MainWindow::showMainView);
        connect(dashboard, &SavingView::monthSelected, this, [this](QDate date) {
            MonthReport *monthReport = new MonthReport(date, this);
            connect(monthReport, &MonthReport::backToSavingButtonWasPressed, this, &MainWindow::onSavingPressed);
            connect(monthReport, &MonthReport::backToMenuButtonWasPressed, this, &MainWindow::showMainView);
            resize(660, 750);
            setCentralWidget(monthReport);
        });
        resize(1080, 650);
        setCentralWidget(dashboard);
    }else{
        QMessageBox::warning(this, "Error", "Please login first");
        onUserSettingsPressed();
    }   
}

void MainWindow::onCurrentMonthPressed() {
    if(!user_id.isEmpty()){
        MonthView *dashboard = new MonthView(QDate::currentDate(), this);
        connect(dashboard, &MonthView::backbutton_was_pressed, this, &MainWindow::showMainView);
        resize(1080, 650);
        setCentralWidget(dashboard);
    }else{
        QMessageBox::warning(this, "Error", "Please login first");
        onUserSettingsPressed();
    }
}

void MainWindow::showMainView() {
    QWidget *oldWidget = centralWidget();
    if (oldWidget) {
        oldWidget->deleteLater();
    }

    // Restore the UI from the .ui file
    ui->setupUi(this);

    // Restore the original window size
    resize(500, 230);

    // Reconnect the button signals
    connect(ui->CurrentMonthButton, &QPushButton::clicked, this, &MainWindow::onCurrentMonthPressed);
    connect(ui->SavingViewButton, &QPushButton::clicked, this, &MainWindow::onSavingPressed);
    connect(ui->pastMonthsButton, &QPushButton::clicked, this, &MainWindow::onPastMonthsButtonPressed);
    connect(ui->userSettingsButton, &QPushButton::clicked, this, &MainWindow::onUserSettingsPressed);
}
    