#ifndef THEME_H
#define THEME_H

#include <QString>

// Paleta de cores e folhas de estilo (QSS) do StreamUFSC — tema "azul
// marinho escuro" usado em todas as janelas do sistema.
//
// Antes, cada janela (.cpp) tinha os mesmos códigos hexadecimais escritos
// várias vezes espalhados pelo código. Centralizar tudo aqui em um só
// lugar traz duas vantagens:
//   1) Trocar uma cor do tema passa a ser uma mudança em um único lugar.
//   2) Evita divergência (ex.: uma janela usando "#E8B342" e outra usando
//      "#E8B343" por engano).
namespace Theme {

// ---- Cores ----
inline constexpr const char *FundoEscuro  = "#0A1628"; // fundo geral das janelas
inline constexpr const char *FundoCard    = "#16324F"; // fundo dos cartões e campos
inline constexpr const char *Borda        = "#1F3A57"; // bordas e hover neutro
inline constexpr const char *Dourado      = "#E8B342"; // cor de destaque do tema
inline constexpr const char *DouradoHover = "#F2C85B";
inline constexpr const char *DouradoPress = "#D4A02E";
inline constexpr const char *TextoClaro   = "#DCE6F0"; // texto principal
inline constexpr const char *TextoSuave   = "#9FB3C8"; // texto secundário
inline constexpr const char *Vermelho     = "#FF6B6B"; // ação de "remover"/desfavoritar

// ---- Folhas de estilo (QSS) prontas, reaproveitadas em várias janelas ----

// Fundo escuro + cor padrão de QLabel. Serve tanto para QDialog quanto
// para QMainWindow (o seletor certo é escolhido por quem chama).
QString estiloFundoDialog();
QString estiloFundoMainWindow();

// QLineEdit / QComboBox / QSpinBox no padrão do sistema.
QString estiloCamposDeEntrada();

// Botão de destaque (dourado), usado em ações principais como
// "Adicionar", "Entrar", "Salvar".
QString estiloBotaoDestaque();

// Botão secundário/neutro, usado em ações como "Sobre", "Cancelar", "Fechar".
QString estiloBotaoSecundario();

// Botões de filtro (abas clicáveis), com estado :checked destacado em dourado.
QString estiloBotaoFiltro();

// Título grande em destaque (dourado) — usado nos cabeçalhos das janelas.
QString estiloTituloDestaque(int tamanhoFontePx);

// Texto secundário pequeno (estatísticas, subtítulos, legendas).
QString estiloTextoSecundario(int tamanhoFontePx);

} // namespace Theme

#endif // THEME_H
