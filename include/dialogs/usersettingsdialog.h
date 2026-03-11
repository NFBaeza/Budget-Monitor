#ifndef USERSETTINGSCLASS_H
#define USERSETTINGSCLASS_H

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
#include <QSettings>


namespace Ui { class userSettingsDialog; }

class UserSettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit UserSettingsDialog(QWidget *parent = nullptr);
    ~UserSettingsDialog();

    static void tryAutoLogin(QNetworkAccessManager *manager,
                             std::function<void(bool success)> callback);
    static void logout();

signals:
    void backToMain();
    void userNameChanged(const QString &name);

private:
    Ui::userSettingsDialog *ui;
    QNetworkAccessManager *m_networkManager;
    QString m_supabaseUrl;
    QString m_supabaseAnonKey;

    void checkSigninPassword();
    void onAcceptButton();
    void signinWithSupabase(const QString &name, const QString &email, const QString &password, const QDate &birthday);
    void loginWithSupabase(const QString &email, const QString &password);
    void handleAuthResponse(const QString requestType, QNetworkReply *reply);
    void updateEmailInSupabase(const QString &newEmail);
    void updateNameInSupabase(const QString &newName);
    void onChangeNameButtonPressed();
    void onChangeEmailButtonPressed();
    void onEraseAccountButtonPressed();
    QNetworkReply* makeAuthenticatedPut(const QJsonObject &body);
};

#endif