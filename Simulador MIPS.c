/* Simulador MIPS: Checkpoint 1
Felipe Francisco Bonilho Lorusso,
Luis Henrique Ferracciu Pagotto Mendes,
Thiago de Melo Tieghi
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <locale.h>

#define SIZE 256

uint32_t r[32];
uint32_t ram[SIZE];
uint32_t operacao, registrador_fonte, registrador_temporario, registrador_destino, quantidade_deslocada, funct, imediato, endereco;

uint32_t instrucao_codigo, contador;

int main(int argc, const char *argv[])
{

  setlocale(LC_ALL, "");

  FILE *arq;
  int numero_Bits, aux;
  uint32_t instrucao;

  arq = fopen(argv[1], "r + b");

  if (argc != 2)
  {
    printf("Número de parametros invalido!\n\n");
    exit(1);
  } // if

  if (arq == NULL)
  {
    printf("Problemas ao abrir arquivo\n");
    exit(1);
  } // if

  numero_Bits = fread(ram, 4, 100, arq);

  for (int i = 0; i < numero_Bits; i++)
  {

    instrucao_codigo = (uint32_t)ram[i];
    instrucao = ((instrucao_codigo & 0xFF000000) >> 24 | (instrucao_codigo & 0x00FF0000) >> 8 | (instrucao_codigo & 0x0000FF00) << 8 | (instrucao_codigo & 0x000000FF) << 24);
    instrucao_codigo = instrucao;
    // Recebe o OPCODE da instrução MIPS
    operacao = (instrucao_codigo & 0xfc000000) >> 26;
    // Recebe os registradores das instruções
    registrador_fonte = (instrucao_codigo & 0x3E00000) >> 21;
    registrador_temporario = (instrucao_codigo & 0x1F0000) >> 16;
    registrador_destino = (instrucao_codigo & 0xF800) >> 11;
    // Recebe o SHAMT e o Funct, são necessários em instruções do tipo R
    quantidade_deslocada = (instrucao_codigo & 0x7C0) >> 6;
    funct = (instrucao_codigo & 0x3F);
    // Recebe o Imediato, necessário em instruções do tipo I
    imediato = (instrucao_codigo & 0xffff);
    // Recebe o endereço, para fazer o salto
    endereco = (instrucao_codigo & 0x3FFFFFF);

    switch (operacao)
    {

    case 0x0:
      switch (funct)
      {

      case 0x20: // ADD
        r[registrador_destino] = r[registrador_temporario] + r[registrador_fonte];
        break;

      case 0x22: // SUB
        r[registrador_destino] = r[registrador_temporario] - r[registrador_fonte];
        break;

      case 0x18: // MULT
        r[registrador_destino] = r[registrador_temporario] * r[registrador_fonte];
        break;

      case 0x1a: // DIV
        r[registrador_destino] = r[registrador_temporario] / r[registrador_fonte];
        break;

      case 0x24: // AND
        r[registrador_destino] = (r[registrador_temporario] & r[registrador_fonte]);
        break;

      case 0x25: // OR
        r[registrador_destino] = (r[registrador_temporario] | r[registrador_fonte]);
        break;

      case 0x26: // XOR
        r[registrador_destino] = (r[registrador_temporario] ^ r[registrador_fonte]);
        break;

      case 0x27: // NOR
        r[registrador_destino] = ~(r[registrador_temporario] | r[registrador_fonte]);
        break;

      case 0x2a: // SLT
        if (r[registrador_temporario] < r[registrador_fonte])
        {
          r[registrador_destino] = 0x01;
        }
        else
        {
          r[registrador_destino] = 0x00;
        } // else
        break;

      case 0x00: // SLL
        r[registrador_destino] = r[registrador_temporario] << quantidade_deslocada;
        break;

      case 0x02: // SRL
        r[registrador_destino] = (r[registrador_temporario]) >> quantidade_deslocada;
        break;

      case 0x03: // SRA
        aux = r[registrador_temporario];
        if (aux < 0)
        {
          r[registrador_destino] = r[registrador_temporario] >> quantidade_deslocada | ~(~0U >> quantidade_deslocada);
        }
        else
        {
          r[registrador_destino] = r[registrador_temporario] >> quantidade_deslocada;
        } // else
        break;

      case 0x08: // JR
        contador = r[31];
        break;

      case 0x0c: // SYSCALL
        switch (r[registrador_destino])
        {
        case 0x01:
          printf("%d\n", r[4]);
          break;

        case 0x0a:
          printf("Fechando o Programa!\n");
          exit(0);
          break;
        } // switch interno
        break;

      default:
        printf("funcao Invalida\n");
        break;
      } // switch externo
      break;

    case 0x02: // OPCODE 2 J
      contador = (endereco - 1);
      break;

    case 0x03: // OPCODE 3 JAL
      r[31] = contador;
      contador = (endereco - 1);
      break;

    case 0x04: // OPCODE 4 BEQ
      if (r[registrador_temporario] == r[registrador_fonte])
      {
        contador = contador + (imediato - 1);
      } // if
      break;

    case 0x05: // OPCODE 5 BNE
      if (r[registrador_temporario] != r[registrador_fonte])
      {
        contador = contador + (imediato - 1);
      } // if
      break;

    case 0x07: // OPCODE 7 BGTZ
      if (r[registrador_fonte] > 0)
      {
        contador = contador + (imediato - 1);
      } // if
      break;

    case 0x08: // OPCODE 8 ADDI
      r[registrador_fonte] = r[registrador_temporario] + imediato;
      break;

    case 0x0a: // OPCODE 10 SLTI
      if (r[registrador_temporario] < imediato)
      {
        r[registrador_fonte] = 0x01;
      }
      else
      {
        r[registrador_fonte] = 0x00;
      } // else
      break;

    case 0x0c: // OPCODE 12 ANDI
      r[registrador_fonte] = (r[registrador_temporario] & imediato);
      break;

    case 0x0d: // OPCODE 13 ORI
      r[registrador_fonte] = (r[registrador_temporario] | imediato);
      break;

    case 0x0e: // OPCODE 14 XORI
      r[registrador_fonte] = (r[registrador_temporario] ^ imediato);
      break;

    case 0x23: // OPCODE LW

      r[registrador_fonte] = SIZE;
      imediato = imediato * (-1);

      if (r[registrador_fonte] + imediato >= r[registrador_fonte] || r[registrador_fonte] + imediato <= numero_Bits)
      {
        printf("Stack Overflow!\n");
      }
      else
      {
        r[registrador_temporario] = ram[r[registrador_fonte] + imediato];
      } // else
      break;

    case 0x2b: // OPCODE SW
      r[registrador_fonte] = SIZE;
      imediato = imediato * (-1);

      if (r[registrador_fonte] + imediato >= r[registrador_fonte] || r[registrador_fonte] + imediato <= numero_Bits)
      {
        printf("Stack Overflow!\n");
      }
      else
      {
        ram[r[registrador_fonte] + imediato] = r[registrador_temporario];
      } // else
      break;

    default:
      printf("OPCODE invalido\n");
      break;
    } // switch externo
  }   // for
  return 0;
} // main
