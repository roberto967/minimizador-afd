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
            size_t token_pos = 0;
            while ((token_pos = valor.find(',')) != string::npos)
            {
                string token = valor.substr(0, token_pos);
                this->alfabeto.push_back(token);
                valor.erase(0, token_pos + 1);
            }
            alfabeto.push_back(valor);
        }
        else if (chave == "estados")
        {
            size_t token_pos = 0;
            while ((token_pos = valor.find(',')) != string::npos)
            {
                string token = valor.substr(0, token_pos);
                this->estados.push_back(token);
                valor.erase(0, token_pos + 1);
            }
            estados.push_back(valor);
        }
        else if (chave == "inicial")
        {
            this->inicial = valor;
        }
        else if (chave == "finais")
        {
            size_t token_pos = 0;
            while ((token_pos = valor.find(',')) != string::npos)
            {
                string token = valor.substr(0, token_pos);
                this->finais.push_back(token);
                valor.erase(0, token_pos + 1);
            }
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

        string estado_atual = tokens[0];
        string estado_destino = tokens[1];
        string simbolo = tokens[2];

        this->transicoes[estado_atual].push_back(make_pair(estado_destino, simbolo));

        vector<string> transicao = {estado_atual, simbolo, estado_destino};
        this->matrizTransicoes.push_back(transicao);
    }

    file.close();
    // preencherMatrizTransicoes();
}

bool Afd::verificarCadeia(const string &cadeia)
{
    string estado_atual = inicial;

    for (char simbolo : cadeia)
    {
        string simbolo_str(1, simbolo);
        bool transicao_encontrada = false;

        // Verifica se o símbolo lido pertence ao alfabeto do AFD
        bool simbolo_presente = false;
        for (const string &s : alfabeto)
        {
            if (s == simbolo_str)
            {
                simbolo_presente = true;
                break;
            }
        }
        if (!simbolo_presente)
        {
            return false;
        }

        // Procura por uma transição apropriada
        for (const pair<string, string> &transicao : transicoes[estado_atual])
        {
            if (transicao.second == simbolo_str)
            {
                estado_atual = transicao.first;
                transicao_encontrada = true;
                break;
            }
        }

        // Se não houver transição para o símbolo lido, a cadeia é rejeitada
        if (!transicao_encontrada)
        {
            return false;
        }
    }

    // Verifica se o estado final alcançado está entre os estados finais
    bool estado_final_encontrado = false;
    for (const string &estado : finais)
    {
        if (estado == estado_atual)
        {
            estado_final_encontrado = true;
            break;
        }
    }

    return estado_final_encontrado;
}

void Afd::criarImagem(const string arquivo_saida)
{
    ofstream file(arquivo_saida + ".dot");
    file << "digraph AFD {" << endl;

    // Configuração do estado final com círculo duplo
    file << "node [shape=circle]; ";
    for (const string &estado : this->finais)
    {
        file << estado << " [peripheries=2]; "; // Define o estado final com círculo duplo
    }
    file << endl;

    // Configuração do estado normal com círculo simples
    file << "node [shape=circle]; ";
    for (const string &estado : this->estados)
    {
        bool eh_estado_final = false;
        for (const string &estado_final : this->finais)
        {
            if (estado == estado_final)
            {
                eh_estado_final = true;
                break;
            }
        }
        if (!eh_estado_final)
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
            file << "start -> " << estado << ";" << endl;
        }

        for (const pair<string, string> &transicao : this->transicoes[estado])
        {
            string estado_destino = transicao.first;
            string simbolo = transicao.second;
            file << estado << " -> " << estado_destino << " [label=\"" << simbolo << "\"];" << endl;
        }
    }

    file << "}" << endl;
    file.close();

    // Gera a imagem usando o Graphviz
    string comando = "dot -Tpng " + arquivo_saida + ".dot -o " + arquivo_saida + ".png";
    system(comando.c_str());
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
    for (const string &estado : this->estados)
    {
        cout << setw(3) << estado;
    }
    cout << "\n";

    for (size_t i = 0; i < this->estados.size(); i++)
    {
        cout << setw(3) << this->estados[i];
        for (size_t j = 0; j <= i; j++) // Considera apenas o triângulo inferior
        {
            cout << setw(3) << (marcados[i][j] ? "X" : " ");
        }
        cout << "\n";
    }
}

Afd Afd::minimizarDFA()
{

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

        cout << "__________Minimizando...___________" << endl;
        imprimirMatrizMinimizacao(marcados);

        for (size_t i = 0; i < this->estados.size(); i++)
        {
            for (size_t j = 0; j < this->estados.size(); j++)
            {
                if (!marcados[i][j])
                {
                    for (const string &simbolo : this->alfabeto)
                    {
                        const vector<pair<string, string>> &transicoesEstado1 = getTransicoes(this->estados[i]);
                        const vector<pair<string, string>> &transicoesEstado2 = getTransicoes(this->estados[j]);

                        // Verificar se os estados possuem transições para o símbolo atual
                        if (!transicoesEstado1.empty() && !transicoesEstado2.empty())
                        {
                            const string &proximoEstado1 = transicoesEstado1[0].first;
                            const string &proximoEstado2 = transicoesEstado2[0].first;

                            size_t index1 = distance(estados.begin(), find(estados.begin(), estados.end(), proximoEstado1));
                            size_t index2 = distance(this->estados.begin(), find(this->estados.begin(), this->estados.end(), proximoEstado2));

                            if (marcados[index1][index2])
                            {
                                marcados[i][j] = true;
                                feito = false;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    cout << "=========== Tabela do minimizado =============" << endl;
    imprimirMatrizMinimizacao(marcados);
    cout << "==============================================" << endl;

    // Etapa 4: Combine os pares não marcados em um único estado no DFA minimizado
    Afd dfaMinimizado;

    dfaMinimizado.alfabeto = this->alfabeto; // Copiando alfabeto para o afd minimizado

    vector<bool> visitado(this->estados.size(), false);

    vector<vector<string>> juntou;

    for (size_t i = 0; i < this->estados.size(); i++)
    {
        if (!visitado[i])
        {
            vector<string> colocarJuntou;
            string novoEstado = this->estados[i];

            if (find(colocarJuntou.begin(), colocarJuntou.end(), novoEstado) == colocarJuntou.end())
            {
                colocarJuntou.push_back(novoEstado);
            }

            for (size_t j = i + 1; j < this->estados.size(); j++)
            {
                if (!visitado[j] && !marcados[j][i]) // Considera apenas o triângulo inferior
                {
                    colocarJuntou.push_back(this->estados[j]);
                    novoEstado += this->estados[j];
                    visitado[j] = true;
                }
            }

            dfaMinimizado.estados.push_back(novoEstado);

            if (novoEstado.find(inicial) != string::npos)
            {
                dfaMinimizado.inicial = novoEstado;
            }

            for (const string &estadoFinal : this->finais)
            {
                if (novoEstado.find(estadoFinal) != string::npos)
                {
                    dfaMinimizado.finais.push_back(novoEstado);
                    break;
                }
            }

            juntou.push_back(colocarJuntou);
        }
    }

    for (const vector<string> &estadosJuntos : juntou)
    {
        string estadoAtualOriginal = estadosJuntos[0];
        string estadoAtualMinimizado;

        for (const string &estado : estadosJuntos)
        {
            estadoAtualMinimizado += estado;
        }

        unordered_map<string, string> mapeamentoDestinos; // Mapeamento dos estados de destino

        for (const string &simbolo : dfaMinimizado.alfabeto)
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

            for (const vector<string> &outrosEstadosJuntos : juntou)
            {
                string linhaJuntou;
                for (const string &estado : outrosEstadosJuntos)
                {
                    linhaJuntou += estado;
                }

                if (linhaJuntou.find(estadoDestinoOriginal) != string::npos)
                {
                    estadoDestinoMinimizado = linhaJuntou;
                    break;
                }
            }

            mapeamentoDestinos[simbolo] = estadoDestinoMinimizado;
        }

        for (const auto &par : mapeamentoDestinos)
        {
            vector<string> transicaoMinimizada = {estadoAtualMinimizado, par.first, par.second};
            dfaMinimizado.matrizTransicoes.push_back(transicaoMinimizada);
        }
    }

    for (const vector<string> &transicao : dfaMinimizado.matrizTransicoes)
    {
        string estadoAtual = transicao[0];
        string simbolo = transicao[1];
        string estadoDestino = transicao[2];

        dfaMinimizado.transicoes[estadoAtual].push_back(make_pair(estadoDestino, simbolo));
    }

    return dfaMinimizado;
}