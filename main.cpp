#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <cstdlib>
#include <queue>
#include <limits>

using namespace std;


class problem;
struct operators;
struct Node;
struct result;

typedef vector<vector<int>> board;
typedef queue<Node> nodes;
typedef vector<board> history;
typedef priority_queue<Node> Nodes;

vector<result> presetResults;


// Solvable boards
board preset1 {{8,0,4},{3,6,5},{7,2,1}};
board preset2 {{1,7,2},{5,6,8},{4,3,0}};
board preset3 {{0,2,6},{4,3,7},{1,5,8}};
board preset4 {{1,3,0},{5,8,7},{6,2,4}};
board preset5 {{5,6,0},{3,4,2},{8,7,1}};
board preset6 {{8,2,5},{3,4,6},{0,1,7}};
board preset7 {{7,6,1},{0,2,3},{4,8,5}};
board preset8 {{3,8,5},{0,4,2},{7,1,6}};
board preset9 {{5,8,2},{6,1,7},{0,3,4}};
board preset10{{8,2,6},{5,1,7},{4,3,0}};

board presetUnsolvable{{1, 7, 3},{4, 6, 5},{8, 2, 0}}; //impossible
board presetError{{1, 0, 7},{4, 6, 3},{8, 2, 5}}; //for debugging
//board preset1{{1,2,3},{4,5,6},{7,8,0}}; //solved
//board preset1{{1, 2, 3},{4, 8, 0},{7, 6, 5}}; //few moves req
board falseBoard{{-1, -1, -1},{-1, -1, -1},{-1, -1, -1}};
history boardHistory;
char algChoice = '0';
int numNodesExpanded = 0;
int maxInQueue = 0;

void newGame();
void startGame();
void solve( problem& );
void solveHeuristically( problem& );
operators expand(board , operators& );
char getBoardChoice();
char getAlgorithmChoice();
vector< int > setVecChoice(int, board);
board getBoard( );
board initBoard();
bool isNum(string );
bool isUnique(int , const board );
void display( board );
int manhattanDist( board );
bool isInRightSpot( int , int , int  );
int getCorrectVal( int , int );
int howFar(int,  int , int );
bool foundDups( vector<int> );
bool goalTest( board );
nodes queueingFunc(nodes , operators );
bool canMoveUp(board );
bool canMoveLeft(board );
bool canMoveDown(board );
bool canMoveRight(board );
board moveUp(board );
board moveLeft(board );
board moveDown(board );
board moveRight(board );
void findThisVal(board , int , int& , int& );
bool boardEquals(board, board);
board minCost( operators , int , int , int , int  );
int minNum(int , int , int , int );
void display(operators );
bool isUnique(board );
void success();
operators getOps(Node );
void failure();
Nodes queueingFunc(Nodes , operators );
int misplacedDist( board );
void displayResults();
void resetData();

struct result {
	int nodesExpanded = 0;
	int maxQueueSize = 0;
	
	result(int a, int b): nodesExpanded(a), maxQueueSize(b) {}
};

struct operators {
	board emptyUp;
	board emptyDown;
	board emptyLeft;
	board emptyRight;
};

struct Node {
	int totalDistance = numeric_limits<int>::max();
	board puzzleBoard;
	operators ops;
	
	Node(board a): puzzleBoard(a){}
	Node(int dist, board a): totalDistance(dist), puzzleBoard(a){}
	
};

bool operator<(const Node& a, const Node& b) {
	if(a.totalDistance != b.totalDistance)
		return a.totalDistance > b.totalDistance; 
}

class problem {
	private:
		board puzzleBoard;
		operators ops;
		char algorithm;
		
	public:
		problem( char alg , board board )
		: algorithm(alg) , puzzleBoard(board) { algChoice = algorithm; }
		
		int mainDriver();
		
		board getBoard()   { return puzzleBoard; }
		char getAlg()	   { return algorithm;   }
		operators getOps(board node) { 
			int counter = 0;
			if( canMoveUp(node) && isUnique(moveUp(node))){
				counter++;
				ops.emptyUp = moveUp(node);
			} else {
				ops.emptyUp = falseBoard;
			}
			
			if( canMoveLeft(node) && isUnique(moveLeft(node))){
				counter++;
				ops.emptyLeft = moveLeft(node);
			} else {
				ops.emptyLeft = falseBoard;
			}
			
			if( canMoveDown(node) && isUnique(moveDown(node))){
				counter++;
				ops.emptyDown = moveDown(node);
			} else {
				ops.emptyDown = falseBoard;
			}
			
			if( canMoveRight(node) && isUnique(moveRight(node))){
				counter++;
				ops.emptyRight = moveRight(node);
			} else {
				ops.emptyRight = falseBoard;
			}
			return ops;
		
		}
};

int main () {
	
	newGame();	
	//startGame();
	
	return 0;
}

void newGame() {
	
	// Step 1 - Problem setup
	problem currentProb( getAlgorithmChoice() ,  getBoard() );
	
	if(algChoice == '1'){
		solve( currentProb );
	} else {
		// Step 2 - Solve problem
		solveHeuristically( currentProb );
	}
	
}

void solveHeuristically( problem& currentBoard ) {
		
	// Step 1 - Setup queue with prob. intial state
	Node initialNode(0, currentBoard.getBoard());
	
	if(algChoice == '2'){
	 initialNode.totalDistance = misplacedDist(currentBoard.getBoard());
	 cout << "INITIAL BOARD HAS A DISTANCE OF " << misplacedDist(initialNode.puzzleBoard) << " FOR: " << endl;
    }
	
	if(algChoice == '3'){
	 initialNode.totalDistance = manhattanDist(currentBoard.getBoard());
	 cout << "INITIAL BOARD HAS A DISTANCE OF " << manhattanDist(initialNode.puzzleBoard) << " FOR: " << endl;
	}
	
	Nodes boardQueue; 
	boardQueue.push(initialNode);
	boardHistory.push_back(initialNode.puzzleBoard);
	
	
	display( initialNode.puzzleBoard );
	
	while( true ) {
		cout << "=============" << endl;
		if( boardQueue.empty() ) {
			cout << "No solution found!" << endl; 
			return;
		}
		
		Node parent = boardQueue.top();
		numNodesExpanded++;
		
		if( goalTest(parent.puzzleBoard) == true ) {
			success();
			return;
		}
		
		operators possibleOperations;
		
		boardQueue = queueingFunc( boardQueue, expand(parent.puzzleBoard, possibleOperations) );
				
		cout << "TAKEN ROUTE: " << endl;
		display(boardQueue.top().puzzleBoard);
		cout << "QUEUE SIZE: " << boardQueue.size() << endl;
		
	}
	
}

operators expand(board node, operators& ops) {
	 ops = getOps(node);
	return ops;
}

operators expand(Node parent, operators& ops) {
	ops = getOps(parent);
	return ops;	
}

Nodes queueingFunc(Nodes boardQueue, operators possibleOps) {
	
	if( algChoice == '1' ){
		boardQueue.pop();
	
		int costUp = numeric_limits<int>::max();
		int costLeft = numeric_limits<int>::max();
		int costDown = numeric_limits<int>::max();
		int costRight = numeric_limits<int>::max();
		
			if( boardEquals(possibleOps.emptyUp, falseBoard) == false ){
				costUp = manhattanDist(possibleOps.emptyUp);
				boardQueue.push( possibleOps.emptyUp );
			}
			if( boardEquals(possibleOps.emptyLeft, falseBoard) == false ){
				costLeft = manhattanDist(possibleOps.emptyLeft);
				boardQueue.push( possibleOps.emptyLeft );
			}
				
			if( boardEquals(possibleOps.emptyDown, falseBoard) == false ){
				costDown = manhattanDist(possibleOps.emptyDown);
				boardQueue.push( possibleOps.emptyDown );
			}
			
			if( boardEquals(possibleOps.emptyRight, falseBoard) == false ){
				costRight = manhattanDist(possibleOps.emptyRight);
				boardQueue.push( possibleOps.emptyRight );
			}
		
		boardQueue.push( minCost(possibleOps, costUp, costLeft, costDown, costRight) );
		boardHistory.push_back(boardQueue.top().puzzleBoard);
		
		if(boardQueue.size() > maxInQueue)
			maxInQueue = boardQueue.size();
			
		return boardQueue;
	
	}
	
	if( algChoice == '2' ) {
		
		Node parent = boardQueue.top(); boardQueue.pop();
		
		Node opUp(misplacedDist(possibleOps.emptyUp), possibleOps.emptyUp);
		
		if( boardEquals(opUp.puzzleBoard, falseBoard) == false){
		cout << misplacedDist(possibleOps.emptyUp)<< " is the cost of up" << endl;
			boardQueue.push(opUp);
			boardHistory.push_back(opUp.puzzleBoard);
		}
			
		Node opLeft(misplacedDist(possibleOps.emptyLeft), possibleOps.emptyLeft);
		
		if( boardEquals(opLeft.puzzleBoard, falseBoard) == false){
		cout << misplacedDist(possibleOps.emptyLeft)<< " is the cost of left" << endl;
			boardQueue.push(opLeft);
			boardHistory.push_back(opLeft.puzzleBoard);
		}
			
		Node opDown(misplacedDist(possibleOps.emptyDown), possibleOps.emptyDown);
		
		if( boardEquals(opDown.puzzleBoard, falseBoard) == false){
		cout << misplacedDist(possibleOps.emptyDown)<< " is the cost of down" << endl;
			boardQueue.push(opDown);
			boardHistory.push_back(opDown.puzzleBoard);
		}
		
		Node opRight(misplacedDist(possibleOps.emptyRight), possibleOps.emptyRight);
		
		if( boardEquals(opRight.puzzleBoard, falseBoard) == false){
		cout << misplacedDist(possibleOps.emptyRight)<< " is the cost of right" << endl;
			boardQueue.push(opRight);
			boardHistory.push_back(opRight.puzzleBoard);
		}
		
		Nodes tempQueue = boardQueue;
		
		
		if(boardQueue.size() > maxInQueue)
			maxInQueue = boardQueue.size();
			
		return boardQueue;	
		
	}
	
	else if( algChoice == '3' ) {
		Node parent = boardQueue.top(); boardQueue.pop();
		
		Node opUp(manhattanDist(possibleOps.emptyUp), possibleOps.emptyUp);
		
		if( boardEquals(opUp.puzzleBoard, falseBoard) == false){
		cout << manhattanDist(possibleOps.emptyUp)<< " is the cost of up" << endl;
			boardQueue.push(opUp);
			boardHistory.push_back(opUp.puzzleBoard);
		}
			
		Node opLeft(manhattanDist(possibleOps.emptyLeft), possibleOps.emptyLeft);
		
		if( boardEquals(opLeft.puzzleBoard, falseBoard) == false){
		cout << manhattanDist(possibleOps.emptyLeft)<< " is the cost of left" << endl;
			boardQueue.push(opLeft);
			boardHistory.push_back(opLeft.puzzleBoard);
		}
			
		Node opDown(manhattanDist(possibleOps.emptyDown), possibleOps.emptyDown);
		
		if( boardEquals(opDown.puzzleBoard, falseBoard) == false){
		cout << manhattanDist(possibleOps.emptyDown)<< " is the cost of down" << endl;
			boardQueue.push(opDown);
			boardHistory.push_back(opDown.puzzleBoard);
		}
		
		Node opRight(manhattanDist(possibleOps.emptyRight), possibleOps.emptyRight);
		
		if( boardEquals(opRight.puzzleBoard, falseBoard) == false){
		cout << manhattanDist(possibleOps.emptyRight)<< " is the cost of right" << endl;
			boardQueue.push(opRight);
			boardHistory.push_back(opRight.puzzleBoard);
		}
		
		Nodes tempQueue = boardQueue;
		
		
		if(boardQueue.size() > maxInQueue)
			maxInQueue = boardQueue.size();
			
		return boardQueue;	
	
	}
	
	return boardQueue;	
	
}

nodes queueingFunc(nodes boardQueue, operators possibleOps) {
	
	boardQueue.pop();
	
	int costUp = numeric_limits<int>::max();
	int costLeft = numeric_limits<int>::max();
	int costDown = numeric_limits<int>::max();
	int costRight = numeric_limits<int>::max();
	
		if( boardEquals(possibleOps.emptyUp, falseBoard) == false ){
			costUp = manhattanDist(possibleOps.emptyUp);
		}
		if( boardEquals(possibleOps.emptyLeft, falseBoard) == false ){
			costLeft = manhattanDist(possibleOps.emptyLeft);
		}
			
		if( boardEquals(possibleOps.emptyDown, falseBoard) == false ){
			costDown = manhattanDist(possibleOps.emptyDown);
		}
		
		if( boardEquals(possibleOps.emptyRight, falseBoard) == false ){
			costRight = manhattanDist(possibleOps.emptyRight);
		}
	
	boardQueue.push( minCost(possibleOps, costUp, costLeft, costDown, costRight) );
	boardHistory.push_back(boardQueue.front().puzzleBoard);
	
	if(boardQueue.size() > maxInQueue)
		maxInQueue = boardQueue.size();
		
	return boardQueue;	
	
}

//How should I handle cost ties?
board minCost( operators ops, int costUp, int costLeft, int costDown, int costRight ) {
	
	int min = numeric_limits<int>::max();
	
	if( costUp == minNum(costUp, costLeft, costDown, costRight) && boardEquals(ops.emptyUp,falseBoard) == false )
		return ops.emptyUp;
		
	if( costLeft == minNum(costUp, costLeft, costDown, costRight) && boardEquals(ops.emptyLeft,falseBoard) == false )
		return ops.emptyLeft;
		
	if( costDown == minNum(costUp, costLeft, costDown, costRight) && boardEquals(ops.emptyDown,falseBoard) == false )
		return ops.emptyDown;
	
	if( costRight == minNum(costUp, costLeft, costDown, costRight) && boardEquals(ops.emptyRight,falseBoard) == false )
		return ops.emptyRight;
	return falseBoard;
	
}

void startGame() {
	
	vector<board> boards{preset1,preset2,preset3,preset4,preset5,preset6,preset7,preset8,preset9,preset10};
	
	char algorithmChoice = getAlgorithmChoice();
	
	for(unsigned i = 0 ; i < boards.size() ; i ++) {
		
		board puzzleBoard = boards[i];
		
		problem a(algorithmChoice, puzzleBoard);
		
		if(algChoice == '1'){
			solve( a );
		} else {
			// Step 2 - Solve problem
			solveHeuristically( a );
		}
		result b(numNodesExpanded, maxInQueue);
		presetResults.push_back(b);
		resetData();
	}
	
	displayResults();
	
}

char getBoardChoice() {
	char a;
	while(a != '1' && a != '2') {
		cout << "Welcome to Hamza's 8-Puzzle Solver" << endl;
		cout << "Press '1' to use the preset puzzle(s), or '2' to enter your a custom puzzle." << endl;
		cin >> a; 
	}
	cin.ignore();
	return a;
}

board getBoard() {
	
	char choice = getBoardChoice();
	
	if( choice == '1' ){
		return preset1;
	}
	else if( choice == '2' ){
		return initBoard();
	}
	
}

char getAlgorithmChoice() {
	char a = '0';
	while(a != '1' && a != '2'&& a != '3') {
		cout << "Enter your choice of algorithm:" << endl
			 << "1. Uniform Cost Search" << endl
			 << "2. A* with the Misplaced Tile heuristic" << endl
			 << "3. A* with the Manhattan Distance heuristic" << endl;
		cin >> a; 
	}
	cin.ignore();
	return a;
}

board initBoard() {
	board puzzleBoard(3);
	cout << "Enter your puzzle, use a zero to represent the blank space" << endl;
	for( unsigned i = 0; i < puzzleBoard.size(); i++ ){
		puzzleBoard[i] = setVecChoice(i, puzzleBoard);
		
	}
	return puzzleBoard;
}

vector< int > setVecChoice (int index, board takenNums){
	
resetLoop:

	vector<int> puzzleRow;
	string inputRow;
	string temp;
	
	int counter = 0;


	cout << "For row #" << index + 1 << ", input three numbers separated by whitespace: " << endl;
	
	getline(cin, inputRow);
	stringstream tokenizer(inputRow);
	
	while( getline(tokenizer , temp , ' ') ){
		if(isNum(temp) && isUnique(stoi(temp), takenNums)){
			
			puzzleRow.push_back(stoi(temp));
		
		}
		
		else {
			cout << "DIDN'T ENTER 3 UNIQUE DIGITS BETWEEN 0 and 8" << endl;
			goto resetLoop;
		}
		
		counter++;
	}
	if( counter != 3 || foundDups(puzzleRow) ){
		puzzleRow.clear();
		goto resetLoop;
	}
	
	
	return puzzleRow;
}

bool foundDups( vector<int> puzzleRow ) {
	for(unsigned i = 0; i < puzzleRow.size(); i ++){
		for(unsigned j = i + 1; j < puzzleRow.size(); j ++){
			if(puzzleRow[i] == puzzleRow[j])
				return true;
		}
	}
	return false;
}

bool isNum(string a) {
	if(a == "0" || a == "1" || a == "2" || a == "3" 
	|| a == "4" || a == "5" || a == "6" || a == "7"
	|| a == "8")
		return true;
	
	return false;
}

bool isUnique(int a, board takenNums) {
	for( unsigned i = 0 ; i < takenNums.size() ; i ++ ) {
		
		for( unsigned j = 0 ; j < takenNums[i].size() ; j ++ ) {
			
			if( a == takenNums[i][j] )
				return false;
				
		}
		
	}
	
	return true;
}

void display( board B ) {
	for( unsigned i = 0 ; i < B.size() ; i ++ ) {
		cout << "Row#" << i + 1 << ": ";
		for( unsigned j = 0 ; j < B[i].size() ; j ++ ) {
			cout << B[i][j] << " ";
		}
		cout << endl;
	}
}

int manhattanDist( board B ) {
	
	/*
	 * 
	 * 1 2 3
	 * 4 5 6
	 * 7 8 0
	 * 
	 */
	 
	int dist = 0;
	
	for( unsigned i = 0 ; i < B.size() ; i ++ ){
		
		for( unsigned j = 0 ; j < B[i].size() ; j ++ ){
			
			int currVal = B[i][j];
			int row = i; 
			int col = j;
			
			if( isInRightSpot( currVal, row, col) == false )
				dist += howFar(currVal, row, col);
			
			else
				continue;
		}
		
	}
	
	return dist;
	
}

bool isInRightSpot( int currVal, int row, int col ) {
	int correctVal = getCorrectVal(row, col);
	return (currVal == correctVal) ? true : false;
}

int getCorrectVal( int row, int col ) {
	return ( row == 0 ) ? row + col + pow(2,row) : fmod(row + col + pow(2,row) + 1, 9);
}

int howFar( int currVal, int row, int col ) {
	
	int targetRow, targetCol = 0;
	
	//Get target row
	if(currVal < 4)
		targetRow = 0;
	else if(currVal < 7)
		targetRow = 1;
	else 
		targetRow = 2;
	
	//Get target col	
	if( isInRightSpot(currVal, targetRow, 0) )
		targetCol = 0;
	else if( isInRightSpot(currVal, targetRow, 1) )
		targetCol = 1;
	else
		targetCol = 2;
		
	return abs(row - targetRow) + abs(col - targetCol);
}

bool goalTest( board puzzleBoard ) {
	for( unsigned i = 0 ; i < puzzleBoard.size() ; i ++ ){

		for( unsigned j = 0 ; j < puzzleBoard.size() ; j ++ ){
			
			int currVal = puzzleBoard[i][j];
			int row = i; 
			int col = j;
			
			if( isInRightSpot( currVal, row, col) == false ){
				return false;
			}
			
			else
				continue;
		}
	
	}
	return true;
}

void findThisVal(board node, int val, int& row, int& col){
	for(unsigned i = 0; i < node.size(); i ++) {
		for(unsigned j = 0; j < node[i].size(); j++) {
			if(node[i][j] == val) {
				row = i;
				col = j;
				return;
			}
		}
	}
	
}

bool canMoveUp(board node){
	int row = 0;
	int col = 0;
	
	findThisVal(node, 0, row, col);
	
	return (row > 0) ? true : false;
	
}

bool canMoveLeft(board node){
	int row = 0;
	int col = 0;
	
	findThisVal(node, 0, row, col);
	
	return (col > 0) ? true : false;
}

bool canMoveDown(board node){
	int row = 0;
	int col = 0;
	
	findThisVal(node, 0, row, col);
	
	return (row < 2) ? true : false;
}

bool canMoveRight(board node) {
	int row = 0;
	int col = 0;
	
	findThisVal(node, 0, row, col);
	
	return (col < 2) ? true : false;
}

board moveUp(board node) {
	
	int row = 0;
	int col = 0;
	
	findThisVal(node, 0, row, col);
	
	int victimRow = row - 1;
	int victimCol = col;
	
	int victimVal = node[victimRow][victimCol];
	
	int tempVal = node[row][col];
	node[row][col] = victimVal;
	node[victimRow][victimCol] = tempVal;
	
	return node;
}
board moveLeft(board node) {
	int row = 0;
	int col = 0;
	
	findThisVal(node, 0, row, col);
	
	int victimRow = row;
	int victimCol = col - 1;
	
	int victimVal = node[victimRow][victimCol];
	
	int tempVal = node[row][col];
	node[row][col] = victimVal;
	node[victimRow][victimCol] = tempVal;
	
	return node;
}
board moveDown(board node) {
	int row = 0;
	int col = 0;
	
	findThisVal(node, 0, row, col);
	
	int victimRow = row + 1;
	int victimCol = col;
	
	int victimVal = node[victimRow][victimCol];
	
	int tempVal = node[row][col];
	node[row][col] = victimVal;
	node[victimRow][victimCol] = tempVal;
	
	return node;
}
board moveRight(board node) {
	int row = 0;
	int col = 0;
	
	findThisVal(node, 0, row, col);
	
	int victimRow = row;
	int victimCol = col + 1;
	
	int victimVal = node[victimRow][victimCol];
	
	int tempVal = node[row][col];
	node[row][col] = victimVal;
	node[victimRow][victimCol] = tempVal;
	
	return node;
}

bool boardEquals(board a, board b) {
	for(unsigned i = 0; i < a.size(); i ++) {
		for(unsigned j = 0; j < a[i].size(); j++) {
			if(a[i][j] != b[i][j])
				return false;
		}
	}
	return true;
}

int minNum(int a, int b, int c, int d){
	return min( min( min(a,b),c),d);
}

void display(operators a){
	if( boardEquals(a.emptyUp, falseBoard) == false ){
		cout << "Move 0 up" << endl;
		display(a.emptyUp);
	}
	if( boardEquals(a.emptyLeft, falseBoard) == false ){
		cout << "Move 0 left" << endl;
		display(a.emptyLeft);
	}
	if( boardEquals(a.emptyDown, falseBoard) == false ){
		cout << "Move 0 down" << endl;
		display(a.emptyDown);
	}
	if( boardEquals(a.emptyRight, falseBoard) == false ){
		cout << "Move 0 right" << endl;
		display(a.emptyRight);
	}
}

bool isUnique(board a){
	for(unsigned i = 0; i < boardHistory.size(); i ++ ){
		if( boardEquals( boardHistory[i], a ) ){
			return false;
		}
	}
	return true;
}

void success() {
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	cout << "Solved!" << endl;
	cout << "To solve this problem, the search algorithm expanded a total of " << numNodesExpanded << " nodes." << endl;
	cout << "The maximum number of nodes in the queue at any one time was " << maxInQueue << "." << endl;
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
}

operators getOps(Node node) { 
	int counter = 0;
	if( canMoveUp(node.puzzleBoard) && isUnique(moveUp(node.puzzleBoard))){
		counter++;
		node.ops.emptyUp = moveUp(node.puzzleBoard);
	} else {
		node.ops.emptyUp = falseBoard;
	}
	
	if( canMoveLeft(node.puzzleBoard) && isUnique(moveLeft(node.puzzleBoard))){
		counter++;
		node.ops.emptyLeft = moveLeft(node.puzzleBoard);
	} else {
		node.ops.emptyLeft = falseBoard;
	}
	
	if( canMoveDown(node.puzzleBoard) && isUnique(moveDown(node.puzzleBoard))){
		counter++;
		node.ops.emptyDown = moveDown(node.puzzleBoard);
	} else {
		node.ops.emptyDown = falseBoard;
	}
	
	if( canMoveRight(node.puzzleBoard) && isUnique(moveRight(node.puzzleBoard))){
		counter++;
		node.ops.emptyRight = moveRight(node.puzzleBoard);
	} else {
		node.ops.emptyRight = falseBoard;
	}
	cout << "NEW POSSIBILITIES: " << counter << endl;
	return node.ops;

}

void failure() {
	cout << "No solution found!" << endl; 
	cout << "The search algorithm expanded a total of " << numNodesExpanded << " nodes." << endl;
	cout << "The maximum number of nodes in the queue at any one time was " << maxInQueue << "." << endl;
	cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
}

int misplacedDist( board a){
	int count = 0;
	for(unsigned i = 0 ; i < a.size() ; i ++) {
		for(unsigned j = 0; j < a[i].size(); j ++) {
			if(isInRightSpot(a[i][j],i,j) == false)
				count++;
		}
	}
	return count;
}

void solve( problem& currentBoard ) {
	
	Node initialNode(0, currentBoard.getBoard());
	cout << "INITIAL STATE WITH COST OF: " << initialNode.totalDistance << " FOR: " << endl;
	display(initialNode.puzzleBoard);
	
	Nodes boardQueue; 
	boardQueue.push(initialNode);
	boardHistory.push_back(initialNode.puzzleBoard);
	
	while( true ) {
		cout << "=============" << endl;
		if( boardQueue.empty() ) {
			cout << "No solution found!" << endl; 
			return;
		}
		
		Node parent = boardQueue.top();
		numNodesExpanded++;
		
		if( goalTest(parent.puzzleBoard) == true ) {
			success();
			return;
		}
		
		operators possibleOperations;
		
		boardQueue = queueingFunc( boardQueue, expand(parent.puzzleBoard, possibleOperations) );
				
		cout << "TAKEN ROUTE: " << endl;
		display(boardQueue.top().puzzleBoard);
		cout << "QUEUE SIZE: " << boardQueue.size() << endl;
		
	}
	
}

void displayResults() {
	//presetresults
	
	for(unsigned i = 0; i < presetResults.size(); i ++) {
		
		cout << "Preset" << i+1 << " expanded " << presetResults[i].nodesExpanded << " nodes and had a max queue size of " << presetResults[i].maxQueueSize << "." << endl;
		
	}
	
}

void resetData() {
	
	boardHistory.clear();
	numNodesExpanded = 0;
	maxInQueue = 0;
	
}

// - Hamza Awad Fall'2019
