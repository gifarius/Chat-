#include "global_window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QDebug>
#include <QTime>
#include <QAbstractSocket>

Global_Window::Global_Window(QTcpSocket* socket, QWidget *parent)
    : QWidget(parent), socket(socket), waveOffset(0)
{
    setWindowTitle("C2 Control Panel (Neo Cyber)");
    resize(1200, 800);
    setStyleSheet(
        "background-color: #1a1a2e;"
        "color: #00ff00;"
        "font-family: 'Courier New', monospace;"
        "font-size: 12px;"
        );

    // Main layout
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // Sidebar (Quick Actions)
    QWidget* sidebar = new QWidget(this);
    sidebar->setFixedWidth(200);
    sidebar->setStyleSheet("background-color: #16213e; border: 2px solid #00ff00; border-radius: 5px;");
    QVBoxLayout* sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setAlignment(Qt::AlignTop);

    QLabel* quickActionsLabel = new QLabel("Quick Actions", sidebar);
    quickActionsLabel->setStyleSheet("color: #00ff00; font-weight: bold; padding: 5px;");
    sidebarLayout->addWidget(quickActionsLabel);

    struct ButtonData { QString text; QString color; };
    QVector<ButtonData> buttons = {
        {"Refresh", "#00ff00"}, {"Chat", "#00ff00"}, {"DDoS", "#00ff00"}, {"Builder", "#00ff00"},
        {"Status", "#00ff00"}, {"Users", "#00ff00"}, {"Logs", "#00ff00"}, {"Screenshot", "#00ff00"},
        {"Audio", "#00ff00"}, {"Stream", "#00ff00"}
    };
    for (const auto& btnData : buttons) {
        QPushButton* btn = new QPushButton(btnData.text, sidebar);
        btn->setStyleSheet(QString("background-color: #16213e; color: %1; border: 2px solid %1; border-radius: 5px; padding: 5px;").arg(btnData.color));
        btn->setFixedHeight(40);
        sidebarLayout->addWidget(btn);
    }

    refreshBtn = new QPushButton("Refresh", sidebar);
    refreshBtn->setStyleSheet("background-color: #16213e; color: #00ff00; border: 2px solid #00ff00; border-radius: 5px; padding: 5px;");
    refreshBtn->setFixedHeight(40);
    connect(refreshBtn, &QPushButton::clicked, this, &Global_Window::refreshClients);
    sidebarLayout->addWidget(refreshBtn);

    clearLogsBtn = new QPushButton("Clear Logs", sidebar);
    clearLogsBtn->setStyleSheet("background-color: #16213e; color: #00ff00; border: 2px solid #00ff00; border-radius: 5px; padding: 5px;");
    clearLogsBtn->setFixedHeight(40);
    connect(clearLogsBtn, &QPushButton::clicked, this, &Global_Window::clearLogs);
    sidebarLayout->addWidget(clearLogsBtn);

    QLabel* clientsLabel = new QLabel("Clients", sidebar);
    clientsLabel->setStyleSheet("color: #00ff00; font-weight: bold; padding: 5px;");
    sidebarLayout->addWidget(clientsLabel);

    clientLabel = new QLabel("Client 2 (2)\nClient 4 (0)", sidebar);
    clientLabel->setStyleSheet("color: #00ff41; padding: 5px;");
    sidebarLayout->addWidget(clientLabel);

    sidebarLayout->addStretch();
    mainLayout->addWidget(sidebar);

    // Main content
    QWidget* content = new QWidget(this);
    content->setStyleSheet("background-color: #0f0f1a; border: 2px solid #00ff00; border-radius: 5px;");
    QVBoxLayout* contentLayout = new QVBoxLayout(content);

    // Header
    QLabel* header = new QLabel("Logs Responses", content);
    header->setStyleSheet("color: #00ff00; font-weight: bold; padding: 5px; background-color: #16213e;");
    header->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(header);

    // Log area
    logArea = new QListWidget(content);
    logArea->setStyleSheet(
        "background-color: #0f0f1a;"
        "color: #00ff00;"
        "border: none;"
        "padding: 10px;"
        );
    logArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    contentLayout->addWidget(logArea, 1);

    // Wave background
    waveBackground = new QLabel(content);
    waveBackground->setStyleSheet("background-color: transparent;");
    waveBackground->setFixedHeight(100);
    contentLayout->addWidget(waveBackground);

    // Input area
    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputField = new QLineEdit(content);
    inputField->setPlaceholderText("Enter command (help for info)...");
    inputField->setStyleSheet(
        "background-color: #16213e;"
        "color: #00ff00;"
        "border: 2px solid #00ff41;"
        "border-radius: 5px;"
        "padding: 5px;"
        );
    inputLayout->addWidget(inputField);

    sendBtn = new QPushButton("Execute", content);
    sendBtn->setStyleSheet(
        "background-color: #16213e;"
        "color: #00ff41;"
        "border: 2px solid #00ff41;"
        "border-radius: 5px;"
        "padding: 5px;"
        );
    inputLayout->addWidget(sendBtn);
    contentLayout->addLayout(inputLayout);

    mainLayout->addWidget(content, 1);

    // Connections
    connect(sendBtn, &QPushButton::clicked, this, &Global_Window::sendMessage);
    connect(socket, &QTcpSocket::readyRead, this, &Global_Window::receiveMessage);

    // Wave animation
    waveTimer = new QTimer(this);
    connect(waveTimer, &QTimer::timeout, this, &Global_Window::updateWaveAnimation);
    waveTimer->start(50); // Update every 50ms
}

Global_Window::~Global_Window()
{
    if (waveTimer) {
        waveTimer->stop();
        delete waveTimer;
    }
    if (socket) {
        socket->disconnectFromHost();
        socket->close();
    }
}

void Global_Window::sendMessage()
{
    QString message = inputField->text().trimmed();
    if (!message.isEmpty() && socket && socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(message.toUtf8() + "\n"); // Add newline for clarity
        addLog("[UI] Command sent: " + message, "#00ff41");
        inputField->clear();
    } else if (!socket) {
        addLog("[Error] No socket connection", "#ff0000");
    } else {
        addLog("[Error] Socket not connected", "#ff0000");
    }
}

void Global_Window::receiveMessage()
{
    if (socket) {
        QString data = QString::fromUtf8(socket->readAll()).trimmed();
        if (!data.isEmpty()) {
            addLog("[Server] " + data, "#ff00ff");
        }
    }
}

void Global_Window::addLog(const QString& message, const QString& color)
{
    QListWidgetItem* item = new QListWidgetItem(message);
    item->setForeground(QColor(color));
    logArea->addItem(item);
    logArea->scrollToBottom();
}

void Global_Window::updateWaveAnimation()
{
    waveOffset = (waveOffset + 5) % width();
    waveBackground->update();
}

void Global_Window::refreshClients()
{
    // Simulate client refresh (replace with actual logic if needed)
    clientLabel->setText("Client 2 (2)\nClient 4 (0)");
    addLog("[UI] Client list refreshed", "#00ff41");
}

void Global_Window::clearLogs()
{
    logArea->clear();
    addLog("[UI] Logs cleared", "#00ff41");
}

void Global_Window::paintEvent(QPaintEvent*)
{
    QPainter painter(waveBackground);
    painter.setRenderHint(QPainter::Antialiasing);
    int height = waveBackground->height();
    int width = waveBackground->width();

    for (int i = 0; i < 3; ++i) {
        QLinearGradient gradient(0, height / 3 * i, 0, height / 3 * (i + 1));
        gradient.setColorAt(0, QColor(0, 255, 255, 50 * (3 - i)));
        gradient.setColorAt(1, QColor(255, 0, 255, 50 * (3 - i)));
        painter.setBrush(gradient);
        painter.setPen(Qt::NoPen);

        QPainterPath path;
        path.moveTo(0, height / 2 + sin((waveOffset + i * 100) * 0.01) * 20);
        for (int x = 0; x <= width; x += 10) {
            qreal y = height / 2 + sin((x + waveOffset + i * 100) * 0.01) * 20;
            path.lineTo(x, y);
        }
        path.lineTo(width, height);
        path.lineTo(0, height);
        path.closeSubpath();
        painter.drawPath(path);
    }
}
