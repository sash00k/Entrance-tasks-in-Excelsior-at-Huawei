#include <fstream>
#include <iostream>
#define fori(n) for(int i = 0; i < n; i++)
#define forj(n) for(int j = 0; j < n; j++)
using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

int** A;
int A_height, A_width;

int** B_in;
int** B_out;
int B_height, B_width;
bool wasAnythingDone;

void read();
void readA();
bool readB();
void findPattern();
bool check(int I, int J);
void replace(int I, int J);
void output();
void immediatelyOutput();
void freeAll();

int main() {

    read();

    if (wasAnythingDone) {
        findPattern();
        output();
    }
    else
        immediatelyOutput();

    fin.close();
    fout.close();
    freeAll();

    return 0;
}

void read() {
    try {
        readA();
        wasAnythingDone = readB(); //if A fits in B at least once readB() returns true
    }
    catch (int errId) {
        cout << "The input is incorrect. Probably ";
        switch (errId) {
        case 1:
            cout << "there is incorrect value of A-matrix size.\n";
            break;
        case 2:
            cout << "there is incorrect value in A-matrix or incorrect sizes entered.\n";
            break;
        case 3:
            cout << "there is incorrect value of B-matrix size.\n";
            break;
        case 4:
            cout << "there is incorrect value in B-matrix or incorrect sizes entered.\n";
            break;
        default:
            break;
        }
        exit(errId);
    }
}

void readA() { //read A matrix
    fin >> A_height >> A_width;
    if (A_height < 1 || A_width < 1)
        throw 1;
    A = new int* [A_height];
    fori(A_height)
        A[i] = new int[A_width];
    fori(A_height) {
        forj(A_width) {
            fin >> A[i][j];
            if (A[i][j] != 0 && A[i][j] != 1)
                throw 2;
        }
    }
}

bool readB() { //read B matrix
    fin >> B_height >> B_width;
    if (B_height < 1 || B_width < 1)
        throw 3;
    if (B_height < A_height || B_width < A_width)
        return false;
    B_in = new int* [B_height]; //pattern search matrix
    B_out = new int* [B_height]; //the matrix in which the result will be entered
    fori(B_height) {
        B_in[i] = new int[B_width];
        B_out[i] = new int[B_width];
    }
    fori(B_height) {
        forj(B_width) {
            fin >> B_in[i][j];
            if (B_in[i][j] != 0 && B_in[i][j] != 1)
                throw 4;
            B_out[i][j] = B_in[i][j];
        }
    }
    return true;
}

bool check(int I, int J) {//checks whether the submatrix of size A beginning (left-top element) in [I][J] is an A-pattern matrix
    fori(A_height) {
        forj(A_width) {
            if (A[i][j] != B_in[I + i][J + j])
                return false;
        }
    }
    return true;
}

void replace(int I, int J) {//change the submatrix in the final matrix in the necessary way
    fori(A_height) {
        forj(A_width) {
            if (B_out[I + i][J + j] == 1)
                B_out[I + i][J + j] = 2;
            if (B_out[I + i][J + j] == 0) //we use 3 instead of * not to work with types
                B_out[I + i][J + j] = 3;
        }
    }
}

void findPattern() { //do the main work of finding and replacing symbols
    fori(B_height - A_height + 1) {
        forj(B_width - A_width + 1) {
            if (check(i, j)) //does the submatrix of size A have the A-pattern? 
                replace(i, j); //if it does, we change the final matrix in the necessary way
        }
    }
}

void output() { //output the result with replacement of 3 by *
    fori(B_height) {
        forj(B_width) {
            if (B_out[i][j] == 3)
                fout << "* ";
            else
                fout << B_out[i][j] << ' ';
        }
        fout << "\n";
    }
}

void immediatelyOutput() { //output without creating B-arrays
    int tmp;
    fori(B_height) {
        forj(B_width) {
            fin >> tmp;
            fout << tmp << ' ';
        }
        fout << '\n';
    }
}

void freeAll() { //free all the used memory
    fori(A_height)
        delete[] A[i];
    delete[] A;

    if (wasAnythingDone) {
        fori(B_height) {
            delete[] B_in[i];
            delete[] B_out[i];
        }
        delete[] B_in;
        delete[] B_out;
    }
}