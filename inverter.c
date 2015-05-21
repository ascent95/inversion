#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct Matrix {
    int size;
    double *data;
} Matrix;

Matrix *readMatrix(char *fileName);
void assignValues(Matrix *mat, char *buffer, int line_number);
void insertMat(Matrix *mat, int col, int row, double x);
void initial(Matrix *mat, int col, int row, double x);
double accessMat(Matrix *mat, int col, int row);
void printMat(Matrix *mat, FILE *fp, int start);
void swapRow(Matrix *mat, int ra, int rb);
int pivMax(Matrix *mat, int k);
void backSub(Matrix *mat);
void inverse(Matrix *mat);

Matrix *createMat(int size) {
    Matrix *mat = malloc(sizeof(Matrix));
    assert(mat != NULL);
    
    mat -> size = size;
    mat -> data = malloc(size * size * 2 * sizeof(double));
    return mat;
}

void destroyMat(Matrix *mat) {
    assert(mat != NULL);
    
    free(mat -> data);
    free(mat);
}

Matrix *readMatrix(char *fileName) {
    FILE *fp = fopen(fileName, "r");
    
    if (fp == NULL) {
        fprintf(stderr, "Cannot open file: %s\n", fileName);
        return NULL;
    }
    int ch;
    int size = 0;
    while (EOF != (ch = fgetc(fp)) && ch != '\n') {
        if (ch == ' ') size++;
    }
    size++;
    rewind(fp);
    
    size_t buffer_size = 80;
    char *buffer = malloc(buffer_size * sizeof(char));
    
    Matrix *mat = createMat(size);
    //read each line and write to matrix.
    int line_number = 0;
    while(-1 != getline(&buffer, &buffer_size, fp)) {
        assignValues(mat, buffer, line_number);
        line_number++;
    }
    fflush(stdout);
    
    //close file and clean up buffer
    fclose(fp);
    free(buffer);
    
    return mat;
}

void writeMatrix(Matrix *mat) {
    FILE *fp;
    fp = fopen("output.txt","w+");
    printMat(mat, fp, mat -> size);
    fclose(fp);
}
void assignValues(Matrix *mat, char *buffer, int line_number) {
    int count;
    char *token;
    double val;
    for(count = 0; count < mat -> size; count++) {
        token = strsep(&buffer, " ");
        sscanf(token, "%lf", &val);
        initial(mat, count, line_number, val);
    }
}

void insertMat(Matrix *mat, int col, int row, double x) {
    //printf("inserting:%lf\n", x);
    mat -> data[col + mat -> size * 2 * row] = x;
}

void initial(Matrix *mat, int col, int row, double x) {
    insertMat(mat, col, row, x);
    if (col == row) mat -> data[col + mat -> size * (1 + 2 * row)] = 1;
    else mat -> data[col + mat -> size * (1 + 2 * row)] = 0;
}

double accessMat(Matrix *mat, int col, int row) { //somthing wrong here
    return mat -> data[col + mat -> size * 2 * row];
}

void printMat(Matrix *mat, FILE *fp, int start) {
    int i,j;
    for (j = 0; j < mat -> size; j++) {
        for (i=start; i<mat -> size*2; i++) {
            fprintf(fp,"%lf ", mat -> data[j*mat -> size*2 + i]);
        }
        fprintf(fp,"\n");
    }
}

void swapRow(Matrix *mat, int ra, int rb) {
    int i;
    double a,b;
    for (i = 0; i < mat -> size * 2; i++) {
        a = accessMat(mat, i, ra);
        b = accessMat(mat, i, rb);
        //printf("a:%lf b:%lf\n",a,b);
        insertMat(mat, i, rb, a);
        insertMat(mat, i, ra, b);
    }
}

int pivMax(Matrix *mat, int k) {
    int i_max = k;
    int i_max_val = 0;
    int i,tmp;
    for (i = k; i < mat -> size; i++) {
        tmp = accessMat(mat, k, i);
        //printf("tmp: %d\n",tmp);
        if (abs(tmp) > i_max_val) {
            i_max_val = abs(tmp);
            i_max = i;
        }
    }
    return i_max;
}

int gaussJordan(Matrix *mat) {
    int i, j,k, i_max;
    double tmp;
    for (k = 0; k < mat -> size; k++) {
        i_max = pivMax(mat, k);
        //printf("i_max: %d k: %d\n",i_max,k);
        assert(accessMat(mat, k, i_max) != 0);
        if(k != i_max) swapRow(mat, k, i_max);
        for (j = k + 1; j < mat -> size; j++) {
            for (i = k + 1; i < mat -> size * 2; i++) {
                tmp = accessMat(mat, i, j) - accessMat(mat, i, k) * (accessMat(mat, k, j) / accessMat(mat, k, k));
                insertMat(mat, i, j, tmp);
            }
            insertMat(mat,k,j,0);
        }
    }
    backSub(mat);
    inverse(mat);
    return 0;
}

void backSub(Matrix* mat) {
    int i, j,k;
    double tmp;
    for (k = mat -> size -1 ; k >= 0; k--) {
        for (j = k - 1; j >= 0; j--) {
            for (i = k + 1; i < mat -> size * 2; i++) {
                tmp = accessMat(mat, i, j) - accessMat(mat, i, k) * (accessMat(mat, k, j) / accessMat(mat, k, k));
                insertMat(mat,i,j,tmp);
            }
            insertMat(mat,k,j,0);
        }
    }
}

void inverse(Matrix *mat) {
    int k,i;
    double current, inv;
    
    for (k = 0; k< mat -> size; k++) {
        current = accessMat(mat, k, k);
        inv = 1/current;
        for (i = k; i < mat -> size * 2; i++) {
            //printf("accessing: (%d,%d)\n",i,k);
            current = accessMat(mat,i,k);
            insertMat(mat,i,k,current * inv);
        }
    }
}

int main() {
    Matrix *mat = readMatrix("input.txt");
    if(gaussJordan(mat) == 1) printf("error\n");;
    //printMat(mat,mat -> size);
    writeMatrix(mat);
    destroyMat(mat);
    return 0;
}
