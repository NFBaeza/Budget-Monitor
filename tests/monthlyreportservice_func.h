#pragma once
#include <QtCore>
#include <QMap>
#include <QVector>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

struct CardDetails{
    int incomes = 0;
    int expenses = 0;
};

int calculateMonthlyExpenses(QSqlDatabase db, QString user_id, QString date_filter)
{
    int result = 0;
    QSqlQuery query(db);

    if (!query.prepare("SELECT SUM(t.amount) "
                       "FROM transactions t "
                       "JOIN categories c ON t.category_id = c.id "
                       "WHERE t.date >= :start AND t.date <= :end "
                       "AND t.user_id = :user "
                       "AND c.type = 'expense' "
                       "GROUP BY c.name")) {
        qDebug() << "[calculateMonthlyExpenses] PREPARE ERROR:" << query.lastError().text();
        return result;
    }

    query.bindValue(":start", QString("%1-01").arg(date_filter));
    query.bindValue(":end",   QString("%1-31").arg(date_filter));
    query.bindValue(":user",  user_id);

    if (query.exec()) {
        while (query.next()) {
            int amount = query.value(0).toInt();
            result += amount;
        }
    } else {
        qDebug() << "[calculateMonthlyExpenses] ERROR:" << query.lastError().text();
    }

    return result;
}

QMap<QString,CardDetails> calculateAmountByTypeOfCard(QSqlDatabase db, QString user_id, QString date_filter, QString type)
{
    QSqlQuery query(db);
    QMap<QString,CardDetails> Card;

    if (!query.prepare("SELECT a.name, c.type, SUM(t.amount) "
                       "FROM transactions t "
                       "JOIN categories c ON t.category_id = c.id "
                       "JOIN accounts a ON t.account_id = a.id "
                       "WHERE t.date >= :start AND t.date <= :end "
                       "AND t.user_id = :user "
                       "AND a.type = :type "
                       "GROUP BY c.type")) {
        qDebug() << "[calculateMonthlyExpenses] PREPARE ERROR:" << query.lastError().text();
        return Card;
    }

    query.bindValue(":start", QString("%1-01").arg(date_filter));
    query.bindValue(":end",   QString("%1-31").arg(date_filter));
    query.bindValue(":user",  user_id);
    query.bindValue(":type",  type);

    if (query.exec()) {
        while (query.next()) {
            QString name = query.value(0).toString();
            
            if(query.value(1).toString() == "expense"){
                Card[name].expenses = query.value(2).toInt();
            } else {
                Card[name].incomes = query.value(2).toInt();
            }
        } 
    } else {
        qDebug() << "[calculateMonthlyExpenses] ERROR:" << query.lastError().text();
    }

    return Card;
}
