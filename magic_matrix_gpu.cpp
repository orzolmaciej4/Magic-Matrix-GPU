#include <stdio.h>
#include <stdbool.h>
#include <omp.h>
#include <stdlib.h>

#pragma omp declare target
void generateMagicSquare(int** pattern, int** modifier, int** magicSquare, int N, int M);
int sumRow( int** matrix, int row, int N);
int sumColumn( int** matrix, int col, int N);
bool isMagicSquare(int** matrix, int N);
bool allEqual(int arr[], int N);
bool isPairwiseDistinct(int** matrix, int N);
#pragma omp end declare target

void generateMagicSquare(int** pattern, int** modifier, int** magicSquare, int N, int M)
{
    int i, j;
    
    #pragma omp target map(to: modifier[:N][:N], pattern[:N][:N]) map(tofrom: magicSquare[:M][:M])
    {
        // Multiply the modifier matrix with M
        #pragma omp parallel for simd collapse(2) schedule(static)
        for (i = 0; i < N; i++)
        {
            for (j = 0; j < N; j++)
            {
                modifier[i][j] *= M;
            }
        }

        // Generate the magic square
        #pragma omp parallel for simd collapse(2) schedule(static)
        for (i = 0; i < M; i++)
        {
            for (j = 0; j < M; j++)
            {
                int patternRow = i % N;
                int patternCol = j % N;
                magicSquare[i][j] = pattern[patternRow][patternCol];
                magicSquare[i][j] += modifier[i/N][j/N];
            }
        }
    }
}

// computes sum of elements in a row
int sumRow( int** matrix, int row, int N)
{
    int sum = 0;

    #pragma omp simd reduction(+:sum) aligned(matrix : 128)
    for (int i = 0; i < N; i++)
    {
        sum += matrix[row][i];
    }
    return sum;
}

// computes sum of elements in a column
int sumColumn( int** matrix, int col, int N)
{
    int sum = 0;

    #pragma omp simd reduction(+:sum) aligned(matrix : 128)
    for (int i = 0; i < N; i++)
    {
        sum += matrix[i][col];
    }
    return sum;
}

// checks if all elements in an array are equal
bool allEqual( int arr[], int N)
{
    bool result = true;

    #pragma omp for reduction(&&:result) aligned(arr : 128)
    for (int i = 1; i < N; i++){
        result = result && (arr[i] == arr[0]);
    }

    return result;
}

bool isPairwiseDistinct( int** matrix, int N) {
    bool isDistinct = true;
    
    #pragma omp parallel for collapse(2) shared(isDistinct)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int currentElement = matrix[i][j];
            for (int row = 0; row < N; row++) {
                for (int col = 0; col < N; col++) {
                    if (row != i || col != j) {
                        int otherElement = matrix[row][col];
                        if (currentElement == otherElement) {
                            isDistinct = false;
                        }
                    }
                }
            }
        }
    }
    return isDistinct;
}

// checks if matrix is a magic square
bool isMagicSquare(int** matrix, int N)
{
    int row_sums[N];
    int col_sums[N];
    int main_diag_sum = 0;
    int anti_diag_sum = 0;
    bool isMagic = false;

    #pragma omp target map(to: matrix[:N][:N], row_sums[:N], col_sums[:N], main_diag_sum, anti_diag_sum) map(tofrom: isMagic)
    {
        #pragma omp parallel
        {
            
            #pragma omp for reduction(+:main_diag_sum, anti_diag_sum)
            for (int i=0; i < N; i++) {
                #pragma omp atomic write
                row_sums[i] = sumRow(matrix, i, N);
                #pragma omp atomic write
                col_sums[i] = sumColumn(matrix, i, N);
                main_diag_sum += matrix[i][i];
                anti_diag_sum += matrix[i][N - 1 - i];
            }
            
            #pragma omp barrier //ensure all threads have computed the sums
            
            // Check if it's a magic square on the GPU
            bool isPartialMagic = (main_diag_sum == anti_diag_sum);
            bool rowsEqual = allEqual(row_sums, N);
            bool colsEqual = allEqual(col_sums, N);

            #pragma omp barrier // Ensure all threads have computed the boolean values

            if (isPartialMagic && rowsEqual && colsEqual) {
                isMagic = isPairwiseDistinct(matrix, N);
            }
            
        }
    }

    return isMagic;
}

int main(int argc, char *argv[])
{
    double start_time, end_time;
    start_time = omp_get_wtime();

    if (argc != 3) {
        printf("Usage: %s <pattern_filename> <modifier_filename>\n", argv[0]);
        return 1;
    }
    
    FILE *pattern_file = fopen(argv[1], "r");
    FILE *modifier_file = fopen(argv[2], "r");

    if (pattern_file == NULL || modifier_file == NULL)
    {
        printf("Error opening files.\n");
        return 1;
    }

    int N;
    if (fscanf(pattern_file, "%d", &N) != 1 || fscanf(modifier_file, "%d", &N) != 1){
        printf("Error reading the size of the matrix.\n");
        fclose(pattern_file);
        fclose(modifier_file);
        return 1;
    }

    int M = N * N;
    int** pattern = new int*[N];
    int** modifier = new int*[N];
    int** magicSquare = new int*[M];
    
    for (int i = 0; i < N; i++) {
        pattern[i] = new int[N];
        modifier[i] = new int[N];
    }
    
    for (int i = 0; i < M; i++) {
        magicSquare[i] = new int[M];
    }

    // read-in matrix data
    for (int i = 0; i < N; i++) {
        for (int j=0; j < N; j++) {
            if (fscanf(pattern_file, "%d", &pattern[i][j]) != 1 || fscanf(modifier_file, "%d", &modifier[i][j]) != 1) {
                printf("Error reading matrix values.\n");
                fclose(pattern_file);
                fclose(modifier_file);
                return 1;
            }
        }
    }

    fclose(pattern_file);
    fclose(modifier_file);
        
    generateMagicSquare(pattern, modifier, magicSquare, N, M);
   
    bool is_magic_square = isMagicSquare(magicSquare, M);
    
    // Print first 3 and last 3 elements of generated and checked matrix 
    for (int i = 0; i < 3; i++)
    {
        printf("%d ", magicSquare[i][0]);
    }

    printf("... \n . \n . \n . \n");

    for (int j = M-3; j < M; j++)
    {
        printf("%d ", magicSquare[M-1][j]);
    }

    printf("\n");
    if(is_magic_square) 
        printf("Generated matrix is a magic square.\n");
    else                
        printf("Generated matrix is not a magic square.\n");
    
    // free dynamically allocated memory
    for (int i = 0; i < M; i++) {
        delete[] magicSquare[i];
    }
    delete[] magicSquare;

    for (int i = 0; i < N; i++) {
	    delete[] pattern[i];
	    delete[] modifier[i];
    }
    delete[] pattern;
    delete[] modifier;

    end_time = omp_get_wtime();
    printf("Computation time: %.10lf\n", end_time-start_time);
}
