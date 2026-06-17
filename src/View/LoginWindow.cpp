#include "View/LoginWindow.h"

#include "View/MainWindow.h"
#include "View/RegisterDialog.h"

#include <QApplication>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>

LoginWindow::LoginWindow(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle(QStringLiteral("Ломбард - вход"));
    setMinimumSize(420, 260);

    auto* title = new QLabel(QStringLiteral("Информационная система ломбарда"), this);
    QFont titleFont = title->font();
    titleFont.setPointSize(titleFont.pointSize() + 3);
    titleFont.setBold(true);
    title->setFont(titleFont);

    m_loginEdit = new QLineEdit(this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_loginEdit->setPlaceholderText(QStringLiteral("admin или user"));

    auto* formLayout = new QFormLayout();
    formLayout->addRow(QStringLiteral("Логин"), m_loginEdit);
    formLayout->addRow(QStringLiteral("Пароль"), m_passwordEdit);

    auto* loginButton = new QPushButton(QStringLiteral("Войти"), this);
    auto* registerButton = new QPushButton(QStringLiteral("Регистрация"), this);
    QIcon loginIcon = QIcon::fromTheme(QStringLiteral("dialog-ok"));
    if (loginIcon.isNull()) {
        loginIcon = style()->standardIcon(QStyle::SP_DialogApplyButton);
    }
    loginButton->setIcon(loginIcon);
    QIcon registerIcon = QIcon::fromTheme(QStringLiteral("list-add-user"));
    if (registerIcon.isNull()) {
        registerIcon = style()->standardIcon(QStyle::SP_FileDialogNewFolder);
    }
    registerButton->setIcon(registerIcon);

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::tryLogin);
    connect(registerButton, &QPushButton::clicked, this, &LoginWindow::openRegistration);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginWindow::tryLogin);

    auto* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(registerButton);
    buttonsLayout->addWidget(loginButton);

    auto* hint = new QLabel(QStringLiteral("Тестовые учетные записи: admin/Admin123! и user/User123!"), this);
    hint->setWordWrap(true);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(title);
    layout->addSpacing(12);
    layout->addLayout(formLayout);
    layout->addLayout(buttonsLayout);
    layout->addStretch();
    layout->addWidget(hint);
}

void LoginWindow::tryLogin()
{
    UserSession session;
    QString error;
    if (!m_authController.login(m_loginEdit->text(), m_passwordEdit->text(), session, &error)) {
        QMessageBox::warning(this, QStringLiteral("Вход"), error);
        return;
    }

    auto* mainWindow = new MainWindow(session);
    mainWindow->show();
    close();
}

void LoginWindow::openRegistration()
{
    RegisterDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString error;
    if (!m_authController.registerUser(dialog.client(), dialog.login(), dialog.password(), &error)) {
        QMessageBox::warning(this, QStringLiteral("Регистрация"), error);
        return;
    }

    QMessageBox::information(this, QStringLiteral("Регистрация"), QStringLiteral("Пользователь создан. Теперь можно войти."));
    m_loginEdit->setText(dialog.login());
    m_passwordEdit->clear();
    m_passwordEdit->setFocus();
}
