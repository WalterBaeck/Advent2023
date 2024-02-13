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

	/* Operation */
  int StartX, EndX;
  char Text[10];
  int Number, StarFound;
  int LookX;

  typedef struct {
    int X,Y;
    int NumberX,NumberY,Number;
  } tStar;
  tStar Star[1000];
  int StarNr, LookNr, NrOfStars=0;

  // Helper function
  int HasStar(int theX, int theY, int foundBefore, int NumX, int NumY, int Num)
  {
    if (theX < 0)       return 0;
    if (theX >= XSize)  return 0;
    if (theY < 0)       return 0;
    if (theY >= YSize)  return 0;
    if (Grid[theY][theX] == '*')
    {
      if (foundBefore)
      {
        fprintf(stderr, "Star found at [%d,%d] but adjacent Number already had star\n",
            theY, theX);
        exit(4);
      }
      tStar* theStar = Star + NrOfStars;
      theStar->X = theX;
      theStar->Y = theY;
      theStar->NumberX = NumX;
      theStar->NumberY = NumY;
      theStar->Number = Num;
      NrOfStars++;
      return 1;
    }
    return 0;
  } /* int HasSymbol(X,Y) */

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
        // We found a digit string, parse the digit string and add number
        memset(Text, 0, 10);
        strncpy(Text, &(Grid[Y][StartX]), EndX-StartX+1);
        if (1 != sscanf(Text, "%d", &Number))
        {
          fprintf(stderr, "Could not parse Number in Text:%s\n", Text);
          exit(2);
        }
        // Now look around it for symbols
        StarFound = 0;
        for (LookX=StartX-1; LookX<=EndX+1; LookX++)
        {
          if (HasStar(LookX, Y-1, StarFound, StartX, Y, Number))  StarFound=1;
          if (HasStar(LookX, Y+1, StarFound, StartX, Y, Number))  StarFound=1;
        }
        if  (HasStar(StartX-1, Y, StarFound, StartX, Y, Number))  StarFound=1;
        if  (HasStar(  EndX+1, Y, StarFound, StartX, Y, Number))  StarFound=1;
        // Show decision
        //if (StarFound)
          //printf("Number %d at coord[%d,%d] has Star\n", Number, Y, StartX);
        // Skip ahead past this digit string
        X = EndX;
      } /* if (Digit) */
  } /* for (Y) */

  // Now that the entire grid has been scanned, look for the same Star found twice
  for (StarNr=0; StarNr<NrOfStars; StarNr++)
  {
    tStar *BaseStar = Star + StarNr;
    int MatchingStarFound = 0;
    for (LookNr=StarNr+1; LookNr<NrOfStars; LookNr++)
    {
      tStar *LookStar = Star + LookNr;
      if ((LookStar->X == BaseStar->X) && (LookStar->Y == BaseStar->Y))
      {
        if (MatchingStarFound)
        {
          fprintf(stderr, "Found a 2nd match [%d,%d] for star [%d,%d] ?\n",
              LookStar->Y, LookStar->X, BaseStar->Y, BaseStar->X);
          exit(4);
        }
        MatchingStarFound = 1;
        // The same Star with two adjacent Numbers, forms a Gear.
        Sum += (BaseStar->Number * LookStar->Number);
        printf("Found gear %d x %d = %d\n",
            BaseStar->Number, LookStar->Number, BaseStar->Number * LookStar->Number);
      }
    }
  }

	/* Finalisation */
	printf("Found %d Star-Number pairs\n", NrOfStars);
  printf("Computed Sum %d\n", Sum);
	return 0;
}
