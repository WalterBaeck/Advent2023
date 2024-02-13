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
	int X=0,Y=0, XSize,YSize, OtherX,OtherY;
	int Grid[20][20], GridNr=0;
	int Sum=0;

  /* Gathering data */
	while (fgets(InputLine, 1000, InputFile))
	{
		// Bookkeeping
		InputLineNr++;
		// Strip line ending
		while ((InputLine[strlen(InputLine)-1]=='\n')
				|| (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
		InputLen = strlen(InputLine);

    if (!InputLen)
		{
			// This Grid is now complete
			printf("Grid #%2d has size [%d,%d]\n", ++GridNr, YSize, XSize);
			// Solve the received Grid
			int MirrorFound = 0;
			// Find a horizontal Mirror ?
			for (Y=1; Y<YSize; Y++)
			{
				int Diff = 0;
				for (OtherY=0; (OtherY <= Y-1) && (Y+OtherY < YSize); OtherY++)
					for (X=0; X<XSize; X++)
						if (Grid[Y+OtherY][X] != Grid[Y-1-OtherY][X])  Diff++;
				if (Diff < 2)
				{
					printf("Horizontal mirror with Diff %d between lines %d and %d\n", Diff, Y, Y+1);
					if (Diff == 1)
					{
						MirrorFound = 1;
						Sum += (100*Y);
					}
				}
			}
			// Find a vertical Mirror ?
			for (X=1; X<XSize; X++)
			{
				int Diff = 0;
				for (OtherX=0; (OtherX <= X-1) && (X+OtherX < XSize); OtherX++)
					for (Y=0; Y<YSize; Y++)
						if (Grid[Y][X+OtherX] != Grid[Y][X-1-OtherX])  Diff++;
				if (Diff < 2)
				{
					printf("Vertical mirror with Diff %d between columns %d and %d\n", Diff, X, X+1);
					if (Diff == 1)
					{
						MirrorFound = 1;
						Sum += X;
					}
				}
			}
			if (!MirrorFound)
			{
				fprintf(stderr, "Could not find any mirror in Grid #%2d up to InputLine #%d\n",
						GridNr, InputLineNr);
				exit(3);
			}
			// Prepare for a new Grid
			XSize=0; YSize=0;
			continue;
		} /* if (!InputLen) */

		// Just accept a new Gridline
		if (!XSize)  XSize = InputLen;
		for (X=0; X<XSize; X++)
			if (InputLine[X]=='#') Grid[YSize][X]=1; else Grid[YSize][X]=0;
		YSize++;
	} /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);

  printf("Found Sum %d\n", Sum);
	return 0;
}
