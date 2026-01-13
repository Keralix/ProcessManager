 #include "SendSignalDialog.h"
#include "ui_sendsignaldialog.h"
#include <QMessageBox>
#include <signal.h>

SendSignalDialog::SendSignalDialog(
    ProcessManager* manager,
    int pid,
    const QString& name,
    QWidget* parent
    )
    : QDialog(parent),
    ui(new Ui::SendSignalDialog),
    manager(manager),
    pid(pid)
{
    ui->setupUi(this);

    ui->processLabel->setText(
        QString("%1 (PID %2)").arg(name).arg(pid)
        );

    ui->signalCombo->addItem("SIGTERM", SIGTERM);
    ui->signalCombo->addItem("SIGKILL", SIGKILL);
    ui->signalCombo->addItem("SIGSTOP", SIGSTOP);
    ui->signalCombo->addItem("SIGCONT", SIGCONT);
    ui->signalCombo->addItem("SIGINT", SIGINT);

    connect(ui->sendButton, &QPushButton::clicked,
            this, &SendSignalDialog::onSendClicked);

    connect(ui->cancelButton, &QPushButton::clicked,
            this, &QDialog::reject);
}

SendSignalDialog::~SendSignalDialog() {
    delete ui;
}

void SendSignalDialog::onSendClicked() {
    int sig = ui->signalCombo->currentData().toInt();

    if (!manager->sendSignal(pid, sig)) {
        QMessageBox::critical(this, "Error", "Failed to send signal.");
        return;
    }

    accept(); // zatvori dialog
}
