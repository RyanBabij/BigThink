// Boards can be either current game or possible future game state
// simple A-B tree: Pick best option for white, then pick best option for black.
// heuristics - Remove bad moves from tree.

// step 1 - random moves

// turn 1 - max 16
// turn 2 - max 16 etc.

//complexity: 16^n

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
	
	char aBoard [8][8];
	bool whiteMove;
	
	
	public:
	Board()
	{
		//for (int 
	}
	
	void reset()
	{
		whiteMove=true;
		
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
			aBoard[i][1] = 'p';
			aBoard[i][6] = 'P';
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
	void getAllMovesFrom(const int x, const int y)
	{
	}
};

int main (int narg, char ** arg)
{
	return 0;
}