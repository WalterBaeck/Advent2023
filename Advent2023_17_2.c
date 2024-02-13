#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int Arguments, char* Argument[])
{
	FILE* InputFile = stdin;
	char InputLine[1000];
	char* InputPtr;
	int InputLineNr=0, InputPos, InputLen;

	if (Arguments == 2)
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
		fprintf(stderr, "%s [InputFile]\n", Argument[0]);
		exit(1);
	}

	/* Initialisation */
	int X,Y, XSize=0,YSize=0;
	typedef enum {
		eUp, eLeft, eDown, eRight, eNone} tDir;
	char* DirName[] = {"Up", "Left", "Down", "Right", "None"};
	tDir Dir;
	typedef struct {
		int Cost;
		int Best[4][7];
	} tGrid;
	tGrid **Grid   = (tGrid**)malloc(200 * sizeof(tGrid*));
	typedef struct {
		int X, Y;
		tDir Dir;
		int Len;
	} tScout;
	tScout *Scout = (tScout*)malloc(1000000 * sizeof(tScout));
	tScout *NewScout = (tScout*)malloc(1000000 * sizeof(tScout));
	tScout *NewScoutPtr, *ScoutPtr;
	int ScoutNr, NrOfScouts, NrOfNewScouts, Len;
	for (Y=0; Y<200; Y++)
	{
		Grid[Y]  = (tGrid*)malloc(200 * sizeof(tGrid));
		// Initially, the whole grid has a high dummy Best score
		for (X=0; X<200; X++)
			for (Dir=eUp; Dir<=eRight; Dir++)
				for (Len=0; Len<7; Len++)
					Grid[Y][X].Best[Dir][Len] = 100000;
	}

  /* Gathering data */
	while (fgets(InputLine, 1000, InputFile))
	{
		InputLineNr++;
		while ((InputLine[strlen(InputLine)-1]=='\n') || (InputLine[strlen(InputLine)-1]=='\r'))
			InputLine[strlen(InputLine)-1] = '\0';
		InputLen = strlen(InputLine);
		if (!InputLine)  break;
		if (!XSize)  XSize=InputLen;
		for (X=0; X<XSize; X++)
			Grid[YSize][X].Cost = InputLine[X]-'0';
		YSize++;
	} /* while (fgets()) */
	printf("%d InputLines were read, Grid [%d,%d]\n", InputLineNr, YSize, XSize);

	/* Setup */
	// Initially, there exists just one Scout, which arrives into the starting cell
	//   with a non-existent direction and maximum length
	Scout->X = 0; 	Scout->Y = 0;  Scout->Dir = eNone;   Scout->Len = 10;
	NrOfScouts = 1;
	// It costs nothing to reach the starting cell 
	for (Dir=eUp; Dir<=eRight; Dir++)
		for (Len=0; Len<7; Len++)
			Grid[0][0].Best[Dir][Len] = 0;

	/* Operation */
	for (;;) // Keep expanding from the current borderline
	{
		NrOfNewScouts = 0;
		NewScoutPtr = NewScout;

		// Over all existing scouts
		for (ScoutNr=0,ScoutPtr=Scout; ScoutNr<NrOfScouts; ScoutNr++,ScoutPtr++)
		{
printf("Scout #%d: Pos[%d,%d] Dir=%s Len=%d\n", ScoutNr, ScoutPtr->Y, ScoutPtr->X, DirName[ScoutPtr->Dir], ScoutPtr->Len);
			// The continuation of the path has basically always 3 possibilities :
			//   make a counterclock turn, a clockwise turn, or keep going straight (Len min=4, max=10).
			// This may spawn 3 new scouts from an existing scout,
			//   but each of these may be refused as non-viable when :
			//   - it drops off the grid
			//   - it does not outperform the Best score at the arrival cell
			// To handle this verification step without duplicating code,
			//  an iterator is used.
			for (int Iterator=0; Iterator<3; Iterator++)
			{
				NewScoutPtr->X = ScoutPtr->X;  NewScoutPtr->Y = ScoutPtr->Y;
				switch (Iterator)
				{
					case 0: // Counter-clockwise turn
						NewScoutPtr->Len = 4;
						switch (ScoutPtr->Dir)
						{
							case eUp:     NewScoutPtr->Dir = eLeft;  NewScoutPtr->X-=4;  break;
							case eLeft:   NewScoutPtr->Dir = eDown;  NewScoutPtr->Y+=4;  break;
							case eDown:   NewScoutPtr->Dir = eRight; NewScoutPtr->X+=4;  break;
							case eRight:  NewScoutPtr->Dir = eUp;    NewScoutPtr->Y-=4;  break;
								// Specifically for starting point:
							case eNone:   NewScoutPtr->Dir = eDown;  NewScoutPtr->Y+=4;  break;
						}
						break;
					case 1: // Clockwise turn
						NewScoutPtr->Len = 4;
						switch (ScoutPtr->Dir)
						{
							case eDown:   NewScoutPtr->Dir = eLeft;  NewScoutPtr->X-=4;  break;
							case eRight:  NewScoutPtr->Dir = eDown;  NewScoutPtr->Y+=4;  break;
							case eUp:     NewScoutPtr->Dir = eRight; NewScoutPtr->X+=4;  break;
							case eLeft:   NewScoutPtr->Dir = eUp;    NewScoutPtr->Y-=4;  break;
								// Specifically for starting point:
							case eNone:   NewScoutPtr->Dir = eRight; NewScoutPtr->X+=4;  break;
						}
						break;
					case 2: // Continue straight
						NewScoutPtr->Len = ScoutPtr->Len + 1;
						NewScoutPtr->Dir = ScoutPtr->Dir;
						switch (ScoutPtr->Dir)
						{
							case eLeft:                              NewScoutPtr->X--;  break;
							case eDown:                              NewScoutPtr->Y++;  break;
							case eRight:                             NewScoutPtr->X++;  break;
							case eUp:                                NewScoutPtr->Y--;  break;
								// Specifically for starting point:
							case eNone: NewScoutPtr->Len = 100;  break;  // make sure this gets refused
						}
						break;
				} /* switch (Iterator) */

printf("  NewScout #%d? Pos[%d,%d] Dir=%s Len=%d\n", NrOfNewScouts, NewScoutPtr->Y, NewScoutPtr->X, DirName[NewScoutPtr->Dir], NewScoutPtr->Len);

				// Verify all reasons to exclude a NewScout
				if (NewScoutPtr->Len > 10)  continue;
				if ((NewScoutPtr->X<0) || (NewScoutPtr->X>=XSize) || (NewScoutPtr->Y<0) || (NewScoutPtr->Y>=YSize))
					continue;
				int NewScore = Grid[NewScoutPtr->Y][NewScoutPtr->X].Cost;
				if (ScoutPtr->Dir < eNone)
				{
					NewScore = Grid[ScoutPtr->Y][ScoutPtr->X].Best[ScoutPtr->Dir][ScoutPtr->Len-4]
						+ Grid[NewScoutPtr->Y][NewScoutPtr->X].Cost;

					printf("    NewScore %d + %d = %d compare to existing %d",
							Grid[ScoutPtr->Y][ScoutPtr->X].Best[ScoutPtr->Dir][ScoutPtr->Len-4],
							Grid[NewScoutPtr->Y][NewScoutPtr->X].Cost, NewScore,
							Grid[NewScoutPtr->Y][NewScoutPtr->X].Best[NewScoutPtr->Dir][NewScoutPtr->Len-4]);
				}

				if (Iterator < 2)
				{
					// Need to also account for intermediate steps in a Len=4 move
					for (int Step=1; Step<4; Step++)
						switch (NewScoutPtr->Dir)
						{
							case eUp:     NewScore += Grid[ScoutPtr->Y-Step][ScoutPtr->X].Cost;  break;
							case eLeft:   NewScore += Grid[ScoutPtr->Y][ScoutPtr->X-Step].Cost;  break;
							case eDown:   NewScore += Grid[ScoutPtr->Y+Step][ScoutPtr->X].Cost;  break;
							case eRight:  NewScore += Grid[ScoutPtr->Y][ScoutPtr->X+Step].Cost;  break;
						}
					printf(" .. and increased to %d\n", NewScore);
				}
				else putchar('\n');

				if (NewScore >= Grid[NewScoutPtr->Y][NewScoutPtr->X].Best[NewScoutPtr->Dir][NewScoutPtr->Len-4])
					continue;
				// This IS really an improvement ; Update the cell highscore, and saveguard this new scout
				Grid[NewScoutPtr->Y][NewScoutPtr->X].Best[NewScoutPtr->Dir][NewScoutPtr->Len-4] = NewScore;
				if (++NrOfNewScouts >= 1000000)
				{
					fprintf(stderr, "Over 1000000 NewScouts ?\n");
					exit(3);
				}
				NewScoutPtr++;
			} /* for (Iterator) */
		} /* for (ScoutNr) */

		if (!NrOfNewScouts)  break;
		printf("= = = Found %d NewScouts = = = =\n", NrOfNewScouts);
		// Move all these NewScouts into the old Scout array
		memcpy(Scout, NewScout, NrOfNewScouts * sizeof(tScout));
		NrOfScouts = NrOfNewScouts;
	} /* forever */

	/* Finalisation */
	int MinCost=100000;
	for (Dir=eUp; Dir<=eRight; Dir++)
		for (Len=0; Len<7; Len++)
			if (Grid[YSize-1][XSize-1].Best[Dir][Len] < MinCost)
				MinCost = Grid[YSize-1][XSize-1].Best[Dir][Len];

	printf("Minimum Cost %d\n", MinCost);
	return 0;
}
