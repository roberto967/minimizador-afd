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

        // Adiciona o símbolo no alfabeto, se ainda não estiver presente
        bool simbolo_presente = false;
        for (const string &s : alfabeto)
        {
            if (s == simbolo)
            {
                simbolo_presente = true;
                break;
            }
        }
        if (!simbolo_presente)
        {
            alfabeto.push_back(simbolo);
        }

        // Adiciona os estados na lista de estados, se ainda não estiverem presentes
        bool estado_atual_presente = false;
        bool estado_destino_presente = false;
        for (const string &estado : estados)
        {
            if (estado == estado_atual)
            {
                estado_atual_presente = true;
            }
            if (estado == estado_destino)
            {
                estado_destino_presente = true;
            }
        }
        if (!estado_atual_presente)
        {
            estados.push_back(estado_atual);
        }
        if (!estado_destino_presente)
        {
            estados.push_back(estado_destino);
        }

        // Adiciona a transição ao dicionário de transições
        transicoes[estado_atual].push_back(make_pair(estado_destino, simbolo));
    }

    file.close();
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