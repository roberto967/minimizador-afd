#ifndef AFD_H
#define AFD_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

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
};

#endif