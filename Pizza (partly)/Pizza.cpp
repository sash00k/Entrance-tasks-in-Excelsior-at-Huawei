// I spent a lot of time trying to solve the problem using polymorphism: a city is composed of objects of class "cell" with subclasses "pizzeria" and 
// "house". They had abstract virtual methods for changing the status of the city. But I can't create any structure like array or vector
// in dynamic or even static memory from elements of an abstract class. 
// I don't understand why a pointer to an object of any subclass can be stored in pointers to an object of the "cell" class with saving its features, 
// but an array (even an array of pointers to static memory) or a vector can't store such information. 
// Why isn't memory allocated just with a margin for the largest possible subclass of the class.

// The proposed solution what I managed to do in the remaining time does not pass the "tree" of all possible ways to "tile" the city completely. It works for all test cases 
// for which there is only one solution. But for cases where there are several solutions and it becomes necessary to try different "tilings", this algorithm passes only one of 
// the paths, and there is no guarantee that this path will lead to "tiling" of the entire plane. (It does'nt wark correctly even for easy 2x2 case from sample input) 

#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

int height;
int width;
int num; //number of pizzerias

class house {

	bool isHouse = true;
	bool fed = false;
	int whose = -1;
	vector <int> potentialFeeders;

public:

	house() : isHouse(true), fed(false), whose(-1) {}

	~house() {
		potentialFeeders.clear();
	}

	bool getIsHouse() { return isHouse; }
	bool isFed() { return fed; }
	int getReach() { return potentialFeeders.size(); }
	int getWhose() { return whose; }

	//get feeding pizzeria input id
	int operator [] (int i) { return potentialFeeders[i]; }

	void zeroReach() { potentialFeeders.clear(); }

	void addPotentialFeeder(int inputId) { potentialFeeders.push_back(inputId); }

	void feed(int inputId) {
		if (!fed) {
			fed = true;
			whose = inputId;
			potentialFeeders.clear();
		}
	}

	void makePizzeria(int inputId) {
		isHouse = false;
		fed = true;
		whose = inputId;
		potentialFeeders.clear();
	}

	void makeHouseNew() {
		isHouse = true;
		fed = false;
		whose = -1;
		zeroReach();
	}
};

house** City;

class pizzeria {

	int x;
	int y;
	int inputId;
	int capacity;
	int north = 0;
	int east = 0;
	int south = 0;
	int west = 0;

public:

	pizzeria(int _x, int _y, int _inputId, int _capacity) : x(_x), y(_y), inputId(_inputId), capacity(_capacity) {}

	int getX() { return x; }
	int getY() { return y; }
	int getInputId() { return inputId; }
	int getCapacity() { return capacity; }

	int getNorth() { return north; }
	int getEast() { return east; }
	int getSouth() { return south; }
	int getWest() { return west; }

	void decreaseCap(int distance) { 
		capacity -= distance;
	}

	void increaseDirection(char type, int cost) {
		switch (type) {
		case 'n':
			north += cost;
			break;
		case 'e':
			east += cost;
			break;
		case 's':
			south += cost;
			break;
		case 'w':
			west += cost;
			break;
		default:
			break;
		}
		decreaseCap(cost);
	}

};

class pizzVector {

	vector <pizzeria> myVector;

public:

	int size() { 
		int mySize = 0; 
		for (int i = 0; i < myVector.size(); i++) {
			if (myVector[i].getCapacity() != 0)
				mySize++;
		}
		return mySize; //number of elements with non-zero capacity
	}

	pizzeria& withInputId(int _inputId) {
		for (int i = 0; i < myVector.size(); i++) {
			if (myVector[i].getInputId() == _inputId)
				return myVector[i];
		}
	}

	pizzeria& operator [] (int id) { return myVector[id]; }

	void readPizzerias() {
		int tmpX, tmpY, tmpCap;
		for (int inputId = 0; inputId < num; inputId++) {
			fin >> tmpX >> tmpY >> tmpCap;
			tmpX--;
			tmpY--;
			City[tmpX][tmpY].makePizzeria(inputId);
			pizzeria tmpPizzeria(tmpX, tmpY, inputId, tmpCap);
			myVector.push_back(tmpPizzeria);
		}
	}

	int getId(int inputId) {
		for (int i = 0; i < myVector.size(); i++) {
			if (myVector[i].getInputId() == inputId) return i;
		}
	}

	void deletePizzeria(int inputId) {
		int id = getId(inputId);
		myVector.push_back(myVector[id]);
		myVector.erase(myVector.begin() + id);
	}

	void output() {
		int n, e, s, w;
		for (int i = 0; i < myVector.size(); i++) {
			n = myVector[getId(i)].getNorth();
			e = myVector[getId(i)].getEast();
			s = myVector[getId(i)].getSouth();
			w = myVector[getId(i)].getWest();
			fout << n << ' ' << e << ' ' << s << ' ' << w << '\n';
		}
	}

	void clear() {
		myVector.clear();
	}

};

pizzVector Pizzerias;

bool feedReached();
void setReachs();
void setNewCity();
void clearAll();
void doWork();

int main() {

	int tmpInt, i = 1;
	fin >> tmpInt;
	while (tmpInt){
		width = tmpInt;
		fout << "Case " << i << ":\n";
		doWork();
		fout << '\n';
		fin >> tmpInt;
		i++;
	}

	clearAll();
	fin.close();
	fout.close();

	return 0;
}

void setReachs() {

	for (int i = height - 1; i > -1; i--) {
		for (int j = 0; j < width; j++) {
			if (City[j][i].getIsHouse())
				City[j][i].zeroReach();
		}
	}

	for (int i = 0; i < Pizzerias.size(); i++) {
		int X = Pizzerias[i].getX(), Y = Pizzerias[i].getY(), tmpCap;
		//go north
		tmpCap = Pizzerias[i].getCapacity();
		for (int j = 1; Y + j < height && j <= tmpCap && City[X][Y + j].getIsHouse(); j++) {
			if (City[X][Y + j].isFed() && City[X][Y + j].getWhose() == Pizzerias[i].getInputId()) {
				tmpCap++;
				continue;
			}
			if (!City[X][Y + j].isFed())
				City[X][Y + j].addPotentialFeeder(Pizzerias[i].getInputId());
		}
		//go east
		tmpCap = Pizzerias[i].getCapacity();
		for (int j = 1; X + j < width && j <= tmpCap && City[X + j][Y].getIsHouse(); j++) {
			if (City[X + j][Y].isFed() && City[X + j][Y].getWhose() == Pizzerias[i].getInputId()) {
				tmpCap++;
				continue;
			}
			if (!City[X + j][Y].isFed())
				City[X + j][Y].addPotentialFeeder(Pizzerias[i].getInputId());
		}
		//go south
		tmpCap = Pizzerias[i].getCapacity();
		for (int j = 1; Y - j > -1 && j <= tmpCap && City[X][Y - j].getIsHouse(); j++) {
			if (City[X][Y - j].isFed() && City[X][Y - j].getWhose() == Pizzerias[i].getInputId()) {
				tmpCap++;
				continue;
			}
			if (!City[X][Y - j].isFed())
				City[X][Y - j].addPotentialFeeder(Pizzerias[i].getInputId());
		}
		//go west
		tmpCap = Pizzerias[i].getCapacity();
		for (int j = 1; X - j > -1 && j <= tmpCap && City[X - j][Y].getIsHouse(); j++) {
			if (City[X - j][Y].isFed() && City[X - j][Y].getWhose() == Pizzerias[i].getInputId()) {
				tmpCap++;
				continue;
			}
			if (!City[X - j][Y].isFed())
				City[X - j][Y].addPotentialFeeder(Pizzerias[i].getInputId());
		}
	}
}

bool feedReached() {
	int minReachNum = 5;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (City[i][j].getReach() != 0 && City[i][j].getReach() < minReachNum) {
				minReachNum = City[i][j].getReach();
				if (minReachNum == 1)
					break;
			}
		}
		if (minReachNum == 1)
			break;
	}

	int fedderId, cost;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (City[i][j].getReach() == minReachNum) {

				fedderId = City[i][j][0];
				int X = Pizzerias.withInputId(fedderId).getX();
				int Y = Pizzerias.withInputId(fedderId).getY();

				if (X == i) { //if house and pizzeria are in the same column
					cost = abs(j - Y);
					if (j > Y) {//if house is higher then pizzeria, we increase north direction by value of "cost"
						for (int it = 1; it <= abs(j - Y); it++) {
							if (it != abs(j - Y) && City[X][Y + it].getIsHouse() && City[X][Y + it].getWhose() == fedderId) //we watch which houses along the way have already been fed by this pizzeria on previous global iterations
								cost--; //if there are any, then we decrease the cost
							City[X][Y + it].feed(fedderId);
						}
						Pizzerias.withInputId(fedderId).increaseDirection('n', cost);
					}
					else {//if house isn't higher then pizzeria (then it't exectly lower), we increase south direction by value of "cost"
						for (int it = 1; it <= abs(j - Y); it++) {
							if (it != abs(j - Y) && City[X][Y - it].getIsHouse() && City[X][Y - it].getWhose() == fedderId) //same as in north direction
								cost--;
							City[X][Y - it].feed(fedderId);
						}
						Pizzerias.withInputId(fedderId).increaseDirection('s', cost);
					}
				}
				else { //if house and pizzeria are in different columns (then they are in the same string)
					cost = abs(i - X);
					if (i > X) {//if house is righter then pizzeria, we increase east direction by value of "cost"
						for (int it = 1; it <= abs(i - X); it++) {
							if (it != abs(i - X) && City[X + it][Y].getIsHouse() && City[X + it][Y].getWhose() == fedderId) //same as in north direction
								cost--;
							City[X + it][Y].feed(fedderId);
						}
						Pizzerias.withInputId(fedderId).increaseDirection('e', cost);
					}
					else {//if house isn't righter then pizzeria (then it't exectly lefter), we increase west direction by value of "cost"
						for (int it = 1; it <= abs(i - X); it++) {
							if (it != abs(i - X) && City[X - it][Y].getIsHouse() && City[X - it][Y].getWhose() == fedderId) //same as in north direction
								cost--;
							City[X - it][Y].feed(fedderId);
						}
						Pizzerias.withInputId(fedderId).increaseDirection('w', cost);
					}
				}
				if (Pizzerias.withInputId(fedderId).getCapacity() == 0) //if the pizzeria has zero capacity, we remove it to the end of the vector, and do not consider it until the output 
					Pizzerias.deletePizzeria(fedderId);
			}
		}
	}
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (City[i][j].getReach() != 0)
				return false;
		}
	}
	return true;
}

void setNewCity() {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++)
			City[i][j].makeHouseNew();
	}
}

void clearAll() {
	Pizzerias.clear();
	for (int i = 0; i < width; i++)
		delete[] City[i];
	delete[] City;
}

void doWork() {
	fin >> height >> num;

	City = new house * [width];
	for (int i = 0; i < width; i++)
		City[i] = new house[height];

	setNewCity();
	Pizzerias.clear();

	Pizzerias.readPizzerias();
	int i = 1;
	bool wasItLastTime = false;
	while (!wasItLastTime) {
		setReachs();
		wasItLastTime = feedReached();
		i++;
	}

	bool taskDone = true;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (!City[i][j].isFed())
				taskDone = false;
		}
	}
	for (int i = 0; i < Pizzerias.size(); i++) {
		if (Pizzerias[i].getCapacity() < 0)
			taskDone = false;
	}

	if (taskDone)
		Pizzerias.output();
	else
		fout << "Task failed\n";

}
