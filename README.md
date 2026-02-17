# Budget Monitor

A desktop application for personal finance management built with Qt6 and C++. Track your income and expenses, visualize spending patterns, and monitor your monthly budget with an intuitive interface.

## Features

- **Transaction Management**: Add, view, and track financial transactions
- **Category-based Organization**: Separate income and expense categories
- **Visual Analytics**: Pie chart visualization of expense distribution
- **Monthly View**: Analyze transactions by month and yearly Savings.
- **Real-time Updates**: Dashboard updates automatically when new transactions are added
- **Multiple Payment Methods**: Support for different payment accounts
- **User Accounts**: Login, signup, and auto-login via Supabase authentication
- **Database Persistence**: Data stored remotely via [Supabase](https://supabase.com/)
- **XLSX Import**: Add transactions via XLSX bank statement files with automatic category classification


## Screenshot
![Month View UI](./imgs/ui_MonthView.png)

## Technical Stack

- **Qt Framework**: 6.x
- **Programming Language**: C++ 17
- **UI Design**: Qt Designer (.ui files)
- **Database**: Supabase (authentication and data persistence)
- **Build System**: CMake
- **Architecture**: Model-View pattern with QSqlTableModel and QSqlRelationalTableModel
- **Networking**: Qt Network module for Supabase REST API calls

## Project Structure

```
budget_monitor/
├── libs/               # External libraries
│   └── XLSXReader/     # XLSX bank statement parser (git submodule)
├── include/            # Header files
│   ├── database/       # Database manager and worker headers
│   ├── dialogs/        # Dialog headers
│   ├── views/          # Main window headers
│   └── widgets/        # Widget headers (Dashboard)
├── src/                # Source files
│   ├── database/       # Database manager and worker implementations
│   ├── dialogs/        # Dialog implementations
│   ├── views/          # View implementations
│   └── widgets/        # Widget implementations
├── ui/                 # Qt Designer UI files
├── imgs/               # Screenshots and assets
└── CMakeLists.txt      # Build configuration
```

## Database Schema

The application uses three main tables (all scoped per user):

- **money_transactions**: Stores all financial transactions
  - `id`, `user_id`, `date`, `amount`, `category_id` (FK), `account_id` (FK), `description`
- **categories**: Income and expense categories
  - `id`, `user_id`, `name`, `type` (income/expense)
- **accounts**: Available payment accounts
  - `id`, `user_id`, `name`

## Building from Source

### Prerequisites

- Qt 6.x or higher
- CMake 3.10 or higher
- C++17 compatible compiler
### Build Steps

```bash
# Clone the repository
git clone --recursive  <repository-url>

# Or, if you already have this repository cloned
git submodule init
git submodule update

# then

cd budget_monitor

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .

# Run the application
./budget_monitor
```

## Usage

1. **Launch Application**:
   - Click "Current Month" to access the current dashboard
   - Click "Savings" to access the yearly savings analysis
   - Click "Past Month" to access the month view of selected month
2. **View Dashboard**: See monthly summary with an income/expense breakdown
3. **Add Transaction**: Click "Add Entry" button
   - Select transaction type (Income/Expense)
   - Choose category and payment method
   - Enter amount and description
   - Click OK to save
4. **Add XLSX**: Click "Add XLSX file" button
   - Select Account (Account must match your file)
   - Add file path
   - Click OK to save
5. **Edit Table**: Double click on a transaction
   - Modify fields and press "OK" button
   - Press "Delete" to remove the transaction
6. **Edit Category**: Click "Edit" button
   - Press "+" to add Income/Expense/Account category
   - Press "-" to remove category (Be careful with this action)
   - Double-click on categories to rename them
7. **View Analytics**: Pie chart shows expense distribution
8. **Navigate**: Use "Back" button to return to main menu

## Roadmap

- [x] User authentication (Supabase)
- [x] XLSX bank statement import with auto-classification
- [x] Remote database integration (Supabase)
- [ ] Date range filtering
- [ ] Export to PDF
- [ ] Budget goals and alerts
- [ ] Multi-currency support

## Author
*Natalia B*
