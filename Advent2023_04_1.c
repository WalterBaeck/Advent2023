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
		fprintf(stderr, "%s <InputFile>\n", Argument[0]);
		exit(1);
	}

	/* Initialisation */
	int Sum=0, Number;
	int WinNr, NrOfWins, HaveNr, NrOfHaves;
	int Win[100], Have[100];
	int CardNr, NrOfMatches, DoingWins;

	/* Operation */
	while (fgets(InputLine, 1000, InputFile))
	{
		// Bookkeeping
		InputLineNr++;
		// Strip line ending
		while ((InputLine[strlen(InputLine)-1]=='\n')
				|| (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
    // Parse this InputLine
		InputLen = strlen(InputLine);
    InputPos = 0;
    InputPtr = InputLine;
    // Start with the Game number
    if (strncmp(InputPtr, "Card ", 5))
		{
			fprintf(stderr, "InputLine #%d does not start with \'Card \'\n", InputLineNr);
			exit(3);
		}
    InputPtr += 5; InputPos += 5;
    if (1 != sscanf(InputPtr, "%d", &CardNr))
		{
			fprintf(stderr, "InputLine #%d does not have CardNr at pos %d\n",
          InputLineNr, InputPos);
			exit(3);
		}
    while (InputPos++, *(++InputPtr) != ':')
    {
      if (InputPos >= InputLen)
      {
        fprintf(stderr, "InputLine #%d does not have seem to contain a colon ?\n",
            InputLineNr);
        exit(3);
      }
    }
		// Start parsing the numbers on this line
		NrOfWins = 0;
		NrOfHaves = 0;
		DoingWins = 1;
    while (InputPos < InputLen)
    {
      while (InputPos++, *(++InputPtr) != ' ')
        if (InputPos >= InputLen)  break;
			if (InputPos >= InputLen)  break;
      InputPtr++; InputPos++;  // Over this space
			if (DoingWins && (*InputPtr == '|'))
			{
				DoingWins = 0;
				continue;
			}
      if (1 != sscanf(InputPtr, "%d", &Number))
      {
        fprintf(stderr, "InputLine #%d does not have a number at pos %d\n",
            InputLineNr, InputPos);
        exit(3);
      }
			if (DoingWins)
				Win[NrOfWins++] = Number;
			else
				Have[NrOfHaves++] = Number;
		} /* while (InputPos) */

		// Now perform the matching
		NrOfMatches = 0;
		for (HaveNr=0; HaveNr<NrOfHaves; HaveNr++)
			for (WinNr=0; WinNr<NrOfWins; WinNr++)
				if (Have[HaveNr] == Win[WinNr])
					NrOfMatches++;
		// Scoring
		if (NrOfMatches)
			Sum += (1<<(NrOfMatches-1));
		// Debugging
		printf("CardNr %d: with %d Matches, add %d to Sum\n", CardNr, NrOfMatches, (1<<(NrOfMatches-1)));
	} /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);

	// Debugging
	printf("Last parse : NrOfWins=%d NrOfHaves=%d\n", NrOfWins, NrOfHaves);

	/* Finalisation */
	printf("Sum found: %d\n", Sum);
	return 0;
}
