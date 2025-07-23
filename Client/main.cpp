#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include "connect.h"
#include "global_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QWidget widget;
    widget.setMinimumSize(500, 300);
    widget.setWindowTitle("gifarius");

    QLineEdit *ip = new QLineEdit(&widget);
    ip->setGeometry(100, 30, 300, 40);
    ip->setPlaceholderText("IP");

    QLineEdit *port = new QLineEdit(&widget);
    port->setGeometry(100, 80, 300, 40);
    port->setPlaceholderText("Порт");

    QPushButton *btn = new QPushButton("Start", &widget);
    btn->setGeometry(100, 130, 300, 40);

    QLabel *errorLabel = new QLabel(&widget);
    errorLabel->setGeometry(100, 190, 300, 30);
    errorLabel->setStyleSheet("color: red;");
    errorLabel->setText("");

    Connector connector;

    QObject::connect(btn, &QPushButton::clicked, [&]() {
        QString result = connector.connectToHost(widget, ip->text(), port->text());
        errorLabel->setText(result);

        if (result == "Успешно подключено") {
            Global_Window *win = new Global_Window(connector.getSocket());
            win->show();
            widget.hide();
        }
    });

    widget.show();
    return app.exec();
}
