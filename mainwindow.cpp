#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CpuBarDelegate.h"
#include <QMessageBox>
#include <QScrollBar>
#include "SendSignalDialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    model(&manager) {

    ui->setupUi(this);
    manager.update();
    model.refresh();
    proxy.setSourceModel(&model);
    proxy.setFilterKeyColumn(ProcessModel::Name);
    proxy.setFilterCaseSensitivity(Qt::CaseInsensitive);

    proxy.setDynamicSortFilter(true);
    ui->tableView->setModel(&proxy);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->sortByColumn(ProcessModel::CPU, Qt::DescendingOrder);
    ui->tableView->setItemDelegateForColumn(ProcessModel::CPU, new CpuBarDelegate(this));
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setFont(QFont("Sans", 10, QFont::Bold));
    ui->tableView->setShowGrid(false);
    ui->killButton->setEnabled(selectedPid > 0);
    ui->sendSignalButton->setEnabled(selectedPid>0);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->verticalHeader()->setDefaultSectionSize(26);
    ui->cpuBar->setTextVisible(true);
    ui->ramBar->setTextVisible(true);
    treeModel = new ProcessTreeModel(&manager, this);
    ui->treeView->setModel(treeModel);

    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeView->setRootIsDecorated(true);
    ui->treeView->setAlternatingRowColors(true);
    ui->treeView->header()->setStretchLastSection(true);
    ui->treeView->setHeaderHidden(false);
    ui->treeView->expandAll();
    ui->treeView->hide();
    ui->tableView->show();
    ui->viewToggleButton->setText("Tree view");

    QString baseBarStyle = R"(
    QProgressBar {
        border: none;
        border-radius: 4px;
        background-color: #ffffff;
        text-align: center;
        color: black;
        font-weight: bold;
    }

    QProgressBar::chunk {
        border-radius: 4px;

    }
    )";
    ui->cpuBar->setStyleSheet(baseBarStyle);
    ui->ramBar->setStyleSheet(baseBarStyle);

    ui->cpuBar->setFixedHeight(18);
    ui->ramBar->setFixedHeight(18);

    connect(ui->exitButton, &QPushButton::clicked,
            this, &QMainWindow::close);
    connect(ui->killButton, &QPushButton::clicked,
            this, &MainWindow::killSelectedProcess);
    connect(ui->filterEdit, &QLineEdit::textChanged,
            &proxy, &QSortFilterProxyModel::setFilterFixedString);
    connect(ui->viewToggleButton,
            &QPushButton::clicked,
            this,
            &MainWindow::toggleView);
    connect(ui->sendSignalButton, &QPushButton::clicked,
            this,
            &MainWindow::openSendSignalDialog);
    connect(&timer, &QTimer::timeout, this, &MainWindow::refresh);
    timer.start(1000);

}

MainWindow::~MainWindow() {
    delete ui;
}
void MainWindow::saveSelection() {
    auto selection = ui->tableView->selectionModel();
    savedScrollValue = ui->tableView->verticalScrollBar()->value();
    if (!selection || !selection->hasSelection()) {
        selectedPid = -1;
        return;
    }

    QModelIndex proxyIndex = selection->selectedRows().first();
    QModelIndex sourceIndex = proxy.mapToSource(proxyIndex);

    selectedPid = model.data(
                           model.index(sourceIndex.row(), ProcessModel::PID),
                           Qt::DisplayRole
                           ).toInt();
    selectedName = model.data(
                            model.index(sourceIndex.row(), ProcessModel::Name),
                            Qt::DisplayRole
                            ).toString();
    ui->killButton->setEnabled(selectedPid > 0);
    ui->sendSignalButton->setEnabled(selectedPid>0);
}

void MainWindow::restoreSelection() {
    if (selectedPid < 0)
        return;

    for (int row = 0; row < model.rowCount(); ++row) {
        QModelIndex idx = model.index(row, ProcessModel::PID);
        int pid = model.data(idx, Qt::DisplayRole).toInt();

        if (pid == selectedPid) {
            QModelIndex proxyIndex = proxy.mapFromSource(idx);
            ui->tableView->selectRow(proxyIndex.row());
            break;
        }
    }
    ui->killButton->setEnabled(selectedPid > 0);
    ui->sendSignalButton->setEnabled(selectedPid>0);
    ui->tableView->verticalScrollBar()->setValue(savedScrollValue);
}
void MainWindow::killSelectedProcess() {
    if (selectedPid <= 0)
        return;
    auto reply = QMessageBox::warning(
        this,
        "Kill process",
        QString("Kill process \"%1\" (PID %2)?")
            .arg(selectedName)
            .arg(selectedPid),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply != QMessageBox::Yes)
        return;
    if (!manager.killProcess(selectedPid)) {
        QMessageBox::critical(this,
                              "Error",
                              "Failed to kill process.");
        return;
    }

    manager.update();
    model.refresh();


}
void MainWindow::updateSystemBar(){
    const System& sys = manager.getSystem();

    int cpu = static_cast<int>(sys.cpuUsage);
    ui->cpuBar->setValue(cpu);
    ui->cpuBar->setFormat(QString("%1 %").arg(cpu));

    if (cpu < 30)
        setBarColor(ui->cpuBar, "#4CAF50");
    else if (cpu < 70)
        setBarColor(ui->cpuBar, "#FFC107");
    else
        setBarColor(ui->cpuBar, "#F44336");

    int ramPercent = 100 * (sys.memTotal - sys.memAvailable) / sys.memTotal;
    ui->ramBar->setValue(ramPercent);
    ui->ramBar->setFormat(QString("%1 %").arg(ramPercent));
    if (ramPercent < 30)
        setBarColor(ui->ramBar, "#4CAF50");
    else if (ramPercent < 70)
        setBarColor(ui->ramBar, "#FFC107");
    else
        setBarColor(ui->ramBar, "#F44336");
}
void MainWindow::setBarColor(QProgressBar* bar, const QString& color) {
    bar->setStyleSheet(bar->styleSheet() +
                       QString("QProgressBar::chunk { background-color: %1; }").arg(color));
}

void MainWindow::refresh() {
    saveSelection();
    if(treeMode){
        saveTreeState();
    }
    manager.update();
    model.refresh();
    if (treeModel){
        treeModel->refresh();
        restoreTreeState();
    }
    if (selectedPid < 0)
        ui->killButton->setEnabled(false);
    if(selectedPid<0) ui->sendSignalButton->setEnabled(false);
    updateSystemBar();
    restoreSelection();
}
void MainWindow::openSendSignalDialog() {
    qDebug() << "openSendSignalDialog called";
    if (selectedPid <= 0)
        return;

    SendSignalDialog dlg(
        &manager,
        selectedPid,
        selectedName,
        this
        );
    if (dlg.exec() == QDialog::Accepted)
        refresh();
}
void MainWindow::toggleView() {
    treeMode = !treeMode;

    if (treeMode) {
        ui->tableView->hide();
        ui->treeView->show();
        ui->treeView->expandAll();
        ui->viewToggleButton->setText("Table view");
    } else {
        ui->treeView->hide();
        ui->tableView->show();
        ui->viewToggleButton->setText("Tree view");
    }
}
void MainWindow::saveTreeState() {
    expandedPids.clear();

    // 🔹 scroll
    treeScrollValue = ui->treeView->verticalScrollBar()->value();

    // 🔹 expanded
    std::function<void(const QModelIndex&)> walk;
    walk = [&](const QModelIndex& index) {
        if (!index.isValid()) return;

        if (ui->treeView->isExpanded(index)) {
            int pid = index.data(Qt::UserRole).toInt();
            expandedPids.insert(pid);
        }

        int rows = treeModel->rowCount(index);
        for (int i = 0; i < rows; ++i)
            walk(treeModel->index(i, 0, index));
    };

    for (int i = 0; i < treeModel->rowCount({}); ++i)
        walk(treeModel->index(i, 0,QModelIndex()));

    // 🔹 selection
    auto sel = ui->treeView->selectionModel();
    if (sel && sel->hasSelection()) {
        selectedTreePid =
            sel->selectedRows().first().data(Qt::UserRole).toInt();
    } else {
        selectedTreePid = -1;
    }
}
void MainWindow::restoreTreeState() {
    std::function<void(const QModelIndex&)> walk;
    walk = [&](const QModelIndex& index) {
        if (!index.isValid()) return;

        int pid = index.data(Qt::UserRole).toInt();

        if (expandedPids.contains(pid))
            ui->treeView->expand(index);

        if (pid == selectedTreePid)
            ui->treeView->setCurrentIndex(index);

        int rows = treeModel->rowCount(index);
        for (int i = 0; i < rows; ++i)
            walk(treeModel->index(i, 0, index));
    };

    for (int i = 0; i < treeModel->rowCount({}); ++i)
        walk(treeModel->index(i, 0,QModelIndex()));

    ui->treeView->verticalScrollBar()->setValue(treeScrollValue);
}


