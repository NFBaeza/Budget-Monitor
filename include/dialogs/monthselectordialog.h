#ifndef MONTHSELECTORDIALOG_H
#define MONTHSELECTORDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QDebug>
#include <QDateTime>
#include <QLayout>
#include <QLabel>
#include <widgets/dashboardwidget.h>

namespace Ui { class MonthSelectorDialog; }

class MonthSelectorDialog : public QDialog {
    Q_OBJECT

public:
    explicit MonthSelectorDialog(QWidget *parent = nullptr);
    ~MonthSelectorDialog();
    void done(int result) override;

    //QString getDate() const;

signals:
    void close_was_pressed();
    void month_clicked(QDate date);

private:
    Ui::MonthSelectorDialog *ui;

    void onButtonClicked(QDate date);
    void initView(QDate date);
    
};

#endif