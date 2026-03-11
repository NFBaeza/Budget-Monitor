#include "mock_supabase_manager.h"
#include "monthlyreportservice_func.h"
#include <gtest/gtest.h>
#include <QSqlQuery>
#include <QSqlError>

class monthlyreportservice_test : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        if (!QCoreApplication::instance()) {
            static int argc = 1;
            static char* argv[] = {(char*)"test"};
            static QCoreApplication app(argc, argv);
        }
    }

    void SetUp() override {
        db = MockDatabase::create();
    }

    void TearDown() override {
        db.close();
        QSqlDatabase::removeDatabase("mock_conn");
    }

    QSqlDatabase db;
};

TEST_F(monthlyreportservice_test, TotalExpensesForJanuary) {
    auto result = calculateMonthlyExpenses(db, "u1", "2026-01");

    EXPECT_EQ(result, 128000);  // 35000+8000+15000+42000+28000
}



TEST_F(monthlyreportservice_test, TotalByCardForJanuary) {
    auto result = calculateAmountByTypeOfCard(db, "u1", "2026-01","credit");

    for(const auto [card, amounts] : result.asKeyValueRange()){
        qDebug()<<"name: "<<card;
        qDebug()<<"incomes: "<<amounts.incomes;
        qDebug()<<"expense: "<<amounts.expenses;
    };
    
    EXPECT_EQ(result["Credit Card"].expenses, 23000);  // Metro(8000) + Cinema(15000)
    EXPECT_EQ(result["Credit Card"].incomes,  0);

    auto result2 = calculateAmountByTypeOfCard(db, "u1", "2026-01","debit");

     for(const auto [card, amounts] : result2.asKeyValueRange()){
        qDebug()<<"name: "<<card;
        qDebug()<<"incomes: "<<amounts.incomes;
        qDebug()<<"expense: "<<amounts.expenses;
    };

    EXPECT_EQ(result2["Checking"].expenses, 105000); 
    EXPECT_EQ(result2["Checking"].incomes,  120000);

}
