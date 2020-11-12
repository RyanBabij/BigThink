// Boards can be either current game or possible future game state
// simple A-B tree: Pick best option for white, then pick best option for black.
// heuristics - Remove bad moves from tree.

// step 1 - random moves

// turn 1 - max 16
// turn 2 - max 16 etc.

//complexity: 16^n

// assume always promotion to queen

#include <Container/Vector/Vector.hpp>

#include <iostream>
#include <string>

class Piece
{
	std::string name;
	std::string shortName;

	public:
	bool hasMoved;
	bool isPromoted;
	bool isWhite;

	Piece(std::string _name, std::string _shortName)
	{
		name = _name;
		shortName = _shortName;
		hasMoved=false;
		isPromoted=false;
		isWhite=true;
	}
	std::string getName()
	{
		return name;
	}
	std::string getShortName()
	{
		return shortName;
	}
};

class Board
{
	// 0 = empty space
	
	// p = white pawn
	// r = white rook
	// n = white knight
	// b = white bishop
	// q = white queen
	// k = white king
	
	// P = black pawn
	// R = black rook
	// N = black knight
	// B = black bishop
	// Q = black queen
	// K = black king
	
	Piece* aBoard [8][8];
	bool whiteMove;
	
	
	public:
	Board()
	{
		std::cout<<"Board const\n";
		//for (int 
	}
	
	void reset()
	{
		std::cout<<"Resetting board\n";
		whiteMove=true;
		
		// wipe the board
		for (int y=0;y<8;++y)
		{
			for (int x=0; x<8; ++x)
			{
				aBoard[x][y] = 0;
			}
		}
		
		//Piece pawn ("pawn");
		
		for (int i=0;i<8;++i)
		{
			aBoard[i][1] = new Piece ("pawn", "p");
			aBoard[i][6] = new Piece("pawn", "P");
		}
		//aBoard[0][0] = 'r';
		//aBoard[7][0] = 'r';
		//aBoard[1][0] = 'n';
		//aBoard[6][0] = 'n';
	}
	
	void getAllMoves()
	{
		// return vector of all possible states which may follow current state
	}
	
	// only return all moves for this piece, in the form of state vector
	Vector <Board*>* getAllMovesFrom(const int x, const int y)
	{
		// unsafe array or nothing there
		if (isSafe(x,y) == false || aBoard[x][y]==0)
		{
			return 0;
		}
		
		Vector <Board*> * vBoard = new Vector <Board*>;
		
		//if (aBoard[x][y] == 'p')
		//{
		//}
		//else 
		
		return vBoard;
	}
	
	bool isSafe(const int x, const int y)
	{
		return (x<8 && x>0 && y<8 && y>0);
	}
	
	std::string getState()
	{
		std::string strBoard = "";
		for (int y=7;y>=0;--y)
		{
			for (int x=0;x<8;++x)
			{
				if ( aBoard[x][y]==0 )
				{
					strBoard+="-";
				}
				else
				{
					strBoard += aBoard[x][y]->getShortName();
				}
			}
			strBoard+="\n";
		}
		return strBoard;
	}
	
	Vector <Piece*>* getAllPieces(bool _isWhite)
	{
		Vector <Piece*>* vPieces = new Vector <Piece*>;
		// wipe the board
		for (int y=7;y>=0;--y)
		{
			for (int x=0; x<8; ++x)
			{
				if (aBoard[x][y]!=0)
				{
					if (aBoard[x][y]->isWhite == _isWhite)
					{
						vPieces->push(aBoard[x][y]);
					}
				}
			}
		}
		
		if (vPieces->size() == 0)
		{
			delete vPieces;
		}
		
		return vPieces;
	}
};

int main (int narg, char ** arg)
{
	
	Board mainBoard;
	mainBoard.reset();

	std::cout<<"Board state:\n"<<mainBoard.getState()<<"\n\n";
	
	return 0;
}