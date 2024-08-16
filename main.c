#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_WITH_BYTES(bytes)                                                  \
  typedef uint8_t bit_t;                                                       \
  typedef bit_t number_t[bytes * 8];                                           \
  const int BITS = bytes * 8;

#ifdef DEBUG
NUM_WITH_BYTES(1)
#else
NUM_WITH_BYTES(4)
#endif

int read_num(const char *ch, number_t *num);
char *to_string(number_t num);
void print_num_dec(number_t num);
void neg(number_t a, number_t res);
void add(number_t a, number_t b, number_t res);
void sub(number_t a, number_t b, number_t res);
void mul(number_t a, number_t b, number_t res);
void divi(number_t a, number_t b, number_t res);
void mod(number_t a, number_t b, number_t res);

int main() {
  number_t a, b, c;
  char INP_FMT[5];
  sprintf(INP_FMT, "%%%ds", BITS);
  char ch_a[9], ch_b[9], op[2];
  printf("Input format:  A op B\n"
         "A, B: %d digits binary number\n"
         "op: + - * /\n\nq to quit\n",
         BITS);

  char *str_a;
  char *str_b;
  char *str_c;
  do {
    fflush(stdin);
    printf("> ");
    if (scanf(INP_FMT, ch_a) == EOF) return 0;
    if (strcmp(ch_a, "q")==0) return 0;
    if (read_num(ch_a, &a)) {
      continue;
    }
    scanf("%1s", op);
    scanf(INP_FMT, ch_b);
    if (read_num(ch_b, &b)) {
      continue;
    }

    switch (op[0]) {
    case '+':
      add(a, b, c);
      break;
    case '-':
      sub(a, b, c);
      break;
    case '*':
    case '/':
      printf("Operator %s is not implemented\n", op);
      continue;
    default:
      printf("Invalid operator %s\n", op);
      continue;
    }

    str_a = to_string(a);
    str_b = to_string(b);
    str_c = to_string(c);

    printf(" %s\n%s\n %s\n=%*s\n %s\n", str_a, op, str_b, BITS, "--------", str_c);
    number_t na;
  } while (1);
  free(str_a);
  free(str_b);
  free(str_c);
  return 0;
}

int read_num(const char *ch, number_t *num) {
  if (strlen(ch) != BITS) {
    printf("Input binary number with %d digits\n", BITS);
    return 1;
  }
  for (int i = 0; i < BITS; ++i) {
    (*num)[i] = ch[i] - '0';
    if ((*num)[i] > 1) {
      printf("Number contains invalid digit %c\n", ch[i]);
      return 1;
    }
  }
  return 0;
}

char *to_string(number_t num) {
  char *res = malloc(BITS + 1);
  for (int i = 0; i < BITS; ++i) {
    res[i] = num[i] + '0';
  }
  res[BITS] = '\0';
  return res;
}

void print_num_dec(number_t num) { return; }

void add(number_t a, number_t b, number_t res) {
  int mem = 0;
  for (int i = BITS - 1; i >= 0; --i) {
    res[i] = a[i] ^ b[i] ^ mem;
    mem = ((mem & (a[i] ^ b[i])) | (a[i] & b[i]));
  }
};
void sub(number_t a, number_t b, number_t res) {
  number_t neg_b;
  neg(b, neg_b);
  add(a, neg_b, res);
};

void mul(number_t a, number_t b, number_t res) {

};
void divi(number_t a, number_t b, number_t res) {

};
void mod(number_t a, number_t b, number_t res) {};
void neg(number_t a, number_t res) {
  number_t temp, one;
  memcpy(temp, a, sizeof(number_t));
  for (int i = 0; i < BITS; ++i) {
    temp[i] = a[i] ^ 1;
    one[i] = 0;
  }
  one[BITS - 1] = 1;
  add(temp, one, res);
}
