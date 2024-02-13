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
  typedef struct {
    int Up, Down, Left, Right;
  } tConnect;
  tConnect **Connect = (tConnect**)malloc(200*sizeof(tConnect*));
  int X,Y;
  int StartX,StartY;
  for (Y=0; Y<200; Y++)
  {
    Connect[Y] = (tConnect*)malloc(200*sizeof(tConnect));
    memset(Connect[Y], 0, 200 * sizeof(tConnect));
  }
  Y=0;

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
    for (X=0; X<InputLen; X++)
      switch(InputLine[X])
      {
        case 'F':
          Connect[Y][X].Right = 1;
          Connect[Y][X].Down  = 1;
          break;
        case '-':
          Connect[Y][X].Left  = 1;
          Connect[Y][X].Right = 1;
          break;
        case 'J':
          Connect[Y][X].Left  = 1;
          Connect[Y][X].Up    = 1;
          break;
        case 'L':
          Connect[Y][X].Right = 1;
          Connect[Y][X].Up    = 1;
          break;
        case '|':
          Connect[Y][X].Up    = 1;
          Connect[Y][X].Down  = 1;
          break;
        case '7':
          Connect[Y][X].Left  = 1;
          Connect[Y][X].Down  = 1;
          break;
        case '.':  break;
        case 'S':
          StartX = X;
          StartY = Y;
          break;
        default:
          fprintf(stderr, "Unrecognized char %c at [%d,%d] ?\n",
              InputLine[X], Y, X);
      } /* switch (InputChar) */
    Y++;
  } /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);

  /* Operation */
  // Find a suitable pipe segment adjacent to the starting point
  // Fortunately, the starting point is not at any edge of the grid
  int LoopLen=1;
  tConnect Prev, Current;
  memset(&Prev, 0, sizeof(tConnect));
  printf("Starting out at [%d,%d]\n", Y,X);
  if (Connect[StartY][StartX-1].Right)
  { Y=StartY; X=StartX-1; Prev.Left=1; }
  else if (Connect[StartY][StartX+1].Left)
  { Y=StartY; X=StartX+1; Prev.Right=1; }
  else if (Connect[StartY-1][StartX].Down)
  { Y=StartY-1; X=StartX; Prev.Up=1; }
  else if (Connect[StartY+1][StartX].Up)
  { Y=StartY+1; X=StartX; Prev.Down=1; }
  // From here, keep following the pipe until we're back at starting point
  for (;;)
  {
    printf("Arrived at [%d,%d]\n", Y,X);
    if ((X==StartX) && (Y==StartY))  break;
    memcpy(&Current, &Connect[Y][X], sizeof(tConnect));
    // Doublecheck that pipe isn't broken, avoid returning
    if (Prev.Up)
    {
      if (!Current.Down)
      {
        fprintf(stderr, "Dir UP into [%d,%d] finds no connecting Down?\n", Y, X);
        exit(3);
      }
      Current.Down = 0;
    }
    else if (Prev.Down)
    {
      if (!Current.Up)
      {
        fprintf(stderr, "Dir DOWN into [%d,%d] finds no connecting Up?\n", Y, X);
        exit(3);
      }
      Current.Up = 0;
    }
    else if (Prev.Right)
    {
      if (!Current.Left)
      {
        fprintf(stderr, "Dir RIGHT into [%d,%d] finds no connecting Left?\n", Y, X);
        exit(3);
      }
      Current.Left = 0;
    }
    else if (Prev.Left)
    {
      if (!Current.Right)
      {
        fprintf(stderr, "Dir LEFT into [%d,%d] finds no connecting Right?\n", Y, X);
        exit(3);
      }
      Current.Right = 0;
    }
    // Now follow the other direction of this segment
    if (Current.Right)  X++;
    else if (Current.Left)  X--;
    else if (Current.Up)  Y--;
    else if (Current.Down)  Y++;
    // Prepare for next iteration
    memcpy(&Prev, &Current, sizeof(tConnect));
    LoopLen++;
  } /* forever */

	/* Finalisation */
    printf("Found LoopLen %d so halfpoint is at %d\n", LoopLen, LoopLen/2);
	return 0;
}
