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
		fprintf(stderr, "%s [InputFile]\n", Argument[0]);
		fprintf(stderr, "\twith no [InputFile] argument, data will be read from stdin\n");
		exit(1);
	}

	/* Initialisation */
	int GameNr, NrOfCubes, ColorNr;
  int MinCubes[3];
  int Sum=0;

	/* Operation */
	while (fgets(InputLine, 1000, InputFile))
	{
		// Bookkeeping
		InputLineNr++;
		// Strip line ending
		while ((InputLine[strlen(InputLine)-1]=='\n')
				|| (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
    // Parse this InputLine
    memset(MinCubes, 0, 3*sizeof(int));
		InputLen = strlen(InputLine);
    InputPos = 0;
    InputPtr = InputLine;
    // Start with the Game number
    if (strncmp(InputPtr, "Game ", 5))
		{
			fprintf(stderr, "InputLine #%d does not start with \'Game \'\n", InputLineNr);
			exit(3);
		}
    InputPtr += 5; InputPos += 5;
    if (1 != sscanf(InputPtr, "%d", &GameNr))
		{
			fprintf(stderr, "InputLine #%d does not have GameNr at pos %d\n",
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
    // Now keep reading draws
    while (InputPos < InputLen)
    {
      while (InputPos++, *(++InputPtr) != ' ')
      {
        if (InputPos >= InputLen)
        {
          fprintf(stderr, "InputLine #%d should have had a space ?\n",
              InputLineNr);
          exit(3);
        }
      }
      InputPtr++; InputPos++;  // Over this space
      if (1 != sscanf(InputPtr, "%d", &NrOfCubes))
      {
        fprintf(stderr, "InputLine #%d does not have NrOfCubes at pos %d\n",
            InputLineNr, InputPos);
        exit(3);
      }
      while (InputPos++, *(++InputPtr) != ' ')
      {
        if (InputPos >= InputLen)
        {
          fprintf(stderr, "InputLine #%d should have had a space ?\n",
              InputLineNr);
          exit(3);
        }
      }
      InputPtr++; InputPos++;  // Over this space
      if (!strncmp(InputPtr, "red", 3))
      {
        ColorNr = 0;
        InputPtr += 3; InputPos += 3;
      }
      else if (!strncmp(InputPtr, "green", 5))
      {
        ColorNr = 1;
        InputPtr += 5; InputPos += 5;
      }
      else if (!strncmp(InputPtr, "blue", 4))
      {
        ColorNr = 2;
        InputPtr += 4; InputPos += 4;
      }
      else
      {
        fprintf(stderr, "InputLine #%d should have had a color at pos #%d ?\n",
            InputLineNr, InputPos);
        exit(3);
      }
      // This might conclude the InputLine
      if (InputPos < InputLen)
      {
        if ((*InputPtr != ';') && (*InputPtr != ','))
        {
          fprintf(stderr, "InputLine #%d should have comma or semicolon at pos #%d ?\n",
              InputLineNr, InputPos);
          exit(3);
        }
      }
      // At this point, verify constraints
      if (NrOfCubes > MinCubes[ColorNr])
        MinCubes[ColorNr] = NrOfCubes;
    } /* while (InputPos) */
    Sum += (MinCubes[0] * MinCubes[1] * MinCubes[2]);
    // Debugging
    printf("GameNr %d: adding power %d\n",
        GameNr, (MinCubes[0] * MinCubes[1] * MinCubes[2]));
	} /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);

	/* Finalisation */
	printf("Sum of game powers: %d\n", Sum);
	return 0;
}
