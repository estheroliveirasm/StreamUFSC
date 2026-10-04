# StreamUFSC

Sistema desktop para organizar e avaliar filmes e séries, desenvolvido em C++17 com Qt Widgets. Projeto final (T2) da disciplina **Linguagem de Programação 2**, da Universidade Federal de Santa Catarina (UFSC), Campus Araranguá.

## Descrição do trabalho

O StreamUFSC resolve o problema de registrar títulos que a pessoa quer assistir, está assistindo ou já assistiu. A aplicação reúne um catálogo visual de filmes e séries, uma coleção pessoal, filtros, pesquisa, avaliações e persistência local.

A coleção começa vazia: nenhum título é marcado como favorito ou recebe nota sem ação da usuária. O catálogo incluído no projeto contém 34 títulos com pôsteres; cada título pode ser adicionado à coleção e avaliado depois.

## Funcionalidades

- Tela inicial com entrada pelo botão ou pela tecla Enter.
- Catálogo com 34 pôsteres e visualização de detalhes de cada título.
- Cadastro de filmes e séries, com gênero, ano, tipo, status e pôster opcional.
- Ações para adicionar à coleção, favoritar, avaliar de 0 a 5, alterar o status e remover títulos.
- Filtros por todos, favoritos, quero assistir, assistindo, assistidos e catálogo.
- Pesquisa por título, gênero, ano ou tipo e filtro entre filmes e séries.
- Grade de pôsteres que se adapta à largura da janela.
- Salvamento automático da coleção em arquivo de texto na pasta de dados do usuário.
