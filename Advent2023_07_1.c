#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int Arguments, char* Argument[])
{
	FILE* InputFile = stdin;
	char InputLine[1000];
	char* InputPtr;
	int InputLineNr=0, InputPos, InputLen;

	if (Arguments <= 2)
	{
		if (!(InputFile = fopen(Argument[1], "r")))
		{
			fprintf(stderr, "Could not open %s for reading\n", Argument[1]);
			exit(2);
		}
	}
	else if (Arguments > 2)
	{
		fprintf(stderr, "Usage:\n");
		fprintf(stderr, "%s <InputFile>\n", Argument[0]);
		exit(1);
	}

	/* Initialisation */
	typedef enum {
		eFive, eFour, eFullHouse, eThree, eTwoPair, eOnePair, eNone } tClass;
	typedef struct {
		int Value[5];
		int Bid;
		tClass Class;
	} tHand;
	tHand *Hand = NULL;
	int HandNr, OtherHandNr, NrOfHands=0;
	int CardNr;
	const char CardOrder[] = "AKQJT98765432";
	Hand = (tHand*)malloc(2000 * sizeof(tHand));

	// Helper function
	tClass ClassifyHand(char* Card, int *Value)
	{
		int ValueList[5], Occurs[5];
		int Index;
		memset(ValueList, 0, 5*sizeof(int));
		memset(Occurs,    0, 5*sizeof(int));
		// Count the occurrence of each value in this Hand
		for (CardNr=0; CardNr<5; CardNr++)
		{
			char* Pos = strchr(CardOrder, Card[CardNr]);
			if (!Pos)
			{
				fprintf(stderr, "Unrecognized char #%d on InputLine #%d : %c\n", CardNr, InputLineNr, Card[CardNr]);
				exit(3);
			}
			Value[CardNr] = Pos - CardOrder + 1;
			for (Index=0; Index<5; Index++)
			{
				if (ValueList[Index] == Value[CardNr])
				{
					Occurs[Index]++;
					break;
				}
				if (!Occurs[Index])
				{
					ValueList[Index] = Value[CardNr];
					Occurs[Index]++;
					break;
				}
			} /* for (Index) */
		} /* for (CardNr) */
		// Decide which poker hand this is
		if (Occurs[0]==5)  return eFive;
		if ((Occurs[0]==4) || (Occurs[1]==4))  return eFour;
		if (((Occurs[0]==3) && (Occurs[1]==2)) ||
				((Occurs[0]==2) && (Occurs[1]==3)))  return eFullHouse;
		if ((Occurs[0]==3) || (Occurs[1]==3) || (Occurs[2]==3))  return eThree;
		if (((Occurs[0]==2) && (Occurs[1]==2)) ||
				((Occurs[1]==2) && (Occurs[2]==2)) ||
				((Occurs[0]==2) && (Occurs[2]==2)))  return eTwoPair;
		if ((Occurs[0]==2) || (Occurs[1]==2) || (Occurs[2]==2) || (Occurs[3]==2))  return eOnePair;
		return eNone;
	} /* ClassifyHand() */

  /* Gathering data */
	int TokenNr;
	tHand ParsedHand;
	while (fgets(InputLine, 1000, InputFile))
	{
		// Bookkeeping
		InputLineNr++;
		// Strip line ending
		while ((InputLine[strlen(InputLine)-1]=='\n')
				|| (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
    if (!strlen(InputLine))  break;
		// Parse this Hand
		for (TokenNr=0; InputPtr=strtok(TokenNr?NULL:InputLine, " "); TokenNr++)
			switch (TokenNr)
			{
				case 0:
					ParsedHand.Class = ClassifyHand(InputPtr, ParsedHand.Value);
					break;
				case 1:
					if (1 != sscanf(InputPtr, "%d", &(ParsedHand.Bid)))
					{
						fprintf(stderr, "Could not scan Bid on InputLine #%d : %s\n", InputLineNr, InputPtr);
						exit(3);
					}
					break;
				default:
					fprintf(stderr, "Not expecting a third string on InputLine #%d : %s\n", InputLineNr, InputPtr);
					exit(3);
			}
		// Now insert this Hand into the existing array
		// First find the proper location to keep the array neatly sorted
		for (HandNr=0; HandNr<NrOfHands; HandNr++)
		{
			if (Hand[HandNr].Class > ParsedHand.Class)  break;
			if (Hand[HandNr].Class < ParsedHand.Class)  continue;
			// For equal Class, determine the sorting order by card value
			int Index, NewerIsBetter=0;
			for (Index=0; Index<5; Index++)
			{
				if (Hand[HandNr].Value[Index] > ParsedHand.Value[Index])
				{
					NewerIsBetter = 1;
					break;
				}
				else if (Hand[HandNr].Value[Index] < ParsedHand.Value[Index])
					break;
			}
			if (Index >= 5)
			{
				fprintf(stderr, "Existing Hand #%d (%d-%d-%d-%d-%d Bid %d) has exact same classification\n",
						HandNr, Hand[HandNr].Value[0], Hand[HandNr].Value[1], Hand[HandNr].Value[2],
						Hand[HandNr].Value[3], Hand[HandNr].Value[4], Hand[HandNr].Bid);
				fprintf(stderr, "as InputLine #%d (%d-%d-%d-%d-%d Bid %d) ?\n",
						InputLineNr, ParsedHand.Value[0], ParsedHand.Value[0], ParsedHand.Value[0],
						ParsedHand.Value[0], ParsedHand.Value[0], ParsedHand.Bid);
				exit(4);
			}
			if (NewerIsBetter)  break;
		}
		// HandNr is the insertion location - shift the remainder of the array out by 1 place
		for (OtherHandNr=NrOfHands; OtherHandNr>HandNr; OtherHandNr--)
			memcpy(Hand + OtherHandNr, Hand + OtherHandNr - 1, sizeof(tHand));
		// And now insert the ParsedHand at location HandNr
		memcpy(Hand + HandNr, &ParsedHand, sizeof(tHand));
		// Register the increase in array size
		NrOfHands++;
	} /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);

	/* Operation */
	int Sum=0;
	for (HandNr=0; HandNr<NrOfHands; HandNr++)
	{
		printf("Hand #%d (%d-%d-%d-%d-%d Bid %d) contributes score %d\n",
				HandNr, Hand[HandNr].Value[0], Hand[HandNr].Value[1], Hand[HandNr].Value[2],
				Hand[HandNr].Value[3], Hand[HandNr].Value[4], Hand[HandNr].Bid,
				(NrOfHands-HandNr) * Hand[HandNr].Bid);
		Sum += ((NrOfHands-HandNr) * Hand[HandNr].Bid);
	} /* for (HandNr) */


	/* Finalisation */
  printf("Found Sum %d\n", Sum);
	return 0;
}
