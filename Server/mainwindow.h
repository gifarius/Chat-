#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "server.h"

class QVBoxLayout;
class QPushButton;
class QTextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void appendLog(const QString &message);

private slots:
    void onStartButtonClicked();

private:
    QVBoxLayout *layout;
    QPushButton *startButton;
    QTextEdit *logTextEdit;
    QThread *serverThread;
    Server *server;
    bool isServerStarted;
};

#endif // MAINWINDOW_H
