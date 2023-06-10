# Minimizador AFD

Aplica o algoritmo de Myhill Nerode (ou Table Filling Method) para minimizar o afd dado como entrada.

Lê o Autômato Finito Deterministico do **'input_AFD.txt'** no formato:

```
alfabeto:a,b,c,d # Lista de símbolos do alfabeto aceito pelo auômato
estado:q0,q1,q2 # Lista de estados no autômato
inicial:q0 # Indica qual é o estado inicial
finais:q1,q2 # Espefifica os estados finais do autômato
transicoes
q0,q1,a # Representa uma transição de q0 para q1 com o símbolo "a"
...
```

## Dependencias externas

- [Graphviz](https://graphviz.org/download/) - Responsável por gerar a imagem
