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

signals:
    void closeWasPressed();
    void monthClicked(QDate date);

private:
    Ui::MonthSelectorDialog *ui;
    static constexpr int numberOfMonthInAYear = 12;

    QVector<QPushButton*> m_buttons = QVector<QPushButton*>(numberOfMonthInAYear);

    void onButtonClicked(QDate date);
    void initView(QDate date);
    
};

#endif