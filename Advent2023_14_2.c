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
	int X=0,Y=0, XSize=0,YSize=0, OtherX,OtherY;
	int Cycle;
	char Grid[200][200];
	int Sum=0;
	char* Result[1000];
	int ResultNr;

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
		if (!XSize)  XSize=InputLen;
		strcpy(Grid[Y++], InputLine);
	} /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);
	YSize = Y;

	/* Operation */
	for (Cycle=0; Cycle<1e9; Cycle++)
	{
		// NORTH tilt
		for (X=0; X<XSize; X++)
			for (Y=1; Y<YSize; Y++)
				if (Grid[Y][X]=='O')
				{
					for (OtherY=Y-1; OtherY>=0; OtherY--)
						if (Grid[OtherY][X] != '.')
							break;
					if (OtherY < Y-1)
					{
						Grid[Y][X] = '.';
						Grid[OtherY+1][X] = 'O';
					}
				}

		// WEST tilt
		for (Y=0; Y<YSize; Y++)
			for (X=1; X<XSize; X++)
				if (Grid[Y][X]=='O')
				{
					for (OtherX=X-1; OtherX>=0; OtherX--)
						if (Grid[Y][OtherX] != '.')
							break;
					if (OtherX < X-1)
					{
						Grid[Y][X] = '.';
						Grid[Y][OtherX+1] = 'O';
					}
				}

		// SOUTH tilt
		for (X=0; X<XSize; X++)
			for (Y=YSize-2; Y>=0; Y--)
				if (Grid[Y][X]=='O')
				{
					for (OtherY=Y+1; OtherY<YSize; OtherY++)
						if (Grid[OtherY][X] != '.')
							break;
					if (OtherY > Y+1)
					{
						Grid[Y][X] = '.';
						Grid[OtherY-1][X] = 'O';
					}
				}

		// EAST tilt
		for (Y=0; Y<YSize; Y++)
			for (X=XSize-2; X>=0; X--)
				if (Grid[Y][X]=='O')
				{
					for (OtherX=X+1; OtherX<XSize; OtherX++)
						if (Grid[Y][OtherX] != '.')
							break;
					if (OtherX > X+1)
					{
						Grid[Y][X] = '.';
						Grid[Y][OtherX-1] = 'O';
					}
				}

#if 0
		// Debugging
		for (Y=0; Y<YSize; Y++)
			printf("%s\n", Grid[Y]);
		putchar('\n');
#endif

		// Store this result
		if (Cycle < 1000)
		{
			Result[Cycle] = (char*)malloc(XSize * YSize + 1);
			*Result[Cycle] = '\0';
			for (Y=0; Y<YSize; Y++)
				strcat(Result[Cycle], Grid[Y]);
			// Compare this Result to all previous Results
			for (ResultNr=0; ResultNr<Cycle; ResultNr++)
				if (!strcmp(Result[ResultNr], Result[Cycle]))
				{
					int Period = Cycle-ResultNr;
					printf("After Cycle %d, the same result as after Cycle %d is observed (period %d)\n",
							Cycle, ResultNr, Period);
					while (Cycle < 1e9)
						Cycle += Period;
					// That was one period too far
					Cycle -= Period;
					printf("Therefore, at Cycle %d the same result will also be seen\n", Cycle);
					break;
				}
		} /* if (Cycle<1000) */

		if ((Cycle&0xFFFFFF) == 0xFFFFFF)
			printf("Cycle %9d ...\n", Cycle);
	} /* for (Cycle) */

	/* Finalisation */
	for (X=0; X<XSize; X++)
		for (Y=0; Y<YSize; Y++)
			if (Grid[Y][X]=='O')
				Sum += (YSize-Y);

  printf("Found Sum %d\n", Sum);
	return 0;
}
