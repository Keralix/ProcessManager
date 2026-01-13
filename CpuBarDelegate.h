#pragma once
#include <QStyledItemDelegate>

class CpuBarDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit CpuBarDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
};
