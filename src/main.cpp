#include <iostream>
#include "./include/Afd.h"

int main()
{
    try
    {
        Afd automato;

        automato.lerTransicoes("input_AFD.txt");
        automato.criarImagem("Afd_Original");

        // string cadeia;
        // cout << "Digite uma cadeia de caracteres: ";
        // cin >> cadeia;

        Afd dfaMinimizado = automato.minimizarDFA();
        dfaMinimizado.criarImagem("Afd_Minimizado");

        // bool aceita = automato.verificarCadeia(cadeia);

        // if (aceita)
        // {
        //     cout << "Cadeia aceita pelo automato." << endl;
        // }
        // else
        // {
        //     cout << "Cadeia rejeitada pelo autômato." << endl;
        // }
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
    }

    return 0;
}