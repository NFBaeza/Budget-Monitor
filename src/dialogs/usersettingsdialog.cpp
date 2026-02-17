#include "dialogs/usersettingsdialog.h"
#include "database/databaseworker.h"
#include "dialogs/categorydialog.h"
#include "./ui_usersettingsdialog.h"
#include <QNetworkRequest>
#include <QSslSocket>
#include <QUrl>

QString user_id = "";
QString user_name = "";


UserSettingsDialog::UserSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::userSettingsDialog)
    , m_networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    if(!user_id.isEmpty()){
        ui->userSettingsWidget->setCurrentWidget(ui->userWidget);
    }else{
        ui->userSettingsWidget->setCurrentWidget(ui->logInWidget);
    }
    

    QMap<QString, QString> env = DatabaseManager::loadEnvFile();
    m_supabaseUrl = env.value("SUPABASE_URL");
    m_supabaseAnonKey = env.value("SUPABASE_ANON_KEY");

    connect(ui->signUpButton, &QPushButton::clicked, this, [this]() {
        ui->userSettingsWidget->setCurrentWidget(ui->signUpWidget);
    });
    connect(ui->logInButton, &QPushButton::clicked, this, [this]() {
        ui->userSettingsWidget->setCurrentWidget(ui->logInWidget);
    });

    connect(ui->logoutButton, &QPushButton::clicked, this, [this](){
        logout();
        emit backToMain();
    });

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &UserSettingsDialog::onAcceptButton);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &UserSettingsDialog::reject);
    ui->warningPasswordLabel->setVisible(false);

    connect(ui->passwordUserSignUpText, &QLineEdit::editingFinished, this, &UserSettingsDialog::checkSigninPassword);
    connect(ui->passwordUserSignUpText_2, &QLineEdit::editingFinished, this, &UserSettingsDialog::checkSigninPassword);

    connect(ui->editSettingsButton, &QPushButton::pressed, this, [this](){
        CategoryDialog dialog(this);
        dialog.exec();
    });

}

UserSettingsDialog::~UserSettingsDialog() {
    delete ui;
}

void UserSettingsDialog::checkSigninPassword(){
    QString password = ui->passwordUserSignUpText_2->text();
    QString password2 = ui->passwordUserSignUpText->text();

    if (password != password2){
        ui->warningPasswordLabel->setVisible(true);
    }else{
        ui->warningPasswordLabel->setVisible(false);
    }
}

void UserSettingsDialog::onAcceptButton(){
    if(ui->userSettingsWidget->currentWidget() == ui->logInWidget){
        QString email = ui->userEmailLogInText->text().trimmed();
        QString password = ui->passwordLogInText->text();

        if(email.isEmpty() || password.isEmpty()){
            QMessageBox::warning(this, "Error", "Please fill all fields");
            return;
        }
        loginWithSupabase(email, password);

    }else if (ui->userSettingsWidget->currentWidget() == ui->signUpWidget) {
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
    }else {
        accept();
    }
}

void UserSettingsDialog::loginWithSupabase(const QString &email, const QString &password) {
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

void UserSettingsDialog::signinWithSupabase(const QString &name, const QString &email, const QString &password, const QDate &birthday) {
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

void UserSettingsDialog::handleAuthResponse(const QString requestType, QNetworkReply *reply) {
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
        qDebug() << "Login successful, user_id:" << user_id;
        QString refreshToken = obj.value("refresh_token").toString();
        QSettings settings("BudgetMonitor", "BudgetMonitor");
        settings.setValue("auth/refresh_token", refreshToken);
        settings.setValue("auth/user_id", user_id);
        accept();

    }else if(requestType == "signin"){
        QJsonObject userObj = obj.value("user").toObject();
        user_id = userObj.value("id").toString();
        qDebug() << "Sign up successful, user_id:" << user_id;
        QMessageBox::information(this, "Success", "Account created successfully. Please check your email to confirm (Spam).");
        ui->userSettingsWidget->setCurrentWidget(ui->logInWidget);
    }
}

void UserSettingsDialog::tryAutoLogin(QNetworkAccessManager *manager,
                                     std::function<void(bool success)> callback) {
    QSettings settings("BudgetMonitor", "BudgetMonitor");
    QString refreshToken = settings.value("auth/refresh_token").toString();

    if (refreshToken.isEmpty()) {
        callback(false);
        return;
    }

    QMap<QString, QString> env = DatabaseManager::loadEnvFile();
    QString supabaseUrl = env.value("SUPABASE_URL");
    QString supabaseAnonKey = env.value("SUPABASE_ANON_KEY");

    QUrl url(supabaseUrl + "/auth/v1/token?grant_type=refresh_token");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("apikey", supabaseAnonKey.toUtf8());

    QJsonObject body;
    body["refresh_token"] = refreshToken;

    QNetworkReply *reply = manager->post(request, QJsonDocument(body).toJson());
    QObject::connect(reply, &QNetworkReply::finished, [reply, callback]() {
        reply->deleteLater();
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();

        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Auto-login failed:" << data;
            logout();
            callback(false);
            return;
        }

        QJsonObject userObj = obj.value("user").toObject();
        QJsonObject userMetadata = userObj.value("user_metadata").toObject();
        user_id = userObj.value("id").toString();
        user_name = user_name = userMetadata.value("name").toString();
        qDebug()<<"username: "<< user_name;
        QString newRefreshToken = obj.value("refresh_token").toString();
        QSettings settings("BudgetMonitor", "BudgetMonitor");
        settings.setValue("auth/refresh_token", newRefreshToken);
        settings.setValue("auth/user_id", user_id);
        settings.setValue("auth/user_name", user_name);

        qDebug() << "Auto-login successful, user_id:" << user_id << user_name;
        callback(true);
    });
}

void UserSettingsDialog::logout() {
    QSettings settings("BudgetMonitor", "BudgetMonitor");
    settings.remove("auth/refresh_token");
    settings.remove("auth/user_id");
    settings.remove("auth/user_name");
    user_id = "";
    user_name = "";
}
