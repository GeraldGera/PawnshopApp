#pragma once

#include "Model/Entities.h"

#include <QDialog>

class QLineEdit;

class RegisterDialog : public QDialog
{
public:
    explicit RegisterDialog(QWidget* parent = nullptr);

    Client client() const;
    QString login() const;
    QString password() const;

protected:
    void accept() override;

private:
    QLineEdit* m_lastNameEdit = nullptr;
    QLineEdit* m_firstNameEdit = nullptr;
    QLineEdit* m_middleNameEdit = nullptr;
    QLineEdit* m_passportEdit = nullptr;
    QLineEdit* m_addressEdit = nullptr;
    QLineEdit* m_phoneEdit = nullptr;
    QLineEdit* m_loginEdit = nullptr;
    QLineEdit* m_passwordEdit = nullptr;
    QLineEdit* m_confirmPasswordEdit = nullptr;
};
