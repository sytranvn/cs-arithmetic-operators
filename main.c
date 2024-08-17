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
char *to_dec_string(number_t num);
void print_dec(number_t a, char *op, number_t b, number_t c);
void neg(number_t a, number_t res);
bit_t add(number_t a, number_t b, number_t res);
bit_t sub(number_t a, number_t b, number_t res);
void mul(number_t a, number_t b, number_t res);
void divmod(number_t a, number_t b, number_t res, number_t mod);
void mod(number_t a, number_t b, number_t res);
void rsh(number_t a);
void lsh(number_t a);

int main() {
  number_t a, b, c;
  char INP_FMT[6];
  sprintf(INP_FMT, "%%%ds", BITS);
  char ch_a[BITS + 1], ch_b[BITS + 1], op[2];
  fprintf(stderr,
          "Input format:  A op B\n"
          "A, B: %d digits binary number\n"
          "op: + - * / %%\n\n"
          "d to print result in decimal\n"
          "q to quit\n",
          BITS);

  char *str_a;
  char *str_b;
  char *str_c;
  do {
    fflush(stdin);
    printf("> ");
    if (scanf(INP_FMT, ch_a) == EOF)
      return 0;
    if (strcmp(ch_a, "q") == 0)
      return 0;
    if (strcmp(ch_a, "d") == 0) {
      print_dec(a, op, b, c);
      continue;
    }
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
      mul(a, b, c);
      break;
    case '/':
      divmod(a, b, c, NULL);
      break;
    case '%':
      divmod(a, b, NULL, c);
      break;
    default:
      fprintf(stderr, "Invalid operator %s\n", op);
      continue;
    }

    str_a = to_string(a);
    str_b = to_string(b);
    str_c = to_string(c);

    printf(" %s\n%s\n %s\n=%*s\n %s\n", str_a, op, str_b, BITS + (BITS - 1) / 8,
           "────────", str_c);
  } while (1);
  free(str_a);
  free(str_b);
  free(str_c);
  return 0;
}

int read_num(const char *ch, number_t *num) {
  if (strlen(ch) != BITS) {
    fprintf(stderr, "%s has %ld digits. Input binary number with %d digits\n", ch,
           strlen(ch), BITS);
    return 1;
  }
  for (int i = 0; i < BITS; ++i) {
    (*num)[i] = ch[i] - '0';
    if ((*num)[i] > 1) {
      fprintf(stderr, "Number '%s' contains invalid digit '%c'\n", ch, ch[i]);
      return 1;
    }
  }
  return 0;
}

char *to_string(number_t num) {
  char *res = malloc(BITS + (BITS - 1) / 8 + 1);
  for (int i = 0, j = 0; i < BITS; ++i) {
    res[j++] = num[i] + '0';
    if (i % 8 == 7)
      res[j++] = ' ';
  }
  res[BITS + BITS / 8] = '\0';
  return res;
}
void print_dec(number_t a, char *op, number_t b, number_t c) {
  char *str_a, *str_b, *str_c;
  str_a = to_dec_string(a);
  str_b = to_dec_string(b);
  str_c = to_dec_string(c);
  printf("%s %s %s = %s\n", str_a, op, str_b, str_c);
  free(str_a);
  free(str_b);
  free(str_c);
}
char *to_dec_string(number_t num) {
  char *res = calloc(11, 1);
  int j = 0;
  number_t temp;
  if (num[0]) {
    res[j++] = '-';
    neg(num, temp);
  } else {
    memcpy(temp, num, sizeof(number_t));
  }
  int32_t val = 0;
  for (int i = 0; i < BITS; ++i) {
    val += temp[i] << (BITS - i - 1);
  }
  sprintf(res + j, "%d", val);
  return res;
}
/**
 * res = a + b
 * return carry bit
 */
bit_t add(number_t a, number_t b, number_t res) {
  bit_t carry = 0;
  bit_t ai, bi;
  for (int i = BITS - 1; i >= 0; --i) {
    ai = a[i];
    bi = b[i]; // keep original bits incase res is a or b
    res[i] = ai ^ bi ^ carry;
    carry = ((carry & (ai ^ bi)) | (ai & bi));
  }
  return carry;
};
bit_t sub(number_t a, number_t b, number_t res) {
  number_t neg_b;
  neg(b, neg_b);
  return add(a, neg_b, res);
};

/**
 * res = a * b
 *
 * overflow ignored
 */
void mul(number_t a, number_t b, number_t res) {
  number_t M, Q, A;
  memcpy(M, a, sizeof(M));
  memcpy(Q, b, sizeof(Q));
  memset(A, 0, sizeof(number_t));
  bit_t C = 0;
  bit_t sign = a[0] ^ b[0];
  if (M[0]) {
    neg(M, M);
  }
  if (Q[0]) {
    neg(Q, Q);
  }
  int k = BITS;
  while (k--) {
    if (Q[BITS - 1]) {
      C = add(A, M, A);
    }
    rsh(Q);
    Q[0] = A[BITS - 1];
    rsh(A);
    A[0] = C;
    C = 0;
  }
  if (sign)
    neg(Q, Q);
  memcpy(res, Q, sizeof(Q));
};
void divmod(number_t a, number_t b, number_t res, number_t mod) {
  number_t Q, M, A;
  memcpy(Q, a, sizeof(Q));
  memcpy(M, b, sizeof(M));
  bit_t sign = a[0] ^ b[0];
  memset(A, 0, sizeof(A));
  if (Q[0])
    neg(Q, Q);
  if (M[0])
    neg(M, M);
  int k = BITS;
  while (k--) {
    lsh(A);
    A[BITS - 1] = Q[0];
    lsh(Q);
    sub(A, M, A);
    if (A[0]) {
      add(A, M, A);
      Q[BITS - 1] = 0;
    } else {
      Q[BITS - 1] = 1;
    }
  }
  if (sign) {
    neg(Q, Q);
    neg(A, A);
  }
  if (res != NULL)
    memcpy(res, Q, sizeof(Q));
  if (mod != NULL)
    memcpy(mod, A, sizeof(A));
};
void mod(number_t a, number_t b, number_t res) {};
void neg(number_t a, number_t res) {
  number_t temp, one;
  memcpy(temp, a, sizeof(temp));
  for (int i = 0; i < BITS; ++i) {
    temp[i] = a[i] ^ 1;
    one[i] = 0;
  }
  one[BITS - 1] = 1;
  add(temp, one, res);
}

void rsh(number_t a) {
  for (int i = BITS - 1; i > 0; --i) {
    a[i] = a[i - 1];
  }
  a[0] = 0;
}

void lsh(number_t a) {
  for (int i = 0; i < BITS - 1; ++i) {
    a[i] = a[i + 1];
  }
  a[BITS - 1] = 0;
}
