#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

int height = 10;
int width = 15;
bool** visited; // =1 if cell was visited in current iteration (also =1 if Table[i][j]=='D'), =0 if it wasn't

class score {
	int totalPoints;
	int totalBalls;
public:
	score() : totalPoints(0), totalBalls(0) {}

	void increaseAndOut(int moveNum, int numOfBalls, int pointX, int pointY, char letter) {
		int curPoints = (numOfBalls - 2) * (numOfBalls - 2);
		totalPoints += curPoints;
		totalBalls += numOfBalls;
		fout << "Move " << moveNum + 1 << " at (" << pointY + 1 << "," << pointX + 1 << "): removed " << numOfBalls;
		fout << " balls of color " << letter << ", got " <<  curPoints << " Points.\n";
	}

	void finalScore() {
		if (height * width - totalBalls == 0)
			totalPoints += 1000;
		fout << "Final score: " << totalPoints << " with " << height * width - totalBalls << " remaining.\n\n";

	}
};

class column {
	vector <char> Letters; //'R' 'G' 'B' from input and 'D' for empty cells
public:

	char getLetter(int y) {
		return Letters[y];
	}

	void addPointToColumn(char _Letter) { //for adding Points to bottoms of incomplete Columns
		Letters.insert(Letters.begin(), _Letter);
	}

	void toTheTop(int y) { //move the y-th point from its position to the top of the column
		Letters.push_back(Letters[y]);
		Letters.erase(Letters.begin()+y);
	}

	char& operator[] (int y) { //for access with standard syntax: Table[x][y]
		return Letters[y];
	}

};

class table {
	vector <column> Columns;
public:

	char getLetter(int x, int y) {
		return Columns[x].getLetter(y);
	}

	void addColumn() { //add new column to table
		column newColumn;
		Columns.push_back(newColumn);
	}

	void addPointToTable(int x, char _Letter) { //add point to bottom of the x-th column
		Columns[x].addPointToColumn(_Letter);
	}

	void toTheTop(int x, int y) { //move the Table[x][y] point from its position to the top of the column
		Columns[x].toTheTop(y);
	}

	column& operator[] (int x) { //for access with standard syntax: Table[x][y]
		return Columns[x];
	}

	void deleteColumn(int i) { //move column consisting only of 'D' to the right
		auto it = Columns.begin() + i;
		rotate(it, it + 1, Columns.end()); 
	}

};

struct point {
	int x;
	int y;

	point(int _x, int _y) : x(_x), y(_y) {}

	int getX() { return x; }

	int getY() { return y; }
};

table Table;

class cluster {
private:
	int minY;
	int minX;
	char color;
	vector <point> Points;
public:
	cluster(char _color) : minY(height), minX(width), color(_color) {}

	void addPointToCluster(int x, int y) {
		if (minY > y) minY = y;
		if (minX > x) minX = x;
		visited[x][y] = true;
		point tmpPoint(x, y);
		Points.push_back(tmpPoint);
	}

	char getColor() { return color; }

	int getSize() { return Points.size(); }

	int getMinY() { return minY; }

	int getMinX() { return minX; }

	bool formCluster(int x, int y) {
		addPointToCluster(x, y);
		bool check = false; //is there at least 2 Points in cluster
		//up
		if (y + 1 < height) {
			if (!visited[x][y + 1] && Table[x][y + 1] == color) {
				check = true;
				formCluster(x, y + 1);
			}
		}
		//down
		if (y - 1 > -1) {
			if (!visited[x][y - 1] && Table[x][y - 1] == color) {
				check = true;
				formCluster(x, y - 1);
			}
		}
		//left
		if (x - 1 > -1) {
			if (!visited[x - 1][y] && Table[x - 1][y] == color) {
				check = true;
				formCluster(x - 1, y);
			}
		}
		//right
		if (x + 1 < width) {
			if (!visited[x + 1][y] && Table[x + 1][y] == color) {
				check = true;
				formCluster(x + 1, y);
			}
		}
		return check;
	}

	void deleteCluster() {
		Points.clear();
	}

	point& operator[] (int i) {
		return Points[i];
	}
};

vector <cluster> Clusters;

bool formClusters();
int maxClusterId();
int  deleteCluster(int id);
void deleteEmptyColumns();
void reorderColumns();
void freeClusters();
void setVisits();
void doWork();
void isInputCorrect();

int main() {
	
	isInputCorrect();
	
	for (int i = 0; i < width; i++) //initialization of table
		Table.addColumn();

	visited = new bool* [width]; //massive containing information about was the cell visited ot not
	for (int i = 0; i < width; i++) 
		visited[i] = new bool[height];

	int countOfGames;
	fin >> countOfGames;
	fin.get(); //skip '\n'
	for (int i = 0; i < countOfGames; i++) {
		fout << "Game " << i + 1 << ":\n";
		doWork();
	}

	for (int i = 0; i < width; i++) 
		delete[] visited[i];
	delete [] visited;

	fin.close();
	fout.close();

	return 0;
}

bool formClusters() {
	bool wasAnythingDone = false;
	bool isItCluster = false;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (visited[j][i])
				continue;
			cluster newCluster(Table[j][i]);
			isItCluster = newCluster.formCluster(j, i);
			if (isItCluster) {
				Clusters.push_back(newCluster);
				wasAnythingDone = true;
			}
		};
	}
	return wasAnythingDone;
}

int maxClusterId() { //returns id of the biggest (the leftest and the lowest) cluster
	int maxNumOfPoints = 0;
	int id = -1, bottom_point = height, left_point = width;
	for (int i = 0; i < Clusters.size(); i++) {
		if (maxNumOfPoints < Clusters[i].getSize()) {
			maxNumOfPoints = Clusters[i].getSize();
			id = i;
			left_point = Clusters[i].getMinX();
			bottom_point = Clusters[i].getMinY();
			continue;
		}

		if (maxNumOfPoints == Clusters[i].getSize()) {//if two Clusters have same number of Points
			if (left_point == Clusters[i].getMinX()) { //if the leftest Points are same
				if (bottom_point > Clusters[i].getMinY()) {
					id = i;
					bottom_point = Clusters[i].getMinY();
					continue;
				}
			}
			else { //if the leftest Points are differnt
				if (left_point > Clusters[i].getMinX()) {
					id = i;
					left_point = Clusters[i].getMinX();
					bottom_point = Clusters[i].getMinY();
					continue;
				}
			}
		}
	};
	return id;
}

void deleteEmptyColumns() { //find and remove all colums consisting of 'D'
	bool isColumnEmpty;
	for (int i = 0; i < width; i++) {
		isColumnEmpty = true;
		for (int j = 0; j < height; j++) {
			if (Table[i][j] != 'D')
				isColumnEmpty = false;
		}
		if (isColumnEmpty) {
			Table.deleteColumn(i);
		}
	}
}

void reorderColumns() { //"shakes" all 'D'-cells down the column, saving the order
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (Table[i][j] == 'D') { //if we see 'D'
				for (int k = 1; k + j < height; k++) { //we goes look upper it
					if (Table[i][j + k] != 'D') { //to find 'R' 'G' or 'B'
						Table.toTheTop(i, j); //if we find one of them, we raise 'D' to the top
						j--; //decrease the iterator of height to check this cell again
						break; //and goes to the next iteration
					}
				}
			}
		}
	}
}

int  deleteCluster(int id) { //delete id-th cluster and move table elements
	for (int i = 0; i < Clusters[id].getSize(); i++)
		Table[Clusters[id][i].getX()][Clusters[id][i].getY()] = 'D';
	deleteEmptyColumns();
	reorderColumns();
	return Clusters[id].getSize();
}

void freeClusters() { //delete cluster (and free memory)
	for (int i = 0; i < Clusters.size(); i++) {
		Clusters[i].deleteCluster();
	}
	Clusters.clear();
}

void setVisits() { //marks all 'D'-cells in table with 1, other - with 0
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (Table[i][j] == 'D')
				visited[i][j] = true;
			else
				visited[i][j] = false;
		}
	}
}

void doWork() {
	char tmp;
	for (int i = 0; i < height; i++) { //input
		for (int j = 0; j < width; j++) {
			fin.get(tmp);
			Table.addPointToTable(j, tmp);
		}
		fin.get(); //skip '\n'
	}

	score Score;
	int maxId;
	bool wasAnythingDone; //marker that checks if at least one cluster was found on the iteration
	for (int i = 0; ; i++) {
		setVisits();
		wasAnythingDone = formClusters();
		if (!wasAnythingDone) {
			Score.finalScore();
			break;
		}
		maxId = maxClusterId();
		Score.increaseAndOut(i, Clusters[maxId].getSize(), Clusters[maxId].getMinX(), Clusters[maxId].getMinY(), Clusters[maxId].getColor());
		deleteCluster(maxId);
		freeClusters();
	}
}

void isInputCorrect() {
	try {
		int countOfGames;
		fin >> countOfGames;
		if (countOfGames < 1)
			throw 1;
		char tmp;
		fin.get(tmp);
		if (tmp != '\n')
			throw 6;
		for (int it = 0; it < countOfGames; it++) {
			for (int i = 0; i < height; i++) {
				int j;
				for (j = 0; j < width; j++) {
					fin.get(tmp);
					if (fin.eof())
						throw 2;
					if (tmp != 'R' && tmp != 'G' && tmp != 'B') {
						if (tmp == '\n')
							throw 2;
						else
							throw 3;
					}
				}
				fin.get(tmp);
				if (i < height - 1 || countOfGames < it + 1) {
					if (fin.eof())
						throw 5;
					if (tmp != '\n') {
						if (tmp != 'R' && tmp != 'G' && tmp != 'B')
							throw 3;
						else
							throw 4;
					}
				}
			}
		}
		if (!fin.eof())
			throw 7;
		fin.clear();
		fin.seekg(0);
	}
	catch (int errId) {
		cout << "The input is incorrect. Probably ";
		switch (errId) {
		case 1:
			cout << "incorrect of games given.\n";
			break;
		case 2:
			cout << "there is too short string.\n";
			break;
		case 3:
			cout << "there is invalid character.\n";
			break;
		case 4:
			cout << "there is too long string.\n";
			break;
		case 5:
			cout << "there is too few lines.\n";
			break;
		case 6:
			cout << "there is no '\\n' after count of games.\n";
			break;
		case 7:
			cout << "there are extra characters.\n";
			break;
		default:
			break;
		}
		exit(errId);
	}
	
}
