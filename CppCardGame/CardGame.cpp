/////////////////////////////////////////////////////////////////////
// CardGame.cpp 								                           				 //
// ver 1.0                                                         //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Platform:    Dell XPS 8900, Windows 10                          //
// Application: A demo for the Producer & consumer problem         //
// Author:      Xinyu Zhang, Syracuse University, 2016 Fall MS     //
//																																 //
/////////////////////////////////////////////////////////////////////
/*
Develop a game of Battle Cards.
====================================================================
Battle Cards has the following rules:
1. There are K (at least 2) players who alternate being dealer.
2. The deck has 52 cards with 4 suits and 13 cards per suit.
3. Before each game the deck is shuffled by the dealer.
4. The deal starts with the non-dealer and alternates between dealer and non-dealer. All cards are dealt from the deck.
5. The players may not look at their cards.
6. The game is made up of a series of battles.
7. Each battle is played to its conclusion. At the end of a battle if a player has less than 5 cards they are out of the game.
8. Every player participates in a battle by putting 1 card on the table face up. The player with the lowest card wins the battle. The Ace is the lowest card in a suit and the king is the highest. If there is a tie for lowest card, both the players play one more cards on to the table face down and another face up. The last card of the two played are then compared. The lowest wins. This step can be repeated until the battle is won.
9. The player winning the battle gets all the cards on the table.
10. If during a battle, a player does not have enough cards to continue, they are out of the game and the game is over.
11. The game is over when there is only one player left.
*/
#define MOVE
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <vector>
#include <queue>
using namespace std;

ofstream out("data1.txt");

class card
{
public:
	int number;
	int suit; //four suits
	static int uniquenumber;
	card();
	void showcard();
	card * next;
};

//0~12 suit 0, 13~25 suit 1...
card::card()
{
	number = uniquenumber % 13;
	suit = (uniquenumber / 13);
	uniquenumber++;
	next = nullptr;
}

void card::showcard()
{

	switch (number)
	{
	case 0: out << "A"; break;  // Ace is low
	case 10: out << "J"; break;
	case 11: out << "Q"; break;
	case 12: out << "K"; break;
	default: out << (number + 1); break;
	}

	switch (suit) {
	case 0: out << "S "; break;
	case 1: out << "H "; break;
	case 2: out << "D "; break;
	case 3: out << "C "; break;
	}
}

class collection
{
public:
	card * start;
	int numberofcards;
	collection(int x);
	collection() { start = nullptr; numberofcards = 0; }
	void showcollection();
	void shuffle();
	card* deal();
	void add(card& a);
private:
	bool inGame;
};


collection::collection(int x)
{

	start = nullptr;
	inGame = true;
	numberofcards = 0;  //
	for (int i = 0; i<x; i++)
	{
#ifndef MOVE
		card temp;
		add(temp);
#else
		card * temp = new card();
		add(*temp);
#endif
	}
}
void collection::showcollection()
{
	int i;
	card * temp;
	temp = start;
	for (i = 0; i<numberofcards; i++)
	{
		temp->showcard();
		temp = temp->next;
	}

}

//Add a card to the front of the linked list.
void collection::add(card& a)
{
#ifndef MOVE
	card *temp = new card;
	temp->number = a.number;
	temp->suit = a.suit;
	temp->next = start;
	start = temp;
#else
	a.next = start;
	start = &a;
	numberofcards++;
#endif

}

void collection::shuffle()
{
	/*
	Randomize the sequence of cards in the deck.
	*/
	card *temp2, *temp3;
	temp2 = start;
	temp3 = temp2;
	int i, j, k;
	char c1;
	int num;
	for (i = numberofcards; i>1; i--) {
		j = rand() % i;
		for (k = 0; k <= j - 1; k++)
			temp3 = temp3->next;
			c1 = temp2->suit;
			num = temp2->number;
			temp2->suit = temp3->suit;
			temp2->number = temp3->number;
			temp3->suit = c1;
			temp3->number = num;
			temp2 = temp2->next;
			temp3 = temp2;
	}

	// Reorganize cards in array randomly
}

card* collection::deal()
{
	// Remove a card from the front of the linked list, and return the card
	if (start == nullptr) return nullptr;
	card *temp = start;
	start = start->next;
	numberofcards--;
	return temp;

}



int card::uniquenumber = 0;

void deal_cards(int k, int d, collection &deck, collection *hand);
void battle(int k, collection *hand, collection *table);
void Enter_Num_players_Dealer(int &k_players, int &dealer);
void Show_Initial_Deck_Hands(int &k_players, int &dealer, collection &deck, collection * hand, collection * table);

int main() {

	collection *hand, *table;
	int num_players, dealer, i, winner = 0;
	srand(time(0));
	Enter_Num_players_Dealer(num_players, dealer);
	collection deck(52);
	hand = new collection[num_players];
	table = new collection[num_players];
	Show_Initial_Deck_Hands(num_players, dealer, deck, hand, table);
	battle(num_players, hand, table);
	return 0;
}

void Show_Initial_Deck_Hands(int &k_players, int &dealer, collection &deck, collection * hand, collection * table) {
	out << "\n\n************** initial deck of cards before shuffle ****\n\n";
	deck.showcollection();
	deck.shuffle(); // Shuffle cards in deck
	out << "\n\n****************** after shuffle **************\n\n";
	deck.showcollection();
	out << "\n\n****************** after cards are dealt **************\n\n";
	deal_cards(k_players, dealer, deck, hand);
	for (int i = 0; i< k_players; i++)
	{
		out << "\ncards for player " << i + 1 << "\n";
		hand[i].showcollection();
	}

}

void Show_Hands_Card(int &k_players, collection * hand) {
	for (int i = 0; i< k_players; i++)
	{
		out << "\n\nHand " << i + 1 << "\n";
		hand[i].showcollection();
	}
}

void Show_Tables_Card(int &k_players, collection * table) {
	for (int i = 0; i< k_players; i++)
	{
		out << "\n\ntable " << i + 1 << "\n";
		table[i].showcollection();
	}
}

void Show_FinalHands_Card(int &k_players, collection * hand) {
	for (int i = 0; i< k_players; i++)
	{
		out << "\n\nFinal hand " << i + 1 << "\n";
		hand[i].showcollection();
	}
}

void Enter_Num_players_Dealer(int &num_players, int &dealer) {

	do {
		cout << "Enter the number (2-10) of players ";
		cin >> num_players;
		if (num_players > 10 || num_players < 2)
			out << "Illegal player number. Enter again\n\n";
	} while (num_players >10 || num_players < 2);

	do {
		cout << "Enter initial dealer (1-k)";
		cin >> dealer;
		if (dealer < 1 || dealer > num_players)
			out << "Illegal dealer ID. Enter again\n\n";
	} while (dealer < 1 || dealer > num_players);
}

std::vector<int> compareCards(int k, collection *table) {
	auto cmp = [](collection& a, collection &b ) {
		return a.start->number > b.start->number;
	};
	std::priority_queue<collection, std::vector<collection>, decltype(cmp)> pq(cmp);
	for (int i = 0; i < k; i++)
		if (table[i].start)
			pq.push(table[i]);
	int min_val = pq.top().start->number;
	vector<int> min_player;


	for (int i = 0; i < k; i++) {
		// make sure there is a card on the table.
		if (table[i].start && table[i].start->number == min_val) {
			min_player.push_back(i);
		}
	}
	return min_player;

}

std::vector<int> compareCardsTie(collection *table, std::vector<int> min_players) {
	auto cmp = [](collection& a, collection &b) {
		return a.start->number > b.start->number;
	};
	std::priority_queue<collection, std::vector<collection>, decltype(cmp)> pq(cmp);
	for (int player : min_players) {
		pq.push(table[player]);
	}

	int min_val = pq.top().start->number;
	std::vector<int> min_player;
	for (int player : min_players) {
		if (table[player].start->number == min_val) {
			min_player.push_back(player);
		}
	}
	return min_player;
}


void battle(int k, collection *hand, collection *table) { //k is num_players
														  /*
														  function battle is the heart of the program.  It implements the card battle operaitons and decides outcomes of card battel according to rules.
														  */
	while (true) {
		Show_Hands_Card(k, hand);
		//put one card
		for (int i = 0; i < k; i++) {
			card * c = hand[i].deal();
			if (c) {
				table[i].add(*c);
			}
		}
		Show_Tables_Card(k, table);

		//compare k cards on the table
		std::vector<int> min_player = compareCards(k, table);

		while (min_player.size() > 1) {
			for (int player : min_player) {
				table[player].add(*hand[player].deal());
				table[player].add(*hand[player].deal());
			}
			Show_Hands_Card(k, hand);
			Show_Tables_Card(k, table);
			min_player = compareCardsTie(table, min_player);
		}

		if (min_player.size() == 1) {
			int winner = min_player[0];
			//winner absorb all the cards on the table.
			for (int i = 0; i < k; i++) {
				card* c = table[i].deal();
				while (c) {
					hand[winner].add(*c);
					c = table[i].deal();
				}

			}

			//check if the cards on each player's hand is less than five.
			//winner absorb all the cards.
			for (int i = 0; i < k; i++) {
				if (hand[i].numberofcards < 5) {
					//hand[i].ingame = false;
					card* c = hand[i].deal();
					while (c) {
						hand[winner].add(*c);
						c = hand[i].deal();
					}
				}
			}
			hand[winner].shuffle();
			//Show_Hands_Card(k, hand);

			//break in only one player left.
			int avail_players = 0;
			int final_winner;
			for (int i = 0; i < k; i++) {
				if (hand[i].numberofcards > 0) {
					avail_players++;
					final_winner = i + 1;
				}
			}
			if (avail_players == 1) {

				out << "\n\nGame Over!!  The winner is player " << final_winner << "!!";
				Show_FinalHands_Card(k, hand);
				break;
			}
		}

	}


}

//a pack of cards "shuffle the deck"
//deal_cards(k_players, dealer, deck, hand);
void deal_cards(int k, int d, collection &deck, collection *hand) {
	//deal cards to each player
	for (int i = d + 1; deck.start != nullptr; i++) {
		card* temp = deck.deal();
		hand[i % k].add(*temp);
	}

}
