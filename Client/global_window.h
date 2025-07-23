#ifndef GLOBAL_WINDOW_H
#define GLOBAL_WINDOW_H

#include <QWidget>
#include <QTcpSocket>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QTimer>
#include <QPainterPath> // Added to resolve incomplete type error

class Global_Window : public QWidget
{
    Q_OBJECT

public:
    explicit Global_Window(QTcpSocket* socket, QWidget *parent = nullptr);
    ~Global_Window(); // Declare destructor

protected:
    void paintEvent(QPaintEvent*); // Declare paintEvent

private slots:
    void sendMessage();
    void receiveMessage();
    void updateWaveAnimation();
    void refreshClients();
    void clearLogs();

private:
    void addLog(const QString& message, const QString& color);

    QTcpSocket* socket;
    QListWidget* logArea;
    QLineEdit* inputField;
    QPushButton* sendBtn;
    QPushButton* refreshBtn;
    QPushButton* clearLogsBtn;
    QLabel* clientLabel;
    QLabel* waveBackground;
    QTimer* waveTimer;
    int waveOffset;
};
#endif // GLOBAL_WINDOW_H
