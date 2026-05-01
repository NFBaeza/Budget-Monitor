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
        QString bankName;
        int incomes  = 0;
        int expenses = 0;
        int investment = 0;
        int savings  = 0;
    };

    struct CreditSummary {
        QString bankName;
        int limit     = 0;
        int used      = 0;
        int available = 0;
    };

    MonthlyReportService(const QDate &month, const QString &userId);
    ~MonthlyReportService();

    QMap<QString, int>      getAmountByCategory(const QDate &month = QDate()) const;
    QMap<QString, int>      getAmountByCategoryAndTypeOfCard(const QString &typeOfCard, const QDate &month = QDate()) const;
    QMap<QString, Totals>   getTotalsByTypeCard(const QString &typeOfCard, const QDate &month = QDate()) const;
    Totals                  getTotalsByNameOfCard(const QString &nameOfCard, const QDate &month = QDate()) const;
    Totals                  getComputeTotals(const QMap<QString, int> &amountByCategory) const;
    
    int                     getCreditCardNumber() const;
    QList<CreditSummary>    getAllCreditSummaries(int maxCards = 3, const QDate &month = QDate()) const;
    CreditSummary           getCreditSummaryByName(const QString &nameOfCard, const QDate &month = QDate()) const;
    QString                 getCategoryType(const QString &categoryName) const; 
    

private:
    QDate         m_month;
    QString       m_userId;
    QSet<QString> m_incomeCategories;
    QSet<QString> m_expenseCategories;
    QSet<QString> m_investmentCategories;
    QSqlTableModel *accountModel{nullptr};

    QPair<QDate, QDate> dateRange(const QDate &month = QDate()) const;
};

#endif // MONTHLYREPORTSERVICE_H
