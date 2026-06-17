#include "View/RegisterDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

RegisterDialog::RegisterDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Регистрация"));
    setMinimumWidth(440);

    m_lastNameEdit = new QLineEdit(this);
    m_firstNameEdit = new QLineEdit(this);
    m_middleNameEdit = new QLineEdit(this);
    m_passportEdit = new QLineEdit(this);
    m_addressEdit = new QLineEdit(this);
    m_phoneEdit = new QLineEdit(this);
    m_loginEdit = new QLineEdit(this);
    m_passwordEdit = new QLineEdit(this);
    m_confirmPasswordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_confirmPasswordEdit->setEchoMode(QLineEdit::Password);

    auto* formLayout = new QFormLayout();
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    formLayout->addRow(QStringLiteral("Фамилия *"), m_lastNameEdit);
    formLayout->addRow(QStringLiteral("Имя *"), m_firstNameEdit);
    formLayout->addRow(QStringLiteral("Отчество"), m_middleNameEdit);
    formLayout->addRow(QStringLiteral("Паспорт *"), m_passportEdit);
    formLayout->addRow(QStringLiteral("Адрес *"), m_addressEdit);
    formLayout->addRow(QStringLiteral("Телефон *"), m_phoneEdit);
    formLayout->addRow(QStringLiteral("Логин *"), m_loginEdit);
    formLayout->addRow(QStringLiteral("Пароль *"), m_passwordEdit);
    formLayout->addRow(QStringLiteral("Повтор пароля *"), m_confirmPasswordEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &RegisterDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &RegisterDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(formLayout);
    layout->addWidget(buttons);
}

Client RegisterDialog::client() const
{
    Client result;
    result.lastName = m_lastNameEdit->text().trimmed();
    result.firstName = m_firstNameEdit->text().trimmed();
    result.middleName = m_middleNameEdit->text().trimmed();
    result.passport = m_passportEdit->text().trimmed();
    result.address = m_addressEdit->text().trimmed();
    result.phone = m_phoneEdit->text().trimmed();
    return result;
}

QString RegisterDialog::login() const
{
    return m_loginEdit->text().trimmed();
}

QString RegisterDialog::password() const
{
    return m_passwordEdit->text();
}

void RegisterDialog::accept()
{
    if (m_passwordEdit->text() != m_confirmPasswordEdit->text()) {
        QMessageBox::warning(this, QStringLiteral("Регистрация"), QStringLiteral("Пароли не совпадают."));
        return;
    }

    QDialog::accept();
}
