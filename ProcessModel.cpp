#include "ProcessModel.h"
#include <QBrush>
#include <QColor>

ProcessModel::ProcessModel(ProcessManager* manager, QObject* parent)
    : QAbstractTableModel(parent), manager(manager){}

int ProcessModel::rowCount(const QModelIndex&) const{
    return manager->getProcessList().size();
}
int ProcessModel::columnCount(const QModelIndex&) const {
    return ColumnCount;
}

QVariant ProcessModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return {};

    const auto& row = manager->getProcessList()[index.row()];
    const auto& p = row.process;

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case PID:   return row.pid;
        case User:  return QString::fromStdString(p.user);
        case Name:  return QString::fromStdString(p.name);
        case CPU:   return {};
        case RAM:   return p.vmRss / 1024;
        case State: return QString::fromStdString(p.state);
        case PPID:  return p.ppid;
        }
    }

    if (role == Qt::UserRole && index.column() == CPU) {
        return p.cpuPercent;
    }

    if (role == Qt::BackgroundRole && index.column() == State) {
        const QString state = QString::fromStdString(p.state);

        if (state.startsWith("R"))
            return QBrush(QColor(0xC8E6C9));
        if (state.startsWith("S"))
            return QBrush(QColor(0xEEEEEE));
        if (state.startsWith("D"))
            return QBrush(QColor(0xFFE082));
        if (state.startsWith("Z"))
            return QBrush(QColor(0xFFCDD2));
        if (state.startsWith("T"))
            return QBrush(QColor(0xE1BEE7));
    }

    return {};
}




QVariant ProcessModel::headerData(int section,
                                  Qt::Orientation orientation,
                                  int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section) {
    case PID:  return "PID";
    case User: return "User";
    case Name: return "Name";
    case CPU:  return "CPU %";
    case RAM:  return "RAM";
    case State:return "State";
    case PPID: return "PPID";
    }
    return {};
}


void ProcessModel::refresh(){
    beginResetModel();
    endResetModel();
}
