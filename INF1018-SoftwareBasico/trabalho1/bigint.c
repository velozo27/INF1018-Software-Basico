/*Nome: Luiza Bretas J Corrêa Luiz    Matrícula: 1911867     Turma: 3WA*/
/*Nome: Pedro Antonio Tibau Velozo    Matrícula: 1812013     Turma: 3WA*/

#include <stdio.h>
#include "bigint.h"

/* Atribuicao */

/* res = val (extensao com sinal) */
void big_val(BigInt res, long val)
{

	/*8 primeiros elementos de res sao os bytes de val*/
	for (int i = 0; i < sizeof(val); i++)
	{
		res[i] = (val >> 8 * i) & 0xFF;
	}

	/*pegando o primeiro bit*/
	int first_bit = val >> (sizeof(val) * 8 - 1);

	/*se primeiro bit == 0, expando val com 0's, se nao,
	  expando com F's*/
	if (first_bit)
	{
		for (int i = 8; i < 16; i++)
			res[i] = 0xFF;
	}
	else
	{
		for (int i = 8; i < 16; i++)
			res[i] = 0x0;
	}
}

/* Operacoes aritmeticas */

/* res = -a */
void big_comp2(BigInt res, BigInt a)
{
	/*
	ideia: seguir algoritmo do complemento a 2,
	inverter os bits e somar 1
	*/

	/*criando e convertendo um long de valor 1 para o tipo BigInt (para poder somar 1, como no algoritmo do comp a 2)*/
	long soma_1_long = 1;
	BigInt soma_1_BigInt;
	big_val(soma_1_BigInt, soma_1_long);

	/*invertendo*/
	for (int i = 0; i < 16; i++)
	{
		res[i] = ~a[i];
	}

	/*somando 1*/
	big_sum(res, res, soma_1_BigInt);
}

/* res = a + b */
void big_sum(BigInt res, BigInt a, BigInt b)
{
	/*ideia: somar byte a byte 'a' e 'b', tendo cuidado com o 'carry'*/

	int carry = 0;

	for (int i = 0; i < 16; i++)
	{
		//res[i] = a[i] + b[i] + carry;

		/*vendo se o ultimo bit de a[i] e b[i] sao 1, se sim,
		temos que carregar o 1 (pq 1+1 carrega o 1 para a proxima operacao)*/
		if ((a[i] & 1) == 1 && (b[i] & 1) == 1)
			carry = 1;
		else
			carry = 0;

		res[i] = (a[i] + b[i]) + carry;
	}
}

/* res = a - b */
void big_sub(BigInt res, BigInt a, BigInt b)
{
	/*
	ideia: fazer complemento a 2 de b, depois somar com a,
	ja que a - b == a + (-b)
	*/
	BigInt b_comp2;
	big_comp2(b_comp2, b);

	big_sum(res, a, b_comp2);
}

/* res = a * b */
void big_mul(BigInt res, BigInt a, BigInt b)
{
	/*tentamos implementar o algoritmo da multiplicacao de Booth*/
	BigInt produto;
	for (int i = 0; i < 8; i++)
		produto[i] = 0;
	for (int i = 8; i < 16; i++)
		produto[i] = a[i];

	char bit_shiftado = 0;

	int num_iteracoes = 0;
	while (num_iteracoes < 8)
	{
		if ((produto[15] & 1) == 1 && bit_shiftado == 0)
		{
			bit_shiftado = (produto[15] & 1);
			big_sub(produto, produto, b);
			big_sar(produto, produto, 1);
		}
		else if ((produto[15] & 1) == 0 && bit_shiftado == 1)
		{
			bit_shiftado = (produto[15] & 1);
			big_sum(produto, produto, b);
			big_sar(produto, produto, 1);
		}
		num_iteracoes++;
	}
	res = produto;
}

/* Operacoes de deslocamento */

/* res = a << n */
void big_shl(BigInt res, BigInt a, int n)
{
	/* n shift para esquerda == multiplicar o valor original por 2^n */
	long dois_elevado_n = 1;
	for (int i = 1; i <= n; i++)
		dois_elevado_n = 2 * dois_elevado_n;

	BigInt BigInt_dois_elevado_n;

	big_val(BigInt_dois_elevado_n, dois_elevado_n);

	big_mul(res, a, BigInt_dois_elevado_n);
}

/* res = a >> n (logico) */
void big_shr(BigInt res, BigInt a, int n)
{
	/*
	ex: a = {A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P}
	a >> 3 = {0, 0, 0, A, B, C, D, E, F, G, H, I, J, K, L, M}
	*/
	int n_em_bytes = n / 8;
	for (int i = 0; i < 16; i++)
	{
		/*botar 0 nos n's primeiros bytes de res*/
		if (i < n_em_bytes)
		{
			res[i] = 0;
		}
		else
		{
			/*botar a[i - n_em_bytes] nas demais posicoes*/
			res[i] = a[i - n_em_bytes];
		}
	}
}

/* res = a >> n (aritmetico) */
void big_sar(BigInt res, BigInt a, int n)
{
	/*logica muito similar ao big_shr*/
	/*pegando 1o bit de a, se for 1 boto 1 nos bits shiftados, se for 0 boto 0 nos bits shiftados*/
	int first_bit = a[0] >> 7;
	int n_em_bytes = n / 8;

	for (int i = 0; i < 16; i++)
	{
		/*botar 0 nos n's primeiros bytes de res se o bit mais significatvio for 0*/
		if (i < n_em_bytes && first_bit == 0)
		{
			res[i] = 0;
		}
		/*botar 1 nos n's primeiros bytes de res se o bit mais significativo for 1*/
		else if (i < n_em_bytes && first_bit == 1)
		{
			res[i] = 0xFF;
		}
		else
		{
			/*botar a[i - n_em_bytes] nas demais posicoes*/
			res[i] = a[i - n_em_bytes];
		}
	}
}
