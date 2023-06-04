#ifndef AFD_H
#define AFD_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

using namespace std;

class Afd
{
private:
    vector<string> alfabeto;
    vector<string> estados;
    string inicial;
    vector<string> finais;
    unordered_map<string, vector<pair<string, string>>> transicoes;

public:
    Afd();
    ~Afd();

    void lerTransicoes(const string &arquivo);
    bool verificarCadeia(const string &cadeia);
    void criarImagem();

    Afd minimizarDFA();
    const vector<pair<string, string>> &getTransicoes(const string &estado);
    unordered_map<string, vector<pair<string, string>>> getTransicoesPorSimbolo();
    // int indiceDoEstado(const string &estado);
};

#endif