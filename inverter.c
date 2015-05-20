#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double *readMatrix(char *fileName, int *size);
void assignValues(double *matrix, char *buffer, int line_number, int size);
void insertMat(double *matrix, int size, int col, int row, double x);
void initial(double *matrix, int size, int col, int row, double x);
double accessMat(double *matrix, int size, int col, int row);
void printMat(double *matrix, int size, int start);
void swapRow(double *matrix, int size, int ra, int rb);
int pivMax(double *matrix, int size, int k);
void backSub(double *matrix, int size);
void inverse(double *matrix, int size);

double *readMatrix(char *fileName, int *size) {
    FILE *fp = fopen(fileName, "r");
    
    if (fp == NULL) {
        fprintf(stderr, "Cannot open file: %s\n", fileName);
        return NULL;
    }
    int ch;
    while (EOF!=(ch=fgetc(fp)) && ch != '\n') {
        if (ch == ' ') (*size)++;
    }
    (*size)++;
    rewind(fp);
    
    size_t buffer_size = 80;
    char *buffer = malloc(buffer_size * sizeof(char));
    
    double *matrix = malloc((*size) * (*size) * sizeof(double) * 2);
    //read each line and write to matrix.
    int line_number = 0;
    while(-1 != getline(&buffer, &buffer_size, fp)) {
        assignValues(matrix, buffer, line_number, *size);
        line_number++;
    }
    fflush(stdout);
    
    //close file and clean up buffer
    fclose(fp);
    free(buffer);
    
    return matrix;
}

void assignValues(double *matrix, char *buffer, int line_number, int size) {
    int count = 0;
    char *token;
    double val;
    while (count < size) {
        token = strsep(&buffer, " ");
        sscanf(token, "%lf", &val);
        initial(matrix, size, count, line_number, val);
        count++;
    }
}

void insertMat(double *matrix, int size, int col, int row, double x) {
    //printf("inserting:%lf\n", x);
    matrix[col + size * 2 * row] = x;
}

void initial(double *matrix, int size, int col, int row, double x) {
    insertMat(matrix, size, col, row, x);
    if (col == row) matrix[col + size * (1 + 2 * row)] = 1;
    else matrix[col + size * (1 + 2 * row)] = 0;
}

double accessMat(double *matrix, int size, int col, int row) { //somthing wrong here
    return matrix[col + size * 2 * row];
}

void printMat(double *matrix, int size, int start) { //It would be interesting to come back to this. For some reason I can't use nested for loops. Printf is causing problems.
    int i,j;
    for (j = 0; j < size; j++) {
        for (i=start; i<size*2; i++) {
            printf("%lf ",matrix[j*size*2 + i]);
        }
        printf("\n");
    }
}

void swapRow(double *matrix, int size, int ra, int rb) {
    int i;
    double a,b;
    for (i = 0; i < size * 2; i++) {
        a = accessMat(matrix, size, i, ra);
        b = accessMat(matrix, size, i, rb);
        //printf("a:%lf b:%lf\n",a,b);
        insertMat(matrix, size, i, rb, a);
        insertMat(matrix, size, i, ra, b);
    }
}

int pivMax(double *matrix, int size, int k) {
    int i_max = k;
    int i_max_val = 0;
    int i,tmp;
    for (i = k; i < size; i++) {
        tmp = accessMat(matrix, size, k, i);
        //printf("tmp: %d\n",tmp);
        if (abs(tmp) > i_max_val) {
            i_max_val = abs(tmp);
            i_max = i;
        }
    }
    return i_max;
}

int gaussJordan(double *matrix, int size) {
    int i, j,k, i_max;
    double tmp;
    for (k = 0; k < size; k++) {
        i_max = pivMax(matrix, size, k);
        //printf("i_max: %d k: %d\n",i_max,k);
        if(accessMat(matrix, size, k, i_max) == 0) return 1;
        if(k != i_max) swapRow(matrix, size, k, i_max);
        
        for (j = k + 1; j < size; j++) {
            for (i = k + 1; i < size * 2; i++) {
                tmp = accessMat(matrix, size, i, j) - accessMat(matrix, size, i, k) * (accessMat(matrix, size, k, j) / accessMat(matrix, size, k, k));
                insertMat(matrix,size,i,j,tmp);
            }
            insertMat(matrix,size,k,j,0);
        }
    }
    backSub(matrix,size);
    inverse(matrix,size);
    return 0;
}

void backSub(double *matrix, int size) {
    int i, j,k;
    double tmp;
    for (k = size -1 ; k >= 0; k--) {
        for (j = k - 1; j >= 0; j--) {
            for (i = k + 1; i < size * 2; i++) {
                tmp = accessMat(matrix, size, i, j) - accessMat(matrix, size, i, k) * (accessMat(matrix, size, k, j) / accessMat(matrix, size, k, k));
                insertMat(matrix,size,i,j,tmp);
            }
            insertMat(matrix,size,k,j,0);
        }
    }
}

void inverse(double *matrix, int size) {
    int k,i;
    double current, inv;
    
    for (k = 0; k< size; k++) {
        current = accessMat(matrix,size,k,k);
        inv = 1/current;
        for (i = k; i < size * 2; i++) {
            //printf("accessing: (%d,%d)\n",i,k);
            current = accessMat(matrix,size,i,k);
            insertMat(matrix,size,i,k,current * inv);
        }
    }
}

int main() {
    int size = 0;
    double *m = readMatrix("input.txt", &size);
    if(gaussJordan(m,size) == 1) printf("error\n");;
    printMat(m,size,size);
    free(m);
    return 0;
}
