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
  char Grid[200][200];
  int XSize=0,YSize;
  int X,Y,Sum=0;

  /* Gathering data */
	while (fgets(InputLine, 1000, InputFile))
	{
		// Bookkeeping
		InputLineNr++;
		// Strip line ending
		while ((InputLine[strlen(InputLine)-1]=='\n')
				|| (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
    if (!XSize)
      XSize = strlen(InputLine);
    strcpy(Grid[InputLineNr-1], InputLine);
	} /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);
  YSize = InputLineNr;

  // Helper function
  int HasSymbol(int theX, int theY)
  {
    if (theX < 0)       return 0;
    if (theX >= XSize)  return 0;
    if (theY < 0)       return 0;
    if (theY >= YSize)  return 0;
    if (Grid[theY][theX] == '.')  return 0;
    if ((Grid[theY][theX] >= '0') && (Grid[theY][theX] <= '9'))  return 0;
    return 1;
  } /* int HasSymbol(X,Y) */

	/* Operation */
  int StartX, EndX;
  char Text[10];
  int Number, SymbolFound;
  int LookX;
  for (Y=0; Y<YSize; Y++)
  {
    for (X=0; X<XSize; X++)
      if ((Grid[Y][X] >= '0') && (Grid[Y][X] <= '9'))
      {
        StartX = X;
        while (++X < XSize)
          if ((Grid[Y][X] < '0') || (Grid[Y][X] > '9'))
            break;
        EndX = X-1;
        // We found a digit string, now look around it for symbols
        SymbolFound = 0;
        for (LookX=StartX-1; LookX<=EndX+1; LookX++)
        {
          if (HasSymbol(LookX, Y-1))  SymbolFound=1;
          if (HasSymbol(LookX, Y+1))  SymbolFound=1;
        }
        if (HasSymbol(StartX-1, Y))   SymbolFound=1;
        if (HasSymbol(  EndX+1, Y))   SymbolFound=1;
        // Now, based on decision, parse the digit string and add number
        memset(Text, 0, 10);
        strncpy(Text, &(Grid[Y][StartX]), EndX-StartX+1);
        if (1 != sscanf(Text, "%d", &Number))
        {
          fprintf(stderr, "Could not parse Number in Text:%s\n", Text);
          exit(2);
        }
        if (SymbolFound)
        {
          Sum += Number;
          printf("Adding Number %d at coord[%d,%d]\n", Number, Y, StartX);
        }
        else
          printf("NOT using Number %d coord[%d,%d]\n", Number, Y, StartX);
        // Skip ahead past this digit string
        X = EndX;
      } /* if (Digit) */
  } /* for (Y) */

	/* Finalisation */
	printf("Sum of valid numbers: %d\n", Sum);
	return 0;
}
