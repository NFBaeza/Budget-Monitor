#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDateTime>
#include <QNetworkAccessManager>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCurrentMonthPressed();
    void onSavingPressed();
    void showMainView();
    void onPastMonthsButtonPressed();
    void onUserSettingsPressed();
    void onCreditCardButtonPressed();
    
private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *m_networkManager;
};

#endif // MAINWINDOW_H