# tests/

Os testes de corretude deste projeto (grafos manuais e cross-check
aleatório entre Dijkstra e A\*) estão implementados em `src/tests.cpp` e
rodam através do próprio executável principal:

```bash
./build/graphsearch --test
```

Essa pasta existe para manter a estrutura de diretórios pedida no
enunciado. Os testes não foram colocados em um binário/framework de teste
separado para não duplicar a lógica de carregamento de grafos e para manter
o projeto simples (um único executável, quatro modos de operação — veja a
Seção 16 do README na raiz do projeto).
