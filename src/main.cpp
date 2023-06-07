#include <iostream>
#include <dirent.h>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>

#include "./include/Afd.h"

#ifdef _WIN32
#include <cstdlib>
#elif __linux__
#include <cstdlib>
#endif

void limparTela()
{
#ifdef _WIN32
    system("cls");
#elif __linux__
    system("clear");
#endif
}

void pausa()
{
#ifdef _WIN32
    system("pause");
#elif __linux__
    system("read -p 'Pressione Enter para continuar...'");
#endif
}

void menu()
{
    cout << "-----Menu-----" << endl;
    cout << "0 - sair" << endl;
    cout << "1 - ler arquivo" << endl;
    cout << "2 - minimizar automato" << endl;
    cout << "3 - vizualizar automato" << endl;
    cout << "4 - inserir cadeia" << endl;
    cout << "--------------" << endl;
}

void menuVisualizacao()
{
    limparTela();
    int opcaoVisualizacao;
    cout << "-----Menu de visualizacao-----" << endl;

    // Listar os arquivos na pasta
    vector<string> arquivos;
    DIR *diretorio;
    struct dirent *arquivo;
    diretorio = opendir("output");

    if (diretorio)
    {
        while ((arquivo = readdir(diretorio)) != nullptr)
        {
            string nomeArquivo = arquivo->d_name;

            // Ignora os arquivos ocultos
            if (nomeArquivo[0] != '.')
            {
                arquivos.push_back(nomeArquivo);
            }
        }
        closedir(diretorio);
    }
    else
    {
        cerr << "Erro ao abrir o diretorio." << endl;
        return;
    }

    while (true)
    {
        // Exibir os arquivos disponíveis para visualização
        cout << "0 - Sair da vizualizacao" << endl;
        for (size_t i = 0; i < arquivos.size(); i++)
        {
            cout << i + 1 << " - " << arquivos[i] << endl;
        }

        cout << "______________________________" << endl;
        cout << "Digite o numero do arquivo que deseja visualizar: ";
        cin >> opcaoVisualizacao;

        if (opcaoVisualizacao == 0)
        {
            limparTela();
            return;
        }
        else if (opcaoVisualizacao >= 1 && opcaoVisualizacao <= arquivos.size())
        {
            string caminhoCompleto = "./output/" + arquivos[opcaoVisualizacao - 1];

#ifdef _WIN32
            wstring comando = L"cmd /c start \"\" \"" + wstring(caminhoCompleto.begin(), caminhoCompleto.end()) + L"\"";
            int resultado = _wsystem(comando.c_str());
            if (resultado != 0)
            {
                cerr << "Erro ao abrir o arquivo PNG." << endl;
            }
#elif __linux__
            string comando = "xdg-open \"" + caminhoCompleto + "\"";
            int resultado = system(comando.c_str());
            if (resultado != 0)
            {
                cerr << "Erro ao abrir o arquivo PNG." << endl;
            }
#endif
        }
        else
        {
            cerr << "Opcao invalida." << endl;
        }
    }
}

int main()
{
    int opcao;
    int opcaoAutomato;

    Afd automato;
    Afd afdMinimizado;

    string cadeia;

    do
    {
        try
        {
            menu();
            cout << "insira a opcao: ";
            cin >> opcao;

            switch (opcao)
            {
            case 0:
                cout << "Saindo..." << endl;
                break;

            case 1:
                automato.limparAutomato();

                automato.lerTransicoes("input_AFD.txt");
                automato.criarImagem("Afd_Original");

                pausa();
                limparTela();
                break;

            case 2:
                limparTela();
                afdMinimizado = automato.minimizarAFD();

                afdMinimizado.criarImagem("Afd_Minimizado");
                pausa();
                limparTela();
                break;

            case 3:
                menuVisualizacao();
                break;

            case 4:
                limparTela();
                cout << "-----automatos disponiveis-----" << endl;
                cout << "0 - cancelar" << endl;
                cout << "1 - automato original" << endl;
                cout << "2 - automato minimizado" << endl;
                cout << "-------------------------------" << endl;
                cout << "insira uma opcao: ";
                cin >> opcaoAutomato;

                switch (opcaoAutomato)
                {
                case 0:
                    cout << "Operacao cancelada." << endl;
                    break;
                case 1:
                    limparTela();

                    cout << "Automato original." << endl;
                    cout << "------------------" << endl;
                    cout << "Insira a cadeia: ";
                    cin.ignore();
                    getline(cin, cadeia);
                    cout << endl;

                    cout << automato.verificarCadeia(cadeia) << endl;
                    break;

                case 2:
                    limparTela();

                    cout << "Automato minimizado." << endl;
                    cout << "--------------------" << endl;
                    cout << "Insira a cadeia: ";
                    cin.ignore();
                    getline(cin, cadeia);
                    cout << endl;

                    cout << afdMinimizado.verificarCadeia(cadeia) << endl;
                    break;

                default:
                    cout << "Opcao invalida." << endl;
                    break;
                }
                pausa();
                limparTela();
                break;

            default:
                cout << "Opcao invalida." << endl;
                pausa();
                limparTela();
                break;
            }
        }
        catch (const exception &e)
        {
            afdMinimizado.limparAutomato();
            automato.limparAutomato();

            cerr << e.what() << endl;
            cerr << "Os dados dos automados foram apagados." << endl;

            pausa();
            limparTela();
        }
    } while (opcao != 0);

    return 0;
}