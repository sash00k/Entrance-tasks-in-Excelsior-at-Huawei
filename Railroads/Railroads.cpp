//more about why this algorithm works in README.txt
#include <iostream>
#include <fstream>
using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

int* arr;
int length;

bool isSubsitutionCorrect();
bool readAndCheck(int firstNum);

int main() {
	int getInt1, getInt2;
	fin >> getInt1;
	while(getInt1 != 0) { //checks if this is the end of the input
		length = getInt1; //if this is not zero we read the length
		arr = new int[length];
		fin >> getInt2; 
		while (getInt2 != 0) { //checks if this is the end of the input for arrays of this length
			fout << (readAndCheck(getInt2) ? "Yes\n" : "No\n"); //if this is not zero we read the first number in array
			fin >> getInt2;
		}
		delete [] arr;
		fout << '\n';
		fin >> getInt1;
	} 

	fin.close();
	fout.close();
	
	return 0;
}


bool isSubsitutionCorrect() {
	bool* natArr;
	natArr = new bool[length + 1]; //auxiliary array for the substitution checks
	for (int i = 1; i < length + 1; i++)
		natArr[i] = false;
	for (int i = 0; i < length; i++) {
		if (arr[i] > length)
			throw 1;
		if (arr[i] < 1)
			throw 2;
		if (natArr[arr[i]] == false) { //in normal at the end of cycle all the elements except 0th will be equaled to "true"
			natArr[arr[i]] = true;
		}
		else
			throw 3;
	}
	delete[] natArr;
}

bool readAndCheck(int firstNum) {
	//the criterion that this sequence is possible for our station is a decreasing by no more than one (if the substitution is correct)
	arr[0] = firstNum;
	for (int i = 1; i < length; i++)
		fin >> arr[i];
	try {
		isSubsitutionCorrect(); //checks if this substitution is valid 
		char tmp;
		fin.get(tmp);
		if (tmp != '\n' && tmp != ' ')
			throw 4;
	}
	catch (int errId) {
		cout << "The input is incorrect. Probably ";
		switch (errId) {
		case 1:
			cout << "there is too big number in one of " << length << "-elemet substitutions.\n";
			break;
		case 2:
			cout << "there is too little number or invalid symbol in one of " << length << "-elemet substitutions.\n";
			break;
		case 3:
			cout << "there is two same numbers in one of " << length << "-elemet substitutions.\n";
			break;
		case 4:
			cout << "there is too long array in one of " << length << "-elemet substitutions.\n";
			break;
		default:
			break;
		}
		exit(errId);
	}

	int prev = arr[0]; //check if the difference arr[i-1] - arr[i] is more than one somewhere in the sequence
	bool isCorrect = true;
	for (int i = 1; i < length; i++) {
		if (prev - arr[i] > 1) {
			isCorrect = false;
			break;
		}
		prev = arr[i];
	}
	return isCorrect;
}
