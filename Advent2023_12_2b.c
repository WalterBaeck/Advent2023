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
	int TokenNr, Index;
	unsigned long long NrOfMatches, Sum = 0;
	char Orig[200], *Text;
	int Streak[100], StreakNr, NrOfStreaks;
	memset(Streak, 0, 100*sizeof(int));
	int SumLength[100];

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
				case 0:
					*Orig = '\0';
					for (Index=0; Index<5; Index++)
					{
						if (Index)  strcat(Orig, "?");
						strcat(Orig, InputPtr);
					}
					break;
     		case 1:
					for (StreakNr=0; Text=strtok(StreakNr ? NULL : InputPtr, ","); StreakNr++)
						if (1 != sscanf(Text, "%d", Streak+StreakNr))
						{
							fprintf(stderr, "Could not scan Streak #%d on InputLine #%d\n", StreakNr, InputLineNr);
							exit(3);
						}
					NrOfStreaks = StreakNr;
					for (Index=1; Index<5; Index++)
						memcpy(Streak + NrOfStreaks*Index, Streak, NrOfStreaks * sizeof(int));
					NrOfStreaks *= 5;
					break;
				default:
					fprintf(stderr, "Not expecting more than 2 tokens on InputLine #%d\n", InputLineNr);
					exit(3);
			}
		// Processing the input
		SumLength[0] = 0;
		for (StreakNr=0; StreakNr<NrOfStreaks; StreakNr++)
			SumLength[StreakNr+1] = SumLength[StreakNr] + Streak[StreakNr];
		int NrBlanks = strlen(Orig) - SumLength[NrOfStreaks] - (NrOfStreaks-1);
		int NrHardcoded = 0;
		for (InputPos=0; InputPos<strlen(Orig); InputPos++)
			if (Orig[InputPos] != '?')  NrHardcoded++;

		// Debugging
		printf("=-=-=-=-=    InputLine %3d    =-=-=-=-=\n", InputLineNr);
		printf("String %s\nStrips %d", Orig, *Streak);
		for (StreakNr=1; StreakNr<NrOfStreaks; StreakNr++)
			printf(",%d", Streak[StreakNr]);
		printf("\nLength %d to be filled in with %d Strips and %d Blanks - %d hardcoded\n",
				strlen(Orig), NrOfStreaks, NrBlanks, NrHardcoded);

		if (!NrBlanks)
		{
			// Fully decided InputLine
			Sum++;
			continue;
		}

		// As a convention, Streak[0] does not get the mandatory separator space with it,
		//   while all further Streaks get it IN FRONT of the filled chars.
		//
		// We will be deciding where to put #NrBlanks extra blanks around the Streaks.
		// There are #NrOfStreaks+1 locations possible, range [0 .. NrOfStreaks],
		//   denoting that the blank goes IN FRONT of the Streak AND of the separator space.
		//
		// Every Choice made, will require verification of the Hardcoded chars within the String.
		// When the same Choice is made as for the previous blank, it's just one more character
		//   that needs to conform to a potential hardcoded space at that position.
		// If the Choice goes further than the previous Choice, then one or more Streaks are
		//   now determined to be in between these two Choices.

	/* Operation */
		int Choice[100]={-1}, ChoiceNr=0;
		NrOfMatches = 0;

		// Debugging
		void PrintAttempt()
		{
			printf("Now at:");
			int ChNr, Pos;
			StreakNr = 0;
			for (ChNr=0; ChNr<ChoiceNr; ChNr++)
			{
				while (StreakNr < Choice[ChNr])
				{
					if (StreakNr)  putchar('+');
					for (Pos=0; Pos<Streak[StreakNr]; Pos++)  putchar('S');
					StreakNr++;
				}
				putchar('-');
			}
			putchar('\n');
		} /* void PrintAttempt() */

		for (;;)
		{
			// --- Part 1 : making a new Choice at the current ChoiceNr ---
			for (++Choice[ChoiceNr]; Choice[ChoiceNr]<=NrOfStreaks; Choice[ChoiceNr]++)
			{
				// Verify that this Choice does not violate Hardcoded chars of the String

				int PrevChoice=0, Pos;
				InputPos = 0;
				if (ChoiceNr)
				{
					PrevChoice = Choice[ChoiceNr-1];
					InputPos = SumLength[PrevChoice] + ChoiceNr + PrevChoice;
					// Account for the fact that Streak[0] carries no separator
					if (PrevChoice)  InputPos--;
				}

//				printf("1a.ChoiceNr[%d]=%d ? Verifying from pos %d\n", ChoiceNr, Choice[ChoiceNr], InputPos);

				for (StreakNr=PrevChoice; StreakNr<Choice[ChoiceNr]; StreakNr++)
				{

//					printf("1b.StreakNr[%d]", StreakNr);

					// Verify the separator char
					if (StreakNr)
					{
//						printf(" No # pos[%d]=%c", InputPos, Orig[InputPos]);
						if (Orig[InputPos++] == '#')
						{
//							putchar('\n');
							break;  // Validation failed
						}
					}
					// Verify the filled chars
					for (Pos=0; Pos<Streak[StreakNr]; Pos++)
					{
//						printf(" - no . pos[%d]=%c", InputPos, Orig[InputPos]);
						if (Orig[InputPos++] == '.')  break;
					}
//					putchar('\n');
					if (Pos<Streak[StreakNr])       break;  // Validation failed
				} /* for (StreakNr) */
				if (StreakNr < Choice[ChoiceNr])  continue;  // Validation failed

				// Verify the extra blank to be placed
//				printf("1c.Lastly, no # pos[%d]=%c\n", InputPos, Orig[InputPos]);
				if (Orig[InputPos++] == '#')      continue;  // Validation failed

				// Getting here means validation succeeded
				break;
			} /* for (++Choice[ChoiceNr]) */

			// --- Part 2 : consider backtracking
			if (Choice[ChoiceNr] > NrOfStreaks)
			{
				// Backtracking indeed needed
				if (!ChoiceNr)  break;  /* out of forever loop */
				ChoiceNr--;

//				printf(" 2.Backtracked to ChoiceNr=%d\n", ChoiceNr);

				continue;             /* with the forever loop */
			} /* if (Choice[ChoiceNr] > NrOfStreaks) */

//			printf("1d.Choice[%d] increased to %d\n", ChoiceNr, Choice[ChoiceNr]);

			// --- Part 3 : registering the new Choice
			if (++ChoiceNr >= NrBlanks)
			{
				int Pos, PrevChoice=Choice[ChoiceNr-1];
				InputPos = SumLength[PrevChoice] + NrBlanks + PrevChoice;
				// Account for the fact that Streak[0] carries no separator
				if (PrevChoice)  InputPos--;

//				printf("3a.Complete match ? Verifying from pos %d\n", InputPos);
//				PrintAttempt();

				for (StreakNr=PrevChoice; StreakNr<NrOfStreaks; StreakNr++)
				{

//					printf("3b.StreakNr[%d]", StreakNr);

					// Verify the separator char
					if (StreakNr)
					{
//						printf(" No # pos[%d]=%c", InputPos, Orig[InputPos]);
						if (Orig[InputPos++] == '#')
						{
//							putchar('\n');
							break;  // Validation failed
						}
					}
					// Verify the filled chars
					for (Pos=0; Pos<Streak[StreakNr]; Pos++)
					{
//						printf(" - no . pos[%d]=%c", InputPos, Orig[InputPos]);
						if (Orig[InputPos++] == '.')  break;
					}
//					putchar('\n');
					if (Pos<Streak[StreakNr])       break;  // Validation failed
				} /* for (StreakNr) */
				if (StreakNr >= NrOfStreaks)  // Validation succeeded
				{
					NrOfMatches++;
//					printf(" * * *   MATCH %9llu   * * *\n", NrOfMatches);
				}

				// And go back to reconsider this last successful Choice
				ChoiceNr--;

//				printf("3c.Backtracking from end to ChoiceNr=%d\n", ChoiceNr);
			} /* if (++ChoiceNr >= NrBlanks) */
			else
			{
				// The next Choice should be at least equal to previous Choice
				Choice[ChoiceNr] = Choice[ChoiceNr-1]-1;

//				printf("3d.Starting next Choice[%d]=%d\n", ChoiceNr, Choice[ChoiceNr]);
//				static int Counter=0;
//				if (!((++Counter)&0x7))  PrintAttempt();

			}
		} /* forever */
		printf("InputLine #%d allows %llu Matches\n", InputLineNr, NrOfMatches);
		Sum += NrOfMatches;
	} /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);

  printf("Found Sum %llu\n", Sum);
	return 0;
}
