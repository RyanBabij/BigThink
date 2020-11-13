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

	bool sideToMove;
	
	Vector <Board*> vSubstates;
	
	
	public:
	
	Piece* aBoard [8][8];
	
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
				}
				else
				{
					aBoard[x][y] = 0;
				}
			}
		}
	}
	
	// Equals operator: Identify identical board states for pruning?
	
	
	bool move (int x1, int y1, int x2, int y2)
	{
		// move piece from (x1,y1) to (x2,y2). Return false if invalid move.
		// assume any target piece is captured.
		if ( isSafe(x1,y1) == false || isSafe(x2,y2) == false ||
		aBoard[x1][y1] == 0 )
		{
			return false;
		}
		
		Piece* movePiece = aBoard[x1][y1];
		aBoard[x1][y1]=0;
		if (aBoard[x2][y2] != 0 )
		{
			delete aBoard[x2][y2];
		}
		aBoard[x2][y2] = movePiece;
		
		return true;
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
		aBoard[0][0] = new Piece ("rook", 'r', WHITE, 0,0);
		aBoard[7][0] = new Piece ("rook", 'r', WHITE, 7,0);
		aBoard[0][7] = new Piece ("rook", 'R', BLACK, 0,7);
		aBoard[7][7] = new Piece ("rook", 'R', BLACK, 7,7);
		
		aBoard[1][0] = new Piece ("knight", 'n', WHITE, 1,0);
		aBoard[6][0] = new Piece ("knight", 'n', WHITE, 6,0);
		aBoard[1][7] = new Piece ("knight", 'N', BLACK, 1,7);
		aBoard[6][7] = new Piece ("knight", 'N', BLACK, 6,7);
		
		aBoard[2][0] = new Piece ("bishop", 'b', WHITE, 2,0);
		aBoard[5][0] = new Piece ("bishop", 'b', WHITE, 5,0);
		aBoard[2][7] = new Piece ("bishop", 'B', BLACK, 2,7);
		aBoard[5][7] = new Piece ("bishop", 'B', BLACK, 5,7);
		
		aBoard[3][0] = new Piece ("queen", 'q', WHITE, 3,0);
		aBoard[4][0] = new Piece ("king", 'k', WHITE, 4,0);
		aBoard[3][7] = new Piece ("queen", 'Q', BLACK, 3,7);
		aBoard[4][7] = new Piece ("king", 'K', BLACK, 4,7);
	}
	
	// Return all states for this side.
	void getAllMoves(bool _whiteTurn)
	{
		// return vector of all possible states which may follow current state
	}
	
	// only return all moves for this piece, in the form of state vector
	Vector <Board*>* getAllMovesFrom(Piece* piece)
	{
		if (piece==0 || isSafe(piece->x,piece->y)==false)
		{
			return 0;
		}
		std::cout<<"Get all from "<<piece->getShortName()<<"\n";
	
		Vector <Board*> * vBoard = new Vector <Board*>;
			
		// pawn, can move up 1 space, attack diagonally.
		if (piece->getShortName() == 'p')
		{
			// can it move up 1 space?
			if (isSafe(piece->x,piece->y+1))
			{
				if ( aBoard[piece->x][piece->y+1] == 0 )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x,piece->y+1);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
					
					// can it move up 2 spaces?
					if (isSafe(piece->x,piece->y+2))
					{
						if ( aBoard[piece->x][piece->y+2] == 0 )
						{
							// piece can move up 1 space. Make state for this.
							Board* subBoard2 = new Board(*this);
							subBoard2->move(piece->x,piece->y,piece->x,piece->y+2);
							std::cout<<"adding move\n";
							vBoard->push(subBoard2);
						}
					}
				}
			}

			
			// can it attack diagonally left?
			if (isSafe(piece->x-1,piece->y+1))
			{
				if ( aBoard[piece->x-1][piece->y+1] != 0 && aBoard[piece->x-1][piece->y+1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-1,piece->y+1);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
			// can it attack diagonally right?
			if (isSafe(piece->x+1,piece->y+1))
			{
				if ( aBoard[piece->x+1][piece->y+1] != 0 && aBoard[piece->x+1][piece->y+1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-1,piece->y+1);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
		}
		// knight, moves in L shape
		if (piece->getShortName() == 'n')
		{
			std::cout<<"checking knight\n";
			// left
			if (isSafe(piece->x-2,piece->y+1))
			{
				if ( aBoard[piece->x-2][piece->y+1] == 0 || aBoard[piece->x-2][piece->y+1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-2,piece->y+1);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
			if (isSafe(piece->x-2,piece->y-1))
			{
				if ( aBoard[piece->x-2][piece->y-1] == 0 || aBoard[piece->x-2][piece->y-1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-2,piece->y-1);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
			// right
			if (isSafe(piece->x+2,piece->y+1))
			{
				if ( aBoard[piece->x+2][piece->y+1] == 0 || aBoard[piece->x+2][piece->y+1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x+2,piece->y+1);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
			if (isSafe(piece->x+2,piece->y-1))
			{
				if ( aBoard[piece->x+2][piece->y-1] == 0 || aBoard[piece->x+2][piece->y-1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x+2,piece->y-1);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
			// up
			if (isSafe(piece->x+1,piece->y+2))
			{
				std::cout<<"safe\n";
				if ( aBoard[piece->x+1][piece->y+2] == 0 || aBoard[piece->x+1][piece->y+2]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x+1,piece->y+2);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
			if (isSafe(piece->x-1,piece->y+2))
			{
				if ( aBoard[piece->x-1][piece->y+2] == 0 || aBoard[piece->x-1][piece->y+2]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-1,piece->y+2);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
			// down
			
		}
			
		if (vBoard->size() == 0)
		{
			delete vBoard;
			return 0;
		}
		
		return vBoard;
	}
	
	// only return all moves for this piece, in the form of state vector
	void addAllMovesFrom(Piece* piece, Vector <Board*> * vBoard)
	{
		if (piece==0 || isSafe(piece->x,piece->y)==false || vBoard==0)
		{
			std::cout<<"addallmoves error\n";
			return;
		}
	
		// pawn, can move up 1 space, attack diagonally.
		if (piece->getShortName() == 'p')
		{
			// can it move up 1 space?
			if (isSafe(piece->x,piece->y+1))
			{
				if ( aBoard[piece->x][piece->y+1] == 0 )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x,piece->y+1);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
					
					// can it move up 2 spaces?
					if (piece->y==1 && isSafe(piece->x,piece->y+2))
					{
						if ( aBoard[piece->x][piece->y+2] == 0 )
						{
							// piece can move up 1 space. Make state for this.
							Board* subBoard2 = new Board(*this);
							subBoard2->move(piece->x,piece->y,piece->x,piece->y+2);
							std::cout<<"adding move\n";
							vBoard->push(subBoard2);
						}
					}
				}
			}

			
			// can it attack diagonally left?
			if (isSafe(piece->x-1,piece->y+1))
			{
				if ( aBoard[piece->x-1][piece->y+1] != 0 && aBoard[piece->x-1][piece->y+1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-1,piece->y+1);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
			// can it attack diagonally right?
			if (isSafe(piece->x+1,piece->y+1))
			{
				if ( aBoard[piece->x+1][piece->y+1] != 0 && aBoard[piece->x+1][piece->y+1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-1,piece->y+1);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
		}
		// knight, moves in L shape
		if (piece->getShortName() == 'n')
		{
			std::cout<<"checking knight\n";
			// left
			if (isSafe(piece->x-2,piece->y+1))
			{
				if ( aBoard[piece->x-2][piece->y+1] == 0 || aBoard[piece->x-2][piece->y+1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-2,piece->y+1);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
			if (isSafe(piece->x-2,piece->y-1))
			{
				if ( aBoard[piece->x-2][piece->y-1] == 0 || aBoard[piece->x-2][piece->y-1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-2,piece->y-1);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
			// right
			if (isSafe(piece->x+2,piece->y+1))
			{
				if ( aBoard[piece->x+2][piece->y+1] == 0 || aBoard[piece->x+2][piece->y+1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x+2,piece->y+1);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
			if (isSafe(piece->x+2,piece->y-1))
			{
				if ( aBoard[piece->x+2][piece->y-1] == 0 || aBoard[piece->x+2][piece->y-1]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x+2,piece->y-1);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
			// up
			if (isSafe(piece->x+1,piece->y+2))
			{
				std::cout<<"safe\n";
				if ( aBoard[piece->x+1][piece->y+2] == 0 || aBoard[piece->x+1][piece->y+2]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x+1,piece->y+2);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
			if (isSafe(piece->x-1,piece->y+2))
			{
				if ( aBoard[piece->x-1][piece->y+2] == 0 || aBoard[piece->x-1][piece->y+2]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-1,piece->y+2);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
			// down
			if (isSafe(piece->x+1,piece->y-2))
			{
				std::cout<<"safe\n";
				if ( aBoard[piece->x+1][piece->y-2] == 0 || aBoard[piece->x+1][piece->y-2]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x+1,piece->y-2);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
			if (isSafe(piece->x-1,piece->y-2))
			{
				if ( aBoard[piece->x-1][piece->y-2] == 0 || aBoard[piece->x-1][piece->y-2]->team != piece->team )
				{
					// piece can move up 1 space. Make state for this.
					Board* subBoard = new Board(*this);
					subBoard->move(piece->x,piece->y,piece->x-1,piece->y-2);
					std::cout<<"adding move\n";
					vBoard->push(subBoard);
				}
			}
			
		}
	}
	
	bool isSafe(const int x, const int y)
	{
		return (x<8 && x>=0 && y<8 && y>=0);
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