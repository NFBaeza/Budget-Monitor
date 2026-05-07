#include "views/mainwindow.h"
#include "./ui_mainwindow.h"
#include "widgets/dashboardwidget.h"
#include "widgets/savingwidget.h"
#include "widgets/monthreportwidget.h"
#include "widgets/creditcardwidget.h"
#include "dialogs/monthselectordialog.h"
#include "dialogs/usersettingsdialog.h"

extern QString user_id;
extern QString user_name;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    setMinimumSize(470, 300);
    ui->userNameLabel->setVisible(false);

    ui->labelTitle->setStyleSheet("font-size: 13pt");

    connect(ui->CurrentMonthButton, &QPushButton::clicked, this, &MainWindow::onCurrentMonthPressed);
    connect(ui->SavingViewButton, &QPushButton::clicked, this, &MainWindow::onSavingPressed);
    connect(ui->userSettingsButton, &QPushButton::clicked, this, &MainWindow::onUserSettingsPressed);
    connect(ui->pastMonthsButton, &QPushButton::clicked, this, &MainWindow::onPastMonthsButtonPressed);
    connect(ui->creditCardReviewButton, &QPushButton::clicked, this, &MainWindow::onCreditCardButtonPressed);
    
    UserSettingsDialog::tryAutoLogin(m_networkManager, [this](bool success) {
        if (success) {
            ui->userNameLabel->setText(user_name);
            ui->userNameLabel->setVisible(true);
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
        connect(dialog, &MonthSelectorDialog::monthClicked, this, [this](QDate date) {
            MonthView *monthView = new MonthView(date, this);
            connect(monthView, &MonthView::backButtonWasPressed, this, &MainWindow::showMainView);
            setCentralWidget(monthView);
            setMinimumSize(monthView->minimumSize());
            resize(monthView->minimumSize());
        });
        connect(dialog, &MonthSelectorDialog::closeWasPressed, this, &MainWindow::showMainView);
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
            setCentralWidget(monthReport);
            setMinimumSize(monthReport->minimumSize());
            resize(monthReport->minimumSize());
        });
        setCentralWidget(dashboard);
        setMinimumSize(dashboard->minimumSize());
        resize(dashboard->minimumSize());
    }else{
        QMessageBox::warning(this, "Error", "Please login first");
        onUserSettingsPressed();
    }
}

void MainWindow::onCreditCardButtonPressed() {
    if(!user_id.isEmpty()){
        CreditCardView *dashboard = new CreditCardView(QDate::currentDate(), this);
        connect(dashboard, &CreditCardView::backToMain, this, &MainWindow::showMainView);
        connect(dashboard, &CreditCardView::goToCurrentMonth, this, &MainWindow::onCurrentMonthPressed);
        setCentralWidget(dashboard);
        setMinimumSize(dashboard->minimumSize());
        resize(dashboard->minimumSize());
    }else{
        QMessageBox::warning(this, "Error", "Please login first");
        onUserSettingsPressed();
    }
}

void MainWindow::onCurrentMonthPressed() {
    if(!user_id.isEmpty()){
        MonthView *dashboard = new MonthView(QDate::currentDate(), this);
        connect(dashboard, &MonthView::backButtonWasPressed, this, &MainWindow::showMainView);
        connect(dashboard, &MonthView::creditcardButtonWasPressed, this, &MainWindow::onCreditCardButtonPressed);
        setCentralWidget(dashboard);
        setMinimumSize(dashboard->minimumSize());
        resize(dashboard->minimumSize());
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
    setMinimumSize(0, 0);
    resize(470, 300);

    // Reconnect the button signals
    connect(ui->CurrentMonthButton, &QPushButton::clicked, this, &MainWindow::onCurrentMonthPressed);
    connect(ui->SavingViewButton, &QPushButton::clicked, this, &MainWindow::onSavingPressed);
    connect(ui->pastMonthsButton, &QPushButton::clicked, this, &MainWindow::onPastMonthsButtonPressed);
    connect(ui->userSettingsButton, &QPushButton::clicked, this, &MainWindow::onUserSettingsPressed);
    connect(ui->creditCardReviewButton, &QPushButton::clicked, this, &MainWindow::onCreditCardButtonPressed);

    if(!user_name.isEmpty()){
        ui->userNameLabel->setText(user_name);
    }else{
        ui->userNameLabel->setText("Please Logging");
    }

}
    