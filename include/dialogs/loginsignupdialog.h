#ifndef LOGINSIGNINDIALOG_H
#define LOGINSIGNINDIALOG_H

#include "database/databasemanager.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTextEdit>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelation>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>


namespace Ui { class LogInSignUpDialog; }

class LogInSignUpDialog : public QDialog {
    Q_OBJECT

public:
    explicit LogInSignUpDialog(QWidget *parent = nullptr);
    ~LogInSignUpDialog();

private:
    Ui::LogInSignUpDialog *ui;
    QNetworkAccessManager *m_networkManager;
    QString m_supabaseUrl;
    QString m_supabaseAnonKey;

    
    void checkSigninPassword();
    void onAcceptButton();
    void signinWithSupabase(const QString &name, const QString &email, const QString &password, const QDate &birthday);
    void loginWithSupabase(const QString &email, const QString &password);
    void handleAuthResponse(const QString requestType ,QNetworkReply *reply);
};

#endif