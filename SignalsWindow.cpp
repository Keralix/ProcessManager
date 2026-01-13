#include "SignalsWindow.h"

SignalsWindow::SignalsWindow(QWidget *parent):QDialog(parent),dialog(new Dialog::signalsWindow) {
    dialog()
}
