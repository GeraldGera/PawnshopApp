#pragma once

#include "Controller/AuthController.h"

#include <QWidget>

class QLineEdit;

class LoginWindow : public QWidget
{
public:
    explicit LoginWindow(QWidget* parent = nullptr);

private:
    void tryLogin();
    void openRegistration();

    AuthController m_authController;
    QLineEdit* m_loginEdit = nullptr;
    QLineEdit* m_passwordEdit = nullptr;
};
