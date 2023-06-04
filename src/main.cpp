#include <iostream>
#include "./include/Afd.h"

int main()
{
    Afd automato;

    automato.lerTransicoes("input_AFD.txt");
    cout << "alooo" << endl;
    automato.criarImagem();

    // string cadeia;
    // cout << "Digite uma cadeia de caracteres: ";
    // cin >> cadeia;

    Afd dfaMinimizado = automato.minimizarDFA();
    dfaMinimizado.criarImagem();

    //bool aceita = automato.verificarCadeia(cadeia);

    // if (aceita)
    // {
    //     cout << "Cadeia aceita pelo automato." << endl;
    // }
    // else
    // {
    //     cout << "Cadeia rejeitada pelo autômato." << endl;
    // }

    return 0;
}