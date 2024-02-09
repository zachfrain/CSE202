/*
 *  CSE202: Big Integer Manipulation Program
 *  Full name: Zach Frain
 *  Full Lehigh Email Address: zhf325@lehigh.edu
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// big_integer structure
struct two_quadwords{
  unsigned long lsq;
  long msq;
};
// big_integer typedef
typedef struct two_quadwords big_integer;

// Union to store big integers as 16 characters or two quadwords
union value{
  big_integer binary;
  char hex[16];
};

// Prototypes of the methods to manipulate big integers

// reads a big integer from string input and stores it in the union v
// returns 0 if the hexadecimal number is invalid, 1 otherwise
int read_big_integer(union value *v, char *input){
  // First, validate input and make sure is correct length of 32 hex digits
  if(strlen(input) != 32){
    return 0;
  }

  strncpy(v->hex,input,16);
  // If input is valid, loop through input and convert to big_integer
  unsigned long temp_lsq = 0;
  long temp_msq = 0;
  for(int i = 0; i < 32; i++){
    char hexChar = input[i];
    int hexValue;
    //Convert hex character to integer value...
    if(hexChar >= '0' && hexChar <= '9'){
      hexValue = hexChar - '0'; // however many positions away from '0' is the value of the character
    } else if (hexChar >= 'a' && hexChar <= 'f') {
      hexValue = (hexChar - 'a') + 10; //if 'a' 'a'-'a' is 0 + 10 is 10, which is true, same for b..f
    } else {
      return 0; // if not 0-9 or a-f, invalid input, so return 0
    }

    if(i<16){
      temp_msq = (temp_msq << 4) | hexValue;
    } else {
      temp_lsq = (temp_lsq << 4) | hexValue;
    }
  }

  v->binary.lsq = temp_lsq;
  v->binary.msq = temp_msq;

  return 1;
}

char hexDigit(char c){
  if(c >= '0' && c <= '9'){
    return c;
  } else if (c >= 'a' && c <= 'f'){
    return c;
  } else {
    return 'p';
  }
}

// writes the value of b to standard output as 32 hex characters
void write_big_integer(union value b){
  printf("%016lx\t%016lx\n", b.binary.msq, b.binary.lsq);
}

char* write_big_integer_s(big_integer b){
  char* temp;
  temp = (char*)malloc(sizeof(temp)+1);
  snprintf(temp,33,"%016lx%016lx", b.msq, b.lsq);
  return temp;
}
// performs b1 & b2 and stores the result in b1_and_b2
void and_big_integers(big_integer b1, big_integer b2, big_integer *b1_and_b2){
  unsigned long b1lsq = b1.lsq;
  unsigned long b2lsq = b2.lsq;
  long b1msq = b1.msq;
  long b2msq = b2.msq;
  unsigned long b1andb2 = (b1lsq) & (b2lsq);
  long b1andb2_2 = (b1msq) & (b2msq);
  b1_and_b2->lsq = b1andb2;
  b1_and_b2->msq = b1andb2_2;
}

// performs b1 | b2 and stores the result in b1_or_b2
void or_big_integers(big_integer b1, big_integer b2, big_integer *b1_or_b2){
  
  b1_or_b2->lsq = b1.lsq |= b2.lsq;
  b1_or_b2->msq = b1.msq |= b2.msq;
}

// performs b1 ^ b2 and stores the result in b1_xor_b2
void xor_big_integers(big_integer b1, big_integer b2, big_integer *b1_xor_b2){
  unsigned long b1lsq = b1.lsq;
  unsigned long b2lsq = b2.lsq;
  long b1msq = b1.msq;
  long b2msq = b2.msq;
  unsigned long b1xorb2 = (b1lsq) ^ (b2lsq);
  long b1xorb2_2 = (b1msq) ^ (b2msq);
  b1_xor_b2->lsq = b1xorb2;
  b1_xor_b2->msq = b1xorb2_2;
}

// performs ~b and stores the result in b
void not_big_integer(big_integer *b){
  big_integer temp;
  temp.lsq = ~(b->lsq);
  temp.msq = ~(b->msq);
  b->lsq = temp.lsq;
  b->msq = temp.msq;
}

// performs b << k and stores the result in b
void shift_big_integer_left(big_integer *b, unsigned k){
  b->msq <<= k;
  unsigned long temp = b->lsq;
  if(k >= 64 && k < 128){
    b->lsq = b->lsq & 0x0000000000000000;
    temp <<= (k - 64);
    b->msq = temp;
  } else if (k < 64){
    temp >>= (64-k);
    b->msq = b->msq + temp;
    b->lsq <<= k;
  } else {
    perror("Invalid shift");
  }
}

// performs b >> k and stores the result in b
void shift_big_integer_right(big_integer *b, unsigned k){
  b->lsq >>= k;
  long temp = b->msq;
  if(k >= 64 && k < 128){
    b->msq = b->msq & 0x0000000000000000;
    temp >>= (k-64);
    b->lsq = temp;
  } else if(k < 64){
    temp <<= (64 - k);
    b->lsq = b->lsq + temp;
    b->msq >>= k;
  } else {
    perror("Invalid shift");
  }
}

// performs b1+b2 and stores the result in sum
// returns 1 if overflow occurs, 0 otherwise
int add_big_integers(big_integer b1, big_integer b2, big_integer *sum){
  sum->lsq = b1.lsq + b2.lsq;
  int carry = (sum->lsq < b1.lsq) ? 1 : 0;
  sum->msq = b1.msq + b2.msq + carry;

  if((b1.msq < 0 && b2.msq < 0 && sum->msq>=0) || (b1.msq >= 0 && b2.msq >= 0 && sum->msq <0)){
    return 1;
  }
  return 0;
}

// prints the message and value followed by the program usage message
void print_usage(const char *message[], const char* value, int op, int a){
  if(a > 1){
    printf("\n\nInvalid hex value: %s\n", message[a]);
  } else {
    if(op == 0){
    printf("\n\nInvalid operation: %s\n", message[1]);
  } else if(op == 1){
    printf("\n\n~ %s =\n  %s\n", message[2], value);
  } else if(op == 2){
    printf("\n\n %s  <<  %s  =\n %s\n", message[3], message[2], value);
  } else if(op == 3){
    printf("\n\n % s >> % s =\n % s\n", message[3], message[2], value);
  } else if(op == 4){
    printf("\n\n %s  &\n %s  =\n %s\n", message[2], message[3], value);
  } else if(op == 5){
    printf("\n\n %s  |\n %s  =\n %s\n", message[2], message[3], value);
  } else if(op == 6) {
    printf("\n\n %s  ^\n %s  =\n %s\n", message[2], message[3], value);
  } else if(op == 7){
    if(a == 1){
      printf("\n\n %s  +\n %s  =\n %s  -  Overflow\n", message[2], message[3], value);
    } else {
      printf("\n\n %s  +\n %s  =\n %s\n", message[2], message[3], value);
    }
  }}
  
}

// return integer representing operation 1-7, 0 if not recognized
int operation(char *op, int argc)
{
  if (argc == 3)
  {
    if (strcmp(op, "not") == 0)
    {
      return 1;
    }
  }
  else if (argc == 4)
  {
    if (strcmp(op, "sl") == 0)
    {
      return 2;
    }
    else if (strcmp(op, "sr") == 0)
    {
      return 3;
    }
    else if (strcmp(op, "and") == 0)
    {
      return 4;
    }
    else if (strcmp(op, "or") == 0)
    {
      return 5;
    }
    else if (strcmp(op, "xor") == 0)
    {
      return 6;
    }
    else if (strcmp(op, "add") == 0)
    {
      return 7;
    }
  }
  return 0;
}

// main method
int main(int argc, char* argv[]){
    int e=0;
    if(argc < 3 || argc > 4){
      printf("Too many or too few arguments\n");
      return 0;
    }
    int op = operation(argv[1], argc);
    if(op == 0){
      
    } else if(op == 1){
      union value not;
      int r1 = read_big_integer(&not, argv[2]);
      if(r1==0){ e=2; }
      not_big_integer(&not.binary);
      char* not_r = write_big_integer_s(not.binary);
      print_usage(argv, not_r, 1, e);
    } else if(op == 2){
      union value sl;
      int r2 = read_big_integer(&sl, argv[3]);
      if(r2==0){ e = 3; }
      shift_big_integer_left(&sl.binary, atoi(argv[2]));
      char* sl_r = write_big_integer_s(sl.binary);
      print_usage(argv, sl_r, 2, e);
    } else if(op == 3){
      union value sr;
      int r3 = read_big_integer(&sr, argv[3]);
      if(r3 ==0){ e=3; }
      shift_big_integer_right(&sr.binary, atoi(argv[2]));
      char *sr_r = write_big_integer_s(sr.binary);
      print_usage(argv, sr_r, 3, e);
    } else if(op == 4){
      union value and1;
      union value and2;
      union value and;
      int r4_1 = read_big_integer(&and1, argv[2]);
      int r4_2 = read_big_integer(&and2, argv[3]);
      int r4_3 = read_big_integer(&and, argv[2]);
      if(r4_1 == 0){ e=2; }else if(r4_2 == 0){e=3;}
      and_big_integers(and1.binary, and2.binary, &and);
      char* and_r = write_big_integer_s(and.binary);
      print_usage(argv, and_r, 4, e);
    } else if(op == 5){

      union value or1;
      union value or2;
      union value or;
      int r5_1 = read_big_integer(&or1, argv[2]);
      int r5_2 = read_big_integer(&or2, argv[3]);
      int r5_3 = read_big_integer(&or, argv[2]);
      if (r5_1==0){ e = 2;} else if(r5_2==0){e=3;}
      or_big_integers(or1.binary, or2.binary, &or);
      char *or_r = write_big_integer_s(or.binary);
      print_usage(argv, or_r, 5, e);
    } else if(op == 6){
      union value xor1;
      union value xor2;
      int r6_1 = read_big_integer(&xor1, argv[2]);
      int r6_2 = read_big_integer(&xor2, argv[3]);
      if (r6_1 == 0)
      {
      e = 2;
      }
      else if (r6_2 == 0)
      {
      e = 3;
      }
      big_integer xor;
      xor_big_integers(xor1.binary, xor2.binary, &xor);
      char *xor_r = write_big_integer_s(xor);
      print_usage(argv, xor_r, 6, e);
    } else if(op == 7){
      union value add1;
      union value add2;
      int r7_1 = read_big_integer(&add1, argv[2]);
      int r7_2 = read_big_integer(&add2, argv[3]);
      if (r7_1 == 0)
      {
      e = 2;
      }
      else if (r7_2 == 0)
      {
      e = 3;
      }
      big_integer add;
      int a = add_big_integers(add1.binary, add2.binary, &add);
      char* add_r = write_big_integer_s(add);
      print_usage(argv,add_r, 7, e+a);
      
    }
    return 0;
}

