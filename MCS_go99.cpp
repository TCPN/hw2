/*
 * This code is provied as a sample code of Hw 2 of "Theory of Computer Game".
 * The "genmove" function will randomly output one of the legal move.
 * This code can only be used within the class.
 *
 * 2015 Nov. Hung-Jui Chang
 * */
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

#define NAME "MCS_go99"
#define BOARDSIZE        9
#define BOUNDARYSIZE    11
#define COMMANDLENGTH 1000
#define DEFAULTTIME     10
#define DEFAULTKOMI      7

#define MAXGAMELENGTH 1000
#define MAXSTRING       50
#define MAXDIRECTION     4

#define NUMINTERSECTION 81
#define HISTORYLENGTH   200

#define EMPTY            0
#define BLACK            1
#define WHITE            2
#define BOUNDARY         3

#define SELF             1
#define OPPONENT         2

#define NUMGTPCOMMANDS      15

#define LOCALVERSION      1
#define GTPVERSION        2
 
using namespace std;
int _board_size = BOARDSIZE;
int _board_boundary = BOUNDARYSIZE;
double _komi =  DEFAULTKOMI;
const int DirectionX[MAXDIRECTION] = {-1, 0, 1, 0};
const int DirectionY[MAXDIRECTION] = { 0, 1, 0,-1};
const char LabelX[]="0ABCDEFGHJ";


/*
 * This function reset the board, the board intersections are labeled with 0,
 * the boundary intersections are labeled with 3.
 * */
void reset(int Board[BOUNDARYSIZE][BOUNDARYSIZE]) {
    for (int i = 1 ; i <= BOARDSIZE; ++i) {
	for (int j = 1 ; j <= BOARDSIZE; ++j) {
	    Board[i][j] = EMPTY;
	}
    }
    for (int i = 0 ; i < BOUNDARYSIZE; ++i) {
	Board[0][i] = Board[BOUNDARYSIZE-1][i] = Board[i][0] = Board[i][BOUNDARYSIZE-1] = BOUNDARY;
    }
}
// TODO: MAKE FIND_LIBERTY MORE EFFICIENT, BY GIVE RESULTS WITH 0, 1, 2(MEANS ">1")
// ::DONE 
// TODO: KEEP A "CHI" LIST OF EACH STRING
/*
 * This function return the total number of liberity of the string of (X, Y) and
 * the string will be label with 'label'.
 * */
int find_liberty(int X, int Y, int label, int Board[BOUNDARYSIZE][BOUNDARYSIZE], int ConnectBoard[BOUNDARYSIZE][BOUNDARYSIZE], int foundLiberty) {
    // Label the current intersection
	// modified!!!!
    //ConnectBoard[X][Y] |= label;
    int total_liberty = 0;
    for (int d = 0 ; d < MAXDIRECTION; ++d) {
		// Check this intersection has been visited or not
		if ((ConnectBoard[X+DirectionX[d]][Y+DirectionY[d]] & (1<<label) )!= 0)
			continue;
		// Check this intersection is not visited yet
		ConnectBoard[X+DirectionX[d]][Y+DirectionY[d]] |=(1<<label);
		// This neighboorhood is empty
		if (Board[X+DirectionX[d]][Y+DirectionY[d]] == EMPTY){
			total_liberty++;
		}
		// This neighboorhood is self stone
		else if (Board[X+DirectionX[d]][Y+DirectionY[d]] == Board[X][Y]) {
			total_liberty += find_liberty(X+DirectionX[d], Y+DirectionY[d], label, Board, ConnectBoard, total_liberty);
		}
		if(total_liberty + foundLiberty > 1)
			return total_liberty;
    }
    return total_liberty;
}

/*
 * This function count the liberties of the given intersection's neighboorhod
 * */
void count_liberty(int X, int Y, int Board[BOUNDARYSIZE][BOUNDARYSIZE], int Liberties[MAXDIRECTION]) {
    int ConnectBoard[BOUNDARYSIZE][BOUNDARYSIZE];
    // Initial the ConnectBoard
	// memset(ConnectBoard, 0, sizeof(int) * BOUNDARYSIZE * BOUNDARYSIZE);
    for (int i = 0 ; i < BOUNDARYSIZE; ++i) {
	for (int j = 0 ; j < BOUNDARYSIZE; ++j) {
	    ConnectBoard[i][j] = 0;
	}
    }
    // Find the same connect component and its liberity
    for (int d = 0 ; d < MAXDIRECTION; ++d) {
	Liberties[d] = 0;
	if (Board[X+DirectionX[d]][Y+DirectionY[d]] == BLACK ||  
	    Board[X+DirectionX[d]][Y+DirectionY[d]] == WHITE    ) {
	    Liberties[d] = find_liberty(X+DirectionX[d], Y+DirectionY[d], d, Board, ConnectBoard, 0);
		//fprintf(stderr, "liberty in dir %d: %d\n", d, Liberties[d]);
	}
    }
}

/*
 * This function count the number of empty, self, opponent, and boundary intersections of the neighboorhod
 * and saves the type in NeighboorhoodState.
 * */
void count_neighboorhood_state(int Board[BOUNDARYSIZE][BOUNDARYSIZE], int X, int Y, int turn, int* empt, int* self, int* oppo ,int* boun, int NeighboorhoodState[MAXDIRECTION]) {
    for (int d = 0 ; d < MAXDIRECTION; ++d) {
	// check the number of nonempty neighbor
	switch(Board[X+DirectionX[d]][Y+DirectionY[d]]) {
	    case EMPTY:    (*empt)++; 
			   NeighboorhoodState[d] = EMPTY;
			   break;
	    case BLACK:    if (turn == BLACK) {
			       (*self)++;
			       NeighboorhoodState[d] = SELF;
			   }
			   else {
			       (*oppo)++;
			       NeighboorhoodState[d] = OPPONENT;
			   }
			   break;
	    case WHITE:    if (turn == WHITE) {
			       (*self)++;
			       NeighboorhoodState[d] = SELF;
			   }
			   else {
			       (*oppo)++;
			       NeighboorhoodState[d] = OPPONENT;
			   }
			   break;
	    case BOUNDARY: (*boun)++;
			   NeighboorhoodState[d] = BOUNDARY;
			   break;
	}
    }
}

/*
 * This function remove the connect component contains (X, Y) with color "turn" 
 * And return the number of remove stones.
 * */
int remove_piece(int Board[BOUNDARYSIZE][BOUNDARYSIZE], int X, int Y, int turn) {
    int remove_stones = (Board[X][Y]==EMPTY)?0:1;
    Board[X][Y] = EMPTY;
    for (int d = 0; d < MAXDIRECTION; ++d) {
	if (Board[X+DirectionX[d]][Y+DirectionY[d]] == turn) {
	    remove_stones += remove_piece(Board, X+DirectionX[d], Y+DirectionY[d], turn);
	}
    }
    return remove_stones;
}
// TODO: MAKE UPDATE_BOARD USE LIBERTIES INFO IF IT'S KNOWN ALREADY
// :: DONE
/*
 * This function update Board with place turn's piece at (X,Y).
 * Note that this function will not check if (X, Y) is a legal move or not.
 * */
void update_board(int Board[BOUNDARYSIZE][BOUNDARYSIZE], int X, int Y, int turn, int KnownLiberties[4]) {
    int num_neighborhood_self = 0;
    int num_neighborhood_oppo = 0;
    int num_neighborhood_empt = 0;
    int num_neighborhood_boun = 0;
    int Liberties[4];
    int NeighboorhoodState[4];
    count_neighboorhood_state(Board, X, Y, turn,
	    &num_neighborhood_empt,
	    &num_neighborhood_self,
	    &num_neighborhood_oppo,
	    &num_neighborhood_boun, NeighboorhoodState);
    // check if there is opponent piece in the neighboorhood
    if (num_neighborhood_oppo != 0) {
		if(KnownLiberties != NULL)
			memcpy(Liberties, KnownLiberties, sizeof(int) * 4);
		else
			count_liberty(X, Y, Board, Liberties);
		for (int d = 0 ; d < MAXDIRECTION; ++d) {
			// check if there is opponent component only one liberty
			if (NeighboorhoodState[d] == OPPONENT && Liberties[d] <= 1 && Board[X+DirectionX[d]][Y+DirectionY[d]]!=EMPTY) {
				//fprintf(stderr, "some dead in direction %d!\n", d);
				remove_piece(Board, X+DirectionX[d], Y+DirectionY[d], Board[X+DirectionX[d]][Y+DirectionY[d]]);
			}
		}
    }
    Board[X][Y] = turn;
}
/*
 * This function update Board with place turn's piece at (X,Y).
 * Note that this function will check if (X, Y) is a legal move or not.
 * */
int update_board_check(int Board[BOUNDARYSIZE][BOUNDARYSIZE], int X, int Y, int turn) {
    // Check the given coordination is legal or not
    if ( X < 1 || X > BOARDSIZE || Y < 1 || Y > BOARDSIZE || Board[X][Y]!=EMPTY)
	return 0;
    int num_neighborhood_self = 0;
    int num_neighborhood_oppo = 0;
    int num_neighborhood_empt = 0;
    int num_neighborhood_boun = 0;
    int Liberties[4];
    int NeighboorhoodState[4];
    count_neighboorhood_state(Board, X, Y, turn,
	    &num_neighborhood_empt,
	    &num_neighborhood_self,
	    &num_neighborhood_oppo,
	    &num_neighborhood_boun, NeighboorhoodState);
    // Check if the move is a legal move
    // Condition 1: there is a empty intersection in the neighboorhood
    int legal_flag = 0;
    count_liberty(X, Y, Board, Liberties);
    if (num_neighborhood_empt != 0) {
	legal_flag = 1;
    }
    else {
	// Condition 2: there is a self string has more than one liberty
	for (int d = 0; d < MAXDIRECTION; ++d) {
	    if (NeighboorhoodState[d] == SELF && Liberties[d] > 1) {
		legal_flag = 1;
	    }
	}
	if (legal_flag == 0) {
	// Condition 3: there is a opponent string has exactly one liberty
	    for (int d = 0; d < MAXDIRECTION; ++d) {
		if (NeighboorhoodState[d] == OPPONENT && Liberties[d] == 1) {
		    legal_flag = 1;
		}
	    }
	}
    }

    if (legal_flag == 1) {
    // check if there is opponent piece in the neighboorhood
	if (num_neighborhood_oppo != 0) {
	    for (int d = 0 ; d < MAXDIRECTION; ++d) {
		// check if there is opponent component only one liberty
		if (NeighboorhoodState[d] == OPPONENT && Liberties[d] == 1 && Board[X+DirectionX[d]][Y+DirectionY[d]]!=EMPTY) {
		    remove_piece(Board, X+DirectionX[d], Y+DirectionY[d], Board[X+DirectionX[d]][Y+DirectionY[d]]);
		}
	    }
	}
	Board[X][Y] = turn;
    }

    return (legal_flag==1)?1:0;
}

int check_legal(int Board[BOUNDARYSIZE][BOUNDARYSIZE], int x, int y, int turn, int game_length, int GameRecord[MAXGAMELENGTH][BOUNDARYSIZE][BOUNDARYSIZE]){

    int NextBoard[BOUNDARYSIZE][BOUNDARYSIZE];
    int num_neighborhood_self;
    int num_neighborhood_oppo;
    int num_neighborhood_empt;
    int num_neighborhood_boun;
    int need_check_history;
    int Liberties[4];
    int NeighboorhoodState[4];
    bool eat_move;
	if (Board[x][y] == EMPTY) {
		// check the liberty of the neighborhood intersections
		num_neighborhood_self = 0;
		num_neighborhood_oppo = 0;
		num_neighborhood_empt = 0;
		num_neighborhood_boun = 0;
		// count the number of empy, self, opponent, and boundary neighboorhood
		count_neighboorhood_state(Board, x, y, turn,
			&num_neighborhood_empt,
			&num_neighborhood_self,
			&num_neighborhood_oppo,
			&num_neighborhood_boun, NeighboorhoodState);
		// check if the emtpy intersection is a legal move
		need_check_history = 0;
		eat_move = 0;
		count_liberty(x, y, Board, Liberties);
		// Case 1: exist empty intersection in the neighborhood
		if (num_neighborhood_empt > 0) {
			need_check_history = 1;
			// check if it is a capture move
			for (int d = 0 ; d < MAXDIRECTION; ++d) {
				if (NeighboorhoodState[d] == OPPONENT && Liberties[d] == 1) {
					 eat_move = 1;
				}
			}
		}
		// Case 2: no empty intersection in the neighborhood
		else {
			// Case 2.1: Surround by the self piece
			if (num_neighborhood_self + num_neighborhood_boun == MAXDIRECTION) {
				int check_flag = 0, check_eye_flag = num_neighborhood_boun;
				for (int d = 0 ; d < MAXDIRECTION; ++d) {
					//modified!!!
					// Avoid fill self eye
					// Check if there is one self component which has more than one liberty
					if (NeighboorhoodState[d]==SELF && Liberties[d] > 1) {
						check_eye_flag++;
						check_flag = 1;
					}
				}
				if (check_flag == 1 && check_eye_flag!=4) {
					need_check_history = 1;
				}
			}	
			// Case 2.2: Surround by opponent or both side's pieces.
			else if (num_neighborhood_oppo > 0) {
				for (int d = 0 ; d < MAXDIRECTION; ++d) {
					// Check if there is one self component which has more than one liberty
					if (NeighboorhoodState[d]==SELF && Liberties[d] > 1) {
						need_check_history = 1;
					}
					// Check if there is one opponent's component which has exact one liberty
					if (NeighboorhoodState[d]==OPPONENT && Liberties[d] == 1) {						
						need_check_history = 1;
						eat_move = 1;
					}
				}
				// modified!!!!
			}	
		}
		if (need_check_history == 1) {
		// copy the current board to next board
			// for (int i = 0 ; i < BOUNDARYSIZE; ++i) {
				// for (int j = 0 ; j < BOUNDARYSIZE; ++j) {
					// NextBoard[i][j] = Board[i][j];
				// }
			// }
			memcpy(NextBoard, Board, sizeof(int) * BOUNDARYSIZE * BOUNDARYSIZE);
			// do the move
			// The move is a capture move and the board needs to be updated.
			if (eat_move == 1) {
				update_board(NextBoard, x, y, turn, Liberties);
			}
			else {
				NextBoard[x][y] = turn;
			}
			// Check the history to avoid the repeat board
			// TODO: FIND A MORE EFFICIENT WAY TO COMPARE HISTORY
			//bool repeat_move = 0;
			for (int t = 0 ; t < game_length; ++t) {
				bool repeat_flag = 1;
				for (int i = 1; i <=BOARDSIZE; ++i) {
					for (int j = 1; j <=BOARDSIZE; ++j) {
						if (NextBoard[i][j] != GameRecord[t][i][j]) {
							repeat_flag = 0;
						}
					}
				}
				if (repeat_flag == 1) {
					//repeat_move = 1;
					return 0;
				}
			}
			//if (repeat_move == 0) {
				// 3 digit zxy, z means eat or not, and put at (x, y)
				return eat_move * 100 + x * 10 + y ;
			// }
			//modified!!!
		}
	}
	return 0;
}

// TODO: FIND A MORE EFFICIENT WAY TO GENERATE ALL LEGAL MOVE
/*
 * This function return the number of legal moves with clor "turn" and
 * saves all legal moves in MoveList
 * */
int gen_legal_move(int Board[BOUNDARYSIZE][BOUNDARYSIZE], int turn, int game_length, int GameRecord[MAXGAMELENGTH][BOUNDARYSIZE][BOUNDARYSIZE], int MoveList[HISTORYLENGTH]) {
    int NextBoard[BOUNDARYSIZE][BOUNDARYSIZE];
    int num_neighborhood_self = 0;
    int num_neighborhood_oppo = 0;
    int num_neighborhood_empt = 0;
    int num_neighborhood_boun = 0;
    int legal_moves = 0;
    int next_x, next_y;
    int Liberties[4];
    int NeighboorhoodState[4];
    bool eat_move = 0;
    for (int x = 1 ; x <= BOARDSIZE; ++x) {
	for (int y = 1 ; y <= BOARDSIZE; ++y) {
	    // check if current 
	    if (Board[x][y] == EMPTY) {
		// check the liberty of the neighborhood intersections
		num_neighborhood_self = 0;
		num_neighborhood_oppo = 0;
		num_neighborhood_empt = 0;
		num_neighborhood_boun = 0;
		// count the number of empy, self, opponent, and boundary neighboorhood
		count_neighboorhood_state(Board, x, y, turn,
			&num_neighborhood_empt,
			&num_neighborhood_self,
			&num_neighborhood_oppo,
			&num_neighborhood_boun, NeighboorhoodState);
		// check if the emtpy intersection is a legal move
		next_x = next_y = 0;
		eat_move = 0;
		count_liberty(x, y, Board, Liberties);
		// Case 1: exist empty intersection in the neighborhood
		if (num_neighborhood_empt > 0) {
			next_x = x;
			next_y = y;
			// check if it is a capture move
			for (int d = 0 ; d < MAXDIRECTION; ++d) {
			if (NeighboorhoodState[d] == OPPONENT && Liberties[d] == 1) {
				 eat_move = 1;
			}
			}

		}
		// Case 2: no empty intersection in the neighborhood
		else {
		    // Case 2.1: Surround by the self piece
		    if (num_neighborhood_self + num_neighborhood_boun == MAXDIRECTION) {
			int check_flag = 0, check_eye_flag = num_neighborhood_boun;
			for (int d = 0 ; d < MAXDIRECTION; ++d) {
			    // Avoid fill self eye
			    if (NeighboorhoodState[d]==SELF && Liberties[d] > 1) {
				check_eye_flag++;
			    }
			    // Check if there is one self component which has more than one liberty
			    if (NeighboorhoodState[d]==SELF && Liberties[d] > 1) {
				check_flag = 1;
			    }
			}
			if (check_flag == 1 && check_eye_flag!=4) {
			    next_x = x;
			    next_y = y;
			}
		    }	
		    // Case 2.2: Surround by opponent or both side's pieces.
		    else if (num_neighborhood_oppo > 0) {
			int check_flag = 0;
			int eat_flag = 0;
			for (int d = 0 ; d < MAXDIRECTION; ++d) {
			    // Check if there is one self component which has more than one liberty
			    if (NeighboorhoodState[d]==SELF && Liberties[d] > 1) {
				check_flag = 1;
			    }
			    // Check if there is one opponent's component which has exact one liberty
			    if (NeighboorhoodState[d]==OPPONENT && Liberties[d] == 1) {
				eat_flag = 1;
			    }
			}
			if (check_flag == 1) {
			    next_x = x;
			    next_y = y;
			    if (eat_flag == 1) {
				eat_move = 1;
			    }
			}
			else { // check_flag == 0
			    if (eat_flag == 1) {
				next_x = x;
				next_y = y;
				eat_move = 1;
			    }
			}
		    }	
		}
		if (next_x !=0 && next_y !=0) {
		// copy the current board to next board
			//memcpy(NextBoard, Board, sizeof(int) * BOUNDARYSIZE * BOUNDARYSIZE);
		    for (int i = 0 ; i < BOUNDARYSIZE; ++i) {
			for (int j = 0 ; j < BOUNDARYSIZE; ++j) {
			    NextBoard[i][j] = Board[i][j];
			}
		    }
		    // do the move
		    // The move is a capture move and the board needs to be updated.
		    if (eat_move == 1) {
			// update_board(NextBoard, next_x, next_y, turn);
			update_board(NextBoard, x, y, turn, Liberties);
		    }
		    else {
			NextBoard[x][y] = turn;
		    }
		    // Check the history to avoid the repeat board
		    bool repeat_move = 0;
		    for (int t = 0 ; t < game_length; ++t) {
			bool repeat_flag = 1;
			for (int i = 1; i <=BOARDSIZE; ++i) {
			    for (int j = 1; j <=BOARDSIZE; ++j) {
				if (NextBoard[i][j] != GameRecord[t][i][j]) {
				    repeat_flag = 0;
				}
			    }
			}
			if (repeat_flag == 1) {
			    repeat_move = 1;
			    break;
			}
		    }
		    if (repeat_move == 0) {
			// 3 digit zxy, z means eat or not, and put at (x, y)
			MoveList[legal_moves] = eat_move * 100 + next_x * 10 + y ;
			legal_moves++;
		    }
		}
	    }
	}
    }
    return legal_moves;
}
// TODO: MAKE RANDOM MOVE MORE EFFICIENT
// ::DONE: Random pick a move and check whether it is legal.
int rand_gen_legal_move(int Board[BOUNDARYSIZE][BOUNDARYSIZE], int turn, int game_length, int GameRecord[MAXGAMELENGTH][BOUNDARYSIZE][BOUNDARYSIZE]) {
    unsigned char MoveList[BOARDSIZE * BOARDSIZE];
	int moveN = 0;
	for(int i = 1; i <= BOARDSIZE; i ++){
		for(int j = 1; j <= BOARDSIZE; j ++){
			MoveList[moveN] = ((i<<4) + j);
			moveN ++;
		}
	}
    int returned_move = 0;
    while(moveN > 0) {
		int move_id = rand()%moveN;
		int move = MoveList[move_id];
		returned_move = check_legal(Board, (move>>4)&15, move&15, turn, game_length, GameRecord);
		if(returned_move > 0)
			return returned_move;
		moveN --;
		MoveList[move_id] = MoveList[moveN];
	}
	return 0;
}
/*
 * This function randomly selects one move from the MoveList.
 * */
int rand_pick_move(int num_legal_moves, int MoveList[HISTORYLENGTH]) {
    if (num_legal_moves == 0)
	return 0;
    else {
	int move_id = rand()%num_legal_moves;
	return MoveList[move_id];
    }
}
/*
 * This function update the Board with put 'turn' at (x,y)
 * where x = (move % 100) / 10 and y = move % 10.
 * Note this function will not check 'move' is legal or not.
 * */
void do_move(int Board[BOUNDARYSIZE][BOUNDARYSIZE], int turn, int move) {
    int move_x = (move % 100) / 10;
    int move_y = move % 10;
    if (move<100) {
	Board[move_x][move_y] = turn;
    }
    else {
	// update_board(Board, move_x, move_y, turn);
	update_board(Board, move_x, move_y, turn, NULL);
    }

}
/* 
 * This function records the current game baord with current
 * game length "game_length"
 * */
void record(int Board[BOUNDARYSIZE][BOUNDARYSIZE], int GameRecord[MAXGAMELENGTH][BOUNDARYSIZE][BOUNDARYSIZE], int game_length) {
		for (int i = 0 ; i < BOUNDARYSIZE; ++i) {
		    for (int j = 0 ; j < BOUNDARYSIZE; ++j) {
			GameRecord[game_length][i][j] = Board[i][j];
		    }
		}
}

/*
 * This function counts the number of points remains in the board by Black's view
 * */
double final_score(int Board[BOUNDARYSIZE][BOUNDARYSIZE]) {
    int black, white;
    black = white = 0;
    int is_black, is_white;
    for (int i = 1 ; i <= BOARDSIZE; ++i) {
	for (int j = 1; j <= BOARDSIZE; ++j) {
	    switch(Board[i][j]) {
		case EMPTY:
		    is_black = is_white = 0;
		    for(int d = 0 ; d < MAXDIRECTION; ++d) {
			if (Board[i+DirectionX[d]][j+DirectionY[d]] == BLACK) is_black = 1;
			if (Board[i+DirectionX[d]][j+DirectionY[d]] == WHITE) is_white = 1;
		    }
		    if (is_black + is_white == 1) {
			black += is_black;
			white += is_white;
		    }
		    break;
		case WHITE:
		    white++;
		    break;
		case BLACK:
		    black++;
		    break;
	    }
	}
    }
    return black - white;
}
#define NEXTTURN(t) ((t)==BLACK?WHITE:BLACK)
/* */
int simulate(int Board[BOUNDARYSIZE][BOUNDARYSIZE], int turn, int game_length, int GameRecord[MAXGAMELENGTH][BOUNDARYSIZE][BOUNDARYSIZE]) {
    // int MoveList[HISTORYLENGTH];
    int num_legal_moves = 0;
    int return_move = 0;
	int prevPassMove = 0;
	
	while(true)
	{
		return_move = rand_gen_legal_move(Board, turn, game_length, GameRecord);
		if(return_move != 0)
		{
			// return_move = rand_pick_move(num_legal_moves, MoveList);
			prevPassMove = 0;
			do_move(Board, turn, return_move);
		}
		else //pass
		{
			if(prevPassMove)
				break;
			prevPassMove = 1;
		}
		game_length ++ ;
		turn = NEXTTURN(turn);
		record(Board, GameRecord, game_length);
	}
	/*
    double result;
    result = final_score(Board);
    result -= _komi;
	if((turn == BLACK && result > 0) || (turn == WHITE && result < 0)) // win
		return 1;
	else*/
		return 0;
}
/* 
 * This function randomly generate one legal move (x, y) with return value x*10+y,
 * if there is no legal move the function will return 0.
 * */
int genmove(int Board[BOUNDARYSIZE][BOUNDARYSIZE], int turn, int time_limit, int game_length, int GameRecord[MAXGAMELENGTH][BOUNDARYSIZE][BOUNDARYSIZE]) {
    clock_t start_t, end_t, now_t;
    // record start time
    start_t = clock();
    // calculate the time bound
    end_t = start_t + CLOCKS_PER_SEC * (time_limit - 0.06);
	fprintf(stderr, "start_t:%.2fs limit:%d end_t:%.2fs\n", start_t/(double)CLOCKS_PER_SEC, time_limit, end_t/(double)CLOCKS_PER_SEC);
	
    int MoveList[HISTORYLENGTH];
    int num_legal_moves = 0;
    int return_move = 0;

    num_legal_moves = gen_legal_move(Board, turn, game_length, GameRecord, MoveList);
	fprintf(stderr, "legal move number: %d\n", num_legal_moves);
	if(num_legal_moves == 0)
		return 0;
	
	
	int m_idx = 0;
	int testTN = 0;
	int TestN[num_legal_moves];
	int WinN[num_legal_moves];
	double score[num_legal_moves];
	int timeUp = 0;
	memset(TestN, 0, sizeof(int) * num_legal_moves);
	memset(WinN, 0, sizeof(int) * num_legal_moves);
	memset(score, 0, sizeof(double) * num_legal_moves);
	
	int SimBoard[BOUNDARYSIZE][BOUNDARYSIZE];
	int SimGameRecord[MAXGAMELENGTH][BOUNDARYSIZE][BOUNDARYSIZE];
	while(true)
	{
		if(m_idx >= num_legal_moves)
			m_idx = 0;
		memcpy(SimBoard, Board, sizeof(int) * BOUNDARYSIZE * BOUNDARYSIZE);
		memcpy(SimGameRecord, GameRecord, sizeof(int) * MAXGAMELENGTH * BOUNDARYSIZE * BOUNDARYSIZE);
		
		do_move(SimBoard, turn, MoveList[m_idx]);
		record(SimBoard, SimGameRecord, game_length+1);
		
		simulate(SimBoard, NEXTTURN(turn), game_length+1, SimGameRecord);
		
		double result = final_score(SimBoard) - _komi;
		if(result > 100) fprintf(stderr, "ERROR win by score=%f\n", result);
		score[m_idx] += result;
		if((turn == BLACK && result > 0) || (turn == WHITE && result < 0)) // win
			WinN[m_idx] ++ ;
		TestN[m_idx] ++;
		testTN ++ ;
	
		//if(WinN[m_idx] > TestN[m_idx]) fprintf(stderr, "ERROR m_idx=%d WinN=%d testN=%d\n", m_idx, WinN[m_idx], TestN[m_idx]);
		now_t = clock();
		//if(end_t < now_t) // time is up
		if((end_t < now_t)) // time is near up, buf time 0.1~0.05
		{
			break;
		}
		m_idx ++;
			
	}
	fprintf(stderr, "simu count:%d  max simu count:%d  now:%.2fs bound:%.2fs\n", testTN, TestN[0], now_t/(double)CLOCKS_PER_SEC, end_t/(double)CLOCKS_PER_SEC);
	
	double maxExScore = -1000;
	double exScore = 0;
	double maxwinP = 0;
	double winP = 0;
	int keeper_idx = 0;
	for(m_idx = 0; m_idx < num_legal_moves; m_idx ++)
	{
		if(TestN[m_idx] > 0)
		{
			winP = (WinN[m_idx] / (double)TestN[m_idx]);
			exScore = score[m_idx] / TestN[m_idx];
			if(exScore > 100) fprintf(stderr, "ERROR exScore=%f score[%d]=%f TestN[%d]=%d \n", exScore, m_idx, score[m_idx], m_idx, TestN[m_idx]);
			//if(winP > 1) fprintf(stderr, "winP=%.2f WinN=%d testN=%d\n", winP, WinN[m_idx], TestN[m_idx]);
		}
		else
		{
			winP = 0;
			exScore = 0;
		}
		if(winP > maxwinP || 
			(winP == maxwinP && 
				((turn == BLACK && exScore > maxExScore) ||
				 (turn == WHITE && exScore < maxExScore)
				)))
		{
			keeper_idx = m_idx;
			maxwinP = winP;
			maxExScore = exScore;
		}
	}
	//return_move = rand_pick_move(num_legal_moves, MoveList);
    return_move = MoveList[keeper_idx];
	fprintf(stderr, "N=%d P(win)=%.2f E(score)=%.2f\n",TestN[keeper_idx], maxwinP, maxExScore);
	fprintf(stderr, ">>> Decision to move:%d\n", return_move);
    do_move(Board, turn, return_move);

    return return_move % 100;
}
/* 
 * Following are commands for Go Text Protocol (GTP)
 *
 * */
const char *KnownCommands[]={
    "protocol_version",
    "name",
    "version",
    "known_command",
    "list_commands",
    "quit",
    "boardsize",
    "clear_board",
    "komi",
    "play",
    "genmove",
    "undo",
    "quit",
    "showboard",
    "final_score"
};

void gtp_final_score(int Board[BOUNDARYSIZE][BOUNDARYSIZE]) {
    double result;
    result = final_score(Board);
    result -= _komi;
    cout << "= ";
    if (result > 0.0) { // Black win
	cout << "B+" << result << endl << endl<< endl;;
    }
    if (result < 0.0) { // White win
	cout << "W+" << -result << endl << endl<< endl;;
    }
    else { // draw
	cout << "0" << endl << endl<< endl;;
    }
}
void gtp_undo(int Board[BOUNDARYSIZE][BOUNDARYSIZE], int game_length, int GameRecord[MAXGAMELENGTH][BOUNDARYSIZE][BOUNDARYSIZE]) {
    if (game_length!=0) {
	for (int i = 1; i <= BOARDSIZE; ++i) {
	    for (int j = 1; j <= BOARDSIZE; ++j) {
		Board[i][j] = GameRecord[game_length][i][j];
	    }
	}
    }
    cout << "= " << endl << endl;
}
void gtp_showboard(int Board[BOUNDARYSIZE][BOUNDARYSIZE]) {
    cout <<"= "<<endl;
    for (int i = 1; i <=BOARDSIZE; ++i) {
	cout << "#";
	cout <<10-i;
	for (int j = 1; j <=BOARDSIZE; ++j) {
	    switch(Board[i][j]) {
		case EMPTY: cout << " .";break;
		case BLACK: cout << " X";break;
		case WHITE: cout << " O";break;
	    }
	}
	cout << endl;
    }
    cout << "#  ";
    for (int i = 1; i <=BOARDSIZE; ++i) 
	cout << LabelX[i] <<" ";
    cout << endl;
    cout << endl;

}
void gtp_protocol_version() {
    cout <<"= 2"<<endl<< endl;
}
void gtp_name() {
    cout <<"= " NAME<< endl<< endl;
}
void gtp_version() {
    cout << "= 1.02" << endl << endl;
}
void gtp_list_commands(){
    cout <<"= "<< endl;
    for (int i = 0 ; i < NUMGTPCOMMANDS; ++i) {
	cout <<KnownCommands[i] << endl;
    }
    cout << endl;
}
void gtp_known_command(const char Input[]) {
    for (int i = 0 ; i < NUMGTPCOMMANDS; ++i) {
	if (strcmp(Input, KnownCommands[i])==0) {
	    cout << "= true" << endl<< endl;
	    return;
	}
    }
    cout << "= false" << endl<< endl;
}
void gtp_boardsize(int size) {
    if (size!=9) {
	cout << "? unacceptable size" << endl<< endl;
    }
    else {
	_board_size = size;
	cout << "= "<<endl<<endl;
    }
}
void gtp_clear_board(int Board[BOUNDARYSIZE][BOUNDARYSIZE], int NumCapture[]) {
    reset(Board);
    NumCapture[BLACK] = NumCapture[WHITE] = 0;
    cout << "= "<<endl<<endl;
}
void gtp_komi(double komi) {
    _komi = komi;
    cout << "= "<<endl<<endl;
}
void gtp_play(char Color[], char Move[], int Board[BOUNDARYSIZE][BOUNDARYSIZE], int game_length, int GameRecord[MAXGAMELENGTH][BOUNDARYSIZE][BOUNDARYSIZE]) {
    int turn, move_i, move_j;
    if (Color[0] =='b' || Color[0] == 'B')
	turn = BLACK;
    else
	turn = WHITE;
    if (strcmp(Move, "PASS") == 0 || strcmp(Move, "pass")==0) {
	record(Board, GameRecord, game_length+1);
    }
    else {
	// [ABCDEFGHJ][1-9], there is no I in the index.
	Move[0] = toupper(Move[0]);
	move_j = Move[0]-'A'+1;
	if (move_j == 10) move_j = 9;
	move_i = 10-(Move[1]-'0');
	// update_board(Board, move_i, move_j, turn);
	update_board(Board, move_i, move_j, turn, NULL);
	record(Board, GameRecord, game_length+1);
    }
    cout << "= "<<endl<<endl;
}
void gtp_genmove(int Board[BOUNDARYSIZE][BOUNDARYSIZE], char Color[], int time_limit, int game_length, int GameRecord[MAXGAMELENGTH][BOUNDARYSIZE][BOUNDARYSIZE]){
    int turn = (Color[0]=='b'||Color[0]=='B')?BLACK:WHITE;
    int move = genmove(Board, turn, time_limit, game_length, GameRecord);
    int move_i, move_j;
    record(Board, GameRecord, game_length+1);
    if (move==0) {
	cout << "= PASS" << endl<< endl<< endl;
    }
    else {
	move_i = (move%100)/10;
	move_j = (move%10);
//	cerr << "#turn("<<game_length<<"): (move, move_i,move_j)" << turn << ": " << move<< " " << move_i << " " << move_j << endl;
	cout << "= " << LabelX[move_j]<<10-move_i<<endl<< endl;
    }
}
/*
 * This main function is used of the gtp protocol
 * */
void gtp_main(int display) {
    char Input[COMMANDLENGTH]="";
    char Command[COMMANDLENGTH]="";
    char Parameter[COMMANDLENGTH]="";
    char Move[4]="";
    char Color[6]="";
    int Board[BOUNDARYSIZE][BOUNDARYSIZE]={{0}};     reset(Board);
    int NumCapture[3]={0};// 1:Black, 2: White
    int GameRecord[MAXGAMELENGTH][BOUNDARYSIZE][BOUNDARYSIZE]={{{0}}};
    int ivalue;
    double dvalue;
    int time_limit = DEFAULTTIME;
    int game_length = 0;
    if (display==1) {
	gtp_list_commands();
	gtp_showboard(Board);
    }
    while (gets(Input) != 0) {
	sscanf(Input, "%s", Command);
	if (Command[0]== '#')
	    continue;

	if (strcmp(Command, "protocol_version")==0) {
	    gtp_protocol_version();
	}
	else if (strcmp(Command, "name")==0) {
	    gtp_name();
	}
	else if (strcmp(Command, "version")==0) {
	    gtp_version();
	}
	else if (strcmp(Command, "list_commands")==0) {
	    gtp_list_commands();
	}
	else if (strcmp(Command, "known_command")==0) {
	    sscanf(Input, "known_command %s", Parameter);
	    gtp_known_command(Parameter);
	}
	else if (strcmp(Command, "boardsize")==0) {
	    sscanf(Input, "boardsize %d", &ivalue);
	    gtp_boardsize(ivalue);
	}
	else if (strcmp(Command, "clear_board")==0) {
	    gtp_clear_board(Board, NumCapture);
	    game_length = 0;
	}
	else if (strcmp(Command, "komi")==0) {
	    sscanf(Input, "komi %lf", &dvalue);
	    gtp_komi(dvalue);
	}
	else if (strcmp(Command, "play")==0) {
	    sscanf(Input, "play %5s %2s", Color, Move);
	    gtp_play(Color, Move, Board, game_length, GameRecord);
	    game_length++;
	    if (display==1) {
		gtp_showboard(Board);
	    }
	}
	else if (strcmp(Command, "genmove")==0) {
	    sscanf(Input, "genmove %s", Color);
		if(time_limit != DEFAULTTIME) fprintf(stderr, "before gen time_limit: %d\n", time_limit);
	    gtp_genmove(Board, Color, time_limit, game_length, GameRecord);
		if(time_limit != DEFAULTTIME) fprintf(stderr, "after gen time_limit: %d\n", time_limit);
	    game_length++;
	    if (display==1) {
		gtp_showboard(Board);
	    }
	}
	else if (strcmp(Command, "quit")==0) {
	    break;
	}
	else if (strcmp(Command, "showboard")==0) {
	    gtp_showboard(Board);
	}
	else if (strcmp(Command, "undo")==0) {
	    game_length--;
	    gtp_undo(Board, game_length, GameRecord);
	    if (display==1) {
		gtp_showboard(Board);
	    }
	}
	else if (strcmp(Command, "final_score")==0) {
	    if (display==1) {
		gtp_showboard(Board);
	    }
	    gtp_final_score(Board);
	}
    }
}
int main(int argc, char* argv[]) {
//    int type = GTPVERSION;// 1: local version, 2: gtp version
    int type = GTPVERSION;// 1: local version, 2: gtp version
    int display = 0; // 1: display, 2 nodisplay
    if (argc > 1) {
	if (strcmp(argv[1], "-display")==0) {
	    display = 1;
	}
	if (strcmp(argv[1], "-nodisplay")==0) {
	    display = 0;
	}
    }
    gtp_main(display);
    return 0;
}
