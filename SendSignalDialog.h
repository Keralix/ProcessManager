#pragma once

#include <QDialog>
#include "ProcessManager.h"

namespace Ui {
class SendSignalDialog;
}

class SendSignalDialog : public QDialog {
    Q_OBJECT

public:
    explicit SendSignalDialog(
        ProcessManager* manager,
        int pid,
        const QString& name,
        QWidget* parent = nullptr
        );
    ~SendSignalDialog();

private slots:
    void onSendClicked();

private:
    Ui::SendSignalDialog* ui;
    ProcessManager* manager;
    int pid;
};
