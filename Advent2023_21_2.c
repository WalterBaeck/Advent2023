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

#define EXPANSION 7
#define MULTIPLICATION (2*EXPANSION+1)

	/* Initialisation */
	int X,Y, SizeX=0,SizeY=0,  StartX,StartY;
	char    **Grid = (char**)malloc(200 * MULTIPLICATION * sizeof(char*));
	char **NewGrid = (char**)malloc(200 * MULTIPLICATION * sizeof(char*));
  // Keep track of First Arrival Times
  int **FirstTime = (int**)malloc(200 * MULTIPLICATION * sizeof(int*));
  for (Y=0; Y<200*MULTIPLICATION; Y++)
  {
    Grid[Y]     = (char*)malloc(200 * MULTIPLICATION * sizeof(char));
    NewGrid[Y]  = (char*)malloc(200 * MULTIPLICATION * sizeof(char));
    FirstTime[Y] = (int*)malloc(200 * MULTIPLICATION * sizeof(int));
    memset(FirstTime[Y], 0, 200*MULTIPLICATION * sizeof(int));
  }

  /* Data gathering */
  int Even=0, Odd=0;
	while (fgets(InputLine, 1000, InputFile))
	{
		// Bookkeeping
		InputLineNr++;
		// Strip line ending
		while ((InputLine[strlen(InputLine)-1]=='\n')
				|| (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
    InputLen = strlen(InputLine);
    if (!InputLen)  break;
		if (!SizeX)  SizeX=InputLen;
		strcpy(Grid[SizeY], InputLine);
		for (X=0; X<SizeX; X++)
    {
			if (Grid[SizeY][X] == 'S')
			{
				Grid[SizeY][X] = '.';
				StartY=SizeY;
				StartX=X;
				printf("Found start at [%d,%d] - %s cell\n", SizeY,X, ((StartY + StartX) & 1) ? "Odd" : "Even");
			}
      // Tally of checkerboard
      if (Grid[SizeY][X] == '.')
      {
        if ((SizeY + X) & 1)
          Odd++;
        else
          Even++;
      }
    } /* for (X) */
		SizeY++;
  } /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);
	printf("Read in Grid[%d x %d] with %d Even and %d Odd open cells\n", SizeY,SizeX, Even,Odd);

  // Duplicate into MULT x MULT Grids
  for (int MulY=0; MulY<MULTIPLICATION; MulY++)
    for (int MulX=0; MulX<MULTIPLICATION; MulX++)
      for (Y=0; Y<SizeY; Y++)
        for (X=0; X<SizeX; X++)
          Grid[MulY*SizeY+Y][MulX*SizeX+X] = Grid[Y][X];
  // Adjust startpos accordingly
  StartY += EXPANSION*SizeY;  StartX += EXPANSION*SizeX;
  printf("Recentered on [%d,%d]\n", StartY,StartX);
  // And put the initial setup back in
  Grid[StartY][StartX] = 'O';
  // Multiply the size
  int OrigSizeX=SizeX, OrigSizeY=SizeY;
  SizeY *= MULTIPLICATION;
  SizeX *= MULTIPLICATION;
  // Preserve string terminations
  for (Y=0; Y<SizeY; Y++)  Grid[Y][SizeX] = '\0';

  /* Analysis */
  typedef struct {
    int Order[200][200];
    int Step;
  } tFullGrid;
  tFullGrid *FullGrid = NULL;
  int FullGridNr, NrOfFullGrids=0;
  // Hypothesize that the FullGrids will eventually repeat with period OrigSizeX=OrigSizeY
  typedef struct {
    int FullGridRef;
    int StartStep;
  } tRepeatGrid;
  int DirY,DirX;
  tRepeatGrid RepeatGrid[3][3];
  memset(RepeatGrid[0], 0, 3 * sizeof(tRepeatGrid));
  memset(RepeatGrid[1], 0, 3 * sizeof(tRepeatGrid));
  memset(RepeatGrid[2], 0, 3 * sizeof(tRepeatGrid));

	/* Operation */
	int StepNr;
	for (StepNr=0; StepNr<1000; StepNr++)
	{
		// Fill NewGrid with blanks
		for (Y=0; Y<SizeY; Y++)
			for (X=0; X<SizeX; X++)
				NewGrid[Y][X] = ' ';
		// Consider all occupied positions in Grid
		for (Y=0; Y<SizeY; Y++)
			for (X=0; X<SizeX; X++)
				switch (Grid[Y][X])
				{
					case '#': NewGrid[Y][X] = '#';  break;
					case '.':
						if (NewGrid[Y][X] != 'O')
							NewGrid[Y][X]='.';
						break;
					case 'O':
						// Will be leaving this spot, so clear it from NewGrid ..
						// .. unless it has already been reached from elsewhere !
						if (NewGrid[Y][X] != 'O')
							NewGrid[Y][X]='.';
						if ((Y)         && (Grid[Y-1][X] != '#'))  NewGrid[Y-1][X] = 'O';
						if ((X)         && (Grid[Y][X-1] != '#'))  NewGrid[Y][X-1] = 'O';
						if ((Y<SizeY-1) && (Grid[Y+1][X] != '#'))  NewGrid[Y+1][X] = 'O';
						if ((X<SizeX-1) && (Grid[Y][X+1] != '#'))  NewGrid[Y][X+1] = 'O';
						break;
					default:
					{
						fprintf(stderr, "Step #%d: unrecognized char \'%c\' at Grid[%d,%d]\n", StepNr+1, Grid[Y][X], Y,X);
						exit(3);
					}
				} /* switch (Grid[Y][X]) */
		// Doublecheck that no blanks remain in NewGrid
		for (Y=0; Y<SizeY; Y++)
			for (X=0; X<SizeX; X++)
				if (NewGrid[Y][X] == ' ')
				{
					fprintf(stderr, "Step #%d: Grid[%d,%d] still contains a blank ?\n", StepNr+1, Y,X);
					exit(3);
				}
		// Now copy the NewGrid into the old Grid
		for (Y=0; Y<SizeY; Y++)
			for (X=0; X<SizeX; X++)
				Grid[Y][X] = NewGrid[Y][X];
    // Monitor the First Time Arrivals
		for (Y=0; Y<SizeY; Y++)
			for (X=0; X<SizeX; X++)
        if ((!FirstTime[Y][X]) && (Grid[Y][X] == 'O'))
          FirstTime[Y][X] = StepNr+1;

#if 0
		// Debugging
		printf("====== Step %d =====\n", StepNr+1);
		for (Y=0; Y<SizeY; Y++)
		{
			//printf(Grid[Y]);
			for (X=0; X<SizeX; X++)
      {
        if (Grid[Y][X] == '#')
          putchar('#');
        else if (FirstTime[Y][X])
        {
          // Place a marker of the _complement_ of First Time Arrival
          if ((StepNr+2-FirstTime[Y][X]) < 10)
            putchar((StepNr+2-FirstTime[Y][X]) + '0');
          else if ((StepNr+2-FirstTime[Y][X]) < 36)
            putchar((StepNr+2-FirstTime[Y][X]) - 10 + 'A');
          else
            putchar((StepNr+2-FirstTime[Y][X]) - 36 + 'a');
        }
        else
          putchar('.');
        if ((X%OrigSizeX)==OrigSizeX-1)  putchar('|');
      } /* for (X) */
			putchar('\n');
      if ((Y%OrigSizeY)==OrigSizeY-1)
      {
        for (X=0; X<SizeX+MULTIPLICATION; X++)  putchar('-');
        putchar('\n');
      }
		} /* for (Y) */
		putchar('\n');
#endif

    // Analysis
    for (int MulY=0; MulY<MULTIPLICATION; MulY++)
      for (int MulX=0; MulX<MULTIPLICATION; MulX++)
      {
        // Scan for subgrids which have just become completed
        int CellJustReached = 0;
        for (Y=0; Y<OrigSizeY; Y++)
          for (X=0; X<OrigSizeX; X++)
            if (StepNr+2-FirstTime[MulY*OrigSizeY+Y][MulX*OrigSizeX+X] == 1)
              CellJustReached = 1;
        if (!CellJustReached)  continue;
        // Also, is this entire subgrid full ?
        int SubGridFull = 1;
        for (Y=0; Y<OrigSizeY; Y++)
          for (X=0; X<OrigSizeX; X++)
            if ((!FirstTime[MulY*OrigSizeY+Y][MulX*OrigSizeX+X]) &&
                (Grid[MulY*OrigSizeY+Y][MulX*OrigSizeX+X] != '#'))
              SubGridFull = 0;
        if (!SubGridFull)  continue;
        // Compare to the existing catalog of FullGrids
        for (FullGridNr=0; FullGridNr<NrOfFullGrids; FullGridNr++)
        {
          int FullGridFound = 1;
          for (Y=0; Y<OrigSizeY; Y++)
            for (X=0; X<OrigSizeX; X++)
              if ((FirstTime[MulY*OrigSizeY+Y][MulX*OrigSizeX+X]) &&
                  (StepNr+2-FirstTime[MulY*OrigSizeY+Y][MulX*OrigSizeX+X] != 
                      FullGrid[FullGridNr].Order[Y][X]))
                FullGridFound = 0;
          if (FullGridFound)
          {
            printf("Repeat of FullGrid[%d] Step=%d found at Dist[%d,%d],Step=%d\n",
                FullGridNr, FullGrid[FullGridNr].Step, MulY-EXPANSION, MulX-EXPANSION, StepNr+1);
            // Hypothesize about predictability of this repetition
            if      (MulY < EXPANSION)  DirY=-1;
            else if (MulY > EXPANSION)  DirY=1;
            else DirY=0;
            if      (MulX < EXPANSION)  DirX=-1;
            else if (MulX > EXPANSION)  DirX=1;
            else DirX=0;
            int Distance = 0;
            if (DirY)  Distance += ((MulY-EXPANSION)/DirY);
            if (DirX)  Distance += ((MulX-EXPANSION)/DirX);
            // Have we already observed a Repeat in this quadrant ?
            if (RepeatGrid[1+DirY][1+DirX].StartStep)
            {
              // Yes, so verify the hypothesis
              if (FullGridNr != RepeatGrid[1+DirY][1+DirX].FullGridRef)
              {
                fprintf(stderr, "Expected FullGridRef %d for quadrant [%d,%d], but got %d\n",
                    RepeatGrid[1+DirY][1+DirX].FullGridRef, DirY,DirX, FullGridNr);
                exit(4);
              }
              if (RepeatGrid[1+DirY][1+DirX].StartStep + Distance*OrigSizeX != StepNr+1)
              {
                fprintf(stderr, "Expected StepNr %d = %d + %dx%d for quadrant [%d,%d], but got %d\n",
                    RepeatGrid[1+DirY][1+DirX].StartStep + Distance*OrigSizeX,
                    RepeatGrid[1+DirY][1+DirX].StartStep, Distance, OrigSizeX,
                    DirY,DirX, StepNr+1);
                exit(4);
              }
              // OK hypothesis confirmed
            } /* Known Repeat in this quadrant */
            else
            {
              // Add the Repeat for this quadrant
              RepeatGrid[1+DirY][1+DirX].FullGridRef = FullGridNr;
              RepeatGrid[1+DirY][1+DirX].StartStep = StepNr+1 - Distance*OrigSizeX;
              printf("Establishing Repeat[%d,%d] with FullGrid[%d] Start %d\n",
                  DirY,DirX, FullGridNr, RepeatGrid[1+DirY][1+DirX].StartStep);
            }
            break;
          } /* if (FullGridFound) */
        } /* for (FullGridNr) */
        if (FullGridNr >= NrOfFullGrids)
        {
          printf("Found new FullGrid[%d] Dist [%d,%d] Step=%d\n",
              FullGridNr, MulY-EXPANSION, MulX-EXPANSION, StepNr+1);
          // Add this newly discovered FullGrid to the array
          FullGrid = (tFullGrid*)realloc(FullGrid, (NrOfFullGrids+1)*sizeof(tFullGrid));
          for (Y=0; Y<OrigSizeY; Y++)
            for (X=0; X<OrigSizeX; X++)
              // Store the _complement_ of First Time Arrival
              if (FirstTime[MulY*OrigSizeY+Y][MulX*OrigSizeX+X])
                FullGrid[FullGridNr].Order[Y][X] = 
                  StepNr+2-FirstTime[MulY*OrigSizeY+Y][MulX*OrigSizeX+X];
              else
                FullGrid[FullGridNr].Order[Y][X] = 0;
          FullGrid[NrOfFullGrids].Step = StepNr+1;
          NrOfFullGrids++;

#if 0
          // Debugging
          for (Y=0; Y<OrigSizeY; Y++)
          {
            for (X=0; X<OrigSizeX; X++)
            {
              if (Grid[MulY*OrigSizeY+Y][MulX*OrigSizeX+X] == '#')
                putchar('#');
              else if (FullGrid[FullGridNr].Order[Y][X])
              {
                if (FullGrid[FullGridNr].Order[Y][X] < 10)
                  putchar(FullGrid[FullGridNr].Order[Y][X] + '0');
                else if (FullGrid[FullGridNr].Order[Y][X] < 36)
                  putchar(FullGrid[FullGridNr].Order[Y][X] - 10 + 'A');
                else
                  putchar(FullGrid[FullGridNr].Order[Y][X] - 36 + 'a');
              }
              else
                putchar('.');
            } /* for (X) */
            putchar('\n');
          } /* for (Y) */
#endif
        } /* if (FullGridNr >= NrOfFullGrids) */
      } /* for (MulX) */

    if (StepNr == 804)  break;  // Don't simulate further
	} /* for (StepNr) */
#define ODD_NROFSTEPS 1

#if 0
  // Display remaining 'walled gardens'
  int MulX=EXPANSION;
  int MulY=EXPANSION;
  for (Y=0; Y<OrigSizeY; Y++)
  {
    for (X=0; X<OrigSizeX; X++)
      if (Grid[MulY*OrigSizeY+Y][MulX*OrigSizeX+X] == '#')
        putchar('#');
      else if (FirstTime[MulY*OrigSizeY+Y][MulX*OrigSizeX+X])
        putchar(' ');
      else
        putchar('?');
    putchar('\n');
  }
#endif

  /* Finalisation */
  unsigned long long Sum;

#if 1
  // Complete census
  Sum = 0;
  for (int MulY=0; MulY<MULTIPLICATION; MulY++)
  {
    for (int MulX=0; MulX<MULTIPLICATION; MulX++)
    {
      int SubGridSum=0;
      for (Y=0; Y<OrigSizeY; Y++)
        for (X=0; X<OrigSizeX; X++)
          if (Grid[MulY*OrigSizeY+Y][MulX*OrigSizeX+X] == 'O')
            SubGridSum++;
      if (SubGridSum)
        //printf("Census subgrid[%d,%d] has %d reached cells..\n",MulY-EXPANSION, MulX-EXPANSION, SubGridSum);
        printf("%4d", SubGridSum);
      else
        printf("    ");
      if ((MulY==EXPANSION) && ((MulX==EXPANSION) || (MulX==EXPANSION-1)))
        putchar('|');
      else
        putchar(' ');
      Sum += SubGridSum;
    } /* for (MulX) */
    putchar('\n');
  } /* for (MulY) */
  printf("Complete census after Step %d shows %d reached cells\n", StepNr+1, Sum);
#endif

  // Helper function
  void Predict(int NrOfSteps)
  {
    printf("=====  Predicting %d Steps =====\n", NrOfSteps);
    int StepNr = NrOfSteps-1;
    // Prediction
    Sum = 0;
    for (DirY=-1; DirY<2; DirY++)
      for (DirX=-1; DirX<2; DirX++)
      {
        if ((!DirY) && (!DirX))
        {
          // Center subgrid
          Sum += (StepNr&1) ? Even : Odd;
          printf("Predict %d reached cells in CENTER subgrid ..\n", (StepNr&1) ? Even : Odd);
        }
        else if ((!DirY) || (!DirX))
        {
          // Axes subgrids
          int Distance = (int)((StepNr+1 - RepeatGrid[1+DirY][1+DirX].StartStep) / OrigSizeX);
          int RemainderSteps = StepNr+1 - RepeatGrid[1+DirY][1+DirX].StartStep  - Distance*OrigSizeX;
          printf("Predict %d full in Axis[%d,%d], %d with %d and %d with %d cells ..\n",
              Distance, DirY,DirX, ((Distance+1)/2), ((StepNr&1) ? Odd : Even),
              (Distance/2), ((StepNr&1) ? Even : Odd));
          Sum += ((Distance+1)/2) * (unsigned long long)((StepNr&1) ? Odd : Even);
          Sum += (Distance/2) * (unsigned long long)((StepNr&1) ? Even : Odd);
          // Two more subgrids which are not yet completely filled
          FullGridNr = RepeatGrid[1+DirY][1+DirX].FullGridRef;
          for (int TipGridNr=1; TipGridNr<=2; TipGridNr++)
          {
            int SubGridSum=0;
            for (Y=0; Y<OrigSizeY; Y++)
              for (X=0; X<OrigSizeX; X++)
              {
                int StepsNeeded = TipGridNr * OrigSizeX - FullGrid[FullGridNr].Order[Y][X];
                // Polarity determination is broken, only works for odd NrOfSteps
#if ODD_NROFSTEPS
                if ((RemainderSteps > StepsNeeded) &&
                    ((RemainderSteps - StepsNeeded)&1))  SubGridSum++;
#else
                if ((RemainderSteps > StepsNeeded) &&
                    ((RemainderSteps - StepsNeeded + StepNr+1)&1))  SubGridSum++;
#endif
              }
//            printf("Axample TipGrid %d top-left Order=%d StepsNeeded=%d\n",
//                TipGridNr, FullGrid[FullGridNr].Order[0][0],
//                TipGridNr * OrigSizeX - FullGrid[FullGridNr].Order[0][0]);
            printf("Predict %d cells in TipGrid %d for Axis[%d,%d] RemSteps %d ..\n",
                SubGridSum, TipGridNr, DirY,DirX, RemainderSteps);
            Sum += SubGridSum;
          }
        } /* axes subgrids */
        else
        {
          // Quadrant subgrids
          int Distance = (int)((StepNr+1 - RepeatGrid[1+DirY][1+DirX].StartStep) / OrigSizeX) - 1;
          int RemainderSteps = StepNr+1 - RepeatGrid[1+DirY][1+DirX].StartStep - (Distance+1)*OrigSizeX;
          int NrOfFull = (Distance * (Distance + 1))/2;
          printf("Predict %dx(%d+1)/2=%d full for Quadrant[%d,%d], %dx%d + %dx%d ..\n",
              Distance, Distance, NrOfFull, DirY,DirX,
              ((Distance+1)/2) * ((Distance+1)/2), (StepNr&1) ? Even : Odd,
              ((Distance/2) * (Distance/2 + 1)), (StepNr&1) ? Odd : Even);
          Sum += ((Distance+1)/2) * (unsigned long long)((Distance+1)/2) * ((StepNr&1) ? Even : Odd);
          Sum += ((Distance/2) * (unsigned long long)(Distance/2 + 1)) * ((StepNr&1) ? Odd : Even);
          // Two more subgrids which are not yet completely filled
          FullGridNr = RepeatGrid[1+DirY][1+DirX].FullGridRef;
          for (int TipGridNr=1; TipGridNr<=2; TipGridNr++)
          {
            int SubGridSum=0;
            for (Y=0; Y<OrigSizeY; Y++)
              for (X=0; X<OrigSizeX; X++)
              {
                int StepsNeeded = TipGridNr * OrigSizeX - FullGrid[FullGridNr].Order[Y][X];
#if ODD_NROFSTEPS
                if ((RemainderSteps > StepsNeeded) &&
                    ((RemainderSteps - StepsNeeded)&1))  SubGridSum++;
#else
                if ((RemainderSteps > StepsNeeded) &&
                    ((RemainderSteps - StepsNeeded + StepNr+1)&1))  SubGridSum++;
#endif
              }
//            printf("Quoting TipGrid %d top-left Order=%d StepsNeeded=%d\n",
//                TipGridNr, FullGrid[FullGridNr].Order[0][0],
//                TipGridNr * OrigSizeX - FullGrid[FullGridNr].Order[0][0]);
            printf("Predict %d x %d cells in TipGrid %d for Quadrant[%d,%d] RemSteps %d..\n",
                (Distance+TipGridNr), SubGridSum, TipGridNr, DirY,DirX, RemainderSteps);
            Sum += (Distance+TipGridNr) * SubGridSum;
          }
        } /* quadrant subgrids */
      } /* for (DirX) */
    printf("Prediction after Step %d : %llu reached cells\n", StepNr+1, Sum);
  }

  Predict(StepNr+1);
#if 1
  Predict(50);
  Predict(100);
  Predict(500);
  Predict(1000);
  Predict(5000);
  Predict(26501365);
#endif

	return 0;
}
