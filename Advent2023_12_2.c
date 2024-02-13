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
	char Orig[200], Copy[200], *Text;
	int Streak[100], StreakNr, NrOfStreaks;
	memset(Streak, 0, 100*sizeof(int));

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

		// Debugging
		printf("String %s to match %d", Orig, *Streak);
		for (StreakNr=1; StreakNr<NrOfStreaks; StreakNr++)
			printf(",%d", Streak[StreakNr]);
		putchar('\n');

	/* Operation */
		int RunningStreak=0, StreakNr=-1;
		int Choice[100]={-1}, ChoiceNr=0;
		strcpy(Copy, Orig);
		InputPos = -1;
		int RunStr[100], StrNr[100], Pos[100];
		*RunStr = RunningStreak;
		*StrNr = StreakNr;
		*Pos = InputPos;
		InputLen = strlen(Orig);
		NrOfMatches = 0;
		// Recurse all possible choices on this inputline.
		// As soon as the prescribed Streak pattern is broken, undo the previous choice.

		// Helper function
		int Matching(int BeginPos, int EndPos)
		{

		// Debugging
			int PrintPos;
//			printf("<RunStr=%d StrNr=%d> Matching [%d-%d] ? ", RunningStreak, StreakNr, BeginPos, EndPos);
//			for (PrintPos=BeginPos; PrintPos<=EndPos; PrintPos++)
//				putchar(Copy[PrintPos]);

			// Save the starting Streak situation
			int OldRunStr = RunningStreak;
			int OldStrNr = StreakNr;
			// Parse the given segment of the string
			int ParsePos;
			for (ParsePos=BeginPos; ParsePos<=EndPos; ParsePos++)
				// Parse the replaced string
				if (Copy[ParsePos] == '#')
				{
					if (RunningStreak)
					{
						// The existing RunningStreak will increase further,
						//   but this might make it too large for the requirements
						if (++RunningStreak > Streak[StreakNr])
						{
//							printf("  NO\n");
							// Restore the starting Streak situation for a later attempt
							RunningStreak = OldRunStr;
							StreakNr = OldStrNr;
							return 0;
						}
					}
					else
					{
						StreakNr++;
						RunningStreak = 1;
					}
				}
				else /* char is '.' */
				{
					if (RunningStreak)
					{
						// This completes the RunningStreak
						if (RunningStreak == Streak[StreakNr])
							RunningStreak = 0;
						else
						{
//							printf("  NO\n");
							// Restore the starting Streak situation for a later attempt
							RunningStreak = OldRunStr;
							StreakNr = OldStrNr;
							return 0;
						}
					}
				}
			// Gotten to the end ? OK then
//			printf("  YES\n");
			return 1;
		} /* int Matching() */

		for (;;)
		{
			// --- Part 1 : making a new Choice at the current ChoiceNr ---
			Choice[ChoiceNr]++;

//			printf("1.At ChoiceNr=%d, increased Choice to %d\n", ChoiceNr, Choice[ChoiceNr]);

			// --- Part 2 : consider backtracking
			if (Choice[ChoiceNr] >= 2)
			{
				// Backtracking indeed needed
				if (!ChoiceNr)  break;
				ChoiceNr--;
				if (ChoiceNr)
				{
					// Restore status at this previous ChoiceNr
					InputPos = Pos[ChoiceNr-1];
					RunningStreak = RunStr[ChoiceNr-1];
					StreakNr = StrNr[ChoiceNr-1];
				}
				else
				{
					// Restore initial situation
					InputPos = -1;
					RunningStreak = 0;
					StreakNr = -1;
				}

//				printf("2.Backtracked to ChoiceNr=%d, restored InputPos=%d RunStr=%d StrNr=%d\n",
//						ChoiceNr, InputPos, RunningStreak, StreakNr);

				continue;
			} /* if (Choice[ChoiceNr] >= 2) */

			// --- Part 3 : registering the new Choice
			// Is there really any question mark remaining ?
			int BumpedEnd = 0;
			while (Orig[++InputPos] != '?')
				if (InputPos >= InputLen-1)
				{
					// No further question mark can be found
					BumpedEnd = 1;
					// Fulfilled all Streak requirements ?

//					printf("3a.No more \'?\' found. Matching(%d,%d) and StreakNr %d==%d ?\n",
//							Pos[ChoiceNr-1]+1, InputLen-1, StreakNr, TargetStreakNr);

					if (Matching(Pos[ChoiceNr-1]+1, InputLen-1))
					{
						// Possibly need to account for the last open RunningStreak
						if (RunningStreak)
						{
							if (RunningStreak == Streak[StreakNr])
							{
								if (StreakNr == NrOfStreaks-1)
									NrOfMatches++;
							}
						}
						else if (StreakNr == NrOfStreaks-1)
							NrOfMatches++;
					}
					// And go back to reconsider this last successful Choice
					ChoiceNr--;
					// Restore status at this previous ChoiceNr
					InputPos = Pos[ChoiceNr-1];
					RunningStreak = RunStr[ChoiceNr-1];
					StreakNr = StrNr[ChoiceNr-1];

//					printf("3a.Backtracking from EOL to ChoiceNr=%d\n", ChoiceNr);

					break;
				} /* if (InputPos >= InputLen-1) */
			if (!BumpedEnd)
			{
				// This question mark here will now be filled in
				if (!Choice[ChoiceNr])         Copy[InputPos] = '.';
				else if (Choice[ChoiceNr]==1)  Copy[InputPos] = '#';
				// Still satisfying the requirements ?
				int StartPos = ChoiceNr ? Pos[ChoiceNr-1]+1 : 0;

//				printf("3b.Found '\?\' at InputPos=%d, Matching from StartPos=%d\n", InputPos, StartPos);

				if (Matching(StartPos, InputPos))
				{
					// Store status at this ChoiceNr
					Pos[ChoiceNr] = InputPos;
					RunStr[ChoiceNr] = RunningStreak;
					StrNr[ChoiceNr] = StreakNr;
					// The next step of the path starts with a clean slate
					Choice[++ChoiceNr] = -1;

//					printf("3b.Saved InputPos=%d RunStr=%d StrNr=%d, now moving ahead to ChoiceNr=%d\n",
//							InputPos, RunningStreak, StreakNr, ChoiceNr);
				}
				else
					// Undo ++InputPos at the beginning of Part 3
					InputPos--;
			} /* (InputPos < InputLen-1) */
		} /* forever */
		printf("InputLine #%d allows %llu Matches\n", InputLineNr, NrOfMatches);
		Sum += NrOfMatches;
	} /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);

  printf("Found Sum %llu\n", Sum);
	return 0;
}
