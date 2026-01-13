#include "ProcessSortProxy.h"
#include "ProcessModel.h"

ProcessSortProxy::ProcessSortProxy(QObject* parent)
    : QSortFilterProxyModel(parent) {}

bool ProcessSortProxy::lessThan(const QModelIndex& left,
                                const QModelIndex& right) const {

    if (left.column() == ProcessModel::CPU && right.column() == ProcessModel::CPU) {
        double l = sourceModel()->data(left, Qt::UserRole).toDouble();
        double r = sourceModel()->data(right, Qt::UserRole).toDouble();
        return l < r;
    }

    return QSortFilterProxyModel::lessThan(left, right);
}
