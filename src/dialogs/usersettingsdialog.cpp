#include "dialogs/usersettingsdialog.h"
#include "database/databaseworker.h"
#include "dialogs/categorydialog.h"
#include "appstate.h"
#include "./ui_usersettingsdialog.h"
#include <QNetworkRequest>
#include <QSslSocket>
#include <QUrl>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>

UserSettingsDialog::UserSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::userSettingsDialog)
    , m_networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    QSettings s("BudgetMonitor", "BudgetMonitor");
    if (!s.value("auth/user_id").toString().isEmpty()) {
        ui->userSettingsWidget->setCurrentWidget(ui->userWidget);
    } else {
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

    connect(ui->logOutButton, &QPushButton::clicked, this, [this](){
        logout();
        emit backToMain();
    });

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &UserSettingsDialog::onAcceptButton);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &UserSettingsDialog::reject);
    ui->warningPasswordLabel->setVisible(false);

    connect(ui->passwordUserSignUpText, &QLineEdit::editingFinished, this, &UserSettingsDialog::checkSigninPassword);
    connect(ui->passwordUserSignUpText_2, &QLineEdit::editingFinished, this, &UserSettingsDialog::checkSigninPassword);

    connect(ui->editCategoryButton, &QPushButton::pressed, this, [this](){
        CategoryDialog dialog(this);
        dialog.exec();
    });

    connect(ui->editAccountButton, &QPushButton::pressed, this, [this](){
        CategoryDialog dialog(this);
        dialog.exec();
    });

    connect(ui->editEmailButton, &QPushButton::pressed, this, &UserSettingsDialog::onChangeEmailButtonPressed);
    connect(ui->editNameButton, &QPushButton::pressed, this, &UserSettingsDialog::onChangeNameButtonPressed);
    connect(ui->eraseAccountButton, &QPushButton::pressed, this, &UserSettingsDialog::onEraseAccountButtonPressed);
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
        QJsonObject userMetadata = userObj.value("user_metadata").toObject();
        user_name = userMetadata.value("name").toString();
        qDebug() << "Login successful, user_id:" << user_id;
        QSettings settings("BudgetMonitor", "BudgetMonitor");
        settings.setValue("auth/refresh_token", obj.value("refresh_token").toString());
        settings.setValue("auth/access_token", obj.value("access_token").toString());
        settings.setValue("auth/user_id", user_id);
        settings.setValue("auth/user_name", user_name);
        settings.setValue("auth/email", userObj.value("email").toString());
        accept();

    }else if(requestType == "signin"){
        qDebug() << "Sign up successful, user_id:" << obj.value("user").toObject().value("id").toString();
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
        user_name = userMetadata.value("name").toString();

        qDebug() << "username:" << user_name;
        QSettings settings("BudgetMonitor", "BudgetMonitor");
        settings.setValue("auth/refresh_token", obj.value("refresh_token").toString());
        settings.setValue("auth/access_token", obj.value("access_token").toString());
        settings.setValue("auth/user_id", user_id);
        settings.setValue("auth/user_name", user_name);

        QString userEmail = userObj.value("email").toString();
        settings.setValue("auth/email", userEmail);

        qDebug() << "Auto-login successful, user_id:" << user_id << user_name;
        callback(true);
    });
}

void UserSettingsDialog::logout() {
    QSettings settings("BudgetMonitor", "BudgetMonitor");
    settings.remove("auth/refresh_token");
    settings.remove("auth/access_token");
    settings.remove("auth/user_id");
    settings.remove("auth/user_name");
    settings.remove("auth/email");
    user_id = "";
    user_name = "";
}


void UserSettingsDialog::onChangeNameButtonPressed(){
    QDialog dialog(this);
    dialog.setWindowTitle("Edit Name");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QSettings settings("BudgetMonitor", "BudgetMonitor");
    QLabel *label = new QLabel("New Name:", &dialog);
    QLineEdit *input = new QLineEdit(&dialog);
    input->setText(settings.value("auth/user_name").toString());
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close, &dialog);

    layout->addWidget(label);
    layout->addWidget(input);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString newName = input->text().trimmed();
        if (!newName.isEmpty())
            updateNameInSupabase(newName);
    }
}

void UserSettingsDialog::onChangeEmailButtonPressed(){
    QDialog dialog(this);
    dialog.setWindowTitle("Edit Email");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QSettings settings("BudgetMonitor", "BudgetMonitor");
    QLabel *label = new QLabel("New Email:", &dialog);
    QLineEdit *input = new QLineEdit(&dialog);
    input->setText(settings.value("auth/email").toString());
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close, &dialog);

    layout->addWidget(label);
    layout->addWidget(input);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString newEmail = input->text().trimmed();
        if (!newEmail.isEmpty())
            updateEmailInSupabase(newEmail);
    }
}
QNetworkReply* UserSettingsDialog::makeAuthenticatedPut(const QJsonObject &body) {
    QSettings settings("BudgetMonitor", "BudgetMonitor");
    QString accessToken = settings.value("auth/access_token").toString();
    if (accessToken.isEmpty())
        return nullptr;

    QUrl url(m_supabaseUrl + "/auth/v1/user");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("apikey", m_supabaseAnonKey.toUtf8());
    request.setRawHeader("Authorization", ("Bearer " + accessToken).toUtf8());

    return m_networkManager->put(request, QJsonDocument(body).toJson());
}

void UserSettingsDialog::updateEmailInSupabase(const QString &newEmail) {
    QJsonObject body;
    body["email"] = newEmail;

    QNetworkReply *reply = makeAuthenticatedPut(body);
    if (!reply) { QMessageBox::warning(this, "Error", "Not logged in"); return; }

    connect(reply, &QNetworkReply::finished, this, [this, reply, newEmail]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();
            QString errorMsg = obj.value("message").toString();
            if (errorMsg.isEmpty()) errorMsg = reply->errorString();
            QMessageBox::warning(this, "Error", errorMsg);
            return;
        }
        QSettings("BudgetMonitor", "BudgetMonitor").setValue("auth/email", newEmail);
        QMessageBox::information(this, "Success", "Email updated. Check your inbox to confirm the change.");
    });
}

void UserSettingsDialog::updateNameInSupabase(const QString &newName) {
    QJsonObject metadata;
    metadata["name"] = newName;
    QJsonObject body;
    body["data"] = metadata;

    QNetworkReply *reply = makeAuthenticatedPut(body);
    if (!reply) { QMessageBox::warning(this, "Error", "Not logged in"); return; }

    connect(reply, &QNetworkReply::finished, this, [this, reply, newName]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();
            QString errorMsg = obj.value("message").toString();
            if (errorMsg.isEmpty()) errorMsg = reply->errorString();
            QMessageBox::warning(this, "Error", errorMsg);
            return;
        }
        user_name = newName;
        QSettings("BudgetMonitor", "BudgetMonitor").setValue("auth/user_name", newName);
        emit userNameChanged(newName);
        QMessageBox::information(this, "Success", "Name updated successfully.");
    });
}

void UserSettingsDialog::onEraseAccountButtonPressed() {
    QMessageBox warning;
    warning.setWindowTitle("Delete Account");
    warning.setIcon(QMessageBox::Critical);
    warning.setText("<b>Are you sure you want to delete your account?</b>");
    warning.setInformativeText(
        "This action is permanent and cannot be undone.\n"
        "All your data, transactions and categories will be lost forever."
    );
    warning.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    warning.setDefaultButton(QMessageBox::Cancel);
    warning.button(QMessageBox::Yes)->setText("Yes, delete my account");

    if (warning.exec() != QMessageBox::Yes)
        return;

    QSettings settings("BudgetMonitor", "BudgetMonitor");
    QString userId = settings.value("auth/user_id").toString();
    if (userId.isEmpty()) {
        QMessageBox::warning(this, "Error", "Not logged in");
        return;
    }

    QMap<QString, QString> env = DatabaseManager::loadEnvFile();
    QString serviceRoleKey = env.value("SUPABASE_SERVICE_ROLE_KEY");
    if (serviceRoleKey.isEmpty()) {
        QMessageBox::warning(this, "Error", "Server configuration missing (SUPABASE_SERVICE_ROLE_KEY)");
        return;
    }

    QUrl url(m_supabaseUrl + "/auth/v1/admin/users/" + userId);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("apikey", serviceRoleKey.toUtf8());
    request.setRawHeader("Authorization", ("Bearer " + serviceRoleKey).toUtf8());

    QNetworkReply *reply = m_networkManager->deleteResource(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();
            QString errorMsg = obj.value("message").toString();
            if (errorMsg.isEmpty()) errorMsg = reply->errorString();
            QMessageBox::warning(this, "Error", errorMsg);
            return;
        }
        logout();
        QMessageBox::information(this, "Account Deleted", "Your account has been permanently deleted.");
        emit backToMain();
    });
}
