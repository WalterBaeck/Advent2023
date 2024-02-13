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
	int X,Y, SizeX=0, SizeY=0;
	char Grid[200][200];
	char NewGrid[200][200];

  /* Data gathering */
	while (fgets(InputLine, 1000, InputFile))
	{
		// Bookkeeping
		InputLineNr++;
		// Strip line ending
		while ((InputLine[strlen(InputLine)-1]=='\n')
				|| (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
    InputLen = strlen(InputLine);
    if (!InputLen)  break;
		if (!SizeX)  SizeX=InputLen;
		strcpy(Grid[SizeY], InputLine);
		for (X=0; X<SizeX; X++)
			if (Grid[SizeY][X] == 'S')
			{
				Grid[SizeY][X] = 'O';
				printf("Found start at [%d,%d]\n", SizeY,X);
			}
		SizeY++;
  } /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);
	printf("Read in Grid[%d x %d]\n", SizeY, SizeX);

	/* Operation */
	int StepNr;
	for (StepNr=0; StepNr<64; StepNr++)
	{
		// Fill NewGrid with blanks
		for (Y=0; Y<SizeY; Y++)
			for (X=0; X<SizeX; X++)
				NewGrid[Y][X] = ' ';
		// Consider all occupied positions in Grid
		for (Y=0; Y<SizeY; Y++)
			for (X=0; X<SizeX; X++)
				switch (Grid[Y][X])
				{
					case '#': NewGrid[Y][X] = '#';  break;
					case '.':
						if (NewGrid[Y][X] != 'O')
							NewGrid[Y][X]='.';
						break;
					case 'O':
						// Will be leaving this spot, so clear it from NewGrid ..
						// .. unless it has already been reached from elsewhere !
						if (NewGrid[Y][X] != 'O')
							NewGrid[Y][X]='.';
						if ((Y)         && (Grid[Y-1][X] != '#'))  NewGrid[Y-1][X] = 'O';
						if ((X)         && (Grid[Y][X-1] != '#'))  NewGrid[Y][X-1] = 'O';
						if ((Y<SizeY-1) && (Grid[Y+1][X] != '#'))  NewGrid[Y+1][X] = 'O';
						if ((X<SizeX-1) && (Grid[Y][X+1] != '#'))  NewGrid[Y][X+1] = 'O';
						break;
					default:
					{
						fprintf(stderr, "Step #%d: unrecognized char \'%c\' at Grid[%d,%d]\n", StepNr+1, Grid[Y][X], Y,X);
						exit(3);
					}
				} /* switch (Grid[Y][X]) */
		// Doublecheck that no blanks remain in NewGrid
		for (Y=0; Y<SizeY; Y++)
			for (X=0; X<SizeX; X++)
				if (NewGrid[Y][X] == ' ')
				{
					fprintf(stderr, "Step #%d: Grid[%d,%d] still contains a blank ?\n", StepNr+1, Y,X);
					exit(3);
				}
		// Now copy the NewGrid into the old Grid
		for (Y=0; Y<SizeY; Y++)
			for (X=0; X<SizeX; X++)
				Grid[Y][X] = NewGrid[Y][X];

#if 1
		// Debugging
    if (StepNr < 15)
    {
      printf("====== Step %d =====\n", StepNr+1);
      for (Y=0; Y<SizeY; Y++)
      {
        printf(Grid[Y]);
        putchar('\n');
      }
      putchar('\n');
    }
#endif

		/* Finalisation */
		int Sum=0;
		for (Y=0; Y<SizeY; Y++)
			for (X=0; X<SizeX; X++)
				if (Grid[Y][X] == 'O')  Sum++;
		printf("After %d steps, potentially arrived on %d cells\n", StepNr+1, Sum);
	} /* for (StepNr) */

	return 0;
}
