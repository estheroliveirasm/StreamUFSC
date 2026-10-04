#include "startwindow.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("UFSC");
    QCoreApplication::setApplicationName("StreamUFSC");

    // Primeiro mostramos a tela inicial (splash) com a foto e as
    // informações do trabalho. O programa só continua quando o
    // usuário aperta ENTER ou clica no botão "Entrar".
    StartWindow telaInicial;
    if (telaInicial.exec() != QDialog::Accepted) {
        return 0; // usuário fechou a tela inicial sem entrar
    }

    MainWindow janelaPrincipal;
    janelaPrincipal.show();

    return app.exec();
}
