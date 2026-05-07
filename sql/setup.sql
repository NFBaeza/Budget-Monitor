-- Budget Monitor - Database Setup
-- Run this script in your Supabase SQL Editor (Dashboard > SQL Editor)

CREATE TABLE IF NOT EXISTS categories (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    user_id UUID NOT NULL,
    name TEXT NOT NULL,
    type TEXT NOT NULL CHECK (type IN ('income', 'expense', 'transfer'))
);

CREATE TABLE IF NOT EXISTS accounts (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    user_id UUID NOT NULL,
    name TEXT NOT NULL,
    type TEXT NOT NULL CHECK (type IN ('debit', 'credit', 'investment'))
);

CREATE TABLE IF NOT EXISTS money_transactions (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    user_id UUID NOT NULL,
    date TIMESTAMP NOT NULL,
    amount INTEGER NOT NULL,
    category_id BIGINT REFERENCES categories(id),
    account_id BIGINT REFERENCES accounts(id),
    description TEXT,
    original_description TEXT,
    CONSTRAINT unique_transaction UNIQUE (user_id, date, amount, account_id, original_description)
);