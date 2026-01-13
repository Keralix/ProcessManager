#ifndef SIGNALSWINDOW_H
#define SIGNALSWINDOW_H
#include <QDialog>
QT_BEGIN_NAMESPACE
namespace Dialog {class signalsWindow;}
QT_END_NAMESPACE
class SignalsWindow : public QDialog{
    Q_OBJECT
public:
    explicit SignalsWindow(QWidget *parrent = nullptr);
    ~SignalsWindow();
private:
    Dialog::signalsWindow *dialog;
};

#endif // SIGNALSWINDOW_H
