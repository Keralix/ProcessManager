#pragma once
#include <QAbstractItemModel>
#include "ProcessManager.h"

class ProcessTreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit ProcessTreeModel(ProcessManager* manager,
                              QObject* parent = nullptr);
    ~ProcessTreeModel();

    QModelIndex index(int row, int column,
                      const QModelIndex& parent) const override;

    QModelIndex parent(const QModelIndex& child) const override;

    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;

    QVariant data(const QModelIndex& index,
                  int role) const override;

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role) const override;

    void refresh();

private:
    struct TreeNode {
        ProcessRow row;
        TreeNode* parent = nullptr;
        QVector<TreeNode*> children;
    };

    ProcessManager* manager;
    TreeNode* root = nullptr;

    TreeNode* nodeFromIndex(const QModelIndex& index) const;
    void buildTree();
    void clearTree(TreeNode* node);
};
