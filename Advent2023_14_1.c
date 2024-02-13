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
	char Grid[200][200];
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
    if (!InputLen)  break;
		if (!XSize)  XSize=InputLen;
		strcpy(Grid[Y++], InputLine);
	} /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);
	YSize = Y;

	/* Operation */
	for (X=0; X<XSize; X++)
		for (Y=0; Y<YSize; Y++)
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

	// Debugging
	for (Y=0; Y<YSize; Y++)
		printf("%s\n", Grid[Y]);

	/* Finalisation */
	for (X=0; X<XSize; X++)
		for (Y=0; Y<YSize; Y++)
			if (Grid[Y][X]=='O')
				Sum += (YSize-Y);

  printf("Found Sum %d\n", Sum);
	return 0;
}
