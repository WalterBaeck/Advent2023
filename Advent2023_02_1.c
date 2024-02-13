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
  const int cMaxCubes[]={12,13,14};
  int Sum=0, ImpossibleGame;

  // Debugging
#define WRITE_OUT 1
#if WRITE_OUT
  FILE *OutputFile;
  if (!(OutputFile = fopen("Advent2023_02_1.out", "w")))
  {
    fprintf(stderr, "Could not open Advent2023_02_1.out for writing.\n");
    exit(4);
  }
#endif

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
#if WRITE_OUT
    fprintf(OutputFile, "Game %d:", GameNr);
#endif
    // Always add this GameNr to the Sum, we'll subtract later upon violation
    Sum += GameNr;
    ImpossibleGame = 0;
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
      // Debugging
      printf("<%d>", NrOfCubes);
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
      if (NrOfCubes > cMaxCubes[ColorNr])
      {
        // Debugging
        printf("InputLine #%d has too many cubes of color #%d at pos %d\n",
            InputLineNr, ColorNr, InputPos);
        printf("GameNr %d   NrOfCubes %d\n", GameNr, NrOfCubes);
        ImpossibleGame = 1;
      }
#if WRITE_OUT
      fprintf(OutputFile, " %d %s%c",
          NrOfCubes, (ColorNr ? (ColorNr==1 ? "green" : "blue") : "red"),
          (InputPos < InputLen) ? (*InputPtr==',' ? ',' : ';') : '\n');
#endif
    } /* while (InputPos) */
    if (ImpossibleGame)
      Sum -= GameNr;
	} /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);

	/* Finalisation */
	printf("Sum of possible game numbers: %d\n", Sum);
	return 0;
}
