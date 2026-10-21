#include "account.h"
#include "mainwindow.h"
#include <QApplication>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFormLayout>
#include <QTextStream>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <fstream>
#include <string>

// ==================== 简单的本地用户管理系统 ====================

static const QString USER_FILE = "users.json";

static QJsonObject loadUsers() {
    QFile file(USER_FILE);
    if (!file.open(QIODevice::ReadOnly)) return QJsonObject();
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    return doc.object();
}

static void saveUsers(const QJsonObject& users) {
    QFile file(USER_FILE);
    file.open(QIODevice::WriteOnly);
    file.write(QJsonDocument(users).toJson());
}

// ==================== 注册/登录对话框 ====================

class AuthDialog : public QDialog {
    Q_OBJECT
public:
    QString username;
    bool loggedIn = false;

    AuthDialog(QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle("银行系统 - 登录/注册");
        setFixedSize(350, 220);

        auto* layout = new QVBoxLayout(this);
        auto* form   = new QFormLayout;

        auto* editUser = new QLineEdit;
        auto* editPass = new QLineEdit;
        editPass->setEchoMode(QLineEdit::Password);
        form->addRow("用户名:", editUser);
        form->addRow("密码:", editPass);
        layout->addLayout(form);

        auto* btnLayout = new QHBoxLayout;
        auto* btnLogin  = new QPushButton("登录");
        auto* btnReg    = new QPushButton("注册");
        btnLayout->addWidget(btnLogin);
        btnLayout->addWidget(btnReg);
        layout->addLayout(btnLayout);

        connect(btnLogin, &QPushButton::clicked, this, [=]() {
            QString u = editUser->text().trimmed();
            QString p = editPass->text().trimmed();
            if (u.isEmpty()) return;

            QJsonObject users = loadUsers();
            if (!users.contains(u)) {
                QMessageBox::warning(this, "错误", "用户不存在，请先注册。");
                return;
            }
            if (users[u].toString() != p) {
                QMessageBox::warning(this, "错误", "密码错误。");
                return;
            }
            username = u;
            loggedIn = true;
            accept();
        });

        connect(btnReg, &QPushButton::clicked, this, [=]() {
            QString u = editUser->text().trimmed();
            QString p = editPass->text().trimmed();
            if (u.isEmpty() || p.isEmpty()) {
                QMessageBox::warning(this, "错误", "用户名和密码不能为空。");
                return;
            }
            QJsonObject users = loadUsers();
            if (users.contains(u)) {
                QMessageBox::warning(this, "错误", "用户已存在，请直接登录。");
                return;
            }
            users[u] = p;
            saveUsers(users);
            QMessageBox::information(this, "成功", "注册成功！请登录。");
        });
    }
};

// ==================== main 入口 ====================

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // 登录/注册
    AuthDialog auth;
    if (auth.exec() != QDialog::Accepted || !auth.loggedIn) return 0;

    // 进入主界面
    MainWindow mainWin(auth.username.toStdString());
    mainWin.show();

    return app.exec();
}

#include "main.moc"
