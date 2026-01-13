#pragma once
#include <QAbstractTableModel>
#include "ProcessManager.h"

class ProcessModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit ProcessModel(ProcessManager* manager, QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orinetation, int role) const override;
    void refresh();
    enum Column {
        PID=0,
        User,
        Name,
        CPU,
        RAM,
        State,
        PPID,
        ColumnCount
    };

private:
    ProcessManager* manager;

};
