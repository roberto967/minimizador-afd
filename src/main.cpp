#include <iostream>
#include "./include/Afd.h"

int main()
{
    Afd automato;

    automato.lerTransicoes("input_AFD.txt");

    string cadeia;
    cout << "Digite uma cadeia de caracteres: ";
    cin >> cadeia;

    bool aceita = automato.verificarCadeia(cadeia);

    if (aceita)
    {
        cout << "Cadeia aceita pelo autômato." << endl;
    }
    else
    {
        cout << "Cadeia rejeitada pelo autômato." << endl;
    }

    return 0;
}