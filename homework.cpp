#include<iostream>
#include<vector>
#include<string>
#include<set>
#include<map>
#include<fstream>
#include<cmath>
#include<climits>

using namespace std;

vector<string>board(19,"                   ");

int minValue(int alpha,int beta,int depth,int desiredDepth,bool secondMove,bool firstMove,char myMove,int lastMoveX,int lastMoveY,int grandMoveX,int grandMoveY);
int maxValue(int alpha,int beta,int depth,int desiredDepth,bool secondMove,bool firstMove,char myMove,int lastMoveX,int lastMoveY,int grandMoveX,int grandMoveY);

int evalScore = 0;

struct penteMove{
    int x,y;

    penteMove(int x,int y){
        this->x = x;
        this->y = y;
    }
};

char returnOpponent(char player){
    switch(player){
        case 'w': return 'b';
                break;
        case 'b': return 'w';
                break;
    }
    return '.';
}

set<pair<int,int> > iCaptured;
set<pair<int,int> > opponentCaptured;


string myColor;
string opponent;

char currentPlayerColor;

int white_captured,black_captured;
int i_captured, they_captured;

bool isAnyMovesLeft(){
    for(int i=0;i<19;i++){
        for(int j=0;j<19;j++){
            if(board[i][j]=='.'){
                return true;
            }
        }
    }
    return false;
}

bool isHorizontalMoveSafe(int moveX,int moveY){
    if(moveX >= 19 || moveX+1 >= 19 || moveX+2 >= 19 || moveX+3 >= 19 || moveX+4>= 19){
        return false;
    }
    return true;
}

bool isVerticalMoveSafe(int moveX,int moveY){
    if(moveY>= 19 || moveY+1 >= 19 || moveY+2 >= 19 || moveY+3 >= 19 || moveY+4 >= 19){
        return false;
    }
    return true;

}

bool isSafe(int coord){
    if(coord >= 19 || coord < 0){
        return false;
    }
    return true;
}

bool isPointSafe(int x,int y,bool secondMove = false){
    if(secondMove){
        if((x >= 7 && x <= 11) && (y>=7 && y<=11)){
            return false;
        }
        else{
            if(x>=0 && x<19 && y>=0 && y<19){
                return true;
            }
            return false;
        }
    }
    if(x>=0 && x<19 && y>=0 && y<19){
        return true;
    }
    return false;
}

/**
0 - continous 5
1 - open 4
2 - open 3
3 - open 2
4 - open 1
 * Difference in coins captured.
*/

int returnScoreClass(int count,int leftDotCount,int rightDotCount,char colorCoin){
    if(count==5){
        return 0;
    }
    if(count == 4 &&((leftDotCount>=1)&&(rightDotCount>=1))){
        //It is open on both ends, I can chose to expand either way. Atleast one on both sides.
        return 1;
    }
    if(count == 3 && ((leftDotCount>=2 && rightDotCount>=1) || (leftDotCount>=1 && rightDotCount>=2))){
        return 2;
    }
    /*
    Not returning for open 2 and open 1. Only series 5,open4,open 3.
    if(count == 2 && count+dotCount >=5 ){
        return 3;
    }
    if(count == 1 && count+dotCount>=5){
        return 4;
    }*/
    return 3;
}


int checkVerticalGrowth(int lastMoveX,int lastMoveY){
    int count = 1;
    bool breakLeftGrowth = false;
    int leftDotCount = 0;
    int rightDotCount = 0;
    //Rationale: I'll be making a call from the last move made. It can be mine or my opponent. The pattern across is it generalised. Grow till blocked by an opponent.
    //Count how many coins I have already placed and how many I can do. See how far I'm away from the win.
    //At the end of the growth, see what coin's growth was checked and based on this return the point -> Return the character that made this growth. Check in evaluation function.
    char colorCoin = board[lastMoveX][lastMoveY];
    char opponent = returnOpponent(colorCoin);
    bool coinUpGrowthBreak = false;
    bool coinDownGrowthBreak = false;
    bool stopCheckingMyColor = false;
    

    for(int i=1;i<5 && !breakLeftGrowth;i++){
        if(count == 5){
            return 0;
        }
        if(isSafe(lastMoveX-i)){
            if(board[lastMoveX-i][lastMoveY]== returnOpponent(colorCoin)){
                breakLeftGrowth = true;
                continue;
            }
            //safe to grow.
            //Can I grow my consecutive coin count?
            //lastMoveX,lastMoveY will always be a coin, because a move was just made here.
            if(board[lastMoveX-i][lastMoveY] == board[lastMoveX][lastMoveY] && !coinUpGrowthBreak){
                count++;
            }
            else if(board[lastMoveX-i][lastMoveY]=='.'){
                leftDotCount++;
                coinUpGrowthBreak = true;
            }
        }
        else{
            //Not safe to grow.
            breakLeftGrowth = true;
        }
    }
    bool breakRightGrowth = false;
    for(int i=1;i<5 && !breakRightGrowth; i++){
        if(count == 5){
            return 0;
        }
        if(isSafe(lastMoveX+i)){
            if(board[lastMoveX+i][lastMoveY] == opponent ){
                breakRightGrowth = true;
                continue;
            }
            if(board[lastMoveX+i][lastMoveY] == board[lastMoveX][lastMoveY] && !coinDownGrowthBreak){
                count++;
            }
            else if(board[lastMoveX+i][lastMoveY]=='.'){
                rightDotCount++;
                coinDownGrowthBreak = true;
            }
        }
        else{
            breakRightGrowth = true;
        }
    }
    //cout<<"Vertical Check:"<<count<<"--"<<dotCount<<endl;
    return returnScoreClass(count,leftDotCount,rightDotCount,colorCoin);
}

int checkHorizontalGrowth(int lastMoveX,int lastMoveY){
    int count = 1;
    bool breakLeftGrowth = false;
    int leftDotCount = 0;
    int rightDotCount = 0;
    char colorCoin = board[lastMoveX][lastMoveY];
    bool coinLeftGrowthBreak = false;
    bool coinRightGrowthBreak = false;
    char opponent = returnOpponent(colorCoin);

    for(int i=1;i<5 && !breakLeftGrowth;i++){
        if(count == 5){
            return 0;
        }
        if(isSafe(lastMoveY-i)){
            if(board[lastMoveX][lastMoveY-i]== returnOpponent(colorCoin)){
                breakLeftGrowth = true;
                continue;
            }
            //safe to grow.
            //Can I grow my consecutive coin count?
            //lastMoveX,lastMoveY will always be a coin, because a move was just made here.
            if(board[lastMoveX][lastMoveY-i] == board[lastMoveX][lastMoveY] && !coinLeftGrowthBreak){
                count++;
            }
            else if(board[lastMoveX][lastMoveY-i]=='.'){
                leftDotCount++;
                coinLeftGrowthBreak = true;
            }
        }
        else{
            //Not safe to grow.
            breakLeftGrowth = true;
        }
    }

    bool breakRightGrowth = false;
    for(int i=1;i<5 && !breakRightGrowth; i++){
        if(count == 5){
            return 0;
        }
        if(isSafe(lastMoveY+i)){
            if(board[lastMoveX][lastMoveY+i] == opponent ){
                breakRightGrowth = true;
                continue;
            }
            if(board[lastMoveX][lastMoveY+i] == board[lastMoveX][lastMoveY] && !coinRightGrowthBreak){
                count++;
            }
            else if(board[lastMoveX][lastMoveY+i]=='.'){
                rightDotCount++;
                coinRightGrowthBreak = true;
            }
        }
        else{
            breakRightGrowth = true;
        }
    }
    return returnScoreClass(count,leftDotCount,rightDotCount,colorCoin);
}



int checkLeftRightDiagonal(int lastMoveX,int lastMoveY){
    int count = 1;
    bool breakLeftGrowth = false;
    int leftDotCount = 0;
    int rightDotCount = 0;
    char colorCoin = board[lastMoveX][lastMoveY];
    bool coinLeftGrowthBreak = false;
    bool coinRightGrowthBreak = false;
    char opponent = returnOpponent(colorCoin);

    for(int i=1;i<5 && !breakLeftGrowth;i++){
        if(count == 5){
            return 0;
        }
        if(isSafe(lastMoveX-i) && isSafe(lastMoveY-i)){
            if(board[lastMoveX-i][lastMoveY-i]== returnOpponent(colorCoin)){
                breakLeftGrowth = true;
                continue;
            }
            //safe to grow.
            //Can I grow my consecutive coin count?
            //lastMoveX,lastMoveY will always be a coin, because a move was just made here.
            if(board[lastMoveX-i][lastMoveY-i] == board[lastMoveX][lastMoveY] && !coinLeftGrowthBreak){
                count++;
            }
            else if(board[lastMoveX-i][lastMoveY-i]=='.'){
                leftDotCount++;
                coinLeftGrowthBreak = true;
            }
        }
        else{
            //Not safe to grow.
            breakLeftGrowth = true;
        }
    }

    bool breakRightGrowth = false;
    for(int i=1;i<5 && !breakRightGrowth; i++){
        if(count == 5){
            return 0;
        }
        if(isSafe(lastMoveX+i) && isSafe(lastMoveY+i)){
            if(board[lastMoveX+i][lastMoveY+i] == opponent ){
                breakRightGrowth = true;
                continue;
            }
            if(board[lastMoveX+i][lastMoveY+i] == board[lastMoveX][lastMoveY] && !coinRightGrowthBreak){
                count++;
            }
            else if(board[lastMoveX+i][lastMoveY+i]=='.'){
                rightDotCount++;
                coinRightGrowthBreak = true;
            }
        }
        else{
            breakRightGrowth = true;
        }
    }
    return returnScoreClass(count,leftDotCount,rightDotCount,colorCoin);   
}

int checkRightLeftDiagonal(int lastMoveX, int lastMoveY){

    int count = 1;
    bool breakLeftGrowth = false;
    int leftDotCount = 0;
    int rightDotCount = 0;
    char colorCoin = board[lastMoveX][lastMoveY];
    bool coinLeftGrowthBreak = false;
    bool coinRightGrowthBreak = false;
    char opponent = returnOpponent(colorCoin);

    for(int i=1;i<5 && !breakLeftGrowth;i++){
        if(count == 5){
            return 0;
        }
        if(isSafe(lastMoveX+i) && isSafe(lastMoveY-i)){
            if(board[lastMoveX+i][lastMoveY-i]== returnOpponent(colorCoin)){
                breakLeftGrowth = true;
                continue;
            }
            //safe to grow.
            //Can I grow my consecutive coin count?
            //lastMoveX,lastMoveY will always be a coin, because a move was just made here.
            if(board[lastMoveX+i][lastMoveY-i] == board[lastMoveX][lastMoveY] && !coinLeftGrowthBreak){
                count++;
            }
            else if(board[lastMoveX+i][lastMoveY-i]=='.'){
                leftDotCount++;
                coinLeftGrowthBreak = true;
            }
        }
        else{
            //Not safe to grow.
            breakLeftGrowth = true;
        }
    }

    bool breakRightGrowth = false;
    for(int i=1;i<5 && !breakRightGrowth; i++){
        if(count == 5){
            //if(colorCoin== currentPlayerColor){
                return 0;
            //}
            
        }
        if(isSafe(lastMoveX-i) && isSafe(lastMoveY+i)){
            if(board[lastMoveX-i][lastMoveY+i] == opponent ){
                breakRightGrowth = true;
                continue;
            }
            if(board[lastMoveX-i][lastMoveY+i] == board[lastMoveX][lastMoveY] && !coinRightGrowthBreak){
                count++;
            }
            else if(board[lastMoveX-i][lastMoveY+i]=='.'){
                rightDotCount++;
                coinRightGrowthBreak = true;
            }
        }
        else{
            breakRightGrowth = true;
        }
    }
    //cout<<"Diagonal Check:"<<count<<dotCount<<endl;
    return returnScoreClass(count,leftDotCount,rightDotCount,colorCoin);
}

bool isTerminalState(){
     if(i_captured>=10 || they_captured>=10){
        return true;
    }
    return false;
}



//Returns my points for the board configuration.
int evaluationFunction(int lastMoveX,int lastMoveY,int depth){
    //Use the board state information.
    bool iCaptured5Pairs = false;
    bool theyCaptured5Pairs = false;
    //Doesnt really happen. Might remove once everything else is done.
    if(board[lastMoveX][lastMoveY]=='.'){
        return 0;
    }
    int horizontalCheck = checkHorizontalGrowth(lastMoveX,lastMoveY);
    int verticalCheck = checkVerticalGrowth(lastMoveX,lastMoveY);
    int diagonalCheckLeftRight = checkLeftRightDiagonal(lastMoveX,lastMoveY);
    int diagonalCheckRightLeft = checkRightLeftDiagonal(lastMoveX,lastMoveY);

    int checkArray[4];
    checkArray[0] = horizontalCheck;
    checkArray[1] = verticalCheck;
    checkArray[2] = diagonalCheckLeftRight;
    checkArray[3] = diagonalCheckRightLeft;


    int sum[4] = {0,0,0,0};

    for(int i=0;i<4;i++){
        sum[checkArray[i]]++;
    }

    int iCapturedPairs = i_captured/2;
    int theyCapturedPairs = they_captured/2;
    int iCapturePoints = 0;
    int theyCapturePoints = 0;

    char moveColor = board[lastMoveX][lastMoveY];
    bool isMyMoveEvaluated = true;

    //This is the opponent playing.
    if(moveColor == returnOpponent(currentPlayerColor)){
        isMyMoveEvaluated = false;
        iCapturedPairs = they_captured/2;
        theyCapturedPairs = i_captured/2;
    }


    //Variables of heuristics:
    int all5s = sum[0];
    int opens4 = sum[1];
    int opens3 = sum[2];

    if(iCapturedPairs!=5 || theyCapturedPairs!=5){
        iCapturePoints = pow(5,iCapturedPairs);
        theyCapturePoints = pow(5,theyCapturedPairs);
    }

    int iCaptured5 = 0;
    int theyCaptured5 = 0;

    if(iCapturedPairs == 5){
        iCaptured5 = 1;
    }
    if(theyCapturedPairs == 5) {
        theyCaptured5 = 1;
    }

    int w1 = 80000; //Weight for 5 captured or 5 in a row.
    int w2 = 9000; // For creating a open 4.
    int w4 = 400; // For creating a open 3.
    //int w3_limit = 500; // The weights subsequent to this shouldn't exceed this. Otherwise it would get overpowered.
    int w3 = (iCapturePoints - theyCapturePoints);
    //int w5 = -1; //For creating a open 2.
    /*
    if(isMyMoveEvaluated){
        evalScore+= (w1*all5s + w1*iCaptured5 + w2*block4s + w3 + w4*blocksOpen4s + w5*opens4 + w6*blocksOpen3s + w7*opens3 + w8*blocksOpen2s + w9*opens2);
    }
    else{
        evalScore-= (w1*all5s + w1*theyCaptured5 + w2*block4s + w3 + w4*blocksOpen4s + w5*opens4 + w6*blocksOpen3s + w7*opens3 + w8*blocksOpen2s + w9*opens2);
    } */
    int prod1 = w1*all5s+(depth*-1);
    int prod2 = w1*iCaptured5+(depth*-1);
    int prod3 = w2*opens4+(depth*-1);
    int prod4 = w4*opens3+(depth*-1);
    w3 = w3+(depth*-1);


    //int prod2 = w2*al
    if(isMyMoveEvaluated){
        evalScore += (prod1 + prod2+ prod3+prod4+w3);
    }
    else{
        evalScore -= (prod1 + prod2 + prod3 +prod4+w3) ;
    }
    return evalScore;
}

/*
To Do:
1. Placing the first white move - middle of the board.
2. Placing the second white move - within 3 intersections of the center piece. i: 6-12, j:6-12.
3. Eval function   
    i. Win condition - +10 for the continously placed condition.
    ii. Lose condition - -10 for the opponent winning.
    iii. Draw condition - +0 for such a condition.
4. minimax algorithm with alpha beta pruning - place it on the board, call the opponent, undo the move - best score for what move : We can use up the depth concept.
   Earlier the victory- incentivise that move.



*/

/*
void checkInput(vector<string> &board,vector<int> &captured_coins,string color, double time){
    cout<<color<<endl;
    cout<<time<<endl;
    cout<<captured_coins[0]<<","<<captured_coins[1]<<endl;
    for(int i=0;i<19;i++){
        for(int j=0;j<19;j++){
            cout<<board[i][j]<<" ";
        }
        cout<<endl;
    }
}*/

void captureCoins(char player,int x,int y,map<pair<int,int>,vector<pair<int,int> > > &capturedMap){
    char opponent;
    if(player =='w'){
        opponent = 'b';
    }
    else{
        opponent = 'w';
    }
    vector<pair<int,int> > capturedCoordinates;
    //Horizontal-Left Capture
    if(isSafe(y-3) && board[x][y-3]==player){
        if(board[x][y-2]==opponent && board[x][y-2] == board[x][y-1]){
            board[x][y-2] = '.';
            board[x][y-1] = '.';
            capturedCoordinates.push_back(make_pair(x,y-1));
            capturedCoordinates.push_back(make_pair(x,y-2));
        }
    }
    //Horizontal-Right Capture
    if(isSafe(y+3) && board[x][y+3]==player){
        if(board[x][y+2]==opponent && board[x][y+2] == board[x][y+1]){
            board[x][y+2] = '.';
            board[x][y+1] = '.';
            capturedCoordinates.push_back(make_pair(x,y+1));
            capturedCoordinates.push_back(make_pair(x,y+2));
        }
    }
    //Vertical-Down capture 
    if(isSafe(x+3) && board[x+3][y]==player){
        if(board[x+2][y] == opponent && board[x+2][y]==board[x+1][y]){
            board[x+2][y] = '.';
            board[x+1][y] = '.';
            capturedCoordinates.push_back(make_pair(x+1,y));
            capturedCoordinates.push_back(make_pair(x+2,y));
        }
    }
    //Vertical-Up capture
    if(isSafe(x-3) && board[x-3][y] == player){
        if(board[x-2][y] == opponent && board[x-2][y] == board[x-1][y]){
            board[x-2][y] = '.';
            board[x-1][y] = '.';
            capturedCoordinates.push_back(make_pair(x-1,y));
            capturedCoordinates.push_back(make_pair(x-2,y));
        }
    }
    //Diagonal-LeftUp
    if(isSafe(x-3) && isSafe(y-3) && board[x-3][y-3] == player){
        if(board[x-2][y-2] == opponent && board[x-2][y-2] == board[x-1][y-1]){
            board[x-2][y-2] = '.';
            board[x-1][y-1] = '.';
            capturedCoordinates.push_back(make_pair(x-2,y-2));
            capturedCoordinates.push_back(make_pair(x-1,y-1));
        }
    }
    //Diagonal-RightDown
    if(isSafe(x+3) && isSafe(y+3) && board[x+3][y+3] == player){
        if(board[x+2][y+2] == opponent && board[x+2][y+2] == board[x+1][y+1]){
            board[x+2][y+2] = '.';
            board[x+1][y+1] = '.';
            capturedCoordinates.push_back(make_pair(x+2,y+2));
            capturedCoordinates.push_back(make_pair(x+1,y+1));
        }
    }
    //Diagonal-RightUp
    if(isSafe(x-3) && isSafe(y+3) && board[x-3][y+3] == player){
        if(board[x-1][y+1] == opponent && board[x-1][y+1] == board[x-2][y+2]){
            board[x-1][y+1] = '.';
            board[x-2][y+2] = '.';
            capturedCoordinates.push_back(make_pair(x-1,y+1));
            capturedCoordinates.push_back(make_pair(x-2,y+2));
        }
    }
    //Diagonal-LeftDown
    if(isSafe(x+3) && isSafe(y-3) && board[x+3][y-3] == player){
        if(board[x+2][y-2] == opponent && board[x+1][y-1] == board[x+2][y-2]){
            board[x+2][y-2] = '.';
            board[x+1][y-1] = '.';
            capturedCoordinates.push_back(make_pair(x+2,y-2));
            capturedCoordinates.push_back(make_pair(x+1,y-1));
        }
    }
    capturedMap.insert(make_pair(make_pair(x,y),capturedCoordinates));
    if(player == currentPlayerColor){
        i_captured+=capturedCoordinates.size();
    }
    else{
        they_captured+=capturedCoordinates.size();
    }

}

void putBackCoins(char toPut,int x,int y,map<pair<int,int>,vector<pair<int,int> > > &capturedMap){
    vector<pair<int,int> > capturedPoints = capturedMap[make_pair(x,y)];
    int og_size = capturedPoints.size();
    int x_c,y_c;
    for(int i=0;i<capturedPoints.size();i++){
        x_c = capturedPoints[i].first;
        y_c = capturedPoints[i].second;
        board[x_c][y_c] = toPut;
    }
    //I'll be adding back the coins of the toPut and subtracting coins from the other color.
    char who_captured = returnOpponent(toPut);
    if(who_captured == currentPlayerColor){
        i_captured = i_captured - capturedPoints.size();
    }
    else{
        they_captured = they_captured - capturedPoints.size();
    }
    capturedMap.erase(make_pair(x,y));
}

/*
void utilityFunctionToCheckCapture(){
    int xco,yco;
    char playerColor;
    xco = 5;
    yco = 8;
    playerColor = 'w';
    captureCoins(playerColor,xco,yco);
    cout<<"I captured: "<<i_captured<<endl;
    cout<<"They captured: "<<they_captured<<endl;
    vector<pair<int,int> >capturedLocations = capturedMap[make_pair(xco,yco)];
    for(int i=0;i<capturedLocations.size();i++){
        cout<<capturedLocations[i].first<<","<<capturedLocations[i].second<<endl;
    }
    cout<<endl;
    for(int i=0;i<19;i++){
        for(int j=0;j<19;j++){
            cout<<board[i][j];
        }
        cout<<endl;
    }
    cout<<"****"<<endl;
    
    putBackCoins(returnOpponent(playerColor),xco,yco);
    for(int i=0;i<19;i++){
        for(int j=0;j<19;j++){
            cout<<board[i][j];
        }
        cout<<endl;
    }
    cout<<"I captured: "<<i_captured<<endl;
    cout<<"They captured: "<<they_captured<<endl;
    capturedLocations = capturedMap[make_pair(xco,yco)];
    for(int i=0;i<capturedLocations.size();i++){
        cout<<capturedLocations[i].first<<","<<capturedLocations[i].second<<endl;
    }
    cout<<endl;
    cout<<"****"<<endl;
}

void utilityFunctionToCheckEvaluationFunction(){
    int xco,yco;
    xco = 9;
    yco = 9;
    int score = evaluationFunction(xco,yco,1);
    cout<<"Score: "<<score<<endl;
} */



/**
 * Internal game playing variable - lastX,lastY.
*/

int lastX,lastY;
bool firstWhiteMove = false;
bool secondWhiteMove = false;


int minValue(int alpha,int beta,int depth,int desiredDepth,bool secondMove,bool firstMove,char myMove,int lastMoveX,int lastMoveY,int grandMoveX,int grandMoveY){
    //cout<<"minValueFunction"<<endl;
    int score = evaluationFunction(lastMoveX,lastMoveY,depth);

    if(abs(score)>40000 || depth == desiredDepth){
        //I dont go one more level in min to call another max and minimise - simply return the heuristic of the last max move.
        return score;
    }
    int v = INT_MAX;
    int previousScore;
    //surrounding opponent move.
    vector<vector<bool> > visited(19,vector<bool>(19,false));
    if(lastMoveX!= -1 && lastMoveY!=-1){
    for(int step = 1;step<=5;step++){
        for(int i = -step;i<=step;i++){
            for(int j = -step;j<=step;j++){
                int currX,currY;
                currX = lastMoveX + i;
                currY = lastMoveY + j;
                if(isPointSafe(currX,currY)){
                    if(!visited[currX][currY]){
                        if(i==0 && j==0){}
                        else{
                            if(board[currX][currY]=='.'){
                                board[currX][currY] = myMove;
                                map<pair<int,int>,vector<pair<int,int> > > capturedMap;
                                captureCoins(board[currX][currY],currX,currY,capturedMap);
                                previousScore = evalScore;
                                v = min(v,maxValue(alpha,beta,depth+1,desiredDepth,false,false,returnOpponent(myMove),currX,currY,lastMoveX,lastMoveY));
                                
                                evalScore = previousScore;
                                putBackCoins(returnOpponent(myMove),currX,currY,capturedMap);
                                board[currX][currY] = '.';
                                if(v<=alpha){
                                    return v;
                                }
                                beta = min(beta,v);
                            }
                        }       
                    }
                    visited[currX][currY] = true;
                }
            }
        }
    }
    }
    //surrounding my move.
    //std::fill(visited.begin(),visited.end(),false);
    if(grandMoveX!=-1 && grandMoveY!=-1){
    for(int step = 1;step<5;step++){
        for(int i = -step;i<=step;i++){
            for(int j = -step;j<=step;j++){
                int currX,currY;
                currX = grandMoveX + i;
                currY = grandMoveY + j;
                if(isPointSafe(currX,currY)){
                    if(!visited[currX][currY]){
                        if(i==0 && j==0){}
                        else{
                            if(board[currX][currY]=='.'){
                                board[currX][currY] = myMove;
                                map<pair<int,int>,vector<pair<int,int> > > capturedMap;
                                captureCoins(board[currX][currY],currX,currY,capturedMap);
                                previousScore = evalScore;
                                v = min(v,maxValue(alpha,beta,depth+1,desiredDepth,false,false,returnOpponent(myMove),currX,currY,lastMoveX,lastMoveY));
                                putBackCoins(returnOpponent(myMove),currX,currY,capturedMap);
                                board[currX][currY] = '.';
                                evalScore = previousScore;
                                if(v<=alpha){
                                    return v;
                                }
                                beta = min(beta,v);
                            }
                        }       
                    }
                    visited[currX][currY] = true;
                }
            }
        }
    }
    }


    return v;
}

int maxValue(int alpha,int beta,int depth,int desiredDepth,bool secondMove,bool firstMove,char myMove,int lastMoveX,int lastMoveY,int grandMoveX, int grandMoveY){
    //cout<<"maxValueFunction"<<endl;
    int score = evaluationFunction(lastMoveX,lastMoveY,depth);
    /*
    if(lastMoveX == 4 and lastMoveY == 11 && grandMoveX == 9 && grandMoveY == 9) {
        cout<<"Score during b,9,9 and w,4,11: "<<score<<"\n";
    }*/
    if(abs(score)>40000 || depth == desiredDepth){
        //I dont go one more level in min to call another max and minimise - simply return the heuristic of the last max move.
        return score;
    }
    int v = INT_MIN;
    int previousScore;
    //surrounding opponent's move.
    vector<vector<bool> > visited(19,vector<bool>(19,false));
    if(lastMoveX!=-1 && lastMoveY!=-1){
    for(int step = 1;step<=5;step++){
        for(int i = -step;i<=step;i++){
            for(int j = -step;j<=step;j++){
                int currX = lastMoveX+i;
                int currY = lastMoveY+j;
                if(isPointSafe(currX,currY)){
                    if(!visited[currX][currY]){
                        if(i==0 && j==0){}
                        else{
                            if(board[currX][currY]=='.'){
                                board[currX][currY] = myMove;
                                map<pair<int,int>,vector<pair<int,int> > > capturedMap;
                                captureCoins(board[currX][currY],currX,currY,capturedMap);
                                previousScore = evalScore;
                                v = max(v,minValue(alpha,beta,depth+1,desiredDepth,false,false,returnOpponent(myMove),currX,currY,lastMoveX,lastMoveY));
                                //cout<<currX<<"---"<<currY<<evalScore<<endl;
                                putBackCoins(returnOpponent(myMove),currX,currY,capturedMap);
                                evalScore = previousScore;
                                board[currX][currY] = '.';
                                if(v>=beta){
                                    return v;
                                }
                                alpha = max(alpha,v);
                            }
                        }       
                    }
                    visited[currX][currY] = true;
                }
            }
        }
    }
    }
    //surrounding opponent's move.
    //std::fill(visited.begin(),visited.end(),false);
    if(grandMoveX!=-1 && grandMoveY!=-1){
    for(int step = 1;step<=5;step++){
        for(int i = -step;i<=step;i++){
            for(int j = -step;j<=step;j++){
                int currX = grandMoveX+i;
                int currY = grandMoveY+j;
                if(isPointSafe(currX,currY)){
                    if(!visited[currX][currY]){
                        if(i==0 && j==0){}
                        else{
                            if(board[currX][currY]=='.'){
                                board[currX][currY] = myMove;
                                map<pair<int,int>,vector<pair<int,int> > > capturedMap;
                                captureCoins(board[currX][currY],currX,currY,capturedMap);
                                previousScore = evalScore;
                                v = max(v,minValue(alpha,beta,depth+1,desiredDepth,false,false,returnOpponent(myMove),currX,currY,lastMoveX,lastMoveY));
                                //cout<<currX<<"---"<<currY<<evalScore<<endl;
                                putBackCoins(returnOpponent(myMove),currX,currY,capturedMap);
                                evalScore = previousScore;
                                board[currX][currY] = '.';
                                if(v>=beta){
                                    return v;
                                }
                                alpha = max(alpha,v);
                            }
                        }       
                    }
                    visited[currX][currY] = true;
                }
            }
        }
    }
    }
    return v;
}



penteMove makeMove(bool firstMove, bool secondMove,int depth,int desiredDepth,bool firstBlackMove,char myMove,int myLastMoveX,int myLastMoveY,int opponentLastMoveX, int opponentLastMoveY){
    int alpha = INT_MIN;
    int beta = INT_MAX;
    int desired_depth = 2;
    int bestX = -1;
    int bestY = -1;
    int middleX = 9;
    int middleY = 9;
    int v = INT_MIN;
    int lastX = opponentLastMoveX;
    int lastY = opponentLastMoveY;
    int previousScore;
    //Special case White moves: First and second.
    if(currentPlayerColor == 'w'){
        if(firstMove){
            board[9][9] = currentPlayerColor;
            return penteMove(9,9);
        }
    }
    if(currentPlayerColor == 'b'){
        if(firstBlackMove){
            board[10][9]= currentPlayerColor;
            return penteMove(10,9);
        }
    }
    //Remaining White moves and Black moves.
    //cout<<"Should go around here - remaining white or black moves "<<endl;
    //Grow around my last move.
    vector<vector<bool> > visited(19,vector<bool>(19,false));
    for(int step = 1;step<=5;step++){
        for(int i = -step;i<=step;i++){
            for(int j = -step;j<=step;j++){
                if(isPointSafe(myLastMoveX+i,myLastMoveY+j,secondMove)){
                    if(!visited[myLastMoveX+i][myLastMoveY+j]){
                        if(i==0 && j==0){}
                        else{
                            if(board[myLastMoveX+i][myLastMoveY+j]=='.'){
                                board[myLastMoveX+i][myLastMoveY+j] = myMove;
                                map<pair<int,int>,vector<pair<int,int> > > capturedMap;
                                captureCoins(board[myLastMoveX+i][myLastMoveY+j],myLastMoveX+i,myLastMoveY+j,capturedMap);
                                previousScore = evalScore;
                                int cost = minValue(alpha,beta,depth+1,desiredDepth,false,false,returnOpponent(myMove),myLastMoveX+i,myLastMoveY+j,lastX,lastY);
                                
                                putBackCoins(returnOpponent(myMove),myLastMoveX+i,myLastMoveY+j,capturedMap);
                                board[myLastMoveX+i][myLastMoveY+j] = '.';
                                evalScore = previousScore;
                                if(cost > v){
                                    //cout<<"Inside the less part "<<cost<<endl;
                                    v = cost;
                                    
                                    bestX = myLastMoveX+i;
                                    bestY = myLastMoveY+j;
                                }
                                alpha = max(alpha,cost);
                            }
                        }
                        visited[myLastMoveX+i][myLastMoveY+j] = true;       
                    }
                }
            }
        }
    }
    //Grow around my opponent's move.
    //std::fill(visited.begin(),visited.end(),false);
    //cout<<"Growing around opponent"<<endl;
    if(opponentLastMoveX != -1 && opponentLastMoveY != -1 ){
    for(int step = 1;step<5;step++){
        for(int i = -step;i<=step;i++){
            for(int j = -step;j<=step;j++){
                if(isPointSafe(opponentLastMoveX+i,opponentLastMoveY+j,secondMove)){
                    if(!visited[opponentLastMoveX+i][opponentLastMoveY+j]){
                        if(i==0 && j==0){}
                        else{
                            if(board[opponentLastMoveX+i][opponentLastMoveY+j]=='.'){
                                board[opponentLastMoveX+i][opponentLastMoveY+j] = myMove;
                                map<pair<int,int>,vector<pair<int,int> > > capturedMap;
                                captureCoins(board[opponentLastMoveX+i][opponentLastMoveY+j],opponentLastMoveX+i,opponentLastMoveY+j,capturedMap);
                                previousScore = evalScore;
                                int cost = minValue(alpha,beta,depth+1,desiredDepth,false,false,returnOpponent(myMove),opponentLastMoveX+i,opponentLastMoveY+j,lastX,lastY);
                                /*
                                if(opponentLastMoveX+i == 9 && opponentLastMoveY+j ==9){
                                    cout<<"Score for 9,9: "<<cost<<endl;
                                }
                                if(opponentLastMoveX+i == 4 && opponentLastMoveY+j ==11){
                                    cout<<"Score for 4,11: "<<cost<<endl;
                                }*/
                                putBackCoins(returnOpponent(myMove),opponentLastMoveX+i,opponentLastMoveY+j,capturedMap);
                                evalScore = previousScore;
                                board[opponentLastMoveX+i][opponentLastMoveY+j] = '.';
                                if(cost > v){
                                    v = cost;
                                    bestX = opponentLastMoveX+i;
                                    bestY = opponentLastMoveY+j;
                                }
                                alpha = max(alpha,cost);
                            }
                        }       
                    }
                    visited[opponentLastMoveX+i][opponentLastMoveY+j] = true;
                }
            }
        }
    }
    }
    //cout<<"here"<<v<<endl;

    //cout<<"Done growing around opponent"<<endl;
    //captureCoins(board[bestX][bestY],bestX,bestY);
    return penteMove(bestX,bestY);
}


int x_axis[19]={19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};
char y_axis[19]={'A','B','C','D','E','F','G','H','J','K','L','M','N','O','P','R','S','T'};


int main(){

    ifstream playData("playdata.txt");
    string coordinates;
    int myPreviousMoveX = -1;
    int myPreviousMoveY = -1;
    int opponentMoveX = -1;
    int opponentMoveY = -1;
    vector<int>intCoordinates;
    vector<string>playDataVector;
    vector<string>previousBoard(19,"                   ");
    bool firstTime = false;
    int cutOffDepth = 4;


    while(getline(playData,coordinates)){
        playDataVector.push_back(coordinates); 
    }
    
    if(playDataVector.size()>0){
        int i,j;
        for(i=0;i<19;i++){
            for(j=0;j<19;j++){
                previousBoard[i][j] = playDataVector[i][j];
            }
        }
        //cout<<i<<endl;
        string lastMoveXCoordinate = playDataVector[i];
        string lastMoveYCoordinate = playDataVector[i+1];
        myPreviousMoveX = stoi(lastMoveXCoordinate);
        myPreviousMoveY = stoi(lastMoveYCoordinate);
    }
    else{
        firstTime = true;
    }

    freopen("input.txt","r",stdin);
	freopen("output.txt","w",stdout);
	ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    string color;
    cin>>myColor;
    if(myColor[0]=='W'){
        currentPlayerColor = 'w';
    }
    else{
        currentPlayerColor = 'b';
    }
    double time;
    cin>>time;
    
    string capture_pair;
    cin>>capture_pair;

    string del = ",";
    vector<int>captured_coins;
    int start, end = -1*del.size();
    
    do {
        start = end + del.size();
        end = capture_pair.find(del, start);
        int capture = stoi(capture_pair.substr(start,end-start));
        captured_coins.push_back(capture);
        //cout << capture_pair.substr(start, end - start) << endl;
    } while (end != -1);

    if(myColor[0]=='W'){
        currentPlayerColor = 'w';
        i_captured = captured_coins[0];
        they_captured = captured_coins[1];
        
    }
    else{
        currentPlayerColor = 'b';
        i_captured = captured_coins[1];
        they_captured = captured_coins[0];
    }

    int i,j;
    bool firstWhiteMove = false;
    int whiteCount = 0;
    int blackCount = 0;
    bool secondWhiteMove = false;
    bool firstBlackMove = false;
    set<char>move;


    for(i=0;i<19;i++){
        for(j=0;j<19;j++){
            cin>>board[i][j];
            if(board[i][j]=='w'){
                whiteCount++;
            }
            if(board[i][j]=='b'){
                blackCount++;
            }
            if(previousBoard[i][j]=='.' && board[i][j]== returnOpponent(currentPlayerColor)){
                opponentMoveX = i;
                opponentMoveY = j;
            }
        }
    }

    if((currentPlayerColor =='w') && whiteCount == 0){
        firstWhiteMove = true;
    }
    if((currentPlayerColor=='w') && whiteCount == 1){
        secondWhiteMove = true;
    }
    if((currentPlayerColor=='b') && blackCount == 0){
        firstBlackMove = true;
        //Using this variable to track the second white move in my minmax run.
    }

    //utilityFunctionToCheckCapture();
    //utilityFunctionToCheckEvaluationFunction();
    
    //cout<<"My move history: "<<myPreviousMoveX<<myPreviousMoveY<<endl;
    //cout<<"Opponent Move: "<<opponentMoveX<<opponentMoveY<<endl;
    //cout<<"PenteMoveArgs: "<<firstWhiteMove<<" "<<secondWhiteMove<<" "<<0<<3<<firstBlackMove<<" "<<currentPlayerColor<<myPreviousMoveX<<myPreviousMoveY<<opponentMoveX<<opponentMoveY<<endl;
    int depthToDecrease = 0;
    if(time<45){
        depthToDecrease = 2;
    }
    penteMove move_ = makeMove(firstWhiteMove,secondWhiteMove,0,cutOffDepth-depthToDecrease,firstBlackMove,currentPlayerColor,myPreviousMoveX,myPreviousMoveY,opponentMoveX,opponentMoveY);
    cout<<x_axis[move_.x]<<y_axis[move_.y];
    board[move_.x][move_.y] = currentPlayerColor;
    map<pair<int,int>,vector<pair<int,int> > > capturedMap;
    captureCoins(currentPlayerColor,move_.x,move_.y,capturedMap);

    std::fstream fout("playdata.txt",fstream::out);
    for(int i=0;i<19;i++){
        for(int j=0;j<19;j++){
            fout<<board[i][j];
        }
        fout<<endl;
    }
    fout<<move_.x<<endl;
    fout<<move_.y;
    //cout<<std::to_string(move_.x)<<move_.y<<endl;

    
    return 0;
}