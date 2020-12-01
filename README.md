# BigThink

Chess engine framework to test AI development. I play a bit of chess but I'm not high-level or anything, so I think it would be interesting to see if I can make a chess engine which can beat me, and maybe even teach me how to play better. I want to start out with a basic engine which uses heuristics and minmax trees, and later on I would like to try and implement a neural net which can find patterns in good positions.

The current design is not very optimal because there's not really a mechanism for traversing the game tree properly. Also game state could probably be optimised using a bitboard instead of object array.

## Things to do

* GUI???
* Playstyles:
	* Random moves (done)
	* Greedy (done)
	* n-depth search
	* heuristics
	* neural net
	
## Notes

### Branching factor

The maximum number of moves by 1 side from a single position seems to be around 218. But this is very unlikely in a real game. The number of opening moves is 20. The average number of moves in an average game state (the branching factor) seems to be 35. This decreases to around 15 in the endgame, and peaks at below 40 in the midgame.

See: https://chess.stackexchange.com/questions/23135/what-is-the-average-number-of-legal-moves-per-turn