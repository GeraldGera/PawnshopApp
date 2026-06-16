#include "Controller/DataController.h"

#include "Model/PasswordUtils.h"
#include "Model/Repositories.h"

#include <QDate>
#include <utility>

namespace
{
QVariantMap toMap(const Client& client)
{
    return {
        {QStringLiteral("id"), client.id},
        {QStringLiteral("last_name"), client.lastName},
        {QStringLiteral("first_name"), client.firstName},
        {QStringLiteral("middle_name"), client.middleName},
        {QStringLiteral("passport"), client.passport},
        {QStringLiteral("address"), client.address},
        {QStringLiteral("phone"), client.phone},
    };
}

QVariantMap toMap(const Profile& profile)
{
    return {
        {QStringLiteral("id"), profile.id},
        {QStringLiteral("client_id"), profile.clientId},
        {QStringLiteral("login"), profile.login},
        {QStringLiteral("password"), QString()},
        {QStringLiteral("role"), userRoleToString(profile.role)},
    };
}

QVariantMap toMap(const Contract& contract)
{
    return {
        {QStringLiteral("id"), contract.id},
        {QStringLiteral("client_id"), contract.clientId},
        {QStringLiteral("loan_amount"), contract.loanAmount},
        {QStringLiteral("commission"), contract.commission},
        {QStringLiteral("issue_date"), contract.issueDate},
        {QStringLiteral("due_date"), contract.dueDate},
        {QStringLiteral("actual_return_date"), contract.actualReturnDate},
        {QStringLiteral("status"), contract.status},
    };
}

QVariantMap toMap(const Product& product)
{
    return {
        {QStringLiteral("id"), product.id},
        {QStringLiteral("name"), product.name},
        {QStringLiteral("description"), product.description},
        {QStringLiteral("status"), product.status},
    };
}

QVariantMap toMap(const Pledge& pledge)
{
    return {
        {QStringLiteral("id"), pledge.id},
        {QStringLiteral("contract_id"), pledge.contractId},
        {QStringLiteral("product_id"), pledge.productId},
        {QStringLiteral("estimated_value"), pledge.estimatedValue},
    };
}

QVariantMap toMap(const Price& price)
{
    return {
        {QStringLiteral("id"), price.id},
        {QStringLiteral("product_id"), price.productId},
        {QStringLiteral("price"), price.price},
        {QStringLiteral("price_date"), price.priceDate},
    };
}

QVariantMap toMap(const Sale& sale)
{
    return {
        {QStringLiteral("id"), sale.id},
        {QStringLiteral("price_id"), sale.priceId},
        {QStringLiteral("sale_date"), sale.saleDate},
    };
}

Client clientFromMap(const QVariantMap& values)
{
    Client client;
    client.id = values.value(QStringLiteral("id")).toInt();
    client.lastName = values.value(QStringLiteral("last_name")).toString().trimmed();
    client.firstName = values.value(QStringLiteral("first_name")).toString().trimmed();
    client.middleName = values.value(QStringLiteral("middle_name")).toString().trimmed();
    client.passport = values.value(QStringLiteral("passport")).toString().trimmed();
    client.address = values.value(QStringLiteral("address")).toString().trimmed();
    client.phone = values.value(QStringLiteral("phone")).toString().trimmed();
    return client;
}

Contract contractFromMap(const QVariantMap& values)
{
    Contract contract;
    contract.id = values.value(QStringLiteral("id")).toInt();
    contract.clientId = values.value(QStringLiteral("client_id")).toInt();
    contract.loanAmount = values.value(QStringLiteral("loan_amount")).toDouble();
    contract.commission = values.value(QStringLiteral("commission")).toDouble();
    contract.issueDate = values.value(QStringLiteral("issue_date")).toDate();
    contract.dueDate = values.value(QStringLiteral("due_date")).toDate();
    contract.actualReturnDate = values.value(QStringLiteral("actual_return_date")).toDate();
    contract.status = values.value(QStringLiteral("status")).toString();
    return contract;
}

Product productFromMap(const QVariantMap& values)
{
    Product product;
    product.id = values.value(QStringLiteral("id")).toInt();
    product.name = values.value(QStringLiteral("name")).toString().trimmed();
    product.description = values.value(QStringLiteral("description")).toString().trimmed();
    product.status = values.value(QStringLiteral("status")).toString();
    return product;
}

Pledge pledgeFromMap(const QVariantMap& values)
{
    Pledge pledge;
    pledge.id = values.value(QStringLiteral("id")).toInt();
    pledge.contractId = values.value(QStringLiteral("contract_id")).toInt();
    pledge.productId = values.value(QStringLiteral("product_id")).toInt();
    pledge.estimatedValue = values.value(QStringLiteral("estimated_value")).toDouble();
    return pledge;
}

Price priceFromMap(const QVariantMap& values)
{
    Price price;
    price.id = values.value(QStringLiteral("id")).toInt();
    price.productId = values.value(QStringLiteral("product_id")).toInt();
    price.price = values.value(QStringLiteral("price")).toDouble();
    price.priceDate = values.value(QStringLiteral("price_date")).toDate();
    return price;
}

Sale saleFromMap(const QVariantMap& values)
{
    Sale sale;
    sale.id = values.value(QStringLiteral("id")).toInt();
    sale.priceId = values.value(QStringLiteral("price_id")).toInt();
    sale.saleDate = values.value(QStringLiteral("sale_date")).toDate();
    return sale;
}

template <typename T>
QVector<QVariantMap> toMaps(const QVector<T>& objects)
{
    QVector<QVariantMap> rows;
    rows.reserve(objects.size());
    for (const auto& object : objects) {
        rows.append(toMap(object));
    }
    return rows;
}

bool fail(const QString& message, QString* errorMessage)
{
    if (errorMessage) {
        *errorMessage = message;
    }
    return false;
}
}

DataController::DataController(UserSession session)
    : m_session(std::move(session))
{
}

bool DataController::isAdmin() const
{
    return m_session.isAdmin();
}

QVector<QVariantMap> DataController::clients(QString* errorMessage) const
{
    if (!requireAdmin(errorMessage)) {
        return {};
    }
    return toMaps(ClientRepository().findAll(errorMessage));
}

QVector<QVariantMap> DataController::profiles(QString* errorMessage) const
{
    if (!requireAdmin(errorMessage)) {
        return {};
    }
    return toMaps(ProfileRepository().findAll(errorMessage));
}

QVector<QVariantMap> DataController::contracts(QString* errorMessage) const
{
    ContractRepository repository;
    return toMaps(isAdmin()
        ? repository.findAll(errorMessage)
        : repository.findByClientId(m_session.client.id, errorMessage));
}

QVector<QVariantMap> DataController::products(QString* errorMessage) const
{
    ProductRepository repository;
    return toMaps(isAdmin()
        ? repository.findAll(errorMessage)
        : repository.findByClientId(m_session.client.id, errorMessage));
}

QVector<QVariantMap> DataController::pledges(QString* errorMessage) const
{
    PledgeRepository repository;
    return toMaps(isAdmin()
        ? repository.findAll(errorMessage)
        : repository.findByClientId(m_session.client.id, errorMessage));
}

QVector<QVariantMap> DataController::prices(QString* errorMessage) const
{
    PriceRepository repository;
    return toMaps(isAdmin()
        ? repository.findAll(errorMessage)
        : repository.findByClientId(m_session.client.id, errorMessage));
}

QVector<QVariantMap> DataController::sales(QString* errorMessage) const
{
    SaleRepository repository;
    return toMaps(isAdmin()
        ? repository.findAll(errorMessage)
        : repository.findByClientId(m_session.client.id, errorMessage));
}

bool DataController::saveClient(QVariantMap& values, QString* errorMessage) const
{
    if (!requireAdmin(errorMessage)) {
        return false;
    }

    Client client = clientFromMap(values);
    if (client.lastName.isEmpty() || client.firstName.isEmpty() || client.passport.isEmpty()
        || client.address.isEmpty() || client.phone.isEmpty()) {
        return fail(QStringLiteral("Заполните обязательные поля клиента."), errorMessage);
    }

    ClientRepository repository;
    const bool ok = client.id == 0 ? repository.create(client, errorMessage) : repository.update(client, errorMessage);
    if (ok) {
        values = toMap(client);
    }
    return ok;
}

bool DataController::saveProfile(QVariantMap& values, QString* errorMessage) const
{
    if (!requireAdmin(errorMessage)) {
        return false;
    }

    Profile profile;
    profile.id = values.value(QStringLiteral("id")).toInt();
    profile.clientId = values.value(QStringLiteral("client_id")).toInt();
    profile.login = values.value(QStringLiteral("login")).toString().trimmed();
    profile.role = userRoleFromString(values.value(QStringLiteral("role")).toString());
    const QString password = values.value(QStringLiteral("password")).toString();

    if (profile.clientId <= 0 || profile.login.isEmpty()) {
        return fail(QStringLiteral("Укажите клиента и логин профиля."), errorMessage);
    }

    ProfileRepository repository;
    if (repository.loginExists(profile.login, profile.id, errorMessage)) {
        if (errorMessage && errorMessage->isEmpty()) {
            *errorMessage = QStringLiteral("Такой логин уже занят.");
        }
        return false;
    }

    const bool isNew = profile.id == 0;
    const bool updatePassword = !password.isEmpty();
    if (isNew && !updatePassword) {
        return fail(QStringLiteral("Для нового профиля нужен пароль."), errorMessage);
    }

    if (updatePassword) {
        if (!PasswordUtils::isStrongPassword(password)) {
            return fail(QStringLiteral("Пароль должен быть не короче 8 символов и содержать цифру, заглавную букву и спецсимвол."), errorMessage);
        }
        profile.passwordHash = PasswordUtils::hashPassword(password);
    }

    const bool ok = isNew ? repository.create(profile, errorMessage) : repository.update(profile, updatePassword, errorMessage);
    if (ok) {
        values = toMap(profile);
    }
    return ok;
}

bool DataController::saveContract(QVariantMap& values, QString* errorMessage) const
{
    if (!requireAdmin(errorMessage)) {
        return false;
    }

    Contract contract = contractFromMap(values);
    if (contract.clientId <= 0 || contract.loanAmount <= 0.0 || contract.commission < 0.0
        || !contract.issueDate.isValid() || !contract.dueDate.isValid() || contract.status.isEmpty()) {
        return fail(QStringLiteral("Проверьте данные договора."), errorMessage);
    }

    ContractRepository repository;
    const bool ok = contract.id == 0 ? repository.create(contract, errorMessage) : repository.update(contract, errorMessage);
    if (ok) {
        values = toMap(contract);
    }
    return ok;
}

bool DataController::saveProduct(QVariantMap& values, QString* errorMessage) const
{
    if (!requireAdmin(errorMessage)) {
        return false;
    }

    Product product = productFromMap(values);
    if (product.name.isEmpty() || product.status.isEmpty()) {
        return fail(QStringLiteral("Укажите название и статус товара."), errorMessage);
    }

    ProductRepository repository;
    const bool ok = product.id == 0 ? repository.create(product, errorMessage) : repository.update(product, errorMessage);
    if (ok) {
        values = toMap(product);
    }
    return ok;
}

bool DataController::savePledge(QVariantMap& values, QString* errorMessage) const
{
    if (!requireAdmin(errorMessage)) {
        return false;
    }

    Pledge pledge = pledgeFromMap(values);
    if (pledge.contractId <= 0 || pledge.productId <= 0 || pledge.estimatedValue <= 0.0) {
        return fail(QStringLiteral("Проверьте данные залога."), errorMessage);
    }

    PledgeRepository repository;
    const bool ok = pledge.id == 0 ? repository.create(pledge, errorMessage) : repository.update(pledge, errorMessage);
    if (ok) {
        values = toMap(pledge);
    }
    return ok;
}

bool DataController::savePrice(QVariantMap& values, QString* errorMessage) const
{
    if (!requireAdmin(errorMessage)) {
        return false;
    }

    Price price = priceFromMap(values);
    if (price.productId <= 0 || price.price <= 0.0 || !price.priceDate.isValid()) {
        return fail(QStringLiteral("Проверьте цену товара."), errorMessage);
    }

    PriceRepository repository;
    const bool ok = price.id == 0 ? repository.create(price, errorMessage) : repository.update(price, errorMessage);
    if (ok) {
        values = toMap(price);
    }
    return ok;
}

bool DataController::saveSale(QVariantMap& values, QString* errorMessage) const
{
    if (!requireAdmin(errorMessage)) {
        return false;
    }

    Sale sale = saleFromMap(values);
    if (sale.priceId <= 0 || !sale.saleDate.isValid()) {
        return fail(QStringLiteral("Проверьте данные продажи."), errorMessage);
    }

    SaleRepository repository;
    const bool ok = sale.id == 0 ? repository.create(sale, errorMessage) : repository.update(sale, errorMessage);
    if (ok) {
        values = toMap(sale);
    }
    return ok;
}

bool DataController::deleteClient(int id, QString* errorMessage) const
{
    return requireAdmin(errorMessage) && ClientRepository().remove(id, errorMessage);
}

bool DataController::deleteProfile(int id, QString* errorMessage) const
{
    return requireAdmin(errorMessage) && ProfileRepository().remove(id, errorMessage);
}

bool DataController::deleteContract(int id, QString* errorMessage) const
{
    return requireAdmin(errorMessage) && ContractRepository().remove(id, errorMessage);
}

bool DataController::deleteProduct(int id, QString* errorMessage) const
{
    return requireAdmin(errorMessage) && ProductRepository().remove(id, errorMessage);
}

bool DataController::deletePledge(int id, QString* errorMessage) const
{
    return requireAdmin(errorMessage) && PledgeRepository().remove(id, errorMessage);
}

bool DataController::deletePrice(int id, QString* errorMessage) const
{
    return requireAdmin(errorMessage) && PriceRepository().remove(id, errorMessage);
}

bool DataController::deleteSale(int id, QString* errorMessage) const
{
    return requireAdmin(errorMessage) && SaleRepository().remove(id, errorMessage);
}

bool DataController::requireAdmin(QString* errorMessage) const
{
    if (isAdmin()) {
        return true;
    }

    if (errorMessage) {
        *errorMessage = QStringLiteral("Операция доступна только привилегированному пользователю.");
    }
    return false;
}
