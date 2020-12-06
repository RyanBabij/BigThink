// Boards can be either current game or possible future game state
// simple A-B tree: Pick best option for white, then pick best option for black.
// heuristics - Remove bad moves from tree.

// step 1 - random moves

// turn 1 - max 16
// turn 2 - max 16 etc.

//complexity: 16^n

// assume always promotion to queen

// idea: use tree to come up with move sets.
// use neural net to decide which states are best aka positional play
// however to start out we can use simple material points system.

// For neural net we can use pattern recognition on board states to recognise
// "good" and "bad" states, and then encourage/avoid transitioning to them.
// Alternatively we can abstract the game to a state hash and transition
// commands. We could break the state up into other useful information too,
// for example a piece's rank and file.

#include <File/FileManagerStatic.hpp>

#include <Container/Vector/Vector.hpp>
#include <Math/Random/RandomLehmer.hpp>

#include <Data/DataTools.hpp>

#include <iostream>
#include <string>
#include <time.h>

#define WHITE true
#define BLACK false

	// board states for analysis.
#define WHITE_CHECK 0b10000000
#define WHITE_CHECKMATE 0b01000000
#define WHITE_NO_KING 0b00100000
#define BLACK_CHECK 0b00001000
#define BLACK_CHECKMATE 0b00000100
#define BLACK_NO_KING 0b00000010
#define STALEMATE_MOVEMENT 0b00001000
#define STALEMATE_MATERIAL 0b00000100
	// pieces
#define WPAWN 244
#define BPAWN 245
#define WROOK 'r'
#define BROOK 'R'
#define WKNIGHT 'n'
#define BKNIGHT 'N'
#define WBISHOP 'b'
#define BBISHOP 'B'
#define WQUEEN 'q'
#define BQUEEN 'Q'
#define WKING 'k'
#define BKING 'K'

// We should convert the team defines to enum
//enum eTeam {WHITE, BLACK, BOTH};

RandomLehmer rng;

#include "Piece.hpp"
#include "Board.hpp"

Board mainBoard;

std::string gameLog = "";

#define LOG_GAMES false

int moveBlackRandom()
{
	if ( mainBoard.randomMove(BLACK) == false )
	{
		// black is unable to move
		// if we are in check, this is checkmate
		if (mainBoard.boardStatus() != 0)
		{
			std::cout<<"Black is in checkmate, white wins.\n";
			return 1;
		}
		else
		{
			std::cout<<"Stalemate\n";
			return 2;
		}
	}
	return 0;
}

int moveBlackGreedy()
{
	if ( mainBoard.greedyMove(BLACK) == false )
	{
		// black is unable to move
		// if we are in check, this is checkmate
		if (mainBoard.boardStatus() != 0)
		{
			std::cout<<"Black is in checkmate, white wins.\n";
			return 1;
		}
		else
		{
			std::cout<<"Stalemate\n";
			return 2;
		}
	}
	return 0;
}

int moveBlackDepth(int _depth, int _breadth)
{
	if ( mainBoard.depthMove(BLACK, _depth, _breadth) == false )
	{
		// black is unable to move
		// if we are in check, this is checkmate
		if (mainBoard.boardStatus() != 0)
		{
			std::cout<<"Black is in checkmate, white wins.\n";
			return 1;
		}
		else
		{
			std::cout<<"Stalemate\n";
			return 2;
		}
	}
	return 0;
}

void printScore()
{
	std::cout<<"Material scores: "<<mainBoard.getMaterialScore(WHITE)-1000
	<<" / "<<mainBoard.getMaterialScore(BLACK)-1000<<"\n";
}

void printBoard(bool _log=true)
{
	std::cout<<mainBoard.getState(true)<<"\n\n";
	if (_log && LOG_GAMES)
	{
		gameLog+=mainBoard.getState(true)+"\n\n";
	}
}

int aiPlay()
{
	// play entire game until somebody wins
	int i=1;
	while (true)
	{	
		std::cout<<"\n\nTurn: "<<i++<<"\n";
		gameLog+="\n\nTurn: "+DataTools::toString(i-1)+"\n\n";
		
		printBoard();
		
		if (mainBoard.hasState(STALEMATE_MATERIAL))
		{
			std::cout<<"Stalemate: Lack of material.\n";
			return 0;
		}
		
		if ( mainBoard.hasState(WHITE_CHECK) )
		{
			std::cout<<"White is in check/checkmate.\n";
			gameLog+="White is in check/checkmate.\n";
		}
		else if ( mainBoard.boardStatus() == 2 )
		{
			std::cout<<"Stalemate.\n";
			gameLog+="Stalemate.\n";
			return 0;
		}
		
		if (mainBoard.hasKing(WHITE) == false)
		{
			std::cout<<"White king is ded.\n";
			return 0;
		}
		
		// if ( mainBoard.randomMove(WHITE) == false )
		// {
			// std::cout<<"White is unable to move.\n";
			// return 0;
		// }
		//mainBoard.materialMove(WHITE);
		//mainBoard.materialDepthMove(WHITE, 2);
		if (mainBoard.greedyMove(WHITE) == false )
		{
			std::cout<<"White cannot move. Stalemate/checkmate.\n";
			return 0;
		}
		
		
		// analysis
		printBoard();
		
		printScore();
		
		if (mainBoard.hasState(STALEMATE_MATERIAL))
		{
			std::cout<<"Stalemate: Lack of material.\n";
			return 0;
		}
		
		if (mainBoard.hasKing(BLACK) == false)
		{
			std::cout<<"Black king is ded.\n";
			return 0;
		}
		
		if (moveBlackDepth(1,999) != 0)
		{
			std::cout<<"White wins\n";
			return 0;
		}
		
		printScore();
	}
	
	// finished
	std::cout<<"Writing game log\n";
	FileManagerStatic::writeFreshString(gameLog,"gamelog.txt");
	return 0;
}

int main (int narg, char ** arg)
{	
	rng.seed(time(NULL));
	
	mainBoard.reset();
	
	return aiPlay();
	
	std::cout<<"\n\nBigThink chess engine\n";
	std::cout<<"Enter 4 digits to make move.\n";
	std::cout<<" a - let AI move\n";
	std::cout<<" b - let AI play full game\n";
	
	int currentTurn=0;
	while(++currentTurn<100)
	{
		
		std::string input;
		
		printBoard();
		
		// command options
		// a - AI turn
		// b - full AI vs AI game
		// [4 digits from 0-7] - make move if valid.
		// s - skip turn for white
		
		std::cout<<"Enter command:\n";
		std::cin>>input;
		
		while (std::cin.fail())
		{
			std::cout<<"Invalid input, try again.\n";
			std::cin.clear();
			std::cin.ignore();
			
			input = "";
			
			--currentTurn;
		}
		
		// process input here.
		if (input.find('b') != std::string::npos)
		{
			// std::ios_base::sync_with_stdio(false);
			// std::cin.tie(NULL);
	
			return aiPlay();
		}
		if (input.find('s') != std::string::npos)
		{
			std::cout<<"Skip turn\n";
			if (moveBlackRandom() != 0)
			{
				return 0;
			}
		}
		else if (input.find('a') != std::string::npos)
		{
			std::cout<<"AI turn\n";
			
			mainBoard.randomMove(WHITE);
			// analysis
			printBoard();
			
			if ( mainBoard.hasKing(BLACK)==false )
			{
				std::cout<<"White wins\n";
				return 0;
			}
			
			mainBoard.randomMove(BLACK);
			
			printBoard();
			
			if ( mainBoard.hasKing(WHITE)==false )
			{
				std::cout<<"Black wins\n";
				return 0;
			}
			
		}
		else
		{
			// process normally.
			// strip the first 4 numbers we find in the string.
			// valid input is 4 numbers from 0-7. Anything else is ignored.
			int digit[4]={-1};
			
			int currentDigit = 0;
			for (unsigned int i=0;i<input.size();++i)
			{
				if (std::isdigit(input[i]))
				{
					int d = DataTools::toInt(input[i]);
					
					if (d >= 0 && d < 8)
					{
						digit[currentDigit] = DataTools::toInt(input[i]);
						++currentDigit;
					}
					if (currentDigit==4)
					{
						break;
					}
				}
			}
			
			// process digits
			if (currentDigit == 4)
			{
				if (mainBoard.move(digit[0],digit[1],digit[2],digit[3]))
				{
					printBoard();
					printScore();
					
					std::cout<<"\n*** Black to move ***\n\n";
					if (moveBlackRandom() != 0)
					{
						return 0;
					}
					//mainBoard.materialMove(BLACK);
					//mainBoard.materialDepthMove(BLACK,2);

					printScore();
				}
			}
			else
			{
				std::cout<<"Error: Invalid input.\n";
			}
		}
	}
	
	return 0;
}