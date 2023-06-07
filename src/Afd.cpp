#include "./include/Afd.h"

using namespace std;

Afd::Afd()
{
}

Afd::~Afd()
{
}

void Afd::lerTransicoes(const string &arquivo)
{
    ifstream file(arquivo);
    string linha;

    while (getline(file, linha))
    {
        if (linha == "transicoes")
        {
            break;
        }

        size_t pos = linha.find(':');
        if (pos == string::npos)
        {
            continue;
        }

        string chave = linha.substr(0, pos);
        string valor = linha.substr(pos + 1);

        if (chave == "alfabeto")
        {
            size_t tokenPos = 0;
            while ((tokenPos = valor.find(',')) != string::npos)
            {
                string token = valor.substr(0, tokenPos);
                this->alfabeto.push_back(token);
                valor.erase(0, tokenPos + 1);
            }
            this->alfabeto.push_back(valor);
        }
        else if (chave == "estados")
        {
            size_t tokenPos = 0;
            while ((tokenPos = valor.find(',')) != string::npos)
            {
                string token = valor.substr(0, tokenPos);
                this->estados.push_back(token);
                valor.erase(0, tokenPos + 1);
            }
            estados.push_back(valor);
        }
        else if (chave == "inicial")
        {
            size_t commaPos = valor.find(',');
            if (commaPos != string::npos)
            {
                throw runtime_error("Erro: Mais de um estado inicial declarado.");
            }
            this->inicial = valor;
        }
        else if (chave == "finais")
        {
            size_t tokenPos = 0;
            while ((tokenPos = valor.find(',')) != string::npos)
            {
                string token = valor.substr(0, tokenPos);
                this->finais.push_back(token);
                valor.erase(0, tokenPos + 1);
            }
            trimString(valor);
            this->finais.push_back(valor);
        }
    }

    while (getline(file, linha))
    {
        size_t pos = 0;
        vector<string> tokens;

        // Divide a linha em tokens separados por vírgula
        while ((pos = linha.find(',')) != string::npos)
        {
            string token = linha.substr(0, pos);
            tokens.push_back(token);
            linha.erase(0, pos + 1);
        }

        tokens.push_back(linha);

        string estadoAtual = tokens[0];
        string estadoDestino = tokens[1];
        string simbolo = tokens[2];

        this->transicoes[estadoAtual].push_back(make_pair(estadoDestino, simbolo));

        this->matrizTransicoes.push_back({estadoAtual, simbolo, estadoDestino});
    }

    file.close();
    verificarValidadeAfd();
    cout << "SUCESSO!" << endl;
}

void Afd::verificarValidadeAfd()
{
    // Verifica se há estado inicial
    if (this->inicial.empty())
    {
        throw runtime_error("Erro: Nao ha estado inicial no AFD.");
    }

    // Adiciona um estado vazio quando não há finais
    if (this->finais.empty())
    {
        this->finais.push_back(" ");
    }

    // Verifica se todas as letras do alfabeto tem transições definidas para cada estado
    for (const auto &estado : this->estados)
    {
        for (const auto &simbolo : this->alfabeto)
        {
            bool temTransicao = false;
            for (const auto &transicao : this->transicoes[estado])
            {
                if (transicao.second == simbolo)
                {
                    temTransicao = true;
                    break;
                }
            }
            if (!temTransicao)
            {
                throw runtime_error("Erro: Falta transicao para o simbolo '" + simbolo + "' no estado '" + estado + "'.");
            }
        }
    }

    // Verifica se há mais de uma transição para o mesmo símbolo em algum estado
    for (const auto &estado : this->estados)
    {
        unordered_set<string> simb; // unordered_set -> Mantém valores únicos
        for (const auto &transicao : this->transicoes[estado])
        {
            if (simb.count(transicao.second) > 0)
            {
                throw runtime_error("Erro: Ha mais de uma transicao para o simbolo '" + transicao.second + "' no estado '" + estado + "'.");
            }
            simb.insert(transicao.second);
        }
    }
}

string Afd::verificarCadeia(const string &cadeiaParam)
{
    string cadeia = cadeiaParam;

    trimString(cadeia);

    if (alfabeto.empty() || estados.empty() || matrizTransicoes.empty())
    {
        throw runtime_error("Erro: Operacao invalida.");
    }

    if (finais.empty() && cadeia == "")
    {
        return "Cadeia aceita pelo automato.";
    }

    if (finais.empty() && cadeia != "")
    {
        return "Cadeia rejeitada pelo automato.";
    }

    string estadoAtual = inicial;

    for (char simbolo : cadeia)
    {
        string simboloStr(1, simbolo);
        bool transicaoEncontrada = false;

        bool simboloPresente = false;
        for (const string &s : alfabeto)
        {
            if (s == simboloStr)
            {
                simboloPresente = true;
                break;
            }
        }
        if (!simboloPresente)
        {
            return "Cadeia rejeitada pelo automato.";
        }

        // Procura por uma transição apropriada
        for (const pair<string, string> &transicao : transicoes[estadoAtual])
        {
            if (transicao.second == simboloStr)
            {
                estadoAtual = transicao.first;
                transicaoEncontrada = true;
                break;
            }
        }

        // Se não houver transição para o símbolo lido, a cadeia é rejeitada
        if (!transicaoEncontrada)
        {
            return "Cadeia rejeitada pelo automato.";
        }
    }

    // Verifica se o estado final alcançado está entre os estados finais
    bool estadoFinalEncontrado = false;
    for (const string &estado : finais)
    {
        if (estado == estadoAtual)
        {
            estadoFinalEncontrado = true;
            break;
        }
    }

    if (estadoFinalEncontrado)
    {
        return "Cadeia aceita pelo automato.";
    }
    else
    {
        return "Cadeia rejeitada pelo automato.";
    }
}

void Afd::criarImagem(const string arquivoSaida)
{
    string arquivoDot = arquivoSaida + ".dot";
    string arquivoPng = arquivoSaida + ".png";

    ofstream file(arquivoDot);
    file << "digraph AFD {" << endl;

    // Configuração do estado final com círculo duplo
    file << "node [shape=circle]; ";
    for (const string &estado : this->finais)
    {
        if (estado == "" || estado == " ") // Não havendo estado final faz nada
        {
        }
        else
        {
            file << estado << " [peripheries=2]; "; // Define o estado final com círculo duplo
        }
    }
    file << endl;

    // Configuração do estado normal com círculo simples
    file << "node [shape=circle]; ";
    for (const string &estado : this->estados)
    {
        bool ehEstadoFinal = false;
        for (const string &estadoFinal : this->finais)
        {
            if (estado == estadoFinal)
            {
                ehEstadoFinal = true;
                break;
            }
        }
        if (!ehEstadoFinal)
        {
            file << estado << "; "; // Define o estado normal com círculo simples
        }
    }
    file << endl;

    file << "node [shape=none];" << endl;

    // Criação dos estados e transições
    for (const string &estado : this->estados)
    {
        if (estado == inicial)
        {
            file << "node [shape=none];" << endl;
            file << "início -> " << estado << ";" << endl;
        }

        for (const pair<string, string> &transicao : this->transicoes[estado])
        {
            string estadoDestino = transicao.first;
            string simbolo = transicao.second;
            file << estado << " -> " << estadoDestino << " [label=\"" << simbolo << "\"];" << endl;
        }
    }

    file << "}" << endl;
    file.close();

    // Gera a imagem usando o Graphviz
    string comando = "dot -Tpng " + arquivoDot + " -o " + arquivoPng;
    system(comando.c_str());

    // Move o arquivo para "output"
    string novoArquivoDot = "output/" + arquivoDot;
    string novoArquivoPng = "output/" + arquivoPng;

    // Exclui o arquivo existente, se já existir
    remove(novoArquivoDot.c_str());
    remove(novoArquivoPng.c_str());

    // Move o arquivo para "output"
    rename(arquivoDot.c_str(), novoArquivoDot.c_str());
    rename(arquivoPng.c_str(), novoArquivoPng.c_str());

    // Exclui o arquivo .dot
    remove(novoArquivoDot.c_str());
}

const vector<pair<string, string>> &Afd::getTransicoes(const string &estado)
{
    return this->transicoes[estado];
}

void Afd::imprimirMatrizTransicoes()
{
    for (size_t i = 0; i < this->matrizTransicoes.size(); i++)
    {
        for (size_t j = 0; j < this->matrizTransicoes[i].size(); j++)
        {
            cout << this->matrizTransicoes[i][j] << " ";
        }
        cout << endl;
    }
}

void Afd::imprimirMatrizMinimizacao(const vector<vector<bool>> marcados)
{
    cout << "   ";
    for (size_t i = 0; i < this->estados.size(); i++)
    {
        cout << "+------";
    }
    cout << "+" << endl;

    cout << "   ";
    for (const string &estado : this->estados)
    {
        cout << "|" << setw(6) << estado << "";
    }
    cout << "|" << endl;

    cout << "   ";
    for (size_t i = 0; i < this->estados.size(); i++)
    {
        cout << "+------";
    }
    cout << "+" << endl;

    for (size_t i = 0; i < this->estados.size(); i++)
    {
        cout << setw(3) << this->estados[i];
        for (size_t j = 0; j <= i; j++)
        { // Considera apenas o triângulo inferior
            cout << "| ";
            if (marcados[i][j])
            {
                cout << " [X] ";
            }
            else
            {
                cout << " [ ] ";
            }
        }
        cout << "|" << endl;

        cout << "   ";
        for (size_t k = 0; k < this->estados.size(); k++)
        {
            cout << "+------";
        }
        cout << "+" << endl;
    }
}

Afd Afd::minimizarAFD()
{
    if (alfabeto.empty() || estados.empty() || matrizTransicoes.empty())
    {
        throw runtime_error("Erro: Operacao invalida.");
    }

    // Etapa 1: Crie os pares de todos os estados envolvidos
    vector<vector<bool>> marcados(this->estados.size(), vector<bool>(this->estados.size(), false));

    // Etapa 2: Marque todos os pares (Qa, Qb) onde Qa é final e Qb não é final
    for (size_t i = 0; i < this->estados.size(); i++)
    {
        for (size_t j = 0; j < this->estados.size(); j++)
        {
            if ((find(this->finais.begin(), this->finais.end(), this->estados[i]) != this->finais.end() && find(this->finais.begin(), this->finais.end(), this->estados[j]) == this->finais.end()) ||
                (find(this->finais.begin(), this->finais.end(), this->estados[i]) == this->finais.end() && find(this->finais.begin(), this->finais.end(), this->estados[j]) != this->finais.end()))
            {
                marcados[i][j] = true;
            }
        }
    }

    // Etapa 3: Repita até não serem feitas mais marcações
    bool feito = false;
    while (!feito)
    {
        feito = true;

        cout << "\n__________Minimizando...___________\n"
             << endl;
        imprimirMatrizMinimizacao(marcados);
        cout << "\n___________________________________\n"
             << endl;

        for (size_t i = 0; i < this->estados.size(); i++)
        {
            for (size_t j = 0; j < this->estados.size(); j++)
            {
                if (!marcados[i][j])
                {
                    for (size_t k = 0; k < this->alfabeto.size(); k++)
                    {
                        const vector<pair<string, string>> &transicoesEstado1 = getTransicoes(this->estados[i]);
                        const vector<pair<string, string>> &transicoesEstado2 = getTransicoes(this->estados[j]);

                        // Verifica se os estados possuem transições para o símbolo atual
                        bool encontrouMarcado = false;
                        for (const auto &transicao1 : transicoesEstado1)
                        {
                            if (transicao1.second == this->alfabeto[k])
                            {
                                for (const auto &transicao2 : transicoesEstado2)
                                {
                                    if (transicao2.second == this->alfabeto[k])
                                    {
                                        size_t index1 = distance(estados.begin(), find(estados.begin(), estados.end(), transicao1.first));
                                        size_t index2 = distance(estados.begin(), find(estados.begin(), estados.end(), transicao2.first));

                                        if (marcados[index1][index2])
                                        {
                                            marcados[i][j] = true;
                                            encontrouMarcado = true;
                                            break;
                                        }
                                    }
                                }
                                if (encontrouMarcado)
                                {
                                    break;
                                }
                            }
                        }
                        if (encontrouMarcado)
                        {
                            break;
                        }
                    }
                }
            }
        }
    }

    cout << "\n=========== Tabela do minimizado =============\n"
         << endl;
    imprimirMatrizMinimizacao(marcados);
    cout << "\n==============================================\n"
         << endl;

    // Etapa 4: Combine os pares não marcados em um único estado no AFD minimizado
    Afd afdMinimizado;

    afdMinimizado.alfabeto = this->alfabeto; // Copiando alfabeto para o afd minimizado

    vector<bool> visitado(this->estados.size(), false);

    vector<vector<string>> mNomesUnidos; // Cada linha representa o nome do estado do afd minimizado

    for (size_t i = 0; i < this->estados.size(); i++)
    {
        if (!visitado[i])
        {
            vector<string> vetorEstadosUnidos;
            string novoEstado = this->estados[i];

            if (find(vetorEstadosUnidos.begin(), vetorEstadosUnidos.end(), novoEstado) == vetorEstadosUnidos.end())
            {
                vetorEstadosUnidos.push_back(novoEstado);
            }

            for (size_t j = i + 1; j < this->estados.size(); j++)
            { // Considera apenas o triângulo inferior
                if (!visitado[j] && !marcados[j][i])
                {
                    vetorEstadosUnidos.push_back(this->estados[j]);
                    novoEstado += this->estados[j];
                    visitado[j] = true;
                }
            }

            afdMinimizado.estados.push_back(novoEstado);

            if (novoEstado.find(inicial) != string::npos)
            {
                afdMinimizado.inicial = novoEstado;
            }

            for (const string &estadoFinal : this->finais)
            {
                if (estadoFinal == "" || estadoFinal == " ")
                {
                    break;
                }
                if (novoEstado.find(estadoFinal) != string::npos)
                {
                    afdMinimizado.finais.push_back(novoEstado);
                    break;
                }
            }

            mNomesUnidos.push_back(vetorEstadosUnidos);
        }
    }

    for (const vector<string> &estadosJuntos : mNomesUnidos)
    {
        string estadoAtualOriginal = estadosJuntos[0];
        string estadoAtualMinimizado;

        for (const string &estado : estadosJuntos)
        {
            estadoAtualMinimizado += estado;
        }

        unordered_map<string, string> mapeamentoDestinos;

        for (const string &simbolo : afdMinimizado.alfabeto)
        {
            vector<pair<string, string>> transicoesOriginais = transicoes[estadoAtualOriginal];
            string estadoDestinoOriginal;

            for (const auto &transicao : transicoesOriginais)
            {
                if (transicao.second == simbolo)
                {
                    estadoDestinoOriginal = transicao.first;
                    break;
                }
            }

            string estadoDestinoMinimizado;

            for (const vector<string> &outrosEstadosJuntos : mNomesUnidos)
            {
                string linhaEstadosUnidos;
                for (const string &estado : outrosEstadosJuntos)
                {
                    linhaEstadosUnidos += estado;
                }

                if (linhaEstadosUnidos.find(estadoDestinoOriginal) != string::npos)
                {
                    estadoDestinoMinimizado = linhaEstadosUnidos;
                    break;
                }
            }

            mapeamentoDestinos[simbolo] = estadoDestinoMinimizado;
        }

        for (const auto &par : mapeamentoDestinos)
        {
            vector<string> transicaoMinimizada = {estadoAtualMinimizado, par.first, par.second};
            afdMinimizado.matrizTransicoes.push_back(transicaoMinimizada);
        }
    }

    for (const vector<string> &transicao : afdMinimizado.matrizTransicoes)
    {
        string estadoAtual = transicao[0];
        string simbolo = transicao[1];
        string estadoDestino = transicao[2];

        afdMinimizado.transicoes[estadoAtual].push_back(make_pair(estadoDestino, simbolo));
    }

    return afdMinimizado;
}

void Afd::trimString(string &str)
{
    // Encontra o primeiro caractere não branco a partir do início
    size_t inicio = str.find_first_not_of(" \t");
    if (inicio == string::npos)
    {
        str.clear();
        return;
    }

    size_t end = str.find_last_not_of(" \t");
    // Remove os espaços em branco do final
    str.erase(end + 1);

    // Remove os espaços em branco do início
    str.erase(0, inicio);
}

void Afd::limparAutomato()
{
    this->alfabeto.clear();
    this->estados.clear();
    this->inicial.clear();
    this->finais.clear();
    this->transicoes.clear();
    this->matrizTransicoes.clear();
}