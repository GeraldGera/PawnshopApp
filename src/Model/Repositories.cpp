#include "Model/Repositories.h"

#include "Model/DatabaseManager.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>

namespace
{
QSqlDatabase db()
{
    return DatabaseManager::instance().database();
}

QVariant value(const QSqlQuery& query, const char* name)
{
    return query.value(query.record().indexOf(QLatin1String(name)));
}

bool exec(QSqlQuery& query, QString* errorMessage)
{
    if (!query.exec()) {
        if (errorMessage) {
            *errorMessage = query.lastError().text();
        }
        return false;
    }
    return true;
}

bool execReturningId(QSqlQuery& query, int& id, QString* errorMessage)
{
    if (!exec(query, errorMessage)) {
        return false;
    }

    if (!query.next()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Запрос не вернул идентификатор созданной записи.");
        }
        return false;
    }

    id = query.value(0).toInt();
    return true;
}

QVariant nullableDate(const QDate& date)
{
    return date.isValid() ? QVariant(date) : QVariant();
}

Client mapClient(const QSqlQuery& query)
{
    Client client;
    client.id = value(query, "id").toInt();
    client.lastName = value(query, "last_name").toString();
    client.firstName = value(query, "first_name").toString();
    client.middleName = value(query, "middle_name").toString();
    client.passport = value(query, "passport").toString();
    client.address = value(query, "address").toString();
    client.phone = value(query, "phone").toString();
    return client;
}

Profile mapProfile(const QSqlQuery& query)
{
    Profile profile;
    profile.id = value(query, "id").toInt();
    profile.clientId = value(query, "client_id").toInt();
    profile.login = value(query, "login").toString();
    profile.passwordHash = value(query, "password_hash").toString();
    profile.role = userRoleFromString(value(query, "role").toString());
    return profile;
}

Contract mapContract(const QSqlQuery& query)
{
    Contract contract;
    contract.id = value(query, "id").toInt();
    contract.clientId = value(query, "client_id").toInt();
    contract.loanAmount = value(query, "loan_amount").toDouble();
    contract.commission = value(query, "commission").toDouble();
    contract.issueDate = value(query, "issue_date").toDate();
    contract.dueDate = value(query, "due_date").toDate();
    contract.actualReturnDate = value(query, "actual_return_date").toDate();
    contract.status = value(query, "status").toString();
    return contract;
}

Product mapProduct(const QSqlQuery& query)
{
    Product product;
    product.id = value(query, "id").toInt();
    product.name = value(query, "name").toString();
    product.description = value(query, "description").toString();
    product.status = value(query, "status").toString();
    return product;
}

Pledge mapPledge(const QSqlQuery& query)
{
    Pledge pledge;
    pledge.id = value(query, "id").toInt();
    pledge.contractId = value(query, "contract_id").toInt();
    pledge.productId = value(query, "product_id").toInt();
    pledge.estimatedValue = value(query, "estimated_value").toDouble();
    return pledge;
}

Price mapPrice(const QSqlQuery& query)
{
    Price price;
    price.id = value(query, "id").toInt();
    price.productId = value(query, "product_id").toInt();
    price.price = value(query, "price").toDouble();
    price.priceDate = value(query, "price_date").toDate();
    return price;
}

Sale mapSale(const QSqlQuery& query)
{
    Sale sale;
    sale.id = value(query, "id").toInt();
    sale.priceId = value(query, "price_id").toInt();
    sale.saleDate = value(query, "sale_date").toDate();
    return sale;
}
}

QVector<Client> ClientRepository::findAll(QString* errorMessage) const
{
    QVector<Client> clients;
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "SELECT id, last_name, first_name, middle_name, passport, address, phone "
        "FROM clients ORDER BY id"));

    if (!exec(query, errorMessage)) {
        return clients;
    }

    while (query.next()) {
        clients.append(mapClient(query));
    }
    return clients;
}

std::optional<Client> ClientRepository::findById(int id, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "SELECT id, last_name, first_name, middle_name, passport, address, phone "
        "FROM clients WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!exec(query, errorMessage) || !query.next()) {
        return std::nullopt;
    }

    return mapClient(query);
}

bool ClientRepository::create(Client& client, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "INSERT INTO clients (last_name, first_name, middle_name, passport, address, phone) "
        "VALUES (:last_name, :first_name, :middle_name, :passport, :address, :phone) "
        "RETURNING id"));
    query.bindValue(QStringLiteral(":last_name"), client.lastName);
    query.bindValue(QStringLiteral(":first_name"), client.firstName);
    query.bindValue(QStringLiteral(":middle_name"), client.middleName);
    query.bindValue(QStringLiteral(":passport"), client.passport);
    query.bindValue(QStringLiteral(":address"), client.address);
    query.bindValue(QStringLiteral(":phone"), client.phone);
    return execReturningId(query, client.id, errorMessage);
}

bool ClientRepository::update(const Client& client, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "UPDATE clients SET last_name = :last_name, first_name = :first_name, "
        "middle_name = :middle_name, passport = :passport, address = :address, phone = :phone "
        "WHERE id = :id"));
    query.bindValue(QStringLiteral(":last_name"), client.lastName);
    query.bindValue(QStringLiteral(":first_name"), client.firstName);
    query.bindValue(QStringLiteral(":middle_name"), client.middleName);
    query.bindValue(QStringLiteral(":passport"), client.passport);
    query.bindValue(QStringLiteral(":address"), client.address);
    query.bindValue(QStringLiteral(":phone"), client.phone);
    query.bindValue(QStringLiteral(":id"), client.id);
    return exec(query, errorMessage);
}

bool ClientRepository::remove(int id, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral("DELETE FROM clients WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);
    return exec(query, errorMessage);
}

QVector<Profile> ProfileRepository::findAll(QString* errorMessage) const
{
    QVector<Profile> profiles;
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "SELECT id, client_id, login, password_hash, role FROM profiles ORDER BY id"));

    if (!exec(query, errorMessage)) {
        return profiles;
    }

    while (query.next()) {
        profiles.append(mapProfile(query));
    }
    return profiles;
}

std::optional<Profile> ProfileRepository::findById(int id, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "SELECT id, client_id, login, password_hash, role FROM profiles WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!exec(query, errorMessage) || !query.next()) {
        return std::nullopt;
    }

    return mapProfile(query);
}

bool ProfileRepository::loginExists(const QString& login, int exceptProfileId, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "SELECT 1 FROM profiles WHERE lower(login) = lower(?) AND (? <= 0 OR id <> ?) LIMIT 1"));
    query.addBindValue(login);
    query.addBindValue(exceptProfileId);
    query.addBindValue(exceptProfileId);

    if (!exec(query, errorMessage)) {
        return true;
    }

    return query.next();
}

bool ProfileRepository::create(Profile& profile, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "INSERT INTO profiles (client_id, login, password_hash, role) "
        "VALUES (:client_id, :login, :password_hash, :role) RETURNING id"));
    query.bindValue(QStringLiteral(":client_id"), profile.clientId);
    query.bindValue(QStringLiteral(":login"), profile.login);
    query.bindValue(QStringLiteral(":password_hash"), profile.passwordHash);
    query.bindValue(QStringLiteral(":role"), userRoleToString(profile.role));
    return execReturningId(query, profile.id, errorMessage);
}

bool ProfileRepository::update(const Profile& profile, bool updatePassword, QString* errorMessage) const
{
    QSqlQuery query(db());
    if (updatePassword) {
        query.prepare(QStringLiteral(
            "UPDATE profiles SET client_id = :client_id, login = :login, "
            "password_hash = :password_hash, role = :role, updated_at = now() WHERE id = :id"));
        query.bindValue(QStringLiteral(":password_hash"), profile.passwordHash);
    } else {
        query.prepare(QStringLiteral(
            "UPDATE profiles SET client_id = :client_id, login = :login, "
            "role = :role, updated_at = now() WHERE id = :id"));
    }

    query.bindValue(QStringLiteral(":client_id"), profile.clientId);
    query.bindValue(QStringLiteral(":login"), profile.login);
    query.bindValue(QStringLiteral(":role"), userRoleToString(profile.role));
    query.bindValue(QStringLiteral(":id"), profile.id);
    return exec(query, errorMessage);
}

bool ProfileRepository::remove(int id, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral("DELETE FROM profiles WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);
    return exec(query, errorMessage);
}

std::optional<UserSession> AuthRepository::findSessionByLogin(const QString& login, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "SELECT "
        "p.id AS profile_id, p.client_id, p.login, p.password_hash, p.role, "
        "c.id AS id, c.last_name, c.first_name, c.middle_name, c.passport, c.address, c.phone "
        "FROM profiles p "
        "JOIN clients c ON c.id = p.client_id "
        "WHERE lower(p.login) = lower(:login)"));
    query.bindValue(QStringLiteral(":login"), login);

    if (!exec(query, errorMessage) || !query.next()) {
        return std::nullopt;
    }

    UserSession session;
    session.client = mapClient(query);
    session.profile.id = value(query, "profile_id").toInt();
    session.profile.clientId = value(query, "client_id").toInt();
    session.profile.login = value(query, "login").toString();
    session.profile.passwordHash = value(query, "password_hash").toString();
    session.profile.role = userRoleFromString(value(query, "role").toString());
    return session;
}

bool AuthRepository::createUser(Client& client, Profile& profile, QString* errorMessage) const
{
    auto& database = DatabaseManager::instance();
    if (!database.beginTransaction(errorMessage)) {
        return false;
    }

    ClientRepository clientRepository;
    ProfileRepository profileRepository;

    if (!clientRepository.create(client, errorMessage)) {
        database.rollback();
        return false;
    }

    profile.clientId = client.id;
    if (!profileRepository.create(profile, errorMessage)) {
        database.rollback();
        return false;
    }

    if (!database.commit(errorMessage)) {
        database.rollback();
        return false;
    }

    return true;
}

QVector<Contract> ContractRepository::findAll(QString* errorMessage) const
{
    QVector<Contract> contracts;
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "SELECT id, client_id, loan_amount, commission, issue_date, due_date, actual_return_date, status "
        "FROM contracts ORDER BY id"));

    if (!exec(query, errorMessage)) {
        return contracts;
    }

    while (query.next()) {
        contracts.append(mapContract(query));
    }
    return contracts;
}

QVector<Contract> ContractRepository::findByClientId(int clientId, QString* errorMessage) const
{
    QVector<Contract> contracts;
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "SELECT id, client_id, loan_amount, commission, issue_date, due_date, actual_return_date, status "
        "FROM contracts WHERE client_id = :client_id ORDER BY id"));
    query.bindValue(QStringLiteral(":client_id"), clientId);

    if (!exec(query, errorMessage)) {
        return contracts;
    }

    while (query.next()) {
        contracts.append(mapContract(query));
    }
    return contracts;
}

bool ContractRepository::create(Contract& contract, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "INSERT INTO contracts (client_id, loan_amount, commission, issue_date, due_date, actual_return_date, status) "
        "VALUES (:client_id, :loan_amount, :commission, :issue_date, :due_date, :actual_return_date, :status) "
        "RETURNING id"));
    query.bindValue(QStringLiteral(":client_id"), contract.clientId);
    query.bindValue(QStringLiteral(":loan_amount"), contract.loanAmount);
    query.bindValue(QStringLiteral(":commission"), contract.commission);
    query.bindValue(QStringLiteral(":issue_date"), contract.issueDate);
    query.bindValue(QStringLiteral(":due_date"), contract.dueDate);
    query.bindValue(QStringLiteral(":actual_return_date"), nullableDate(contract.actualReturnDate));
    query.bindValue(QStringLiteral(":status"), contract.status);
    return execReturningId(query, contract.id, errorMessage);
}

bool ContractRepository::update(const Contract& contract, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "UPDATE contracts SET client_id = :client_id, loan_amount = :loan_amount, commission = :commission, "
        "issue_date = :issue_date, due_date = :due_date, actual_return_date = :actual_return_date, status = :status "
        "WHERE id = :id"));
    query.bindValue(QStringLiteral(":client_id"), contract.clientId);
    query.bindValue(QStringLiteral(":loan_amount"), contract.loanAmount);
    query.bindValue(QStringLiteral(":commission"), contract.commission);
    query.bindValue(QStringLiteral(":issue_date"), contract.issueDate);
    query.bindValue(QStringLiteral(":due_date"), contract.dueDate);
    query.bindValue(QStringLiteral(":actual_return_date"), nullableDate(contract.actualReturnDate));
    query.bindValue(QStringLiteral(":status"), contract.status);
    query.bindValue(QStringLiteral(":id"), contract.id);
    return exec(query, errorMessage);
}

bool ContractRepository::remove(int id, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral("DELETE FROM contracts WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);
    return exec(query, errorMessage);
}

QVector<Product> ProductRepository::findAll(QString* errorMessage) const
{
    QVector<Product> products;
    QSqlQuery query(db());
    query.prepare(QStringLiteral("SELECT id, name, description, status FROM products ORDER BY id"));

    if (!exec(query, errorMessage)) {
        return products;
    }

    while (query.next()) {
        products.append(mapProduct(query));
    }
    return products;
}

QVector<Product> ProductRepository::findByClientId(int clientId, QString* errorMessage) const
{
    QVector<Product> products;
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "SELECT DISTINCT p.id, p.name, p.description, p.status "
        "FROM products p "
        "JOIN pledges pl ON pl.product_id = p.id "
        "JOIN contracts c ON c.id = pl.contract_id "
        "WHERE c.client_id = :client_id ORDER BY p.id"));
    query.bindValue(QStringLiteral(":client_id"), clientId);

    if (!exec(query, errorMessage)) {
        return products;
    }

    while (query.next()) {
        products.append(mapProduct(query));
    }
    return products;
}

bool ProductRepository::create(Product& product, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "INSERT INTO products (name, description, status) "
        "VALUES (:name, :description, :status) RETURNING id"));
    query.bindValue(QStringLiteral(":name"), product.name);
    query.bindValue(QStringLiteral(":description"), product.description);
    query.bindValue(QStringLiteral(":status"), product.status);
    return execReturningId(query, product.id, errorMessage);
}

bool ProductRepository::update(const Product& product, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "UPDATE products SET name = :name, description = :description, status = :status WHERE id = :id"));
    query.bindValue(QStringLiteral(":name"), product.name);
    query.bindValue(QStringLiteral(":description"), product.description);
    query.bindValue(QStringLiteral(":status"), product.status);
    query.bindValue(QStringLiteral(":id"), product.id);
    return exec(query, errorMessage);
}

bool ProductRepository::remove(int id, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral("DELETE FROM products WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);
    return exec(query, errorMessage);
}

QVector<Pledge> PledgeRepository::findAll(QString* errorMessage) const
{
    QVector<Pledge> pledges;
    QSqlQuery query(db());
    query.prepare(QStringLiteral("SELECT id, contract_id, product_id, estimated_value FROM pledges ORDER BY id"));

    if (!exec(query, errorMessage)) {
        return pledges;
    }

    while (query.next()) {
        pledges.append(mapPledge(query));
    }
    return pledges;
}

QVector<Pledge> PledgeRepository::findByClientId(int clientId, QString* errorMessage) const
{
    QVector<Pledge> pledges;
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "SELECT pl.id, pl.contract_id, pl.product_id, pl.estimated_value "
        "FROM pledges pl "
        "JOIN contracts c ON c.id = pl.contract_id "
        "WHERE c.client_id = :client_id ORDER BY pl.id"));
    query.bindValue(QStringLiteral(":client_id"), clientId);

    if (!exec(query, errorMessage)) {
        return pledges;
    }

    while (query.next()) {
        pledges.append(mapPledge(query));
    }
    return pledges;
}

bool PledgeRepository::create(Pledge& pledge, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "INSERT INTO pledges (contract_id, product_id, estimated_value) "
        "VALUES (:contract_id, :product_id, :estimated_value) RETURNING id"));
    query.bindValue(QStringLiteral(":contract_id"), pledge.contractId);
    query.bindValue(QStringLiteral(":product_id"), pledge.productId);
    query.bindValue(QStringLiteral(":estimated_value"), pledge.estimatedValue);
    return execReturningId(query, pledge.id, errorMessage);
}

bool PledgeRepository::update(const Pledge& pledge, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "UPDATE pledges SET contract_id = :contract_id, product_id = :product_id, "
        "estimated_value = :estimated_value WHERE id = :id"));
    query.bindValue(QStringLiteral(":contract_id"), pledge.contractId);
    query.bindValue(QStringLiteral(":product_id"), pledge.productId);
    query.bindValue(QStringLiteral(":estimated_value"), pledge.estimatedValue);
    query.bindValue(QStringLiteral(":id"), pledge.id);
    return exec(query, errorMessage);
}

bool PledgeRepository::remove(int id, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral("DELETE FROM pledges WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);
    return exec(query, errorMessage);
}

QVector<Price> PriceRepository::findAll(QString* errorMessage) const
{
    QVector<Price> prices;
    QSqlQuery query(db());
    query.prepare(QStringLiteral("SELECT id, product_id, price, price_date FROM prices ORDER BY id"));

    if (!exec(query, errorMessage)) {
        return prices;
    }

    while (query.next()) {
        prices.append(mapPrice(query));
    }
    return prices;
}

QVector<Price> PriceRepository::findByClientId(int clientId, QString* errorMessage) const
{
    QVector<Price> prices;
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "SELECT DISTINCT pr.id, pr.product_id, pr.price, pr.price_date "
        "FROM prices pr "
        "JOIN products p ON p.id = pr.product_id "
        "JOIN pledges pl ON pl.product_id = p.id "
        "JOIN contracts c ON c.id = pl.contract_id "
        "WHERE c.client_id = :client_id ORDER BY pr.id"));
    query.bindValue(QStringLiteral(":client_id"), clientId);

    if (!exec(query, errorMessage)) {
        return prices;
    }

    while (query.next()) {
        prices.append(mapPrice(query));
    }
    return prices;
}

bool PriceRepository::create(Price& price, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "INSERT INTO prices (product_id, price, price_date) "
        "VALUES (:product_id, :price, :price_date) RETURNING id"));
    query.bindValue(QStringLiteral(":product_id"), price.productId);
    query.bindValue(QStringLiteral(":price"), price.price);
    query.bindValue(QStringLiteral(":price_date"), price.priceDate);
    return execReturningId(query, price.id, errorMessage);
}

bool PriceRepository::update(const Price& price, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "UPDATE prices SET product_id = :product_id, price = :price, price_date = :price_date WHERE id = :id"));
    query.bindValue(QStringLiteral(":product_id"), price.productId);
    query.bindValue(QStringLiteral(":price"), price.price);
    query.bindValue(QStringLiteral(":price_date"), price.priceDate);
    query.bindValue(QStringLiteral(":id"), price.id);
    return exec(query, errorMessage);
}

bool PriceRepository::remove(int id, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral("DELETE FROM prices WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);
    return exec(query, errorMessage);
}

QVector<Sale> SaleRepository::findAll(QString* errorMessage) const
{
    QVector<Sale> sales;
    QSqlQuery query(db());
    query.prepare(QStringLiteral("SELECT id, price_id, sale_date FROM sales ORDER BY id"));

    if (!exec(query, errorMessage)) {
        return sales;
    }

    while (query.next()) {
        sales.append(mapSale(query));
    }
    return sales;
}

QVector<Sale> SaleRepository::findByClientId(int clientId, QString* errorMessage) const
{
    QVector<Sale> sales;
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "SELECT DISTINCT s.id, s.price_id, s.sale_date "
        "FROM sales s "
        "JOIN prices pr ON pr.id = s.price_id "
        "JOIN products p ON p.id = pr.product_id "
        "JOIN pledges pl ON pl.product_id = p.id "
        "JOIN contracts c ON c.id = pl.contract_id "
        "WHERE c.client_id = :client_id ORDER BY s.id"));
    query.bindValue(QStringLiteral(":client_id"), clientId);

    if (!exec(query, errorMessage)) {
        return sales;
    }

    while (query.next()) {
        sales.append(mapSale(query));
    }
    return sales;
}

bool SaleRepository::create(Sale& sale, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "INSERT INTO sales (price_id, sale_date) VALUES (:price_id, :sale_date) RETURNING id"));
    query.bindValue(QStringLiteral(":price_id"), sale.priceId);
    query.bindValue(QStringLiteral(":sale_date"), sale.saleDate);
    return execReturningId(query, sale.id, errorMessage);
}

bool SaleRepository::update(const Sale& sale, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral(
        "UPDATE sales SET price_id = :price_id, sale_date = :sale_date WHERE id = :id"));
    query.bindValue(QStringLiteral(":price_id"), sale.priceId);
    query.bindValue(QStringLiteral(":sale_date"), sale.saleDate);
    query.bindValue(QStringLiteral(":id"), sale.id);
    return exec(query, errorMessage);
}

bool SaleRepository::remove(int id, QString* errorMessage) const
{
    QSqlQuery query(db());
    query.prepare(QStringLiteral("DELETE FROM sales WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);
    return exec(query, errorMessage);
}
