#include "View/RecordDialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <utility>

RecordDialog::RecordDialog(const QString& title,
                           QVector<FieldMeta> fields,
                           QVariantMap values,
                           QWidget* parent)
    : QDialog(parent)
    , m_fields(std::move(fields))
    , m_initialValues(std::move(values))
{
    setWindowTitle(title);
    setMinimumWidth(440);

    auto* rootLayout = new QVBoxLayout(this);
    auto* formLayout = new QFormLayout();
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    for (const FieldMeta& field : m_fields) {
        if (field.hiddenInForm) {
            continue;
        }

        QWidget* editor = createEditor(field, m_initialValues.value(field.key));
        editor->setEnabled(!field.readOnly);
        formLayout->addRow(field.label + (field.required ? QStringLiteral(" *") : QString()), editor);
    }

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &RecordDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &RecordDialog::reject);

    rootLayout->addLayout(formLayout);
    rootLayout->addWidget(buttons);
}

QVariantMap RecordDialog::values() const
{
    QVariantMap result = m_initialValues;
    for (const FieldMeta& field : m_fields) {
        if (field.hiddenInForm) {
            continue;
        }
        result[field.key] = editorValue(field);
    }
    return result;
}

void RecordDialog::accept()
{
    const QVariantMap currentValues = values();

    for (const FieldMeta& field : m_fields) {
        if (field.hiddenInForm || !field.required) {
            continue;
        }

        const QVariant value = currentValues.value(field.key);
        bool valid = true;
        switch (field.type) {
        case FieldType::Text:
        case FieldType::Password:
        case FieldType::MultilineText:
        case FieldType::Choice:
            valid = !value.toString().trimmed().isEmpty();
            break;
        case FieldType::Integer:
            valid = value.toInt() >= static_cast<int>(field.minimum);
            break;
        case FieldType::Decimal:
            valid = value.toDouble() >= field.minimum;
            break;
        case FieldType::Date:
            valid = value.toDate().isValid();
            break;
        }

        if (!valid) {
            QMessageBox::warning(this, QStringLiteral("Проверка данных"),
                                 QStringLiteral("Поле \"%1\" заполнено некорректно.").arg(field.label));
            return;
        }
    }

    QDialog::accept();
}

QWidget* RecordDialog::createEditor(const FieldMeta& field, const QVariant& value)
{
    switch (field.type) {
    case FieldType::Integer: {
        auto* editor = new QSpinBox(this);
        editor->setRange(static_cast<int>(field.minimum), static_cast<int>(field.maximum));
        editor->setValue(value.isValid() ? value.toInt() : static_cast<int>(field.minimum));
        m_editors[field.key] = editor;
        return editor;
    }
    case FieldType::Decimal: {
        auto* editor = new QDoubleSpinBox(this);
        editor->setRange(field.minimum, field.maximum);
        editor->setDecimals(2);
        editor->setValue(value.isValid() ? value.toDouble() : field.minimum);
        m_editors[field.key] = editor;
        return editor;
    }
    case FieldType::Date: {
        auto* editor = new QDateEdit(this);
        editor->setCalendarPopup(true);
        editor->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
        const QDate date = value.toDate();
        editor->setDate(date.isValid() ? date : QDate::currentDate());
        m_editors[field.key] = editor;

        if (!field.required) {
            auto* wrapper = new QWidget(this);
            auto* layout = new QHBoxLayout(wrapper);
            layout->setContentsMargins(0, 0, 0, 0);
            auto* checkbox = new QCheckBox(QStringLiteral("задать"), wrapper);
            checkbox->setChecked(date.isValid());
            editor->setEnabled(date.isValid());
            connect(checkbox, &QCheckBox::toggled, editor, &QDateEdit::setEnabled);
            layout->addWidget(checkbox);
            layout->addWidget(editor, 1);
            m_optionalDateChecks[field.key] = checkbox;
            return wrapper;
        }

        return editor;
    }
    case FieldType::Choice: {
        auto* editor = new QComboBox(this);
        editor->addItems(field.choices);
        const QString selected = value.toString();
        const int index = editor->findText(selected);
        if (index >= 0) {
            editor->setCurrentIndex(index);
        }
        m_editors[field.key] = editor;
        return editor;
    }
    case FieldType::Password: {
        auto* editor = new QLineEdit(this);
        editor->setEchoMode(QLineEdit::Password);
        editor->setText(value.toString());
        if (!field.required) {
            editor->setPlaceholderText(QStringLiteral("оставьте пустым, чтобы не менять"));
        }
        m_editors[field.key] = editor;
        return editor;
    }
    case FieldType::MultilineText: {
        auto* editor = new QTextEdit(this);
        editor->setMinimumHeight(90);
        editor->setText(value.toString());
        m_editors[field.key] = editor;
        return editor;
    }
    case FieldType::Text:
    default: {
        auto* editor = new QLineEdit(this);
        editor->setText(value.toString());
        m_editors[field.key] = editor;
        return editor;
    }
    }
}

QVariant RecordDialog::editorValue(const FieldMeta& field) const
{
    QWidget* editor = m_editors.value(field.key, nullptr);
    if (!editor) {
        return m_initialValues.value(field.key);
    }

    switch (field.type) {
    case FieldType::Integer:
        return qobject_cast<QSpinBox*>(editor)->value();
    case FieldType::Decimal:
        return qobject_cast<QDoubleSpinBox*>(editor)->value();
    case FieldType::Date: {
        const QCheckBox* checkbox = m_optionalDateChecks.value(field.key, nullptr);
        if (checkbox && !checkbox->isChecked()) {
            return QDate();
        }
        return qobject_cast<QDateEdit*>(editor)->date();
    }
    case FieldType::Choice:
        return qobject_cast<QComboBox*>(editor)->currentText();
    case FieldType::Password:
    case FieldType::Text:
        return qobject_cast<QLineEdit*>(editor)->text();
    case FieldType::MultilineText:
        return qobject_cast<QTextEdit*>(editor)->toPlainText();
    }

    return {};
}
