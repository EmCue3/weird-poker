#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_CARDS 63
#define MAX_HAND 10
#define MAX_COLOURS 7
#define MAX_RANK 9
#define HAND_SIZE 6

const char COLOURS [MAX_COLOURS][12] = {"red", "blue", "green", "yellow", "pink", "violet", "magenta"};
const char RANKS [MAX_RANK][12] = {"two", "three", "four", "five", "six", "jack", "queen", "king", "ace"};
const char TYPES [14][30] = {"OnePair","TwoPair","ThreeOfKind","Bust","ThreeOfKindAndPair","ThreePair","Straight","FourOfKind","FourOfKindPair","TwoThreeOfKind","FiveOfKind","Flush","SixOfKind","StraightFlush"};

typedef enum {red, blue, green, yellow, pink, violet, magenta} colourType;
typedef enum {two, three, four, five, six, jack, queen, king, ace} rankType;
typedef enum {OnePair,TwoPair,ThreeOfKind,Bust,ThreeOfKindAndPair,ThreePair,Straight,FourOfKind,FourOfKindPair,TwoThreeOfKind,FiveOfKind,Flush,SixOfKind,StraightFlush} comboType;

typedef struct{
	colourType colour;
	rankType rank;
	int used = 0;
}cardType;

typedef struct{
	cardType card[HAND_SIZE];
	int mults[HAND_SIZE+1] = {0,0,0,0,0,0,0}; //multiples of ranks, 7 elements for readibility
	comboType combo;
}handType;

int rb (int min, int max){
	return rand() % (max-min+1) + min;
}

void merge (int *A,int lo, int mid, int hi){ //sort for card ranks
	int B[hi];
	int i, j, k;
	
	i = lo;
	j = mid+1;
	k = lo;
	
	while (i <= mid && j <= hi && k <= hi){
		if (A[i] <= A[j]){
			B[k] = A[i];
			i++;
		}
		else {
			B[k] = A[j];
			j++;
		}
		k++;
	}
	
	if (i > mid){
		while (j <= hi){
			B[k] = A[j];
			j++;
			k++;
		}
	}
	
	if (j > hi){
		while (i <= mid){
			B[k] = A[i];
			i++;
			k++;
		}
	}
	
	for (int l = lo;l<=hi;l++){
		A[l] = B[l];
	}
}

void bubbleSortHand(handType *A, int n){ //sort by hand type
	handType temp;
	
	for (int i = 0; i < n-1; i++){
		for(int j = 0; j < n-i-1; j++){
			if (A[j].combo < A[j+1].combo){				
				temp = A[j];
				A[j] = A[j+1];
				A[j+1] = temp;
			}
		}
	}
}

void mergeSort(int *A, int lo, int hi){ //sort for card ranks
	int mid;
	
	if (lo < hi){
		mid = (lo+hi)/2;
		mergeSort(A,lo,mid);
		mergeSort(A,mid+1,hi);
		merge(A,lo,mid,hi);
	}
}

void initDeck(cardType *deck){
	for (int i = red; i <= magenta; i++){
		for (int j = two; j <= ace; j++){
			int index = i*MAX_RANK + j;
			deck[index].colour = (colourType) i;
			deck[index].rank = (rankType) j;
			deck[index].used = 0;
		}
	}
}

void dealHand(handType *hand, cardType *deck, int deal){
	int handIndex;
	for (int i = 0; i < HAND_SIZE; i++){
		
		do{
			handIndex = rb(0,NUM_CARDS-1);
		}while (deck[handIndex].used != 0);
		
		hand[deal].card[i].colour = deck[handIndex].colour;
		hand[deal].card[i].rank = deck[handIndex].rank;
		deck[handIndex].used = 1;
	}
}

int countHand (handType *hand, int deal){ //count number of each rank and if there are multiples
	int counter[MAX_RANK];
	
	for (int i = 0; i < MAX_RANK; i++)
		counter[i] = 0;
		
	for (int i = 0; i < HAND_SIZE+1; i++){
		hand[deal].mults[i] = 0;
	}
	
	for (int i = 0; i < HAND_SIZE; i++){
		counter[hand[deal].card[i].rank]++;
	}
	
	for (int i = 0; i < MAX_RANK; i++){ //counts multiples
		if (counter[i] == 1)
			hand[deal].mults[1]++;
		if (counter[i] == 2)
			hand[deal].mults[2]++;
		if (counter[i] == 3)
			hand[deal].mults[3]++;
		if (counter[i] == 4)
			hand[deal].mults[4]++;
		if (counter[i] == 5)
			hand[deal].mults[5]++;
		if (counter[i] == 6)
			hand[deal].mults[6]++;
	}
}

void typeHand (handType *hand, int deal){
	
	int A[HAND_SIZE];
		
	if (hand[deal].mults[6] == 1)
		hand[deal].combo = SixOfKind;
		
	else if (hand[deal].mults[5] == 1)
		hand[deal].combo = FiveOfKind;
		
	else if (hand[deal].mults[4] == 1){
		if (hand[deal].mults[2] == 1)
			hand[deal].combo = FourOfKindPair;
		
		else hand[deal].combo = FourOfKind;
	}
	
	else if (hand[deal].mults[3]){
		if (hand[deal].mults[3] == 2)
			hand[deal].combo = TwoThreeOfKind;
			
		else if (hand[deal].mults[3] == 1 && hand[deal].mults[2] == 1)
			hand[deal].combo = ThreeOfKindAndPair;
			
		else hand[deal].combo = ThreeOfKind;
	}
	
	else if (hand[deal].mults[2] == 3)
		hand[deal].combo = ThreePair;
		
	else if (hand[deal].mults[2] == 2)
		hand[deal].combo = TwoPair;
		
	else if (hand[deal].mults[2] == 1)
		hand[deal].combo = OnePair;
	
	else{
		int A[HAND_SIZE];
		
		for (int i = 0; i < HAND_SIZE; i++)
			A[i] = hand[deal].card[i].rank;
			
		mergeSort(A,0,HAND_SIZE-1); //sort hand by rank
		
		int testCol = hand[deal].card[0].colour;
		int counter = 1;
		int sameCol = 1;
		
		//see if all have same colour
		for (int counter = 1; counter < HAND_SIZE; counter++){
			if (hand[deal].card[counter].colour != testCol)
				sameCol = 0;
		}
		
		if (sameCol == 1 && A[HAND_SIZE-1] - A[0] == 5) //if same colour and straight using sorted hand ranks
			hand[deal].combo = StraightFlush;
			
		else if (sameCol == 1)
			hand[deal].combo = Flush;	
			
		else if (A[HAND_SIZE-1] - A[0] == 5)
			hand[deal].combo = Straight;	
		
		else hand[deal].combo = Bust;
	}
}

int main(){
	srand (time(NULL));
	
	FILE *out;
	out = fopen("hands.txt","w");

	cardType deck[NUM_CARDS];
	handType hand[MAX_HAND];
	
	int repeat = 0; //if quit or not
	char noQuit[50]; //if quit or not
	int numHand;
		
	do{
		repeat = 0;
		numHand = 0;
		initDeck(deck); //resets deck
		
		do{
			printf("Enter the number of hands to be dealt, between 2 and 10 inclusive:\n");
			scanf("%i",&numHand);
		}while (numHand < 2 || numHand > 10);
		
		fprintf(out,"The following %i hands were dealt:",numHand);
		fprintf(out,"\n____________________________________");

		for(int i = 0; i < numHand; i++){
			dealHand(hand,deck,i); //deals hand from deck
			countHand(hand, i); //counts ranks and if there are multiples
			typeHand(hand, i); //determines hand type
			
			//print hands to file
			fprintf(out,"\n\nHand type: %s\n\n",TYPES[hand[i].combo]);
			
			for(int j = 0; j < HAND_SIZE; j++){
				fprintf(out,"%s %s\n",COLOURS[hand[i].card[j].colour], RANKS[hand[i].card[j].rank]);
			}
		}
		
		bubbleSortHand(hand,numHand); //sort by hand type
		
		fprintf(out,"\n\nHands sorted by type:");
		fprintf(out,"\n____________________________________");
		
		//print hands to file
		for(int i = 0; i < numHand; i++){
			fprintf(out,"\n\nHand type: %s\n\n",TYPES[hand[i].combo]);
			
			for(int j = 0; j < HAND_SIZE; j++){
				fprintf(out,"%s %s\n",COLOURS[hand[i].card[j].colour], RANKS[hand[i].card[j].rank]);
			}
		}
		
		printf("\nHands will be written to file. Type YES to deal new set of hands, type anything else to finish.\n");
		scanf("%s",&noQuit);
		fprintf(out,"\n");
		
		//quit or no
		if (noQuit[0] == 'Y' && noQuit[1] == 'E' && noQuit[2] == 'S' && noQuit[3] == 0)
			repeat = 1;
		
	}while(repeat == 1);
	
	fclose(out);
}
