// Boards can be either current game or possible future game state
// simple A-B tree: Pick best option for white, then pick best option for black.
// heuristics - Remove bad moves from tree.

// step 1 - random moves

// turn 1 - max 16
// turn 2 - max 16 etc.

//complexity: 16^n

// assume always promotion to queen

// idea: use tree to come up with move sets.
// use neural net to decide which states are best.
// however to start out we can use simple material points system.

#include <Container/Vector/Vector.hpp>

#include <iostream>
#include <string>

#define WHITE true
#define BLACK false

class Piece
{
	std::string name;
	char shortName;

	public:
	bool hasMoved;
	bool isPromoted;
	bool team;
	
	int x,y;

	Piece(std::string _name, char _shortName, bool _team, int _x, int _y)
	{
		name = _name;
		shortName = _shortName;
		hasMoved=false;
		isPromoted=false;
		team=_team;
		x=_x;
		y=_y;
	}
	std::string getName()
	{
		return name;
	}
	char getShortName()
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
	bool sideToMove;
	
	Vector <Board*> vSubstates;
	
	
	public:
	~Board()
	{
		// wipe the board array
		for (int y=0;y<8;++y)
		{ 
			for (int x=0; x<8; ++x)
			{
				if ( aBoard[x][y] != 0 )
				{
					delete aBoard[x][y];
				}
				aBoard[x][y] = 0;
			}
		}
	}
	
	Board()
	{
		std::cout<<"Board const\n";
		
		sideToMove=WHITE;
		// wipe the board array
		for (int y=0;y<8;++y)
		{ 
			for (int x=0; x<8; ++x)
			{
				//if ( aBoard[x][y] != 0 )
				//{
					//delete aBoard[x][y];
				//}
				aBoard[x][y] = 0;
			}
		}
		std::cout<<"End board const\n";
	}
	Board(const Board& board) // copy constructor 
	{
		// I assume the default copy constructor copys the pointers in aBoard,
		// but doesn't copy the Piece objects.
		sideToMove=board.sideToMove;
		
		// wipe the board array
		for (int y=0;y<8;++y)
		{ 
			for (int x=0; x<8; ++x)
			{
				if ( board.aBoard[x][y] != 0 )
				{
					// copy Piece
					aBoard[x][y] = new Piece (*board.aBoard[x][y]);
					//Piece * p = new Piece (*board.aBoard[x][y]);
					//aBoard[x][y] = new Piece("pawn", 'p', WHITE, 0, 0);
				}
				aBoard[x][y] = 0;
			}
		}
	}

	void reset()
	{
		std::cout<<"Resetting board\n";
		sideToMove=WHITE;
		
		// wipe the board
		for (int y=0;y<8;++y)
		{ 
			for (int x=0; x<8; ++x)
			{
				aBoard[x][y] = 0;
			}
		}
		
		for (int i=0;i<8;++i)
		{
			aBoard[i][1] = new Piece ("pawn", 'p', WHITE, i, 1);
			aBoard[i][6] = new Piece("pawn", 'P', BLACK, i, 6);
		}
		//aBoard[0][0] = 'r';
		//aBoard[7][0] = 'r';
		//aBoard[1][0] = 'n';
		//aBoard[6][0] = 'n';
	}
	
	// Return all states for this side.
	void getAllMoves(bool _whiteTurn)
	{
		// return vector of all possible states which may follow current state
	}
	
	// only return all moves for this piece, in the form of state vector
	Vector <Board*>* getAllMovesFrom(Piece* piece)
	{
		if (piece==0 || isSafe(piece->x,piece->y))
		{
			return 0;
		}
	
		 //Vector <Board*>* vBoard = new Vector <Board*>;
		
		// unsafe array or nothing there
		//if (isSafe(x,y) == false || aBoard[x][y]==0)
		//{
		//	return 0;
		//}
		
		Vector <Board*> * vBoard = new Vector <Board*>;
		
		//if (aBoard[x][y] == 'p')
		//{
		//}
		//else 
			
		// pawn, can move up 1 space, attack diagonally.
		if (piece->getShortName() == 'p')
		{
			// can it move up 1 space?
			if (isSafe(piece->x,piece->y+1))
			{
				if ( aBoard[piece->x][piece->y+1] == 0 )
				{
					// piece can move up 1 space. Make state for this.
				}
			}
			// can it attack diagonally left?
			if (isSafe(piece->x-1,piece->y+1))
			{
				if ( aBoard[piece->x-1][piece->y+1] != 0 && aBoard[piece->x-1][piece->y+1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
				}
			}
			// can it attack diagonally right?
			if (isSafe(piece->x+1,piece->y+1))
			{
				if ( aBoard[piece->x+1][piece->y+1] != 0 && aBoard[piece->x+1][piece->y+1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
				}
			}
		}
			
		if (vBoard->size() == 0)
		{
			delete vBoard;
			return 0;
		}
		
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
	
	Vector <Piece*>* getAllPieces(bool _team)
	{
		Vector <Piece*>* vPieces = new Vector <Piece*>;
		// wipe the board
		for (int y=7;y>=0;--y)
		{
			for (int x=0; x<8; ++x)
			{
				if (aBoard[x][y]!=0)
				{
					if (aBoard[x][y]->team == _team)
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
	
	// get all white pieces
	auto vPiece = mainBoard.getAllPieces(WHITE);
	
	std::cout<<"Printing all white pieces:\n";
	if (vPiece == 0)
	{
		std::cout<<"Error: No white pieces found.\n";
		return 0;
	}
	for (int i=0;i<vPiece->size();++i)
	{
		std::cout<<(*vPiece)(i)->getName()<<"\n";
	}
	
	for (int i=0;i<10;++i)
	{
		std::cout<<"Turn: "<<i<<"\n";
		std::cout<<mainBoard.getState()<<"\n";
	}
	
	return 0;
}