#include "ProcessTreeModel.h"

ProcessTreeModel::ProcessTreeModel(
    ProcessManager* manager, QObject* parent)
    : QAbstractItemModel(parent), manager(manager),
    root(new TreeNode{})
{
    refresh();
}

ProcessTreeModel::~ProcessTreeModel() {
    clearTree(root);
}

void ProcessTreeModel::refresh() {
    beginResetModel();

    clearTree(root);
    root = new TreeNode{};

    buildTree();

    endResetModel();
}

void ProcessTreeModel::buildTree() {
    const auto& list = manager->getProcessList();

    QHash<int, TreeNode*> map;

    for (const auto& row : list) {
        TreeNode* node = new TreeNode;
        node->row = row;
        map[row.pid] = node;
    }


    for (auto node : map) {
        int ppid = node->row.process.ppid;

        if (map.contains(ppid)) {
            node->parent = map[ppid];
            map[ppid]->children.push_back(node);
        } else {
            node->parent = root;
            root->children.push_back(node);
        }
    }
}

void ProcessTreeModel::clearTree(TreeNode* node) {
    if (!node)
        return;

    for (auto* child : node->children)
        clearTree(child);

    delete node;
}

ProcessTreeModel::TreeNode*
ProcessTreeModel::nodeFromIndex(const QModelIndex& index) const {
    if (index.isValid())
        return static_cast<TreeNode*>(index.internalPointer());

    return root;
}

QModelIndex ProcessTreeModel::index(int row, int column, const QModelIndex& parentIndex) const {
    TreeNode* parentNode = nodeFromIndex(parentIndex);
    if (!parentNode || row < 0 || row >= parentNode->children.size())
        return {};

    TreeNode* child = parentNode->children.at(row);
    return createIndex(row, column, child);
}
QModelIndex ProcessTreeModel::parent(const QModelIndex& childIndex) const {
    if (!childIndex.isValid())
        return {};

    TreeNode* node = nodeFromIndex(childIndex);
    TreeNode* parentNode = node->parent;

    if (!parentNode || parentNode == root)
        return {};

    TreeNode* grandParent = parentNode->parent;
    int row = grandParent ? grandParent->children.indexOf(parentNode) : 0;

    return createIndex(row, 0, parentNode);
}

int ProcessTreeModel::rowCount(const QModelIndex& parentIndex) const {
    TreeNode* parentNode = nodeFromIndex(parentIndex);
    return parentNode ? parentNode->children.size() : 0;
}

int ProcessTreeModel::columnCount(const QModelIndex&) const {
    return 4;
}

QVariant ProcessTreeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return {};

    TreeNode* node = nodeFromIndex(index);
    const auto& p = node->row.process;

    if (role == Qt::UserRole && index.column() == 0)
        return node->row.pid;

    if (role != Qt::DisplayRole)
        return {};

    switch (index.column()) {
    case 0: return node->row.pid;
    case 1: return QString::fromStdString(p.name);
    case 2: return QString::number(p.cpuPercent, 'f', 1);
    case 3: return p.vmRss / 1024;
    }

    return {};
}

QVariant ProcessTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section) {
    case 0: return "PID";
    case 1: return "Name";
    case 2: return "CPU %";
    case 3: return "RAM";
    }

    return {};
}
