#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ll{
    char *name;
    long value;
    struct ll *next;
};
/* absolute value of a number */
extern int abs(int a);

/* addition and difference with overflow check */
extern int average(unsigned a, unsigned b, unsigned char* cptr);

/* solutions of a quadratic equation */
extern int quadratic(int a, int b, int c, int *x1, int *x2);

/* Minimum value in an array of longs */
extern short min(short* list, unsigned size);

/* sum of the elements of a linkedlist */
extern int sum(struct ll *list);

/* Product of two vectors */
extern void product(float *a, float *b, float *c, unsigned size);



int main(int argc, char **argv){
    int op1, op2, op3;
    float va[3], vb[3], vc[3];
    struct ll *llist, *temp, *current;
    if(argc < 3 || argc > 10 || argc == 9 || argc == 6){
        printf("Wrong number of arguments");
        exit(0);
    }
    switch(argc){
        case 3:
            if(strcmp(argv[1], "abs") == 0){
                op1 = atoi(argv[2]);
                printf("|%d| = %d\n", op1, abs(op1));
            }
            else{
                printf("Operation %s not supported", argv[1]);
                exit(0);  
            }
            break;
        case 4: // average
            if(strcmp(argv[1], "average") == 0){
                unsigned a = atoi(argv[2]);
                unsigned b = atoi(argv[3]);
                unsigned char overflow = 0;
                unsigned avg = average(a, b, &overflow);
                printf("(%u + %u)/2 = %u overflow? %s\n", 
                         a, b, avg, overflow?"true":"false");
            }else{
                printf("Operation %s not supported", argv[1]);
                exit(0); 
            }
            break;
        case 5: // quadratic
            if(strcmp(argv[1], "quadratic") == 0){
                op1 = atoi(argv[2]);
                op2 = atoi(argv[3]);
                op3 = atoi(argv[4]);
                int x1, x2;
                int solutions = quadratic(op1, op2, op3, &x1, &x2);
                if( solutions < 0){
                    printf("\"%d x^2 + %d x + %d\" has no solution!\n", op1, op2, op3);
                }
                else if(solutions == 0){
                    printf("\"%d x^2 + %d x + %d\" has one solution = %d\n", op1, op2, op3, x1);
                }
                else{
                    printf("\"%d x^2 + %d x + %d\" has two solutions = (%d, %d)\n", op1, op2, op3, x1, x2);
                }
                break;
            } else{
                printf("Operation %s not supported", argv[1]);
                exit(0); 
            }
        case 7: // minimum
            if(strcmp(argv[1], "min") == 0){
                short numbers[5];
                numbers[0] = atoi(argv[2]);
                numbers[1] = atoi(argv[3]);
                numbers[2] = atoi(argv[4]);
                numbers[3] = atoi(argv[5]);
                numbers[4] = atoi(argv[6]);
                short minimum = min(numbers, 5);
                printf("min(%d, %d, %d, %d, %d) = %d\n", 
                    numbers[0], numbers[1], numbers[2], numbers[3], numbers[4], minimum);
            }
            else{
                printf("Operation %s not supported", argv[1]);
                exit(0); 
            }
            break;
        case 8: // product
            if(strcmp(argv[1], "product") == 0){
                for(int i=0; i<3; i++){
                    va[i] = atof(argv[i+2]);
                    vb[i] = atof(argv[i+5]);
                }
                product(va, vb, vc, 3);
                printf("[%.2f, %.2f, %.2f] x [%.2f, %.2f, %.2f] = [%.2f ,%.2f, %.2f]\n",
                        va[0], va[1], va[2], vb[0], vb[1], vb[2], vc[0], vc[1], vc[2]);
            }
            else{
                printf("Operation %s not supported", argv[1]);
                exit(0); 
            }
            break;
        case 10: // sum
            if(strcmp(argv[1], "sum") == 0){
                llist = current = (struct ll*) malloc(sizeof(struct ll));
                char *name = (char*) malloc(strlen(argv[2])+1);
                strcpy(name, argv[2]);
                current->name = name;
                current->value = atol(argv[3]);
                current->next = NULL;
                for(int i=4; i<10; i+=2){
                    temp = (struct ll*) malloc(sizeof(struct ll));
                    name = (char*) malloc(strlen(argv[i])+1);
                    strcpy(name, argv[i]);
                    temp->name = name;
                    temp->value = atol(argv[i+1]);
                    temp->next = NULL;
                    current->next = temp;
                    current = temp;
                }
                long llsum = sum(llist);
                current = llist;
                printf("sum("); 
                while(current->next){
                    printf("(%s, %ld), ", current->name, current->value);
                    current = current->next;
                }
                printf("(%s, %ld)) = %ld\n", current->name, current->value, llsum);
            }
            else{
                printf("Operation %s not supported", argv[1]);
                exit(0); 
            }
            break;
    }
}