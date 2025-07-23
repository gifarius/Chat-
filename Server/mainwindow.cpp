#include "mainwindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , layout(new QVBoxLayout)
    , startButton(new QPushButton("Start"))
    , logTextEdit(new QTextEdit)
    , serverThread(new QThread(this))
    , server(nullptr)
    , isServerStarted(false)
{
    // Create central widget and layout
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setLayout(layout);

    // Configure start button
    startButton->setFixedSize(200, 60);
    layout->addWidget(startButton, 0, Qt::AlignCenter);

    // Configure log text edit
    logTextEdit->setReadOnly(true);
    logTextEdit->setMinimumHeight(400);
    layout->addWidget(logTextEdit);
    layout->addStretch();

    // Set window properties
    setWindowTitle("gifarius - Mitnick Mode");
    setMinimumSize(800, 600);

    // Apply Kevin Mitnick hacker style
    setStyleSheet(
        "QMainWindow {"
        "    background-color: #1a1a1a;"
        "}"
        "QTextEdit {"
        "    background-color: #000000;"
        "    color: #00ff00;"
        "    border: 1px solid #00ff00;"
        "    font-family: 'Courier New', Courier, monospace;"
        "    font-size: 12px;"
        "}"
        "QPushButton {"
        "    background-color: #333333;"
        "    color: #00ff00;"
        "    border: 2px solid #00ff00;"
        "    padding: 5px;"
        "    font-family: 'Courier New', Courier, monospace;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #00ff00;"
        "    color: #000000;"
        "}"
        "QMenuBar {"
        "    background-color: #1a1a1a;"
        "    color: #00ff00;"
        "    font-family: 'Courier New', Courier, monospace;"
        "}"
        "QMenuBar::item {"
        "    background-color: #1a1a1a;"
        "    color: #00ff00;"
        "}"
        "QMenuBar::item:selected {"
        "    background-color: #00ff00;"
        "    color: #000000;"
        "}"
        "QStatusBar {"
        "    background-color: #1a1a1a;"
        "    color: #00ff00;"
        "    font-family: 'Courier New', Courier, monospace;"
        "}"
        );

    // Connect the start button
    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartButtonClicked);

    // Initialize with a welcome message
    appendLog("System initialized. Ready to hack the planet!");
}

MainWindow::~MainWindow()
{
    if (server) {
        QMetaObject::invokeMethod(server, "stopServer", Qt::QueuedConnection);
        serverThread->quit();
        serverThread->wait();
        delete server;
    }
    delete serverThread;
    delete logTextEdit;
    delete startButton;
    delete layout;
}

void MainWindow::onStartButtonClicked()
{
    isServerStarted = !isServerStarted;
    startButton->setText(isServerStarted ? "Finish" : "Start");

    if (isServerStarted) {
        server = new Server();
        server->moveToThread(serverThread);

        // Connect server signals to UI
        connect(serverThread, &QThread::started, server, &Server::startServer);
        connect(serverThread, &QThread::finished, server, &QObject::deleteLater);
        connect(server, &Server::logMessage, this, &MainWindow::appendLog);

        serverThread->start();
        appendLog("Initializing server...");
    } else {
        if (server) {
            QMetaObject::invokeMethod(server, "stopServer", Qt::QueuedConnection);
            serverThread->quit();
            serverThread->wait();
            delete server;
            server = nullptr;
            appendLog("Server shutdown complete.");
        }
    }
}

void MainWindow::appendLog(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    logTextEdit->append(QString("[%1] %2").arg(timestamp, message));
}
