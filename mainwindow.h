#include <QMainWindow>
#include <QTimer>
#include "ProcessModel.h"
#include <QSortFilterProxyModel>
#include "ProcessSortProxy.h"
#include <QProgressBar>
#include "ProcessTreeModel.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void toggleView();
    void openSendSignalDialog();

private:
    Ui::MainWindow *ui;
    ProcessSortProxy proxy;
    ProcessManager manager;
    ProcessModel model;
    QTimer timer;
    QString selectedName;
    ProcessTreeModel* treeModel = nullptr;
    bool treeMode = false;
    int savedScrollValue = 0;
    int selectedPid =-1;
    void saveSelection();
    void restoreSelection();
    void refresh();
    void killSelectedProcess();
    void updateSystemBar();
    void setBarColor(QProgressBar* bar, const QString& color);
    QSet<int> expandedPids;
    int treeScrollValue = 0;
    int selectedTreePid = -1;

    void saveTreeState();
    void restoreTreeState();

};
