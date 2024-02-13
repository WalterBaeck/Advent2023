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
	int NrOfStars=0, StarNr, OtherStarNr;
	unsigned long long StarX[1000],StarY[1000];
	unsigned long long X,Y=0,SizeX,SizeY;
	int BlankLine;

#define EMPTY_SPACE 1000000

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
		// Parse any stars on this InputLine
		BlankLine = 1;
		for (X=0; X<InputLen; X++)
			if (InputLine[X]=='#')
			{
				BlankLine = 0;
				StarX[NrOfStars]=X; StarY[NrOfStars]=Y;
				NrOfStars++;
			}
		// Expansion for empty Rows can be done right during input parsing
		if (BlankLine)
		{
			printf("Expanding SizeY because InputLine #%d contains no stars\n", InputLineNr);
			Y += EMPTY_SPACE;
		}
		else
			// Bookkeeping
			Y++;
	} /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);
	SizeX=InputLen; SizeY=Y;

	// Expansion for empty Columns is done after full Grid hs been parsed.
	// Do it back-to-front to not distort coordinates
	for (X=SizeX; X; X--)
	{
		for (StarNr=0; StarNr<NrOfStars; StarNr++)
			if (StarX[StarNr] == X-1)
				break;
		if (StarNr<NrOfStars)
			// A star was found within this column
			continue;
		// No stars found in this column ? Expand everything to the Right side of this Column.
		for (StarNr=0; StarNr<NrOfStars; StarNr++)
			if (StarX[StarNr] > X-1)  StarX[StarNr] += (EMPTY_SPACE - 1);
		printf("Expanding SizeX because column #%d contains no stars\n", X-1);
		SizeX += (EMPTY_SPACE - 1);
	} /* for (X) */
	printf("Found %d stars in an expanded universe SizeY=%llu,SizeX=%llu\n", NrOfStars, SizeY, SizeX);

#if 0
	// Debugging
	printf("Expanded star coordinates:\n");
	for (StarNr=0; StarNr<NrOfStars; StarNr++)
		printf("Star[%d] at [%llu,%llu]\n", StarNr+1, StarY[StarNr], StarX[StarNr]);
#endif

	/* Operation */
	unsigned long long Sum=0;
	signed long long AbsDist;
	// Sum all "taxi" distances between pairs of stars
	for (StarNr=0; StarNr<NrOfStars; StarNr++)
		for (OtherStarNr=0; OtherStarNr<StarNr; OtherStarNr++)
		{
			AbsDist = (StarX[StarNr] - StarX[OtherStarNr]);
			if (AbsDist<0)  Sum -= AbsDist;  else  Sum += AbsDist;
			AbsDist = (StarY[StarNr] - StarY[OtherStarNr]);
			if (AbsDist<0)  Sum -= AbsDist;  else  Sum += AbsDist;
		}

	/* Finalisation */
  printf("Found Sum %llu\n", Sum);
	return 0;
}
