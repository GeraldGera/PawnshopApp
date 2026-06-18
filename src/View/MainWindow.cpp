#include "View/MainWindow.h"

#include "View/CrudPage.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStyle>
#include <QTabWidget>
#include <QVBoxLayout>

namespace
{
FieldMeta baseField(const QString& key, const QString& label, FieldType type, bool required = true)
{
    FieldMeta field;
    field.key = key;
    field.label = label;
    field.type = type;
    field.required = required;
    return field;
}

FieldMeta idField()
{
    FieldMeta field = baseField(QStringLiteral("id"), QStringLiteral("ID"), FieldType::Integer, false);
    field.readOnly = true;
    field.hiddenInForm = true;
    field.minimum = 0;
    return field;
}

FieldMeta fkField(const QString& key, const QString& label)
{
    FieldMeta field = baseField(key, label, FieldType::Integer, true);
    field.minimum = 1;
    return field;
}

FieldMeta moneyField(const QString& key, const QString& label, double minimum)
{
    FieldMeta field = baseField(key, label, FieldType::Decimal, true);
    field.minimum = minimum;
    return field;
}

FieldMeta choiceField(const QString& key, const QString& label, const QStringList& choices)
{
    FieldMeta field = baseField(key, label, FieldType::Choice, true);
    field.choices = choices;
    return field;
}

FieldMeta optionalDateField(const QString& key, const QString& label)
{
    FieldMeta field = baseField(key, label, FieldType::Date, false);
    return field;
}

QLineEdit* lineEdit(const QString& value, QWidget* parent)
{
    auto* edit = new QLineEdit(parent);
    edit->setText(value);
    return edit;
}
}

MainWindow::MainWindow(const UserSession& session, QWidget* parent)
    : QMainWindow(parent)
    , m_session(session)
    , m_dataController(session)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QStringLiteral("Ломбард - %1 (%2)")
                       .arg(m_session.client.displayName(), userRoleToString(m_session.profile.role)));
    resize(1180, 720);

    m_tabs = new QTabWidget(this);
    setCentralWidget(m_tabs);

    m_tabs->addTab(createProfileTab(), QStringLiteral("Профиль"));
    if (m_session.isAdmin()) {
        m_tabs->addTab(createAdminTabs(), QStringLiteral("Администрирование"));
    } else {
        addUserTabs();
    }
}

QWidget* MainWindow::createProfileTab()
{
    auto* page = new QWidget(this);
    auto* rootLayout = new QVBoxLayout(page);

    auto* group = new QGroupBox(QStringLiteral("Регистрационные данные"), page);
    auto* form = new QFormLayout(group);
    form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    auto* lastNameEdit = lineEdit(m_session.client.lastName, group);
    auto* firstNameEdit = lineEdit(m_session.client.firstName, group);
    auto* middleNameEdit = lineEdit(m_session.client.middleName, group);
    auto* passportEdit = lineEdit(m_session.client.passport, group);
    auto* addressEdit = lineEdit(m_session.client.address, group);
    auto* phoneEdit = lineEdit(m_session.client.phone, group);
    auto* loginEdit = lineEdit(m_session.profile.login, group);
    auto* passwordEdit = new QLineEdit(group);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText(QStringLiteral("оставьте пустым, чтобы не менять"));
    auto* roleEdit = lineEdit(userRoleToString(m_session.profile.role), group);
    roleEdit->setReadOnly(true);

    form->addRow(QStringLiteral("Фамилия *"), lastNameEdit);
    form->addRow(QStringLiteral("Имя *"), firstNameEdit);
    form->addRow(QStringLiteral("Отчество"), middleNameEdit);
    form->addRow(QStringLiteral("Паспорт *"), passportEdit);
    form->addRow(QStringLiteral("Адрес *"), addressEdit);
    form->addRow(QStringLiteral("Телефон *"), phoneEdit);
    form->addRow(QStringLiteral("Логин *"), loginEdit);
    form->addRow(QStringLiteral("Новый пароль"), passwordEdit);
    form->addRow(QStringLiteral("Роль"), roleEdit);

    auto* saveButton = new QPushButton(QStringLiteral("Сохранить"), page);
    QIcon saveIcon = QIcon::fromTheme(QStringLiteral("document-save"));
    if (saveIcon.isNull()) {
        saveIcon = style()->standardIcon(QStyle::SP_DialogSaveButton);
    }
    saveButton->setIcon(saveIcon);

    auto* statusLabel = new QLabel(page);
    connect(saveButton, &QPushButton::clicked, this, [=]() {
        Client updatedClient;
        updatedClient.id = m_session.client.id;
        updatedClient.lastName = lastNameEdit->text().trimmed();
        updatedClient.firstName = firstNameEdit->text().trimmed();
        updatedClient.middleName = middleNameEdit->text().trimmed();
        updatedClient.passport = passportEdit->text().trimmed();
        updatedClient.address = addressEdit->text().trimmed();
        updatedClient.phone = phoneEdit->text().trimmed();

        QString error;
        if (!m_profileController.updateCurrentUser(m_session, updatedClient, loginEdit->text(), passwordEdit->text(), &error)) {
            QMessageBox::warning(page, QStringLiteral("Профиль"), error);
            return;
        }

        passwordEdit->clear();
        statusLabel->setText(QStringLiteral("Изменения сохранены."));
        setWindowTitle(QStringLiteral("Ломбард - %1 (%2)")
                           .arg(m_session.client.displayName(), userRoleToString(m_session.profile.role)));
    });

    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(statusLabel, 1);
    buttonLayout->addWidget(saveButton);

    rootLayout->addWidget(group);
    rootLayout->addLayout(buttonLayout);
    rootLayout->addStretch();
    return page;
}

QWidget* MainWindow::createAdminTabs()
{
    auto* tabs = new QTabWidget(this);

    tabs->addTab(new CrudPage(
                     QStringLiteral("Клиенты"),
                     clientFields(),
                     [this](QString* error) { return m_dataController.clients(error); },
                     [this](QVariantMap& values, QString* error) { return m_dataController.saveClient(values, error); },
                     [this](int id, QString* error) { return m_dataController.deleteClient(id, error); },
                     true,
                     tabs),
                 QStringLiteral("Клиенты"));

    tabs->addTab(new CrudPage(
                     QStringLiteral("Профили"),
                     profileFields(),
                     [this](QString* error) { return m_dataController.profiles(error); },
                     [this](QVariantMap& values, QString* error) { return m_dataController.saveProfile(values, error); },
                     [this](int id, QString* error) { return m_dataController.deleteProfile(id, error); },
                     true,
                     tabs),
                 QStringLiteral("Профили"));

    tabs->addTab(new CrudPage(
                     QStringLiteral("Договоры"),
                     contractFields(),
                     [this](QString* error) { return m_dataController.contracts(error); },
                     [this](QVariantMap& values, QString* error) { return m_dataController.saveContract(values, error); },
                     [this](int id, QString* error) { return m_dataController.deleteContract(id, error); },
                     true,
                     tabs),
                 QStringLiteral("Договоры"));

    tabs->addTab(new CrudPage(
                     QStringLiteral("Залог"),
                     pledgeFields(),
                     [this](QString* error) { return m_dataController.pledges(error); },
                     [this](QVariantMap& values, QString* error) { return m_dataController.savePledge(values, error); },
                     [this](int id, QString* error) { return m_dataController.deletePledge(id, error); },
                     true,
                     tabs),
                 QStringLiteral("Залог"));

    tabs->addTab(new CrudPage(
                     QStringLiteral("Товары"),
                     productFields(),
                     [this](QString* error) { return m_dataController.products(error); },
                     [this](QVariantMap& values, QString* error) { return m_dataController.saveProduct(values, error); },
                     [this](int id, QString* error) { return m_dataController.deleteProduct(id, error); },
                     true,
                     tabs),
                 QStringLiteral("Товары"));

    tabs->addTab(new CrudPage(
                     QStringLiteral("Цены"),
                     priceFields(),
                     [this](QString* error) { return m_dataController.prices(error); },
                     [this](QVariantMap& values, QString* error) { return m_dataController.savePrice(values, error); },
                     [this](int id, QString* error) { return m_dataController.deletePrice(id, error); },
                     true,
                     tabs),
                 QStringLiteral("Цены"));

    tabs->addTab(new CrudPage(
                     QStringLiteral("Продажи"),
                     saleFields(),
                     [this](QString* error) { return m_dataController.sales(error); },
                     [this](QVariantMap& values, QString* error) { return m_dataController.saveSale(values, error); },
                     [this](int id, QString* error) { return m_dataController.deleteSale(id, error); },
                     true,
                     tabs),
                 QStringLiteral("Продажи"));

    return tabs;
}

void MainWindow::addUserTabs()
{
    m_tabs->addTab(new CrudPage(
                       QStringLiteral("Мои договоры"),
                       contractFields(),
                       [this](QString* error) { return m_dataController.contracts(error); },
                       {},
                       {},
                       false,
                       m_tabs),
                   QStringLiteral("Мои договоры"));

    m_tabs->addTab(new CrudPage(
                       QStringLiteral("Мой залог"),
                       pledgeFields(),
                       [this](QString* error) { return m_dataController.pledges(error); },
                       {},
                       {},
                       false,
                       m_tabs),
                   QStringLiteral("Мой залог"));

    m_tabs->addTab(new CrudPage(
                       QStringLiteral("Мои товары"),
                       productFields(),
                       [this](QString* error) { return m_dataController.products(error); },
                       {},
                       {},
                       false,
                       m_tabs),
                   QStringLiteral("Мои товары"));

    m_tabs->addTab(new CrudPage(
                       QStringLiteral("Мои цены"),
                       priceFields(),
                       [this](QString* error) { return m_dataController.prices(error); },
                       {},
                       {},
                       false,
                       m_tabs),
                   QStringLiteral("Мои цены"));

    m_tabs->addTab(new CrudPage(
                       QStringLiteral("Мои продажи"),
                       saleFields(),
                       [this](QString* error) { return m_dataController.sales(error); },
                       {},
                       {},
                       false,
                       m_tabs),
                   QStringLiteral("Мои продажи"));
}

QVector<FieldMeta> MainWindow::clientFields() const
{
    return {
        idField(),
        baseField(QStringLiteral("last_name"), QStringLiteral("Фамилия"), FieldType::Text),
        baseField(QStringLiteral("first_name"), QStringLiteral("Имя"), FieldType::Text),
        baseField(QStringLiteral("middle_name"), QStringLiteral("Отчество"), FieldType::Text, false),
        baseField(QStringLiteral("passport"), QStringLiteral("Паспорт"), FieldType::Text),
        baseField(QStringLiteral("address"), QStringLiteral("Адрес"), FieldType::Text),
        baseField(QStringLiteral("phone"), QStringLiteral("Телефон"), FieldType::Text),
    };
}

QVector<FieldMeta> MainWindow::profileFields() const
{
    FieldMeta password = baseField(QStringLiteral("password"), QStringLiteral("Пароль"), FieldType::Password, false);
    password.hiddenInTable = true;

    return {
        idField(),
        fkField(QStringLiteral("client_id"), QStringLiteral("ID клиента")),
        baseField(QStringLiteral("login"), QStringLiteral("Логин"), FieldType::Text),
        password,
        choiceField(QStringLiteral("role"), QStringLiteral("Роль"), {QStringLiteral("admin"), QStringLiteral("user")}),
    };
}

QVector<FieldMeta> MainWindow::contractFields() const
{
    return {
        idField(),
        fkField(QStringLiteral("client_id"), QStringLiteral("ID клиента")),
        moneyField(QStringLiteral("loan_amount"), QStringLiteral("Сумма займа"), 0.01),
        moneyField(QStringLiteral("commission"), QStringLiteral("Комиссия"), 0.0),
        baseField(QStringLiteral("issue_date"), QStringLiteral("Дата выдачи"), FieldType::Date),
        baseField(QStringLiteral("due_date"), QStringLiteral("Срок возврата"), FieldType::Date),
        optionalDateField(QStringLiteral("actual_return_date"), QStringLiteral("Дата возврата")),
        choiceField(QStringLiteral("status"), QStringLiteral("Статус"), {
            QStringLiteral("active"),
            QStringLiteral("returned"),
            QStringLiteral("overdue"),
            QStringLiteral("sold"),
            QStringLiteral("closed"),
        }),
    };
}

QVector<FieldMeta> MainWindow::productFields() const
{
    return {
        idField(),
        baseField(QStringLiteral("name"), QStringLiteral("Название"), FieldType::Text),
        baseField(QStringLiteral("description"), QStringLiteral("Описание"), FieldType::MultilineText, false),
        choiceField(QStringLiteral("status"), QStringLiteral("Статус"), {
            QStringLiteral("in_pledge"),
            QStringLiteral("stored"),
            QStringLiteral("for_sale"),
            QStringLiteral("sold"),
            QStringLiteral("returned"),
        }),
    };
}

QVector<FieldMeta> MainWindow::pledgeFields() const
{
    return {
        idField(),
        fkField(QStringLiteral("contract_id"), QStringLiteral("ID договора")),
        fkField(QStringLiteral("product_id"), QStringLiteral("ID товара")),
        moneyField(QStringLiteral("estimated_value"), QStringLiteral("Оценочная стоимость"), 0.01),
    };
}

QVector<FieldMeta> MainWindow::priceFields() const
{
    return {
        idField(),
        fkField(QStringLiteral("product_id"), QStringLiteral("ID товара")),
        moneyField(QStringLiteral("price"), QStringLiteral("Цена"), 0.01),
        baseField(QStringLiteral("price_date"), QStringLiteral("Дата"), FieldType::Date),
    };
}

QVector<FieldMeta> MainWindow::saleFields() const
{
    return {
        idField(),
        fkField(QStringLiteral("price_id"), QStringLiteral("ID цены")),
        baseField(QStringLiteral("sale_date"), QStringLiteral("Дата"), FieldType::Date),
    };
}
// Привязка сигналов закрытия окон
