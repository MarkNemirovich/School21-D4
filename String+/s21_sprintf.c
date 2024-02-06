//#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "s21_string.h"
#include <math.h>

#define s21_NULL ((void *)0)
#define S21_TEXTMAX 2048

typedef long unsigned s21_size_t;

typedef struct specificator {
  int minus, plus, space, point;
} specificator;

s21_size_t get_num(char **format) {
  s21_size_t num = 0;
  int power = 1;
  for (; **format >= '0' && **format <= '9'; (*format)++) {
    num *= power;
    num += ((**format) - '0');
    power *= 10;
  }
  return num;
}

void modificate_specificator(char **format, specificator *specificators,
                             int *padding, int *accuracy) {
  if (**format == '-') {
    specificators->minus = 1;
    (*format)++;
  } else if (**format == '+') {
    specificators->plus = 1;
    (*format)++;
  } else if (**format == ' ') {
    specificators->space = 1;
    (*format)++;
  }
  if (**format >= '0' && **format <= '9') {
    *padding = get_num(format);
  }
  if (**format == '.') {
    specificators->point = 1;
    (*format)++;
    *accuracy = get_num(format);
  }
}

void add_spaces(char *str, int *i, int padding) {
  while (padding > 0) {
    str[*i] = ' ';
    (*i)++;
    padding--;
  }
}

void print_number(char *str, int *i, char *number, int length, specificator specificators) {  
  if (specificators.plus) {
    str[*i] = '+';
    (*i)++;
  } else if (specificators.space) {
    str[*i] = ' ';
    (*i)++;
  }
  for (; length > 0; (*i)++, number++, length--) {
    str[*i] = *number;
  }
}

int c_specific(char *str, int *i, char symbol, specificator specificators,
               int padding) {
  if (specificators.minus) {
    str[*i] = symbol;
    (*i)++;
    add_spaces(str, i, padding - 1);
  } else {
    add_spaces(str, i, padding - 1);
    str[*i] = symbol;
    (*i)++;
  }
  return padding - specificators.minus - 1;
}

// Reverses a string 'str' of length 'len'
void reverse(char *str, int len) {
  int temp;
  for (int i = 0, j = len - 1; i < j; i++, j--) {
    temp = str[i];
    str[i] = str[j];
    str[j] = temp;
  }
}

// Converts a given integer x to string str[].
// n is the number of digits required in the output.
// If n is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(int x, char *str, int n) {
  int i = 0;
  while (x) {
    str[i++] = (x % 10) + '0';
    x = x / 10;
  }
  // If number of digits required is more, then
  // add 0s at the beginning
  while (i < n) str[i++] = '0';
  reverse(str, i);
  str[i] = '\0';
  return i;
}

int s21_itoa(int n, char *res, int accuracy) {
  // convert integer to string
  return intToStr(n, res, accuracy);
}

int s21_ftoa(float n, char *res, int accuracy) {
  // Extract integer part
  int ipart = (int)n;
  // Extract floating part
  float fpart = n - (float)ipart;
  // convert integer part to string
  int i = intToStr(ipart, res, 0);
  // check for display option after point
  if (accuracy != 0) {
    res[i] = '.';  // add dot
    i++;
    // Get the value of fraction part upto given no.
    // of points after dot. The third parameter
    // is needed to handle cases like 233.007
    fpart = fpart * pow(10, accuracy);
    i += intToStr((int)fpart, res + i, accuracy);
  }
  return i;
}

int d_specific(char *str, int *i, int number, specificator specificators,
               int padding, int accuracy) {
  char num[S21_TEXTMAX];
  int length = s21_itoa(number, num, accuracy);
  if (specificators.minus) {
    print_number(str, i, num, length, specificators);
    if (length > accuracy)
      add_spaces(str, i,
                 padding - length - specificators.plus - specificators.space);
    else
      add_spaces(str, i,
                 padding - accuracy - specificators.plus - specificators.space);
  } else {
    if (length > accuracy)
      add_spaces(str, i,
                 padding - length - specificators.plus - specificators.space);
    else
      add_spaces(str, i,
                 padding - accuracy - specificators.plus - specificators.space);
    print_number(str, i, num, length, specificators);
  }
  if (accuracy > length) length = accuracy;
  if (padding > length) length = padding;
  return length - specificators.minus;
}

int f_specific(char *str, int *i, float number, specificator specificators,
               int padding, int accuracy) {
  if (!specificators.point) accuracy = 6;
  char num[S21_TEXTMAX];
  int length = s21_ftoa(number, num, accuracy);
  if (specificators.minus) {
    print_number(str, i, num, length, specificators);
    if (length > accuracy)
      add_spaces(str, i, padding - length - specificators.plus - specificators.space);
    else
      add_spaces(str, i, padding - accuracy - specificators.plus - specificators.space);
  } else {
    if (length > accuracy)
      add_spaces(str, i, padding - length - specificators.plus - specificators.space);
    else
      add_spaces(str, i, padding - accuracy - specificators.plus - specificators.space);
    print_number(str, i, num, length, specificators);
  }
  if (accuracy > length) length = accuracy;
  if (padding > length) length = padding;
  return length - specificators.minus;
}

int transform_specificator(char *str, int *i, va_list list, char **format,
                           specificator specificators, int padding,
                           int accuracy) {
  int length_change = 0;
  switch (**format) {
    case 'c':
      length_change =
          c_specific(str, i, va_arg(list, int), specificators, padding);
      (*format)++;
      break;
    case 'd':
      length_change = d_specific(str, i, va_arg(list, int), specificators,
                                 padding, accuracy);
      (*format)++;
      break;
    case 'f':
      length_change = f_specific(str, i, (float)va_arg(list, double),
                                 specificators, padding, accuracy);
      (*format)++;
      break;
    case 's':
      break;
    case 'u':
      break;
    default:
      break;
  }
  return length_change;
}

int s21_sprintf(char *str, const char *format, ...) {
  va_list list;
  va_start(list, format);
  int size = (int)s21_strlen(format);
  int i = 0;
  specificator specificators = {0};
  while (*format != 0 && i < size) {
    if (*format++ == '%') {
      specificators = (specificator){0};
      int padding = 0;
      int accuracy = 0;
      modificate_specificator((char **)&format, &specificators, &padding,
                              &accuracy);
      size += transform_specificator(str, &i, list, (char **)&format,
                                     specificators, padding, accuracy);
    } else {
      str[i++] = *(format - 1);
      str[i] = '\0';
    }
  }
  va_end(list);
  return 0;
}

int main() {
  int number = 123;
  char buffer[50];

  s21_sprintf(buffer, "sum of %12.6d and 20 is 30\n", number);
  printf("%s", buffer);

  sprintf(buffer, "sum of %12.6d and 20 is 30\n", number);
  printf("%s", buffer);
  return 0;
}