-- Budget Monitor - Database Setup
-- Run this script in your Supabase SQL Editor (Dashboard > SQL Editor)

CREATE TABLE IF NOT EXISTS categories (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    user_id UUID NOT NULL,
    name TEXT NOT NULL,
    type TEXT NOT NULL CHECK (type IN ('income', 'expense'))
);

CREATE TABLE IF NOT EXISTS accounts (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    user_id UUID NOT NULL,
    name TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS money_transactions (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    user_id UUID NOT NULL,
    date DATE NOT NULL,
    amount INTEGER NOT NULL,
    category_id BIGINT REFERENCES categories(id),
    account_id BIGINT REFERENCES accounts(id),
    description TEXT
);