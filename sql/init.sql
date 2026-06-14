BEGIN;

DROP TABLE IF EXISTS sales CASCADE;
DROP TABLE IF EXISTS prices CASCADE;
DROP TABLE IF EXISTS pledges CASCADE;
DROP TABLE IF EXISTS products CASCADE;
DROP TABLE IF EXISTS contracts CASCADE;
DROP TABLE IF EXISTS profiles CASCADE;
DROP TABLE IF EXISTS clients CASCADE;

CREATE TABLE clients (
    id SERIAL PRIMARY KEY,
    last_name VARCHAR(80) NOT NULL,
    first_name VARCHAR(80) NOT NULL,
    middle_name VARCHAR(80),
    passport VARCHAR(30) NOT NULL UNIQUE,
    address VARCHAR(255) NOT NULL,
    phone VARCHAR(30) NOT NULL,
    CONSTRAINT clients_required_not_blank CHECK (btrim(last_name) <> '' AND btrim(first_name) <> '' AND btrim(address) <> ''),
    CONSTRAINT clients_passport_not_blank CHECK (btrim(passport) <> ''),
    CONSTRAINT clients_phone_not_blank CHECK (btrim(phone) <> '')
);

CREATE TABLE profiles (
    id SERIAL PRIMARY KEY,
    client_id INTEGER NOT NULL UNIQUE REFERENCES clients(id) ON DELETE CASCADE,
    login VARCHAR(80) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    role VARCHAR(16) NOT NULL CHECK (role IN ('admin', 'user')),
    created_at TIMESTAMP NOT NULL DEFAULT now(),
    updated_at TIMESTAMP NOT NULL DEFAULT now(),
    CONSTRAINT profiles_login_not_blank CHECK (btrim(login) <> ''),
    CONSTRAINT profiles_hash_format CHECK (password_hash LIKE 'pbkdf2_sha256:%')
);

CREATE TABLE contracts (
    id SERIAL PRIMARY KEY,
    client_id INTEGER NOT NULL REFERENCES clients(id) ON DELETE RESTRICT,
    loan_amount NUMERIC(12, 2) NOT NULL CHECK (loan_amount > 0),
    commission NUMERIC(12, 2) NOT NULL CHECK (commission >= 0),
    issue_date DATE NOT NULL,
    due_date DATE NOT NULL,
    actual_return_date DATE,
    status VARCHAR(20) NOT NULL CHECK (status IN ('active', 'returned', 'overdue', 'sold', 'closed')),
    CONSTRAINT contracts_due_after_issue CHECK (due_date >= issue_date),
    CONSTRAINT contracts_return_after_issue CHECK (actual_return_date IS NULL OR actual_return_date >= issue_date)
);

CREATE TABLE products (
    id SERIAL PRIMARY KEY,
    name VARCHAR(120) NOT NULL,
    description TEXT,
    status VARCHAR(20) NOT NULL CHECK (status IN ('in_pledge', 'stored', 'for_sale', 'sold', 'returned')),
    CONSTRAINT products_name_not_blank CHECK (btrim(name) <> '')
);

CREATE TABLE pledges (
    id SERIAL PRIMARY KEY,
    contract_id INTEGER NOT NULL REFERENCES contracts(id) ON DELETE CASCADE,
    product_id INTEGER NOT NULL REFERENCES products(id) ON DELETE RESTRICT,
    estimated_value NUMERIC(12, 2) NOT NULL CHECK (estimated_value > 0),
    CONSTRAINT pledges_contract_product_unique UNIQUE (contract_id, product_id)
);

CREATE TABLE prices (
    id SERIAL PRIMARY KEY,
    product_id INTEGER NOT NULL REFERENCES products(id) ON DELETE CASCADE,
    price NUMERIC(12, 2) NOT NULL CHECK (price > 0),
    price_date DATE NOT NULL,
    CONSTRAINT prices_product_date_unique UNIQUE (product_id, price_date)
);

CREATE TABLE sales (
    id SERIAL PRIMARY KEY,
    price_id INTEGER NOT NULL REFERENCES prices(id) ON DELETE RESTRICT,
    sale_date DATE NOT NULL
);

CREATE UNIQUE INDEX idx_profiles_login_lower ON profiles (lower(login));
CREATE INDEX idx_contracts_client_id ON contracts (client_id);
CREATE INDEX idx_pledges_contract_id ON pledges (contract_id);
CREATE INDEX idx_pledges_product_id ON pledges (product_id);
CREATE INDEX idx_prices_product_id ON prices (product_id);
CREATE INDEX idx_sales_price_id ON sales (price_id);

CREATE OR REPLACE FUNCTION set_updated_at()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = now();
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER profiles_set_updated_at
BEFORE UPDATE ON profiles
FOR EACH ROW
EXECUTE FUNCTION set_updated_at();

INSERT INTO clients (id, last_name, first_name, middle_name, passport, address, phone) VALUES
    (1, 'Администратор', 'Системный', NULL, '0000 000001', 'Служебная запись', '+7 900 000-00-01'),
    (2, 'Иванов', 'Иван', 'Иванович', '1234 567890', 'г. Москва, ул. Тверская, д. 1', '+7 900 111-22-33');

INSERT INTO profiles (id, client_id, login, password_hash, role) VALUES
    (1, 1, 'admin', 'pbkdf2_sha256:120000:cGF3bnNob3AtYWRtaW4tc2FsdA==:6e8URxWgBfJLrdQq5v7/PChQpZcb+1pWg+g7J7tDdc8=', 'admin'),
    (2, 2, 'user', 'pbkdf2_sha256:120000:cGF3bnNob3AtdXNlci1zYWx0:LtvADaNfqjQ9MQjVSDmzq2V2xXuA/FPoqEZ3JNzbp9A=', 'user');

INSERT INTO contracts (id, client_id, loan_amount, commission, issue_date, due_date, actual_return_date, status) VALUES
    (1, 2, 25000.00, 2500.00, DATE '2026-06-01', DATE '2026-07-01', NULL, 'active'),
    (2, 2, 12000.00, 1200.00, DATE '2026-04-01', DATE '2026-05-01', DATE '2026-05-20', 'sold');

INSERT INTO products (id, name, description, status) VALUES
    (1, 'Ноутбук Lenovo', 'Ноутбук в рабочем состоянии, зарядное устройство в комплекте.', 'in_pledge'),
    (2, 'Смартфон Samsung', 'Смартфон после оценки, выставлен и продан.', 'sold');

INSERT INTO pledges (id, contract_id, product_id, estimated_value) VALUES
    (1, 1, 1, 35000.00),
    (2, 2, 2, 18000.00);

INSERT INTO prices (id, product_id, price, price_date) VALUES
    (1, 1, 42000.00, DATE '2026-06-01'),
    (2, 2, 21000.00, DATE '2026-05-10');

INSERT INTO sales (id, price_id, sale_date) VALUES
    (1, 2, DATE '2026-05-20');

SELECT setval(pg_get_serial_sequence('clients', 'id'), COALESCE(MAX(id), 1), true) FROM clients;
SELECT setval(pg_get_serial_sequence('profiles', 'id'), COALESCE(MAX(id), 1), true) FROM profiles;
SELECT setval(pg_get_serial_sequence('contracts', 'id'), COALESCE(MAX(id), 1), true) FROM contracts;
SELECT setval(pg_get_serial_sequence('products', 'id'), COALESCE(MAX(id), 1), true) FROM products;
SELECT setval(pg_get_serial_sequence('pledges', 'id'), COALESCE(MAX(id), 1), true) FROM pledges;
SELECT setval(pg_get_serial_sequence('prices', 'id'), COALESCE(MAX(id), 1), true) FROM prices;
SELECT setval(pg_get_serial_sequence('sales', 'id'), COALESCE(MAX(id), 1), true) FROM sales;

COMMIT;
