#include "CpuBarDelegate.h"
#include <QPainter>
#include <QApplication>

CpuBarDelegate::CpuBarDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}

void CpuBarDelegate::paint(QPainter* painter,
                           const QStyleOptionViewItem& option,
                           const QModelIndex& index) const {
    double cpu = index.data(Qt::UserRole).toDouble();

    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    QApplication::style()->drawControl(
        QStyle::CE_ItemViewItem, &opt, painter);

    QRect rect = option.rect.adjusted(4, 6, -4, -6);

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0xE0E0E0));
    painter->drawRoundedRect(rect, 4, 4);

    int barWidth = static_cast<int>(rect.width() * (cpu / 100.0));
    QRect barRect = rect;
    barRect.setWidth(barWidth);

    QColor color;
    if (cpu < 30)
        color = QColor(0x4CAF50);
    else if (cpu < 70)
        color = QColor(0xFFC107);
    else
        color = QColor(0xF44336);

    painter->setBrush(color);
    painter->drawRoundedRect(barRect, 4, 4);

    painter->setPen(Qt::black);
    painter->drawText(
        option.rect,
        Qt::AlignCenter,
        QString::number(cpu, 'f', 1) + "%"
        );
}
