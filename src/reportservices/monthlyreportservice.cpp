
#include "reportservices/monthlyreportservice.h"

MonthlyReportService::MonthlyReportService(const QDate &month, const QString &userId)
    : m_month(month), m_userId(userId)
{
    accountModel = DatabaseManager::instance().getAccountModel(nullptr);

    auto loadCategoriesByType = [this](const QString &type, QSet<QString> &out, const char *label) {
        QSqlQuery query(DatabaseManager::instance().getDatabase());
        if (!query.prepare("SELECT name FROM categories WHERE type = :type AND user_id = :user")) {
            qDebug() << "[MonthlyReportService] PREPARE ERROR loading" << label << "categories:" << query.lastError().text();
            return;
        }
        query.bindValue(":type", type);
        query.bindValue(":user", m_userId);
        if (!query.exec()) {
            qDebug() << "[MonthlyReportService] ERROR loading" << label << "categories:" << query.lastError().text();
            return;
        }
        while (query.next()) {
            out.insert(query.value(0).toString().toLower());
        }
        query.finish();
    };

    loadCategoriesByType("income",     m_incomeCategories,     "income");
    loadCategoriesByType("expense",    m_expenseCategories,    "expense");
    loadCategoriesByType("investment", m_investmentCategories, "investment");
}

MonthlyReportService::~MonthlyReportService()
{
    delete accountModel;
}

QPair<QDate, QDate> MonthlyReportService::dateRange(const QDate &month) const
{
    const QDate &ref = month.isValid() ? month : m_month;
    return {
        QDate(ref.year(), ref.month(), 1),
        QDate(ref.year(), ref.month(), ref.daysInMonth())
    };
}

QString MonthlyReportService::getCategoryType(const QString &categoryName) const
{
    if(m_incomeCategories.contains(categoryName.toLower())){
        return "income";
    }

    if(m_expenseCategories.contains(categoryName.toLower())){
        return "expense";
    }

    if(m_investmentCategories.contains(categoryName.toLower())){
        return "investment";
    }

    return "transfer";
}

QMap<QString, int> MonthlyReportService::getAmountByCategory(const QDate &month) const
{
    QMap<QString, int> result;
    QSqlQuery query(DatabaseManager::instance().getDatabase());

    if (!query.prepare("SELECT c.name, SUM(t.amount) "
                       "FROM money_transactions t "
                       "JOIN categories c ON t.category_id = c.id "
                       "WHERE t.date >= :start AND t.date <= :end AND t.user_id = :user "
                       "GROUP BY c.name")) {
        qDebug() << "[MonthlyReportService::getAmountByCategory] PREPARE ERROR:" << query.lastError().text();
        return result;
    }

    auto [first, last] = dateRange(month);
    query.bindValue(":start", first.toString("yyyy-MM-dd"));
    query.bindValue(":end",   last.toString("yyyy-MM-dd"));
    query.bindValue(":user",  m_userId);

    if (query.exec()) {
        while (query.next()) {
            result[query.value(0).toString().toLower()] = query.value(1).toInt();
        }
    } else {
        qDebug() << "[MonthlyReportService::getAmountByCategory] ERROR:" << query.lastError().text();
    }

    return result;
}

QMap<QString, int> MonthlyReportService::getAmountByCategoryAndTypeOfCard(const QString &typeOfCard, const QDate &month) const
{
    QMap<QString, int> result;
    QSqlQuery query(DatabaseManager::instance().getDatabase());

    if (!query.prepare("SELECT c.name, SUM(t.amount) "
                       "FROM money_transactions t "
                       "JOIN categories c ON t.category_id = c.id "
                       "JOIN accounts a ON t.account_id = a.id "
                       "WHERE t.date >= :start AND t.date <= :end "
                       "AND t.user_id = :user "
                       "AND a.type = :type "
                       "GROUP BY c.name")) {
        qDebug() << "[MonthlyReportService::getAmountByCategoryAndTypeOfCard] PREPARE ERROR:" << query.lastError().text();
        return result;
    }

    auto [first, last] = dateRange(month);
    query.bindValue(":start", first.toString("yyyy-MM-dd"));
    query.bindValue(":end",   last.toString("yyyy-MM-dd"));
    query.bindValue(":type",  typeOfCard);
    query.bindValue(":user",  m_userId);

    if (query.exec()) {
        while (query.next()) {
            result[query.value(0).toString().toLower()] = query.value(1).toInt();
        }
    } else {
        qDebug() << "[MonthlyReportService::getAmountByCategoryAndTypeOfCard] ERROR:" << query.lastError().text();
    }

    return result;
}

QMap<QString, MonthlyReportService::Totals> MonthlyReportService::getTotalsByTypeCard(const QString &typeOfCard, const QDate &month) const
{
    QMap<QString, MonthlyReportService::Totals> response;
    QSqlQuery query(DatabaseManager::instance().getDatabase());

    if (!query.prepare("SELECT a.name, c.type, SUM(t.amount) "
                       "FROM money_transactions t "
                       "JOIN categories c ON t.category_id = c.id "
                       "JOIN accounts a ON t.account_id = a.id "
                       "WHERE t.date >= :start AND t.date <= :end "
                       "AND t.user_id = :user "
                       "AND a.type = :type "
                       "GROUP BY a.name, c.type")) {
        qDebug() << "[MonthlyReportService::getTotalsByTypeCard] PREPARE ERROR:" << query.lastError().text();
        return response;
    }

    auto [first, last] = dateRange(month);
    query.bindValue(":start", first.toString("yyyy-MM-dd"));
    query.bindValue(":end",   last.toString("yyyy-MM-dd"));
    query.bindValue(":user",  m_userId);
    query.bindValue(":type",  typeOfCard);

    if (query.exec()) {
        while (query.next()) {
            QString name = query.value(0).toString();

            if (query.value(1).toString() == "investment") {
                response[name].investment = query.value(2).toInt();
                continue;
            }

            if (query.value(1).toString() == "expense") {
                response[name].expenses = query.value(2).toInt();
            } else {
                response[name].incomes = query.value(2).toInt();
            }
        }
    } else {
        qDebug() << "[MonthlyReportService::getTotalsByTypeCard] ERROR:" << query.lastError().text();
    }
    return response;
}

MonthlyReportService::Totals MonthlyReportService::getComputeTotals(const QMap<QString, int> &amountByCategory) const
{
    Totals totals;
    for (auto it = amountByCategory.cbegin(); it != amountByCategory.cend(); ++it) {
        QString type = getCategoryType(it.key());
        if (type == "expense")  {totals.expenses += it.value();} 
        if (type == "income" )  {totals.incomes  += it.value();}
        if (type == "investment" )  {totals.investment  += it.value();}
    }
    totals.savings = totals.incomes - totals.expenses;
    return totals;
}

MonthlyReportService::Totals MonthlyReportService::getTotalsByNameOfCard(const QString &nameOfCard, const QDate &month) const
{
    QMap<QString, int> amountByCategory;
    QSqlQuery query(DatabaseManager::instance().getDatabase());

    if (!query.prepare("SELECT c.name, SUM(t.amount) "
                       "FROM money_transactions t "
                       "JOIN categories c ON t.category_id = c.id "
                       "JOIN accounts a ON t.account_id = a.id "
                       "WHERE t.date >= :start AND t.date <= :end "
                       "AND t.user_id = :user "
                       "AND a.name = :name "
                       "GROUP BY c.name")) {
        qDebug() << "[MonthlyReportService::getTotalsByNameOfCard] PREPARE ERROR:" << query.lastError().text();
        return {};
    }

    auto [first, last] = dateRange(month);
    query.bindValue(":start", first.toString("yyyy-MM-dd"));
    query.bindValue(":end",   last.toString("yyyy-MM-dd"));
    query.bindValue(":name",  nameOfCard);
    query.bindValue(":user",  m_userId);

    if (query.exec()) {
        while (query.next()) {
            amountByCategory[query.value(0).toString().toLower()] = query.value(1).toInt();
        }
    } else {
        qDebug() << "[MonthlyReportService::getTotalsByNameOfCard] ERROR:" << query.lastError().text();
    }

    Totals totals = getComputeTotals(amountByCategory);
    totals.bankName = nameOfCard;
    return totals;
}

int MonthlyReportService::getCreditCardNumber() const
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    if (query.prepare("SELECT COUNT(*) FROM accounts WHERE type = 'credit' AND user_id = :user")) {
        query.bindValue(":user", m_userId);
        if (query.exec() && query.next()) {
            return query.value(0).toInt();
        }
    }
    qDebug() << "[MonthlyReportService::getCreditCardNumber] ERROR:" << query.lastError().text();
    return 0;
}

MonthlyReportService::CreditSummary MonthlyReportService::getCreditSummaryByName(const QString &nameOfCard, const QDate &month) const
{
    CreditSummary summaryByName;
    QSqlQuery query(DatabaseManager::instance().getDatabase());

    if (!query.prepare("SELECT a.name, a.limit, COALESCE(SUM(t.amount), 0) "
                       "FROM accounts a "
                       "LEFT JOIN money_transactions t ON t.account_id = a.id "
                       "  AND t.date >= :start AND t.date <= :end AND t.user_id = :user "
                       "WHERE a.type = 'credit' AND a.user_id = :user "
                       "AND a.name = :name "
                       "GROUP BY a.name, a.limit")) {
        qDebug() << "[MonthlyReportService::getCreditSummaryByName] PREPARE ERROR:" << query.lastError().text();
        return summaryByName;
    }

    auto [first, last] = dateRange(month);
    query.bindValue(":start", first.toString("yyyy-MM-dd"));
    query.bindValue(":end",   last.toString("yyyy-MM-dd"));
    query.bindValue(":user",  m_userId);
    query.bindValue(":name",  nameOfCard);

    if (query.exec() && query.next()) {
        summaryByName.bankName  = query.value(0).toString();
        summaryByName.limit     = query.value(1).toInt();
        summaryByName.used      = query.value(2).toInt();
        summaryByName.available = summaryByName.limit - summaryByName.used;
    } else if (!query.isActive()) {
        qDebug() << "[MonthlyReportService::getCreditSummaryByName] ERROR:" << query.lastError().text();
    }

    return summaryByName;
}

QList<MonthlyReportService::CreditSummary> MonthlyReportService::getAllCreditSummaries(int maxCards, const QDate &month) const
{
    QList<CreditSummary> result;
    QSqlQuery query(DatabaseManager::instance().getDatabase());

    if (!query.prepare("SELECT a.name, a.limit, COALESCE(SUM(t.amount), 0) "
                       "FROM accounts a "
                       "LEFT JOIN money_transactions t ON t.account_id = a.id "
                       "  AND t.date >= :start AND t.date <= :end AND t.user_id = :user "
                       "WHERE a.type = 'credit' AND a.user_id = :user "
                       "GROUP BY a.name, a.limit "
                       "LIMIT :max")) {
        qDebug() << "[MonthlyReportService::getAllCreditSummaries] PREPARE ERROR:" << query.lastError().text();
        return result;
    }

    auto [first, last] = dateRange(month);
    query.bindValue(":start", first.toString("yyyy-MM-dd"));
    query.bindValue(":end",   last.toString("yyyy-MM-dd"));
    query.bindValue(":user",  m_userId);
    query.bindValue(":max",   maxCards);

    if (query.exec()) {
        while (query.next()) {
            CreditSummary cs;
            cs.bankName  = query.value(0).toString();
            cs.limit     = query.value(1).toInt();
            cs.used      = query.value(2).toInt();
            cs.available = cs.limit - cs.used;
            result.append(cs);
        }
    } else {
        qDebug() << "[MonthlyReportService::getAllCreditSummaries] ERROR:" << query.lastError().text();
    }

    return result;
}