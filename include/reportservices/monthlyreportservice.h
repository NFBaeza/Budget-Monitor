#ifndef MONTHLYREPORTSERVICE_H
#define MONTHLYREPORTSERVICE_H

#include "database/databasemanager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDate>
#include <QMap>
#include <QList>
#include <QSet>
#include <QString>

class MonthlyReportService
{
public:
    struct Totals {
        int incomes  = 0;
        int expenses = 0;
        int savings  = 0;
    };

    struct CreditSummary {
        QString bankName;
        int limitCredit     = 0;
        int usedCredit      = 0;
        int availableCredit = 0;
    };

    MonthlyReportService(const QDate &month, const QString &userId);
    ~MonthlyReportService();

    QMap<QString, int>   getAmountByCategory() const;
    QMap<QString, Totals>getAmountByTypeCard(QString type_of_card) const;
    Totals               getComputeTotals(const QMap<QString, int> &amountByCategory) const;
    QList<CreditSummary> getCreditSummaries(int maxCards = 3) const;
    QString              getCategoryType(const QString &categoryName) const; 
    int                  getCreditCardNumber() const;

private:
    QDate         m_month;
    QString       m_userId;
    QSet<QString> m_incomeCategories;
    QSqlTableModel *accountModel{nullptr};

    QPair<QDate, QDate> dateRange() const;
};

#endif // MONTHLYREPORTSERVICE_H
