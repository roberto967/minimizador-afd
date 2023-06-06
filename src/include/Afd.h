#ifndef AFD_H
#define AFD_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <unordered_map>
#include <iomanip>

using namespace std;

class Afd
{
private:
public:
    vector<string> alfabeto;
    vector<string> estados;
    string inicial;
    vector<string> finais;
    unordered_map<string, vector<pair<string, string>>> transicoes;

    vector<vector<string>> matrizTransicoes;

    Afd();
    ~Afd();

    void lerTransicoes(const string &arquivo);
    bool verificarCadeia(const string &cadeia);

    void criarImagem(const string arquivo_saida);

    Afd minimizarDFA();

    const vector<pair<string, string>> &getTransicoes(const string &estado);

    void imprimirMatrizTransicoes();
};

#endif