#pragma once

#include <QSortFilterProxyModel>

class ProcessSortProxy : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit ProcessSortProxy(QObject* parent = nullptr);

protected:
    bool lessThan(const QModelIndex& left,
                  const QModelIndex& right) const override;
};
