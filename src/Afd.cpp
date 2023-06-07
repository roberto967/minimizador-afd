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
            this->alfabeto.push_back(valor);
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
            size_t comma_pos = valor.find(',');
            if (comma_pos != string::npos)
            {
                throw runtime_error("Erro: Mais de um estado inicial declarado.");
            }
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

        string estado_atual = tokens[0];
        string estado_destino = tokens[1];
        string simbolo = tokens[2];

        this->transicoes[estado_atual].push_back(make_pair(estado_destino, simbolo));

        this->matrizTransicoes.push_back({estado_atual, simbolo, estado_destino});
    }

    file.close();
    verificarValidadeAfd();
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

    // Verifica se todas as letras do alfabeto têm transições definidas para cada estado
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
        if (estado == "" || estado == " ")
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
            file << "início -> " << estado << ";" << endl;
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
        cout << "___________________________________" << endl;

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

                        // Verificar se os estados possuem transições para o símbolo atual
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

    cout << "\n=========== Tabela do minimizado =============" << endl;
    imprimirMatrizMinimizacao(marcados);
    cout << "==============================================" << endl;

    // Etapa 4: Combine os pares não marcados em um único estado no AFD minimizado
    Afd afdMinimizado;

    afdMinimizado.alfabeto = this->alfabeto; // Copiando alfabeto para o afd minimizado

    vector<bool> visitado(this->estados.size(), false);

    vector<vector<string>> mNomesUnidos; // Cada linha representa o nome do estado

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

        unordered_map<string, string> mapeamentoDestinos; // Mapeamento dos estados de destino

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