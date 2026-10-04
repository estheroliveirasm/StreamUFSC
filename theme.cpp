#include "theme.h"

namespace Theme {

QString estiloFundoDialog()
{
    return QString(
               "QDialog { background-color: %1; }"
               "QLabel { color: %2; }")
        .arg(FundoEscuro, TextoClaro);
}

QString estiloFundoMainWindow()
{
    return QString(
               "QMainWindow { background-color: %1; }"
               "QScrollArea { background-color: %1; border: none; }"
               "QWidget#gradeContainer { background-color: %1; }"
               "QLabel { color: %2; }")
        .arg(FundoEscuro, TextoClaro);
}

QString estiloCamposDeEntrada()
{
    return QString(
               "QLineEdit, QComboBox, QSpinBox { padding: 6px; border: 1px solid %1; "
               "  border-radius: 5px; background-color: %2; color: %3; }")
        .arg(Borda, FundoCard, TextoClaro);
}

QString estiloBotaoDestaque()
{
    return QString(
               "QPushButton { background-color: %1; color: %2; font-weight: bold; "
               "  padding: 8px 16px; border-radius: 6px; border: none; }"
               "QPushButton:hover { background-color: %3; }"
               "QPushButton:pressed { background-color: %4; }")
        .arg(Dourado, FundoEscuro, DouradoHover, DouradoPress);
}

QString estiloBotaoSecundario()
{
    return QString(
               "QPushButton { background-color: %1; color: %2; padding: 8px 16px; "
               "  border-radius: 6px; border: 1px solid %3; }"
               "QPushButton:hover { background-color: %3; }")
        .arg(FundoCard, TextoClaro, Borda);
}

QString estiloBotaoFiltro()
{
    return QString(
               "QPushButton { background-color: %1; color: %2; padding: 6px 14px; "
               "  border-radius: 14px; border: 1px solid %3; }"
               "QPushButton:checked { background-color: %4; color: %5; font-weight: bold; }"
               "QPushButton:hover { background-color: %3; }")
        .arg(FundoCard, TextoSuave, Borda, Dourado, FundoEscuro);
}

QString estiloTituloDestaque(int tamanhoFontePx)
{
    return QString("font-size: %1px; font-weight: bold; color: %2;")
        .arg(tamanhoFontePx).arg(Dourado);
}

QString estiloTextoSecundario(int tamanhoFontePx)
{
    return QString("font-size: %1px; color: %2;").arg(tamanhoFontePx).arg(TextoSuave);
}

} // namespace Theme
