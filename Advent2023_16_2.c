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
	int Sum,SumMax=0;
	int X,Y, XSize=0,YSize=0;
	char **Grid   = (char**)malloc(200 * sizeof(char*));
	int ***Alight = (int***)malloc(200 * sizeof(int**));
	for (Y=0; Y<200; Y++)
	{
		Grid[Y]  = (char*)malloc(200);
		Alight[Y] = (int**)malloc(200 * sizeof(int*));
		for (X=0; X<200; X++)
			Alight[Y][X] = (int*)malloc(4 * sizeof(int));
	} /* for (Y) */

  /* Gathering data */
	while (fgets(InputLine, 1000, InputFile))
	{
		InputLineNr++;
		while ((InputLine[strlen(InputLine)-1]=='\n') || (InputLine[strlen(InputLine)-1]=='\r'))
			InputLine[strlen(InputLine)-1] = '\0';
		InputLen = strlen(InputLine);
		if (!InputLine)  break;
		if (!XSize)  XSize=InputLen;
		strcpy(Grid[YSize], InputLine);
		YSize++;
	} /* while (fgets()) */
	printf("%d InputLines were read, Grid [%d,%d]\n", InputLineNr, YSize, XSize);

	/* Operation */
	typedef enum {
		eUp, eLeft, eDown, eRight} tDir;
	char* DirName[] = {"Up", "Left", "Down", "Right"};
	typedef struct {
		int StartX, StartY;
		tDir Dir;
	} tScout;
	tScout *Scout = (tScout*)malloc(1000 * sizeof(tScout));
	int ScoutNr, NrOfScouts=0;

	tDir IteratorDir=eUp;
	int IteratorX=0, IteratorY=YSize-1;

	// Keep iterating the light beam starting pos,dir
	for (;;)
	{
	// Clear the Alight array
	for (Y=0; Y<200; Y++)
		for (X=0; X<200; X++)
			memset(Alight[Y][X], 0, 4 * sizeof(int));

	// Make the current iterator active
	tDir Dir=IteratorDir;
	X=IteratorX; Y=IteratorY;

	// Over all scouts
	for(;;)
	{
		printf("Starting new Scout: Start[%d,%d] dir %s\n", Y,X,DirName[Dir]);
		// Follow this scout till it goes off-grid
		for (;;)
		{
			// At this place, this scout switches on the light in this direction
			printf("Alight[%d,%d][%d] with dir %s and char %c\n", Y,X,Dir, DirName[Dir], Grid[Y][X]);
			// Perhaps it was already on ?
			if (Alight[Y][X][Dir])
			{
				printf(".. which was already on; stop following this scout\n");
				break;
			}
			else
			{
				printf("Setting Alight\n");
				Alight[Y][X][Dir] = 1;
				printf("Alight has been set\n");
			}
			// What is this place ? That will determine the next location
			switch (Grid[Y][X])
			{
				case '\\':  // Deflector
					switch (Dir)
					{
						case eUp:     Dir=eLeft;   X--;  break;
						case eLeft:   Dir=eUp;     Y--;  break;
						case eDown:   Dir=eRight;  X++;  break;
						case eRight:  Dir=eDown;   Y++;  break;
					}
					break;
				case '/':  // Deflector
					switch (Dir)
					{
						case eUp:     Dir=eRight;  X++;  break;
						case eLeft:   Dir=eDown;   Y++;  break;
						case eDown:   Dir=eLeft;   X--;  break;
						case eRight:  Dir=eUp;     Y--;  break;
					}
					break;
				case '-':  // Splitter
					switch (Dir)
					{
						// The same outcome, regardless of original Dir
						case eUp:
						case eDown:   Dir=eLeft;   X--;
							// Also create a new Scout facing eRight
							Scout[NrOfScouts].StartX = (X+2);
							Scout[NrOfScouts].StartY = Y;
							Scout[NrOfScouts].Dir    = eRight;
							if (X+2 < XSize)
							{
								printf("Creating new Scout[%d] Start[%d,%d] Dir=%s\n",
										NrOfScouts, Scout[NrOfScouts].StartY, Scout[NrOfScouts].StartX,
										DirName[Scout[NrOfScouts].Dir]);
								if (++NrOfScouts>=1000)
								{
									fprintf(stderr, "Over 1000 Scouts!\n");
									exit(3);
								}
							}
							break;
						case eLeft:   X--;  break;
						case eRight:  X++;  break;
					}
					break;
				case '|':  // Splitter
					switch (Dir)
					{
						// The same outcome, regardless of original Dir
						case eLeft:
						case eRight:   Dir=eUp;   Y--;
							// Also create a new Scout facing eDown
							Scout[NrOfScouts].StartX = X;
							Scout[NrOfScouts].StartY = (Y+2);
							Scout[NrOfScouts].Dir    = eDown;
							if (Y+2 < YSize)
							{
								printf("Creating new Scout[%d] Start[%d,%d] Dir=%s\n",
										NrOfScouts, Scout[NrOfScouts].StartY, Scout[NrOfScouts].StartX,
										DirName[Scout[NrOfScouts].Dir]);
								if (++NrOfScouts>=1000)
								{
									fprintf(stderr, "Over 1000 Scouts!\n");
									exit(3);
								}
							}
							break;
						case eUp:    Y--;  break;
						case eDown:  Y++;  break;
					}
					break;
				default:  // Blank
					switch (Dir)
					{
						case eUp:    Y--;  break;
						case eDown:  Y++;  break;
						case eLeft:   X--;  break;
						case eRight:  X++;  break;
					}
			} /* switch (Grid[Y][X]) */
			// Perhaps this scout has wandered off the grid
			if ((X<0) || (X>=XSize) || (Y<0) || (Y>=YSize))
				break;
		} /* forever following a scout */

		printf("Scout finished [%d,%d]\n", Y,X);

		// Is there another Scout to follow ?
		if (NrOfScouts)
		{
			Dir = Scout[NrOfScouts-1].Dir;
			X=Scout[NrOfScouts-1].StartX;
			Y=Scout[NrOfScouts-1].StartY;
			NrOfScouts--;
		}
		else
			break;
	} /* over all scouts */

	// Compute the score for this starting light
	Sum = 0;
	for (Y=0; Y<YSize; Y++)
		for (X=0; X<XSize; X++)
			for (Dir=eUp; Dir<=eRight; Dir++)
				if (Alight[Y][X][Dir])
				{
					Sum++;
					break;
				}
	if (Sum > SumMax)
		SumMax = Sum;

	// Advance the iterator combo
	int EndReached=0;
	switch (IteratorDir)
	{
		case eUp:
			if (++IteratorX >= XSize)
			{
				IteratorDir = eLeft;
				IteratorX = XSize-1;
				IteratorY = 0;
			}
			break;
		case eLeft:
			if (++IteratorY >= YSize)
			{
				IteratorDir = eDown;
				IteratorX = 0;
				IteratorY = 0;
			}
			break;
		case eDown:
			if (++IteratorX >= XSize)
			{
				IteratorDir = eRight;
				IteratorX = 0;
				IteratorY = 0;
			}
			break;
		case eRight:
			if (++IteratorY >= YSize)  EndReached=1;
			break;
	}
	if (EndReached)  break;
	} /* over all starting lights */

	/* Finalisation */
	printf("Maximum Sum %d\n", SumMax);
	return 0;
}
