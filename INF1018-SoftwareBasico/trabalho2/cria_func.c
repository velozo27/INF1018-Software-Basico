/*Nome: Luiza Bretas J Corrêa Luiz    Matrícula: 1911867     Turma: 3WA*/
/*Nome: Pedro Antonio Tibau Velozo    Matrícula: 1812013     Turma: 3WA*/

#include <stdlib.h>
#include "cria_func.h"

void *cria_func(void *f, DescParam params[], int n)
{
    unsigned char *codigo = (unsigned char *)malloc(100);

    // Alinhando a pilha
    codigo[0] = 0x55;
    codigo[1] = 0x48;
    codigo[2] = 0x89;
    codigo[3] = 0xe5;

    int pos_no_array = 4;
    int param_count = 0;

    /*determinando o numero de parametros*/
    for (int i = 0; i < n; i++)
    {
        if (params[i].orig_val == PARAM)
            param_count++;
    }

    // inserindo o codigo de maquina no array codigo
    for (int i = n - 1; i >= 0; i--)
    { // percorrendo de tras pra frente para nao sobreescrever nenhum registrador
        if (i == 0)
        {
            if (params[i].tipo_val == INT_PAR)
            {
                // parametro INTEIRO
                if (params[i].orig_val == FIX)
                {
                    // parametro fixo, vou passar o valor localizado em params[i].valor.v_int para %rdi
                    /*
                    0x48 0xc7 0xc7 eh o comando de movq para %rdi,
                    seguido do numero "fixo" em hexadecimal nos 4 bytes seguintes
                    */
                    int fixo = params[i].valor.v_int;
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0xc7;
                    pos_no_array++;
                    codigo[pos_no_array] = 0xc7;
                    pos_no_array++;

                    union
                    {
                        int numero;
                        char numero_separado_em_4_bytes[4];
                    } w;
                    w.numero = fixo;

                    for (int j = 0; j < 4; j++)
                    {
                        codigo[pos_no_array] = w.numero_separado_em_4_bytes[j];
                        pos_no_array++;
                    }
                }
                if (params[i].orig_val == PARAM)
                {
                    // passar o paremetro em %rdi para %rdi
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x89;
                    pos_no_array++;
                    codigo[pos_no_array] = 0xff;
                    pos_no_array++;

                    param_count--;
                }
                if (params[i].orig_val == IND)
                {
                    // parametro indireto, vou receber o endereco da variavel. tenho que passar esse endereco
                    // para um registrador qualquer e depois passar seu conteudo para %rdi
                    // ex:
                    //    movq    $0xffffffffffffffff, %rcx
                    //    movq    (%rcx), %rdi
                    void *endereco = params[i].valor.v_ptr;

                    // vou sempre usar o registrador %rcx como temporario logo tenho q botar esses bytes no array codigo
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0xb9;
                    pos_no_array++;

                    // agora vou botar o endereco nos proximos 8 bytes
                    union
                    {
                        void *endereco;
                        unsigned char endereco_separado_em_8_bytes[8];
                    } a;
                    a.endereco = endereco;
                    for (int j = 0; j < 8; j++)
                    {
                        codigo[pos_no_array] = a.endereco_separado_em_8_bytes[j];
                        pos_no_array++;
                    }
                    // agora tenho q passar o conteudo de %rcx para %rdi, ou seja,
                    // movq (%rcx), %rdi
                    // codigo de maquina
                    // 48 8b 39                mov    (%rcx),%rdi
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x8b;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x39;
                    pos_no_array++;
                }
            }
            // PARAMETRO EH UM PONTEIRO
            else if (params[i].tipo_val == PTR_PAR)
            {
                if (params[i].orig_val == FIX)
                {
                    // primeiro passo o endereco para %rcx, depois passo o conteudo de %rcx para %rdi
                    void *endereco = params[i].valor.v_ptr;

                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0xb9;
                    pos_no_array++;

                    union
                    {
                        void *endereco;
                        unsigned char endereco_separado_em_8_bytes[8];
                    } a;
                    a.endereco = endereco;
                    for (int j = 0; j < 8; j++)
                    {
                        codigo[pos_no_array] = a.endereco_separado_em_8_bytes[j];
                        pos_no_array++;
                    }
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x8b;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x39;
                    pos_no_array++;
                }
                else if (params[i].orig_val == PARAM)
                {
                    // passar o paremetro em %rdi para %rdi
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x8b;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x3f;
                    pos_no_array++;

                    param_count--;
                }
                else if (params[i].orig_val == IND)
                {
                    // passo o endereco para %rcx, depois passo o conteudo de %rcx para %rcx e, por fim, passo seu conteudo para %rdi
                    //movq    $0x7ffcfa055f64, %rcx     (0x7ffcfa055f64 == exemplo de endereco)
                    //movq    (%rcx), %rcx
                    //movq    (%rcx), %rdi
                    void *endereco = params[i].valor.v_ptr;
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0xb9;
                    pos_no_array++;

                    union
                    {
                        void *endereco;
                        unsigned char endereco_separado_em_8_bytes[8];
                    } b;

                    b.endereco = endereco;

                    for (int j = 0; j < 8; j++)
                    {
                        codigo[pos_no_array] = b.endereco_separado_em_8_bytes[j];
                        pos_no_array++;
                    }
                    // movq (%rcx), %rcx
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x8b;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x09;
                    pos_no_array++;

                    // movq (%rcx), %rdi
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x8b;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x39;
                    pos_no_array++;
                }
            }
        }
        else if (i == 1)
        {
            if (params[i].tipo_val == INT_PAR)
            {
                // parametro INTEIRO

                if (params[i].orig_val == FIX)
                {
                    // parametro fixo, vou passar o valor localizado em params[i].valor.v_int para %rsi
                    // 0x48 0xc6 0xc6 eh o comando de movq para %rsi,
                    // seguido do numero "fixo" em hexadecimal nos 4 bytes seguintes

                    int fixo = params[i].valor.v_int;

                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0xc7;
                    pos_no_array++;
                    codigo[pos_no_array] = 0xc6;
                    pos_no_array++;

                    union
                    {
                        int numero;
                        char numero_separado_em_4_bytes[4];
                    } y;
                    y.numero = fixo;

                    for (int j = 0; j < 4; j++)
                    {
                        codigo[pos_no_array] = y.numero_separado_em_4_bytes[j];
                        pos_no_array++;
                    }
                }
                if (params[i].orig_val == PARAM)
                {
                    // aqui temos que verificar o contador de parametros, ele vai nos dizer em que registrador
                    //o parametro que esta sendo passado se encontra

                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x89;
                    pos_no_array++;

                    if (param_count == 1)
                        // parametro estara em %rdi
                        codigo[pos_no_array] = 0xfe;
                    else if (param_count == 2)
                        // parametro estara em %rsi
                        codigo[pos_no_array] = 0xf6;
                    pos_no_array++;
                    param_count--;
                }
                if (params[i].orig_val == IND)
                {
                    // parametro indireto, vou receber o endereco da variavel. tenho que passar esse endereco
                    // para um registrador qualquer e depois passar seu conteudo para %rsi
                    // ex:
                    //    movq    $0xffffffffffffffff, %rcx => 48 b9 endereco em bytes
                    //    movq    (%rcx), %rsi
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0xb9;
                    pos_no_array++;

                    void *endereco = params[i].valor.v_ptr;

                    union
                    {
                        void *endereco;
                        unsigned char endereco_separado_em_8_bytes[8];
                    } b;

                    b.endereco = endereco;

                    for (int j = 0; j < 8; j++)
                    {
                        codigo[pos_no_array] = b.endereco_separado_em_8_bytes[j];
                        pos_no_array++;
                    }
                    // agora tenho q passar o conteudo de %rcx para %rsi, ou seja,
                    // movq (%rcx), %rsi
                    // codigo de maquina
                    // 48 8b 31                mov    (%rcx),%rdi
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x8b;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x31;
                    pos_no_array++;
                }
            }
            // PARAMETRO EH UM PONTEIRO
            else if (params[i].tipo_val == PTR_PAR)
            {
                if (params[i].orig_val == FIX)
                {
                    // parametro indireto, vou receber o endereco da variavel. tenho que passar esse endereco
                    // para um registrador qualquer e depois passar seu conteudo para %rsi
                    // ex:
                    //    movq    $0xffffffffffffffff, %rcx => 48 b9 endereco em bytes
                    //    movq    (%rcx), %rsi
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0xb9;
                    pos_no_array++;

                    void *endereco = params[i].valor.v_ptr;

                    union
                    {
                        void *endereco;
                        unsigned char endereco_separado_em_8_bytes[8];
                    } b;

                    b.endereco = endereco;

                    for (int j = 0; j < 8; j++)
                    {
                        codigo[pos_no_array] = b.endereco_separado_em_8_bytes[j];
                        pos_no_array++;
                    }
                    // agora tenho q passar o conteudo de %rcx para %rsi, ou seja,
                    // movq (%rcx), %rsi
                    // codigo de maquina
                    // 48 8b 31                mov    (%rcx),%rdi
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x8b;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x31;
                    pos_no_array++;
                }
                else if (params[i].orig_val == PARAM)
                {
                    // aqui temos que verificar o contador de parametros, ele vai nos dizer em que registrador
                    //o parametro que esta sendo passado se encontra

                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x8b;
                    pos_no_array++;

                    if (param_count == 1)
                        // parametro estara em %rdi
                        codigo[pos_no_array] = 0x37;
                    else if (param_count == 2)
                        // parametro estara em %rsi
                        codigo[pos_no_array] = 0x36;
                    pos_no_array++;
                    param_count--;
                }
                else if (params[i].orig_val == IND)
                {
                    //movq    $0x7ffcfa055f64, %rcx     (0x7ffcfa055f64 == exemplo de endereco)
                    //movq    (%rcx), %rcx
                    //movq    (%rcx), %rsi
                    void *endereco = params[i].valor.v_ptr;
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0xb9;
                    pos_no_array++;

                    union
                    {
                        void *endereco;
                        unsigned char endereco_separado_em_8_bytes[8];
                    } b;

                    b.endereco = endereco;

                    for (int j = 0; j < 8; j++)
                    {
                        codigo[pos_no_array] = b.endereco_separado_em_8_bytes[j];
                        pos_no_array++;
                    }
                    // movq (%rcx), %rcx
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x8b;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x09;
                    pos_no_array++;

                    // movq (%rcx), %rsi
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x8b;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x31;
                    pos_no_array++;
                }
            }
        }
        // i == 2
        else if (i == 2)
        {
            if (params[i].tipo_val == INT_PAR)
            { // parametro INTEIRO
                if (params[i].orig_val == FIX)
                {
                    // parametro fixo, vou passar o valor localizado em params[i].valor.v_int para %rdx
                    int fixo = params[i].valor.v_int;

                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0xc7;
                    pos_no_array++;
                    codigo[pos_no_array] = 0xc2;
                    pos_no_array++;

                    union
                    {
                        int numero;
                        char numero_separado_em_4_bytes[4];
                    } z;
                    z.numero = fixo;

                    for (int j = 0; j < 4; j++)
                    {
                        codigo[pos_no_array] = z.numero_separado_em_4_bytes[j];
                        pos_no_array++;
                    }
                }
                if (params[i].orig_val == PARAM)
                {
                    // aqui temos que verificar o contador de parametros, ele vai nos dizer em que registrador
                    //o parametro que esta sendo passado se encontra
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x89;
                    pos_no_array++;

                    if (param_count == 1)
                        // parametro estara em %rdi
                        codigo[pos_no_array] = 0xfa;
                    else if (param_count == 2)
                        // parametro estara em %rsi
                        codigo[pos_no_array] = 0xf2;
                    else
                        // (param_count == 3)
                        // parametro estara em %rdx
                        codigo[pos_no_array] = 0xd2;
                    pos_no_array++;
                }
                if (params[i].orig_val == IND)
                {
                    // parametro indireto, vou receber o endereco da variavel. tenho que passar esse endereco
                    // para um registrador qualquer e depois passar seu conteudo para %rdx
                    // ex:
                    //    movq    $0xffffffffffffffff, %rcx => 48 b9 endereco em bytes
                    //    movq    (%rcx), %rdx
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0xb9;
                    pos_no_array++;

                    void *endereco = params[i].valor.v_ptr;

                    union
                    {
                        void *endereco;
                        unsigned char endereco_separado_em_8_bytes[8];
                    } c;

                    c.endereco = endereco;

                    for (int j = 0; j < 8; j++)
                    {
                        codigo[pos_no_array] = c.endereco_separado_em_8_bytes[j];
                        pos_no_array++;
                    }
                    // agora tenho q passar o conteudo de %rcx para %rsi, ou seja,
                    // movq (%rcx), %rsi
                    // codigo de maquina
                    // 48 8b 11                mov    (%rcx),%rdx
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x8b;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x11;
                    pos_no_array++;
                }
            }
            // PARAMETRO EH UM PONTEIRO
            else if (params[i].tipo_val == PTR_PAR)
            {
                if (params[i].orig_val == FIX)
                {
                    // parametro indireto, vou receber o endereco da variavel. tenho que passar esse endereco
                    // para um registrador qualquer e depois passar seu conteudo para %rdx
                    // ex:
                    //    movq    $0xffffffffffffffff, %rcx => 48 b9 endereco em bytes
                    //    movq    (%rcx), %rdx
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0xb9;
                    pos_no_array++;

                    void *endereco = params[i].valor.v_ptr;

                    union
                    {
                        void *endereco;
                        unsigned char endereco_separado_em_8_bytes[8];
                    } c;

                    c.endereco = endereco;

                    for (int j = 0; j < 8; j++)
                    {
                        codigo[pos_no_array] = c.endereco_separado_em_8_bytes[j];
                        pos_no_array++;
                    }
                    // agora tenho q passar o conteudo de %rcx para %rsi, ou seja,
                    // movq (%rcx), %rsi
                    // codigo de maquina
                    // 48 8b 11                mov    (%rcx),%rdx
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x8b;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x11;
                    pos_no_array++;
                }
                else if (params[i].orig_val == PARAM)
                {
                    // aqui temos que verificar o contador de parametros, ele vai nos dizer em que registrador
                    //o parametro que esta sendo passado se encontra
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x8b;
                    pos_no_array++;

                    if (param_count == 1)
                    {
                        // parametro estara em %rdi
                        codigo[pos_no_array] = 0x17;
                    }
                    else if (param_count == 2)
                    {
                        // parametro estara em %rsi
                        codigo[pos_no_array] = 0x16;
                    }
                    else
                    { // (param_count == 3)
                        // parametro estara em %rdx
                        codigo[pos_no_array] = 0x12;
                    }
                    pos_no_array++;
                    param_count--;
                }
                else if (params[i].orig_val == IND)
                {
                    //movq    $0x7ffcfa055f64, %rcx     (0x7ffcfa055f64 == exemplo de endereco)
                    //movq    (%rcx), %rcx
                    //movq    (%rcx), %rdx
                    void *endereco = params[i].valor.v_ptr;
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0xb9;
                    pos_no_array++;

                    union
                    {
                        void *endereco;
                        unsigned char endereco_separado_em_8_bytes[8];
                    } b;

                    b.endereco = endereco;

                    for (int j = 0; j < 8; j++)
                    {
                        codigo[pos_no_array] = b.endereco_separado_em_8_bytes[j];
                        pos_no_array++;
                    }
                    // movq (%rcx), %rcx
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x8b;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x09;
                    pos_no_array++;

                    // movq (%rcx), %rdx
                    codigo[pos_no_array] = 0x48;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x8b;
                    pos_no_array++;
                    codigo[pos_no_array] = 0x11;
                    pos_no_array++;
                }
            }
        }
    }

    // agora ja temos os parametros no array codigo, precisamos fazer
    // um call para a funcao que queremos usar (lembrar lab13)
    // depois da instrucao 0xe8 temos que botar a distancia entre o endereco
    // da funcao que o call chama e a instrucao seguinte
    codigo[pos_no_array] = 0xe8; // call
    pos_no_array++;

    unsigned char distancia[4];
    long distancia_long = (long)f - ((long)codigo + pos_no_array);
    int distancia_int = (int)distancia_long;

    union
    {
        int i;
        char c[4];
    } u;

    u.i = distancia_int;

    distancia[0] = u.c[0];
    distancia[1] = u.c[1];
    distancia[2] = u.c[2];
    distancia[3] = u.c[3];

    codigo[pos_no_array] = distancia[0];
    pos_no_array++;
    codigo[pos_no_array] = distancia[1];
    pos_no_array++;
    codigo[pos_no_array] = distancia[2];
    pos_no_array++;
    codigo[pos_no_array] = distancia[3];
    pos_no_array++;

    // botando leave e ret no final do array codigo
    codigo[pos_no_array] = 0xc9;
    pos_no_array++;
    codigo[pos_no_array] = 0xc3;

    return codigo;
}

void libera_func(void *func)
{
    free(func);
}
