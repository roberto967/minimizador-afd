#include "./include/Afd.H"

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
                alfabeto.push_back(token);
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
                estados.push_back(token);
                valor.erase(0, token_pos + 1);
            }
            estados.push_back(valor);
        }
        else if (chave == "inicial")
        {
            inicial = valor;
        }
        else if (chave == "finais")
        {
            size_t token_pos = 0;
            while ((token_pos = valor.find(',')) != string::npos)
            {
                string token = valor.substr(0, token_pos);
                finais.push_back(token);
                valor.erase(0, token_pos + 1);
            }
            finais.push_back(valor);
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

        transicoes[estado_atual].push_back(make_pair(estado_destino, simbolo));
    }

    file.close();
    preencherMatrizTransicoes();
}

vector<string> Afd::splitEstadoMinimizado(const string &estadoMinimizado)
{
    vector<string> estadosOriginais;
    string estadoOriginal;

    for (char c : estadoMinimizado)
    {
        if (c == 'E') // Separador de estados originais
        {
            if (!estadoOriginal.empty())
            {
                estadosOriginais.push_back(estadoOriginal);
                estadoOriginal.clear();
            }
        }
        else
        {
            estadoOriginal.push_back(c);
        }
    }

    if (!estadoOriginal.empty())
    {
        estadosOriginais.push_back(estadoOriginal);
    }

    return estadosOriginais;
}

string Afd::obterEstadoCombinado(const vector<string> &estadosOriginais)
{
    string estadoCombinado;

    for (const string &estado : estadosOriginais)
    {
        estadoCombinado += estado;
    }

    return estadoCombinado;
}

unordered_map<string, vector<pair<string, string>>> Afd::getTransicoesPorSimbolo()
{
    unordered_map<string, vector<pair<string, string>>> transicoesPorSimbolo;

    for (const auto &transicao : transicoes)
    {
        const string &estadoAtual = transicao.first;
        const vector<pair<string, string>> &transicoesEstado = transicao.second;

        for (const auto &transicaoEstado : transicoesEstado)
        {
            const string &estadoDestino = transicaoEstado.first;
            const string &simbolo = transicaoEstado.second;

            transicoesPorSimbolo[simbolo].push_back(make_pair(estadoAtual, estadoDestino));
        }
    }

    return transicoesPorSimbolo;
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

void Afd::criarImagem()
{
    string arquivo_saida = "afd";

    ofstream file(arquivo_saida + ".dot");
    file << "digraph AFD {" << endl;

    // Configuração do estado final com círculo duplo
    file << "node [shape=circle]; ";
    for (const string &estado : finais)
    {
        file << estado << " [peripheries=2]; "; // Define o estado final com círculo duplo
    }
    file << endl;

    // Configuração do estado normal com círculo simples
    file << "node [shape=circle]; ";
    for (const string &estado : estados)
    {
        bool eh_estado_final = false;
        for (const string &estado_final : finais)
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
    for (const string &estado : estados)
    {
        if (estado == inicial)
        {
            file << "node [shape=none];" << endl;
            file << "start -> " << estado << ";" << endl;
        }

        for (const pair<string, string> &transicao : transicoes[estado])
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
    return transicoes[estado];
}

#include <iomanip> // Biblioteca para espaçamento
#include <typeinfo>

Afd Afd::minimizarDFA()
{
    // Etapa 1: Crie os pares de todos os estados envolvidos
    vector<vector<bool>> marcados(estados.size(), vector<bool>(estados.size(), false));

    // Etapa 2: Marque todos os pares (Qa, Qb) onde Qa é final e Qb não é final
    for (size_t i = 0; i < estados.size(); i++)
    {
        for (size_t j = 0; j < estados.size(); j++)
        {
            if ((find(finais.begin(), finais.end(), estados[i]) != finais.end() && find(finais.begin(), finais.end(), estados[j]) == finais.end()) ||
                (find(finais.begin(), finais.end(), estados[i]) == finais.end() && find(finais.begin(), finais.end(), estados[j]) != finais.end()))
            {
                marcados[i][j] = true;
            }
        }
    }

    // Imprimir a matriz marcados com os estados na lateral usando a biblioteca iomanip
    cout << "   ";
    for (const string &estado : estados)
    {
        cout << setw(3) << estado;
    }
    cout << "\n";

    for (size_t i = 0; i < estados.size(); i++)
    {
        cout << setw(3) << estados[i];
        for (size_t j = 0; j < estados.size(); j++)
        {
            cout << setw(3) << (marcados[i][j] ? "X" : " ");
        }
        cout << "\n";
    }

    cout << "=======================================" << endl;
    // cout << transicoes["q1"][]

    for (const auto &transicao : transicoes["q1"])
    {
        string estadoDestino = transicao.first;
        string simbolo = transicao.second;

        cout << "q1"
             << "-" << simbolo << "->" << estadoDestino << endl;
    }

    // Etapa 3: Repita até não serem feitas mais marcações
    bool feito = false;
    while (!feito)
    {
        feito = true;

        for (size_t i = 0; i < estados.size(); i++)
        {
            for (size_t j = 0; j < estados.size(); j++)
            {
                if (!marcados[i][j])
                {
                    for (const string &simbolo : alfabeto)
                    {
                        const vector<pair<string, string>> &transicoesEstado1 = getTransicoes(estados[i]);
                        const vector<pair<string, string>> &transicoesEstado2 = getTransicoes(estados[j]);

                        // Verificar se os estados possuem transições para o símbolo atual
                        if (!transicoesEstado1.empty() && !transicoesEstado2.empty())
                        {
                            const string &proximoEstado1 = transicoesEstado1[0].first;
                            const string &proximoEstado2 = transicoesEstado2[0].first;

                            size_t index1 = distance(estados.begin(), find(estados.begin(), estados.end(), proximoEstado1));
                            size_t index2 = distance(estados.begin(), find(estados.begin(), estados.end(), proximoEstado2));

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

    cout << "__________depois do passo 3___________" << endl;
    cout << "   ";
    for (const string &estado : estados)
    {
        cout << setw(3) << estado;
    }
    cout << "\n";

    for (size_t i = 0; i < estados.size(); i++)
    {
        cout << setw(3) << estados[i];
        for (size_t j = 0; j < estados.size(); j++)
        {
            cout << setw(3) << (marcados[i][j] ? "X" : " ");
        }
        cout << "\n";
    }

    // Etapa 4: Combine os pares não marcados em um único estado no DFA minimizado
    Afd dfaMinimizado;

    vector<bool> visitado(estados.size(), false);

    for (size_t i = 0; i < estados.size(); i++)
    {
        if (!visitado[i])
        {
            string novoEstado = estados[i] + "E";

            for (size_t j = i + 1; j < estados.size(); j++)
            {
                if (!visitado[j] && !marcados[j][i])
                { // Considera apenas o triângulo inferior
                    novoEstado += estados[j];
                    visitado[j] = true;
                }
            }

            if (!novoEstado.empty() && novoEstado.back() == 'E')
            {
                novoEstado = novoEstado.substr(0, novoEstado.size() - 1);
            }

            dfaMinimizado.estados.push_back(novoEstado);

            if (novoEstado.find(inicial) != string::npos)
            {
                dfaMinimizado.inicial = novoEstado;
            }

            for (const string &estadoFinal : finais)
            {
                if (novoEstado.find(estadoFinal) != string::npos)
                {
                    dfaMinimizado.finais.push_back(novoEstado);
                    break;
                }
            }
        }
    }

    imprimirMatrizTransicoes();

    return dfaMinimizado;
}

void Afd::preencherMatrizTransicoes()
{
    matrizTransicoes.resize(estados.size(), vector<string>(alfabeto.size(), ""));

    for (size_t i = 0; i < estados.size(); i++)
    {
        for (size_t j = 0; j < alfabeto.size(); j++)
        {
            string estadoOrigem = estados[i];
            string simbolo = alfabeto[j];

            if (transicoes.find(estadoOrigem) != transicoes.end())
            {
                const vector<pair<string, string>> &estadoTransicoes = transicoes.at(estadoOrigem);

                for (const auto &transicao : estadoTransicoes)
                {
                    if (transicao.second == simbolo)
                    {
                        string estadoDestino = transicao.first;
                        matrizTransicoes[i][j] = estadoDestino;
                        break;
                    }
                }
            }
        }
    }
}

// Função para visualizar a matriz de transições
void Afd::imprimirMatrizTransicoes()
{
    // Imprimir cabeçalho da matriz com os símbolos do alfabeto
    std::cout << std::setw(6) << "Estado";
    for (const auto &simbolo : alfabeto)
    {
        std::cout << std::setw(6) << simbolo;
    }
    std::cout << std::endl;

    // Imprimir as transições de cada estado na matriz
    for (std::size_t i = 0; i < estados.size(); i++)
    {
        std::cout << std::setw(6) << estados[i];
        for (std::size_t j = 0; j < alfabeto.size(); j++)
        {
            std::cout << std::setw(6) << matrizTransicoes[i][j];
        }
        std::cout << std::endl;
    }
}

vector<string> Afd::getEstados() const
{
    return estados;
}