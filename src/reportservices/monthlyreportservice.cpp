
#include "reportservices/monthlyreportservice.h"

MonthlyReportService::MonthlyReportService(const QDate &month, const QString &userId)
    : m_month(month), m_userId(userId)
{
    accountModel = DatabaseManager::instance().getAccountModel(nullptr);

    QSqlQuery query(DatabaseManager::instance().getDatabase());
    if (query.prepare("SELECT name FROM categories WHERE type = 'income' AND user_id = :user")) {
        query.bindValue(":user", m_userId);
        if (query.exec()) {
            while (query.next()) {
                m_incomeCategories.insert(query.value(0).toString().toLower());
            }
        } else {
            qDebug() << "[MonthlyReportService] ERROR loading income categories:" << query.lastError().text();
        }
    }
}

MonthlyReportService::~MonthlyReportService()
{
    delete accountModel;
}

QPair<QDate, QDate> MonthlyReportService::dateRange() const
{
    return {
        QDate(m_month.year(), m_month.month(), 1),
        QDate(m_month.year(), m_month.month(), m_month.daysInMonth())
    };
}

QString MonthlyReportService::getCategoryType(const QString &categoryName) const
{
    return m_incomeCategories.contains(categoryName.toLower()) ? "income" : "expense";
}

QMap<QString, int> MonthlyReportService::getAmountByCategory() const
{
    QMap<QString, int> result;
    QSqlQuery query(DatabaseManager::instance().getDatabase());

    if (!query.prepare("SELECT c.name, SUM(t.amount) "
                       "FROM money_transactions t "
                       "JOIN categories c ON t.category_id = c.id "
                       "WHERE t.date >= :start AND t.date <= :end AND t.user_id = :user "
                       "GROUP BY c.name")) {
        qDebug() << "[MonthlyReportService::amountByCategory] PREPARE ERROR:" << query.lastError().text();
        return result;
    }

    auto [first, last] = dateRange();
    query.bindValue(":start", first.toString("yyyy-MM-dd"));
    query.bindValue(":end",   last.toString("yyyy-MM-dd"));
    query.bindValue(":user",  m_userId);

    if (query.exec()) {
        while (query.next()) {
            result[query.value(0).toString().toLower()] = query.value(1).toInt();
        }
    } else {
        qDebug() << "[MonthlyReportService::amountByCategory] ERROR:" << query.lastError().text();
    }

    return result;
}

MonthlyReportService::Totals MonthlyReportService::getComputeTotals(const QMap<QString, int> &amountByCategory) const
{
    Totals totals;
    for (auto it = amountByCategory.cbegin(); it != amountByCategory.cend(); ++it) {
        if (getCategoryType(it.key()) == "expense") {
            totals.expenses += it.value();
        } else {
            totals.incomes += it.value();
        }
    }
    totals.savings = totals.incomes - totals.expenses;
    return totals;
}


int MonthlyReportService::getCreditCardNumber() const
{
    QString creditAccountFilter = QString("accounts.type = '%1' AND accounts.user_id = '%2'").arg("credit").arg(m_userId);
    
    accountModel->setFilter(creditAccountFilter);
    accountModel->select();

    int numberOfCreditCards = accountModel->rowCount();

    return numberOfCreditCards;
}

QList<MonthlyReportService::CreditSummary> MonthlyReportService::getCreditSummaries(int maxCards) const
{
    QList<CreditSummary> result;
    QSqlQuery query(DatabaseManager::instance().getDatabase());

    // NOTE: replace 'account_limit' with the actual credit limit column name in your schema
    if (!query.prepare("SELECT a.name, a.limit, COALESCE(SUM(t.amount), 0) "
                       "FROM accounts a "
                       "LEFT JOIN money_transactions t ON t.account_id = a.id "
                       "  AND t.date >= :start AND t.date <= :end AND t.user_id = :user "
                       "WHERE a.type = 'credit' AND a.user_id = :user2 "
                       "GROUP BY a.name, a.limit "
                       "LIMIT :max")) {
        qDebug() << "[MonthlyReportService::creditSummaries] PREPARE ERROR:" << query.lastError().text();
        return result;
    }

    auto [first, last] = dateRange();
    query.bindValue(":start", first.toString("yyyy-MM-dd"));
    query.bindValue(":end",   last.toString("yyyy-MM-dd"));
    query.bindValue(":user",  m_userId);
    query.bindValue(":user2", m_userId);
    query.bindValue(":max",   maxCards);

    if (query.exec()) {
        while (query.next()) {
            CreditSummary cs;
            cs.bankName        = query.value(0).toString();
            cs.limitCredit     = query.value(1).toInt();
            cs.usedCredit      = query.value(2).toInt();
            cs.availableCredit = cs.limitCredit - cs.usedCredit;
            result.append(cs);
        }
    } else {
        qDebug() << "[MonthlyReportService::creditSummaries] ERROR:" << query.lastError().text();
    }

    return result;
}
