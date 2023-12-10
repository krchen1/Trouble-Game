// ECE 209 - Fall 2023 - Program 1
// This is an implementation of the Trouble board game.
// Allows for up to four players.

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
// add other #include statements as needed

// symbols for the various colors for players
// using global constants instead of #define
const int Red = 0;
const int Green = 1;
const int Yellow = 2;
const int Blue = 3;
const char * const gPlayerNames[] = {"Red", "Green", "Yellow", "Blue"};

// NOTE: We are using global variable to store the state of the game.
// This is appropriate because there is only one game, and different functions
// need access to all of the board information.

// A player's pieces are represented by an integer. There are four pieces per player (0-3),
// and a piece is represented by an integer p = 4 * color + piece.
// So, red's pieces will be 0, 1, 2, 3, green's pieces will be 4, 5, 6, 7, etc.
int gPieces[16];   // the current position of each piece

// board information
int gHome[4];        // number of players in the home area for each player
int gRing[28];       // which piece is in each spot on the ring
int gFinish[4][4];   // which pieces are in each player's finish line

// Each board position is represented by an integer.
// Position -1 is the home area for any player.
//
// Positions 0 through 27 are positions along the ring.
// The ring is divided into four regions, one for each color. Each region has seven positions.
// Red is region 0, Green is region 1, Yellow is region 2, Blue is region 3.
// A "region" begins with the position that transitions from one color to the next.
//
// Each player's starting point (where it enters the ring)
// is position 5 within that player's region.
//
// The finish line positions are represented by 100 + player number * 10 + position.
// Example: Position 132 is the third position in the finish line for player 3 (Blue).

// LOCATION OF EACH PIECE
// The location (position) of each piece is stored in the gPieces array.
// The Ring array contains the identifier of the piece that occupies each ring position,
// or -1 if the position is empty.
// The Finish array contains the identifier of the piece that occupies each finish line position,
// or -1 if the position is empty.

// STRING REPRESENTATION OF A BOARD POSITION
//
// NOTE: Since we are communicating with humans who are not used to counting from zero,
// our printed position values will start counting from 1.
//
// Print "H" for any player's home area.
//
// When a ring position is printed, we specify the color of the region and the
// position within that region. (See the picture in the spec.)
// For a given position on the ring, the color of a region is given by position / 7, and the
// number within the region is given by (index % 7)+1.
// Example: Position 0 is written as "R1" (meaning "red region, position 1")
// because 0 / 7 == 0 (Red) and (0 % 7)+1 == 1.
// Example: Position 19 is written as "Y6" (meaning "yellow region, position 6")
// because 19 / 7 == 2 (Yellow) and (19 % 7)+1 == 6.
//
// The finish line positions are written as "xF1" through "xF4", where x is the color
// (e.g., "RF2" for red's finish line position 2).

// MANDATORY FUNCTIONS ================================================================
// functions that you must implement -- you can add others if you want

// set up the board for a new game (see the spec)
void initializeBoard();


void printStatus();        // print the current position of each player's pieces (see the spec)

int playerTurn(int player);   // do everything necessary for this player's turn
// return 1 if player wins, otherwise return 0;

int printMoves(int player, int roll);   // print the legal moves for the player
// return the number of legal moves

int movePiece(int player, int from, int spaces);    // move a piece forward
// player is requesting a move from a specified place on the board
// return 1 if the move is legal, 0 otherwise

int checkWin(int player);   // check if the player has won the game

// OPTIONAL FUNCTIONS =================================================================
// Please declare all functions that you have created for this program. Do not define
// them here. Put the code **after** the main() function, in the designated area.



// HELPER FUNCTIONS ===================================================================
// These functions are defined for you. (See code below main.)
// DO NOT delete these declarations or alter that code in any way.
void seedRandom(int);   // seed the random number generator
int pop_o_matic();  // "pop" to roll the dice, returns a number between 1 and 6

// MAIN FUNCTION ======================================================================

#ifndef TEST_MAIN   // DO NOT REMOVE this line -- it is needed for grading purposes.
int main() {
   int i,j;  // generic loop counters
   // initialize the board
   initializeBoard();
   // checking the board
   for (i=0; i<4; i++) assert(gHome[i] == 4);
   for (i=0; i<28; i++) assert(gRing[i] == -1);
   for (i=0; i<4; i++) {
      for (j=0; j<4; j++) assert(gFinish[i][j] == -1);
   }
   for (i=0; i<16; i++) assert(gPieces[i] == -1);

   // ask user for a random seed, and for the number of players
   int seed = 0;
   int numPlayers = 0;
   printf("Enter a random seed: ");
   fflush(stdout);
   scanf("%d", &seed);
   if (seed == -1) return 0;   // for testing only -- don't enter -1 in the real game
   seedRandom(seed);
   while (!numPlayers) {
      printf("Enter the number of players (2, 3, 4): \n");
      fflush(stdout);
      scanf("%d", &numPlayers);
      if (numPlayers < 2 || numPlayers > 4) numPlayers = 0;
   }

   // play the game
   int player = Red;     // we'll start with Red
   int gameOver = 0;

   while (!gameOver) {
      // perform this player's turn
      gameOver = playerTurn(player);
      if (numPlayers == 2) player = (player + 2) % 4;
      else player = (player + 1) % numPlayers;
   }
   return 0;
}
#endif  // TEST_MAIN: DO NOT REMOVE this line -- it is needed for grading purposes.

// FUNCTION DEFINITIONS ===============================================================
// Please put your function definitions here. Do not put a main() function here.

void initializeBoard(){
    for(int i = 0; i < 28; i++) {
        gRing[i] = -1;
    }
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            gFinish[i][j] = -1;
        }
    }
    for(int i = 0; i < 16; i++){
        gPieces[i] = -1;
    }
    for(int i = 0; i < 4; i++){
        gHome[i] = 4;
    }
}

// print the current position of each player's pieces (see the spec)
void printStatus(){
    for(int i = 0; i < 4; i++){
        //print color
        printf("%6s: ", gPlayerNames[i]);
        //print location of each piece of each color
        for(int j = 0; j < 4; j++){
            //check home
            if(gPieces[i * 4 + j] == -1){
                printf("H");
            }
            //check final
            else if(gPieces[i * 4 + j] >= 100){
                if(gPieces[i * 4 + j] <= 103){
                    printf("RF%d", gPieces[i * 4 + j] % 10 + 1);
                }
                else if(gPieces[i * 4 + j] <= 113){
                    printf("GF%d", gPieces[i * 4 + j] % 10 + 1);
                }
                else if(gPieces[i * 4 + j] <= 123){
                    printf("YF%d", gPieces[i * 4 + j] % 10 + 1);
                }
                else{
                    printf("BF%d", gPieces[i * 4 + j] % 10 + 1);
                }
                
            }
            //on board
            else{
                //get color
                if(gPieces[i * 4 + j] / 7 < 1){
                    printf("R");
                }
                else if(gPieces[i * 4 + j] / 7 < 2){
                    printf("G");
                }
                else if(gPieces[i * 4 + j] / 7 < 3){
                    printf("Y");
                }
                else{
                    printf("B");
                }
                printf("%d", gPieces[i * 4 + j] % 7 + 1);
            }
            if(j < 3){
                printf(",");
            }
        }
        printf("\n");
    }
}

// do everything necessary for this player's turn
// return 1 if player wins, otherwise return 0;

int playerTurn(int player) {
    printf("---- %s's turn\n", gPlayerNames[player]);
    //loop between step 2 and 9
    while(1){
        printStatus();
        int roll = pop_o_matic();
        printf("You rolled %d.\n", roll);
        //step 4
        if (printMoves(player, roll) == 0) {
            printf("You have no legal moves.\n");
            if (roll == 6) {
                printf("You rolled 6, so you get to roll again.\n");
                continue;
            }
            else{
                return 0;
            }
        }
        char str[4];
        printf("Enter position of piece to move: ");
        scanf("%s", str);
        if (str[0] == 'Q') {
            return 1;
        }
        int num = 0;
        // converting string to number
        for (int i = 0; str[i] != '\0'; i++) {
            if(str[i] == 'H'){
                num = -1;
                break;
            }
            else if(str[i] == 'R'){
                num += 0;
                ;
            }
            else if(str[i] == 'G'){
                num += 7;

            }
            else if(str[i] == 'Y'){
                num += 14;

            }
            else if(str[i] == 'B'){
                num += 21;

            }
            else {
                num = num * 10 + (str[i] - 48) - 1;
            }
        }
        if(movePiece(player, num, roll) == 0){
            printf("Illegal move, try again.\n");
            continue;
        }
        else{
            movePiece(player, num, roll);
        }
        if(checkWin(player)) {
            return 1;
        }
        if (roll == 6) {
            printf("You rolled 6, so you get to roll again.\n");
        }
        else {
            break;
        }
    }
    return 0;
}

// print the legal moves for the player
// return the number of legal moves
int printMoves(int player, int roll) {
    int numMoves = 0;
    //examine moves for each piece
    int home = 0;
    for (int i = 0; i < 4; i++) {
        //check if can move in final
        if (gPieces[player * 4 + i] >= 100 && roll < 4) {
            int index = gPieces[player * 4 + i] % 10;
            if (index + roll < 4 && gFinish[player][index + roll] == -1) {
                if (player == 0) {
                    printf("RF%d to RF%d\n", index + 1, index + roll + 1);
                }
                else if (player == 1) {
                    printf("GF%d to GF%d\n", index + 1, index + roll + 1);
                }
                else if (player == 2) {
                    printf("YF%d to YF%d\n", index + 1, index + roll + 1);
                }
                else {
                    printf("BF%d to BF%d\n", index + 1, index + roll + 1);
                }
                numMoves++;
            }
        }
        //check home
        else if (gPieces[player * 4 + i] == -1 && home == 0) {
            //check if roll is 6
            if (roll == 6 && (gRing[player * 7 + 4] / 4 != player || gRing[player * 7 + 4] == -1)) {
                printf("H to ");
                if (player == 0) {
                    printf("R5\n");
                } else if (player == 1) {
                    printf("G5\n");
                } else if (player == 2) {
                    printf("Y5\n");
                } else {
                    printf("B5\n");
                }
                numMoves++;
                home++;
            }
        }
        //check if going to final or normal ring move
        else {
            int nextPosition = gPieces[player * 4 + i] + roll;
            if(nextPosition > 27) {
                nextPosition -= 28;
            }
            //check player color
            //player red
            if(player == 0){
                //check if in red area and going to final
                if(gPieces[player * 4 + i] < 4 && gPieces[player * 4 + i] > -1 && nextPosition >= 4){
                    if(nextPosition - 4 < 4 && gFinish[player][nextPosition - 4] == -1){
                        printf("R%d to RF%d\n", gPieces[player * 4 + i] % 7 + 1, nextPosition - 3);
                        numMoves++;
                    }
                }
                //check if in blue area going to final
                else if(gPieces[player * 4 + i] >= 26 && nextPosition >= 4){
                    if(gFinish[player][nextPosition - 4] == -1){
                        printf("B%d to RF%d\n", gPieces[player * 4 + i] % 7 + 1, nextPosition - 3);
                        numMoves++;
                    }
                }
                //red ring move
                else{
                    if((gRing[nextPosition] / 4 != player || gRing[nextPosition] == -1) && gPieces[player * 4 + i] > -1){
                        if(gPieces[player * 4 + i] / 7 < 1){
                            printf("R");
                        }
                        else if(gPieces[player * 4 + i] / 7 < 2){
                            printf("G");
                        }
                        else if(gPieces[player * 4 + i] / 7 < 3){
                            printf("Y");
                        }
                        else{
                            printf("B");
                        }
                        printf("%d to ", gPieces[player * 4 + i] % 7 + 1);

                        if(nextPosition / 7 < 1){
                            printf("R");
                        }
                        else if(nextPosition / 7 < 2){
                            printf("G");
                        }
                        else if(nextPosition / 7 < 3){
                            printf("Y");
                        }
                        else{
                            printf("B");
                        }
                        printf("%d\n", nextPosition % 7 + 1);
                        numMoves++;
                    }
                }
            }
            //green
            else if(player == 1){
                //green area to green final
                if(nextPosition - 11 < 4 && gPieces[player * 4 + i] < 11 && gPieces[player * 4 + i] >= 7 && nextPosition >= 11 && gFinish[player][nextPosition - 11] == -1){
                    printf("G%d to GF%d\n", gPieces[player * 4 + i] % 7 + 1, nextPosition - 10);
                    numMoves++;
                }
                //red to green final
                else if(gPieces[player * 4 + i] < 7 && nextPosition >= 11 && gFinish[player][nextPosition - 11] == -1){
                    printf("R%d to GF%d\n", gPieces[player * 4 + i] % 7 + 1, nextPosition - 10);
                    numMoves++;
                }
                //green move ring
                else {
                    if ((gRing[nextPosition] / 4 != player) && gPieces[player * 4 + i] > -1) {
                        if (gPieces[player * 4 + i] / 7 < 1) {
                            printf("R");
                        } else if (gPieces[player * 4 + i] / 7 < 2) {
                            printf("G");
                        } else if (gPieces[player * 4 + i] / 7 < 3) {
                            printf("Y");
                        } else {
                            printf("B");
                        }
                        printf("%d to ", gPieces[player * 4 + i] % 7 + 1);

                        if (nextPosition / 7 < 1) {
                            printf("R");
                        } else if (nextPosition / 7 < 2) {
                            printf("G");
                        } else if (nextPosition / 7 < 3) {
                            printf("Y");
                        } else {
                            printf("B");
                        }
                        printf("%d\n", nextPosition % 7 + 1);
                        numMoves++;
                    }
                }
            }
                //yellow player
            else if(player == 2){
                //yellow area to yellow final
                if (nextPosition - 18 < 4 && gPieces[player * 4 + i] < 18 && gPieces[player * 4 + i] >= 14 &&
                nextPosition >= 18 && gFinish[player][nextPosition - 18] == -1){
                    printf("Y%d to YF%d\n", gPieces[player * 4 + i] % 7 + 1, gPieces[player * 4 + i] + roll - 17);
                    numMoves++;
                }
                //green to yellow final
                else if (gPieces[player * 4 + i] < 14 && nextPosition >= 18 &&
                    gFinish[player][nextPosition - 18] == -1) {
                    printf("G%d to YF%d\n", gPieces[player * 4 + i] % 7 + 1, nextPosition - 17);
                    numMoves++;
                }
                //yellow move in ring
                else {
                    if (gRing[nextPosition] / 4 != player && gPieces[player * 4 + i] > -1) {
                        if (gPieces[player * 4 + i] / 7 < 1) {
                            printf("R");
                        } else if (gPieces[player * 4 + i] / 7 < 2) {
                            printf("G");
                        } else if (gPieces[player * 4 + i] / 7 < 3) {
                            printf("Y");
                        } else {
                            printf("B");
                        }
                        printf("%d to ", gPieces[player * 4 + i] % 7 + 1);
                        if (nextPosition / 7 < 1) {
                            printf("R");
                        } else if (nextPosition / 7 < 2) {
                            printf("G");
                        } else if (nextPosition / 7 < 3) {
                            printf("Y");
                        } else {
                            printf("B");
                        }
                        printf("%d\n", nextPosition % 7 + 1);
                        numMoves++;
                    }
                }
            }
                //blue player
            else{
                // blue to blue final
                if (nextPosition - 25 < 4 && gPieces[player * 4 + i] < 25 && gPieces[player * 4 + i] >= 21 &&
                    nextPosition >= 25 && gFinish[player][nextPosition - 25] == -1){
                    printf("B%d to BF%d\n", gPieces[player * 4 + i] % 7 + 1, gPieces[player * 4 + i] + roll - 24);
                    numMoves++;
                }
                    //yellow to blue final
                else if (gPieces[player * 4 + i] < 14 && nextPosition >= 25 &&
                         gFinish[player][nextPosition - 25] == -1) {
                    printf("Y%d to BF%d\n", gPieces[player * 4 + i] % 7 + 1, nextPosition - 24);
                    numMoves++;
                }
                //blue move in ring
                else{
                    if (gRing[nextPosition] / 4 != player && gPieces[player * 4 + i] > -1) {
                        if (gPieces[player * 4 + i] / 7 < 1) {
                            printf("R");
                        } else if (gPieces[player * 4 + i] / 7 < 2) {
                            printf("G");
                        } else if (gPieces[player * 4 + i] / 7 < 3) {
                            printf("Y");
                        } else {
                            printf("B");
                        }
                        printf("%d to ", gPieces[player * 4 + i] % 7 + 1);

                        if (nextPosition / 7 < 1) {
                            printf("R");
                        } else if (nextPosition / 7 < 2) {
                            printf("G");
                        } else if (nextPosition / 7 < 3) {
                            printf("Y");
                        } else {
                            printf("B");
                        }
                        printf("%d\n", nextPosition % 7 + 1);
                        numMoves++;
                    }
                }
            }
        }
    }
    return numMoves;
}

// move a piece forward
// player is requesting a move from a specified place on the board
// return 1 if the move is legal, 0 otherwise
int movePiece(int player, int from, int spaces){
    //moves from home
    if(from == -1){
        if (spaces == 6 && (gRing[player * 7 + 4] / 4 != player || gRing[player * 7 + 4] == -1)){
            //boot other piece back home
            if(gRing[player * 7 + 4] != -1){
                gPieces[gRing[player * 7 + 4]] = -1;
                //update home
                ++gHome[gRing[player * 7 + 4] / 4];
            }
            //moving piece from home to ring
            for(int i = player * 4; i < (player * 4 + 4); i++){
                if(gPieces[i] == -1){
                    gPieces[i] = player * 7 + 4;
                    gRing[player * 7 + 4] = i;
                    --gHome[player];
                    return 1;
                }
            }
        }
        else{
            return 0;
        }
    }
    //move illegal piece
    else if(gRing[from] / 4 != player && from < 28){
        return 0;
    }
    //within final
    else if(from % 10 + spaces < 4 && from >= 100){
        if(gFinish[player][from % 10 + spaces] == -1){
            gFinish[player][from % 10 + spaces] = gFinish[player][from % 10];
            gPieces[gFinish[player][from % 10]] = 100 + player * 10 + from % 10 + spaces;
            gFinish[player][from % 10] = -1;
            return 1;
        }
        else{
            return 0;
        }
    }
    //ring in ring or ring to final
    else{
        int nextPosition = from + spaces;
            if (nextPosition > 27) {
                nextPosition -= 28;
            }

        //check if next position is empty
        //red
        if(player == 0){
            //check if in red area and going to final
            if(from < 4 && nextPosition >= 4){
                if(nextPosition - 4 < 4 && gFinish[player][nextPosition - 4] == -1){
                    gFinish[player][nextPosition - 4] = gRing[from];
                    gRing[from] = -1;
                    gPieces[gRing[from]] = 100 + player * 10 + nextPosition - 4;
                    return 1;
                }
                else{
                    return 0;
                }
            }
                //check if in blue area going to final
            else if(from >= 26 && nextPosition >= 4){
                gFinish[player][nextPosition - 4] = gRing[from];
                gRing[from] = -1;
                gPieces[gRing[from]] = 100 + player * 10 + nextPosition - 4;
                return 1;
            }
            //red ring move
            else{
                if(gRing[nextPosition] / 4 != player || gRing[nextPosition] == -1){
                    //boot other piece back home
                    if(gRing[nextPosition] != -1){
                        gPieces[gRing[nextPosition]] = -1;
                        //update home
                        ++gHome[gRing[nextPosition] / 4];
                    }
                    gRing[nextPosition] = gRing[from];
                    gPieces[gRing[from]] = nextPosition;
                    gRing[from] = -1;
                    return 1;
                }
                else{
                    return 0;
                }
            }
        }
            //green
        else if(player == 1){
            //green piece to green final
            if(nextPosition - 11 < 4 && from < 11 && nextPosition >= 11 && gFinish[player][nextPosition - 11] == -1){
                gFinish[player][nextPosition - 11] = gPieces[gRing[from]];
                gRing[from] = -1;
                return 1;
            }
                //green ring move
            else {
                if(gRing[nextPosition] / 4 != player && from > -1){
                    //boot other piece back home
                    if(gRing[nextPosition] != -1){
                        gPieces[gRing[nextPosition]] = -1;
                        //update home
                        ++gHome[gRing[nextPosition] / 4];
                    }
                    gRing[nextPosition] = gRing[from];
                    gPieces[gRing[from]] = nextPosition;
                    gRing[from] = -1;
                    return 1;
                }
            }
        }
            //yellow player
        else if(player == 2){
            //yellow piece to yellow final
            if (nextPosition - 18 < 4 && from < 18 &&
                nextPosition >= 18 && gFinish[player][nextPosition - 18] == -1){
                gFinish[player][nextPosition - 18] = gPieces[gRing[from]];
                gRing[from] = -1;
                return 1;
            }
                //yellow ring move
            else {
                if(gRing[nextPosition] / 4 != player && from > -1){
                    //boot other piece back home
                    if(gRing[nextPosition] != -1){
                        gPieces[gRing[nextPosition]] = -1;
                        //update home
                        ++gHome[gRing[nextPosition] / 4];
                    }
                    gRing[nextPosition] = gPieces[from];
                    gPieces[gRing[from]] = nextPosition;
                    gRing[from] = -1;
                    return 1;
                }
            }
        }
            //blue player
        else{
            //blue piece to final
            if(nextPosition - 25 < 4 && from < 25 && nextPosition >= 21 &&
               nextPosition >= 25 && gFinish[player][nextPosition - 25] == -1 && from > -1){
                gFinish[player][nextPosition - 25] = gPieces[gRing[from]];
                gPieces[gRing[from]] = -1;
                return 1;
            }
            //blue ring move
            else{
                if(gRing[nextPosition] / 4 != player && from > -1){
                    //boot other piece back home
                    if(gRing[nextPosition] != -1){
                        gPieces[gRing[nextPosition]] = -1;
                        //update home
                        ++gHome[gRing[nextPosition] / 4];
                    }
                    gRing[nextPosition] = gPieces[gRing[from]];
                    gPieces[gRing[from]] = nextPosition;
                    gRing[from] = -1;
                    return 1;
                }
            }
        }
    }
    return 0;
}

// check if the player has won the game
int checkWin(int player){
    for(int i = 0; i < 4; i++){
        if(gFinish[player][i] == -1){
            return 0;
        }
    }
    return 1;
}
















//-----------------------------------------------------------------------------------------
// Pseudorandom number generator -- DO NOT edit or remove this variable or these functions

// Using 16-bit linear feedback shift register (LFSR) for pseudorandom number generator
int lfsr = 0;  // global variable -- state of random number generator

// lfsrNext -- function to advance Xorshift LFSR for pseudorandom number generation
// uses global variable (lfsr)
// code from: https://en.wikipedia.org/wiki/Linear-feedback_shift_register
int lfsrNext(void) {
   lfsr ^= lfsr >> 7;
   lfsr ^= lfsr << 9;
   lfsr ^= lfsr >> 13;
   return (int) (lfsr & INT_MAX);  // return a positive int
}

// set starting value for LFSR-based random number generator
void seedRandom(int seed) {
   if (seed) lfsr = seed;
}
// return a random integer between 0 and limit-1
int getRandom(int limit) {
   return lfsrNext() % limit;
}

int pop_o_matic() {
   return getRandom(6) + 1;
}
//-----------------------------------------------------------------------------------------

