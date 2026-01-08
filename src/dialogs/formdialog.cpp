#include "dialogs/formdialog.h"
#include "./ui_formdialog.h"

FormDialog::FormDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FormDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &FormDialog::onAcceptClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &FormDialog::onCancelClicked);
    connect(ui->IncomeRadioButton, &QRadioButton::toggled, this, &FormDialog::updateComboText);
    initView();
}

FormDialog::~FormDialog() {
    delete ui;
}

void FormDialog::onAcceptClicked() {
    // Validar campos obligatorios
    if (ui->InputAmountText->text().isEmpty() || ui->DateTimeSelected->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill all the options");
        return;
    }

    // Insertar la transacción
    if (!insertTransaction()) {
        QMessageBox::critical(this, "Error", "Failed to save transaction. Check console for details.");
        return;
    }

    // Mostrar mensaje de éxito
    QMessageBox::information(this, "Success", "Transaction saved successfully!");

    // Emitir señal para notificar al dashboard
    emit dataInserted();

    // Cerrar el diálogo
    accept();
}

void FormDialog::onCancelClicked() {
    reject();
}

QString FormDialog::getAmount() const {
    return ui->InputAmountText->text();
}

QString FormDialog::getDate() const {
    return ui->DateTimeSelected->dateTime().toString("yyyy-MM-dd HH:mm:ss");
}

QString FormDialog::getCategory() const {
    return ui->ListCategoryDialog->currentText();
}

QString FormDialog::getAcount() const {
    return ui->ListAccountDialog->currentText();
}

QString FormDialog::getDescription() const {
    return ui->DescriptionText->text();
}

void FormDialog::updateComboText(){
    //qDebug()<< "EL toggle fue activado con texto igual a  " << ;
    if(ui->IncomeRadioButton->isChecked()){
        categoryModel->setFilter("type == 'income'");
    }else{
        categoryModel->setFilter("type == 'expense'");
    }
    
    categoryModel->select();
    
    ui->ListCategoryDialog->setModel(categoryModel);
    ui->ListCategoryDialog->setModelColumn(1); 
    return;
}

void FormDialog::initView(){
    ui->TitleFormDialog->setText("Add new data");

    // Configurar modelo de categorías
    categoryModel->setTable("categories");
    categoryModel->select();
    updateComboText();

    // Configurar modelo de métodos de pago
    accountModel->setTable("payment_methods");
    accountModel->select();
    ui->ListAccountDialog->setModel(accountModel);
    ui->ListAccountDialog->setModelColumn(1);
}

bool FormDialog::insertTransaction() {
    // Obtener el ID de la categoría seleccionada
    int categoryRow = ui->ListCategoryDialog->currentIndex();
    int categoryId = categoryModel->index(categoryRow, 0).data().toInt();

    // Obtener el ID del método de pago seleccionado
    int accountRow = ui->ListAccountDialog->currentIndex();
    int accountId = accountModel->index(accountRow, 0).data().toInt();

    // Preparar datos
    QString date = ui->DateTimeSelected->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    int amount = ui->InputAmountText->text().toDouble();
    QString description = ui->DescriptionText->text();

    QSqlQuery query(db);
    query.prepare("INSERT INTO money_transactions (date, amount, category, account, description) "
                  "VALUES (:date, :amount, :category, :account, :description)");

    query.bindValue(":date", date);
    query.bindValue(":amount", amount);
    query.bindValue(":category", categoryId);
    query.bindValue(":account", accountId);
    query.bindValue(":description", description);

    // Ejecutar la inserción
    if (!query.exec()) {
        qDebug() << "[insertTransaction] ERROR:" << query.lastError().text();
        return false;
    }

    qDebug() << "[insertTransaction] Inserción exitosa! Last inserted ID:" << query.lastInsertId();

    // Verificar que se insertó
    QSqlQuery verifyQuery(db);
    verifyQuery.exec("SELECT COUNT(*) FROM money_transactions");
    if (verifyQuery.next()) {
        qDebug() << "[insertTransaction] Total transacciones en BD:" << verifyQuery.value(0).toInt();
    }

    return true;
}