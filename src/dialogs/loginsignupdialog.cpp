#include "dialogs/loginsignupdialog.h"
#include "database/databaseworker.h"
#include "./ui_loginsignupdialog.h"
#include <QNetworkRequest>
#include <QSslSocket>
#include <QUrl>

extern QString user_id = "";

LogInSignUpDialog::LogInSignUpDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LogInSignUpDialog)
    , m_networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    ui->logInSignUpWidget->setCurrentWidget(ui->logInWidget);

    QMap<QString, QString> env = DatabaseManager::loadEnvFile();
    m_supabaseUrl = env.value("SUPABASE_URL");
    m_supabaseAnonKey = env.value("SUPABASE_ANON_KEY");

    connect(ui->signUpButton, &QPushButton::clicked, this, [this]() {
        ui->logInSignUpWidget->setCurrentWidget(ui->signUpWidget);
    });
    connect(ui->logInButton, &QPushButton::clicked, this, [this]() {
        ui->logInSignUpWidget->setCurrentWidget(ui->logInWidget);
    });

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &LogInSignUpDialog::onAcceptButton);
    ui->warningPasswordLabel->setVisible(false);

    connect(ui->passwordUserSignUpText, &QLineEdit::editingFinished, this, &LogInSignUpDialog::checkSigninPassword);
    connect(ui->passwordUserSignUpText_2, &QLineEdit::editingFinished, this, &LogInSignUpDialog::checkSigninPassword);
}

LogInSignUpDialog::~LogInSignUpDialog() {
    delete ui;
}

void LogInSignUpDialog::checkSigninPassword(){
    QString password = ui->passwordUserSignUpText_2->text();
    QString password2 = ui->passwordUserSignUpText->text();

    if (password != password2){
        ui->warningPasswordLabel->setVisible(true);
    }else{
        ui->warningPasswordLabel->setVisible(false);
    }
}

void LogInSignUpDialog::onAcceptButton(){
    if(ui->logInSignUpWidget->currentWidget() == ui->logInWidget){
        QString email = ui->userEmailLogInText->text().trimmed();
        QString password = ui->passwordLogInText->text();

        if(email.isEmpty() || password.isEmpty()){
            QMessageBox::warning(this, "Error", "Please fill all fields");
            return;
        }

        loginWithSupabase(email, password);

    }else{
        QString email = ui->userEmailSignUpText->text().trimmed();
        QString password = ui->passwordUserSignUpText->text();
        QString userName = ui->userNameText->text().trimmed();
        QDate birthday = ui->birthDateUSerSignUpDate->date();
        QDate today = QDate::currentDate();

        if (birthday.addYears(18) > today) {
            QMessageBox::warning(this, "Error", "You must be at least 18 years old");
            return;
        }

        if(email.isEmpty() || password.isEmpty() || userName.isEmpty()){
            QMessageBox::warning(this, "Error", "Please fill all fields");
            return;
        }

        signinWithSupabase(userName, email, password, birthday);
        qDebug() << "Sign up:" << userName << email;
    }
}

void LogInSignUpDialog::loginWithSupabase(const QString &email, const QString &password) {
    QUrl url(m_supabaseUrl + "/auth/v1/token?grant_type=password");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("apikey", m_supabaseAnonKey.toUtf8());

    QJsonObject body;
    body["email"] = email;
    body["password"] = password;

    QNetworkReply *reply = m_networkManager->post(request, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleAuthResponse("login",reply);
    });
}

void LogInSignUpDialog::signinWithSupabase(const QString &name, const QString &email, const QString &password, const QDate &birthday) {
    qDebug() << "SSL support:" << QSslSocket::supportsSsl();
    qDebug() << "SSL version:" << QSslSocket::sslLibraryBuildVersionString();
    qDebug() << "Supabase URL:" << m_supabaseUrl;
    QUrl url(m_supabaseUrl + "/auth/v1/signup");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("apikey", m_supabaseAnonKey.toUtf8());

    QJsonObject metadata;
    metadata["name"] = name;
    metadata["birthday"] = birthday.toString("yyyy-MM-dd");

    QJsonObject body;
    body["email"] = email;
    body["password"] = password;
    body["data"] = metadata;

    QNetworkReply *reply = m_networkManager->post(request, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleAuthResponse("signin",reply);
    });
}

void LogInSignUpDialog::handleAuthResponse(const QString requestType, QNetworkReply *reply) {
    reply->deleteLater();
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    if(reply->error() != QNetworkReply::NoError){
        QString errorMsg = obj.value("error_description").toString();
        if(errorMsg.isEmpty())
            errorMsg = obj.value("msg").toString();
        if(errorMsg.isEmpty())
            errorMsg = reply->errorString();
        QMessageBox::warning(this, "Error", errorMsg);
        return;
    }

    if(requestType == "login"){
        QJsonObject userObj = obj.value("user").toObject();
        user_id = userObj.value("id").toString();
        //qDebug() << "Login successful, user_id:" << user_id;
        accept();

    }else if(requestType == "signin"){
        QJsonObject userObj = obj.value("user").toObject();
        user_id = userObj.value("id").toString();
        //qDebug() << "Sign up successful, user_id:" << user_id;
        QMessageBox::information(this, "Success", "Account created successfully. Please check your email to confirm (Spam).");
        ui->logInSignUpWidget->setCurrentWidget(ui->logInWidget);
    }
}
