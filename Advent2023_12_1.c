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
	int TokenNr;
	int NrOfMatches, Sum = 0;
	char Orig[100], Copy[100], Result[100], Computed[100];
	unsigned BitNr, NrOfBits;
	unsigned long long BitMask, BitMax, BitFilter;

  /* Gathering data */
	while (fgets(InputLine, 1000, InputFile))
	{
		// Bookkeeping
		InputLineNr++;
		// Strip line ending
		while ((InputLine[strlen(InputLine)-1]=='\n')
				|| (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
		InputLen = strlen(InputLine);
    if (!InputLen)  break;
		for (TokenNr=0; InputPtr=strtok(TokenNr ? NULL : InputLine, " "); TokenNr++)
			switch (TokenNr)
			{
				case 0: 	strcpy(Orig, InputPtr);  break;
     		case 1: strcpy(Result, InputPtr);  break;
				default:
					fprintf(stderr, "Not expecting more than 2 tokens on InputLine #%d\n", InputLineNr);
					exit(3);
			}

	/* operation */
		NrOfMatches=0;
		InputLen = strlen(Orig);
		NrOfBits = 0;
		for (InputPos=0; InputPos<InputLen; InputPos++)
			if (Orig[InputPos]=='?')  NrOfBits++;
		BitMax = (1<<NrOfBits);
		for (BitMask=0; BitMask<BitMax; BitMask++)
		{
			// Evaluate each of these mask possibilities
			strcpy(Copy, Orig);
			for (BitNr=0,BitFilter=1,InputPos=0; BitNr<NrOfBits; BitNr++,BitFilter<<=1)
			{
				while (Copy[InputPos]!='?')  InputPos++;
				if (BitMask & BitFilter)  Copy[InputPos]='#'; else Copy[InputPos]='.';
				InputPos++;
			}

// Debugging
//printf("BitMask %X : Copy %s  ", BitMask, Copy);

			*Computed = '\0';
			int FirstSeen=0,EndPos;
			for (InputPos=0; InputPos<InputLen; InputPos++)
				if (Copy[InputPos]=='#')
				{
//printf("<Start %d-End ", InputPos);
					for (EndPos=InputPos; EndPos<InputLen; EndPos++)
						if (Copy[EndPos]!='#')  break;
//printf("%d>", EndPos);
					if (FirstSeen)
						strcpy(Computed + strlen(Computed), ",");
					FirstSeen = 1;
					sprintf(Computed + strlen(Computed), "%d", EndPos-InputPos);
					InputPos = EndPos;
				}

// Debugging
//printf("Computed %s\n", Computed);

			// Now compare Computed listing with actual desired Result
			if (!strcmp(Computed, Result))  NrOfMatches++;
		} /* for (BitMask) */

		printf("After %d BitMasks, withheld %d Matches\n", BitMax, NrOfMatches);
		Sum += NrOfMatches;

	} /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);

  printf("Found Sum %d\n", Sum);
	return 0;
}
