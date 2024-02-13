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
  int X=0,Y=0; // From here, keep following the pipe until we're back at starting point
	int TokenNr, Len, Step;
	char Dir, Text[6];
	// Store the task
	int TaskLen[1000];
	char TaskDir[1000];
	int NrOfTasks=0, TaskNr;
	// Compose arrays of X,Y markers
	int MarkX[1000]={0}, MarkY[1000]={0};
	int NrOfMarkX=0, MarkXNr, NrOfMarkY=0, MarkYNr;
	int OtherX,OtherY;

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
		for (TokenNr=0; InputPtr=strtok(TokenNr?NULL:InputLine, " "); TokenNr++)
			switch (TokenNr)
			{
				case 2:
					if ((InputPtr[0] != '(') || (InputPtr[1] != '#') || (InputPtr[8] != ')'))
					{
						fprintf(stderr, "Unrecognized RGB string %s on InputLine #%d\n", InputPtr, InputLineNr);
						exit(3);
					}
					strncpy(Text, InputPtr+2, 5);
					Text[5] = '\0';
					if (1 != sscanf(Text, "%x", &Len))
					{
						fprintf(stderr, "Could not hex-scan Len in %s on InputLine #%d\n", InputPtr, InputLineNr);
						exit(3);
					}
					Dir = InputPtr[7];
					break;
			}
		// Store this task item
		TaskLen[NrOfTasks] = Len;
		TaskDir[NrOfTasks] = Dir;
		NrOfTasks++;
printf("InputLine #%d  Len 0x%X=%d  Dir %c\n", InputLineNr, Len, Len, Dir);
		// Follow the edge coordinates
		switch (Dir)
		{
			case '3': Y-=Len; break;
			case '2': X-=Len; break;
			case '1': Y+=Len; break;
			case '0': X+=Len; break;
			default:
				fprintf(stderr, "Unrecognized char %c at [%d,%d] ?\n",
						InputLine[X], Y, X);
				exit(5);
		} /* switch (Dir) */
		if ((Dir=='1') || (Dir=='3'))  // Down or Up
		{
			// Need to insert a Y marker into the array
			for (MarkYNr=0; MarkYNr<NrOfMarkY; MarkYNr++)
				if (MarkY[MarkYNr] >= Y)  break;
			if ((MarkYNr<NrOfMarkY) && (MarkY[MarkYNr] == Y))  continue;
			printf("Inserting %d in front of MarkY[%d]=%d into existing NrOfMarkY=%d\n",Y,MarkYNr,MarkY[MarkYNr],NrOfMarkY);
			// Make room by shifting all further markers out
			for (OtherY=NrOfMarkY; OtherY>MarkYNr; OtherY--)
				MarkY[OtherY] = MarkY[OtherY-1];
			// Now insert the new Y marker
			MarkY[MarkYNr] = Y;
			// Extend the array size
			NrOfMarkY++;
		}
		else  // Right or Left
		{
			// Need to insert an X marker into the array
			for (MarkXNr=0; MarkXNr<NrOfMarkX; MarkXNr++)
				if (MarkX[MarkXNr] >= X)  break;
			if ((MarkXNr<NrOfMarkX) && (MarkX[MarkXNr] == X))  continue;
			printf("Inserting %d in front of MarkX[%d]=%d into existing NrOfMarkX=%d\n",X,MarkXNr,MarkX[MarkXNr],NrOfMarkX);
			// Make room by shifting all further markers out
			for (OtherX=NrOfMarkX; OtherX>MarkXNr; OtherX--)
				MarkX[OtherX] = MarkX[OtherX-1];
			// Now insert the new X marker
			MarkX[MarkXNr] = X;
			// Extend the array size
			NrOfMarkX++;
		}
  } /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);
	// Verification
	if ((X) || (Y))
	{
		fprintf(stderr, "Not back at starting point but at [%d,%d] instead ?\n", Y,X);
		exit(4);
	}

	// The X and Y markers will now lead to a Grid of rectangles.
	// By convention, each rectangle can only have Left and/or Upper edge active.
	printf("Found %d X markers (from %d to %d) and %d Y markers (from %d to %d)\n",
			NrOfMarkX, *MarkX, MarkX[NrOfMarkX-1], NrOfMarkY, *MarkY, MarkY[NrOfMarkY-1]);
	printf("Markers X:");
	for (MarkXNr=0; MarkXNr<NrOfMarkX; MarkXNr++)  printf(" [%d]=%d", MarkXNr, MarkX[MarkXNr]);
	putchar('\n');
	printf("Markers Y:");
	for (MarkYNr=0; MarkYNr<NrOfMarkY; MarkYNr++)  printf(" [%d]=%d", MarkYNr, MarkY[MarkYNr]);
	putchar('\n');

	int **Up = (int**)malloc(NrOfMarkY * sizeof(int*));
	int **Left = (int**)malloc(NrOfMarkY * sizeof(int*));
	for (MarkYNr=0; MarkYNr<NrOfMarkY; MarkYNr++)
	{
		Up[MarkYNr] = (int*)malloc(NrOfMarkX * sizeof(int));
		memset(Up[MarkYNr], 0, NrOfMarkX * sizeof(int));
		Left[MarkYNr] = (int*)malloc(NrOfMarkX * sizeof(int));
		memset(Left[MarkYNr], 0, NrOfMarkX * sizeof(int));
	}

	// Trace the Task again, to mark Grid edges
  X=0; Y=0; // From here, keep following the pipe until we're back at starting point
	int Seq=0;
	// Find corresponding spot in arrays of X and Y markers
	for (MarkXNr=0; MarkXNr<NrOfMarkX; MarkXNr++)
		if (MarkX[MarkXNr] >= X)  break;
	if (MarkX[MarkXNr] != X)
	{
		fprintf(stderr, "Odd, tried to find X marker %d but only found[%d]=%d\n", X, MarkXNr,MarkX[MarkXNr]);
		exit(5);
	}
	for (MarkYNr=0; MarkYNr<NrOfMarkY; MarkYNr++)
		if (MarkY[MarkYNr] >= Y)  break;
	if (MarkY[MarkYNr] != Y)
	{
		fprintf(stderr, "Odd, tried to find Y marker %d but only found[%d]=%d\n", Y, MarkYNr,MarkY[MarkYNr]);
		exit(5);
	}
	for (TaskNr=0; TaskNr<NrOfTasks; TaskNr++)
	{
		Dir = TaskDir[TaskNr];
		Len = TaskLen[TaskNr];
		++Seq;
		printf("Task[%d] Dir=%c Len=%d Seq=%d Mark[%d,%d] Pos[%d,%d]\n", TaskNr, Dir, Len, Seq, MarkYNr,MarkXNr, Y,X);
		switch (Dir)
		{
			case '3':  // Up
				Y-=Len;
				for (OtherY=MarkYNr-1; OtherY>=0; OtherY--)
					if (MarkY[OtherY] <= Y)  break;
				if (MarkY[OtherY] != Y)
				{
					fprintf(stderr, "Odd, tried to find Y marker %d but only found[%d]=%d\n", Y, OtherY,MarkY[OtherY]);
					exit(6);
				}
				for (Step=OtherY; Step<MarkYNr; Step++)
					Left[Step][MarkXNr] = Seq;
				MarkYNr = OtherY;
				break;
			case '2':  // Left
				X-=Len;
				for (OtherX=MarkXNr-1; OtherX>=0; OtherX--)
					if (MarkX[OtherX] <= X)  break;
				if (MarkX[OtherX] != X)
				{
					fprintf(stderr, "Odd, tried to find X marker %d but only found[%d]=%d\n", X, OtherX,MarkX[OtherX]);
					exit(6);
				}
				for (Step=OtherX; Step<MarkXNr; Step++)
					Up[MarkYNr][Step] = Seq;
				MarkXNr = OtherX;
				break;
			case '1':  // Down
				Y+=Len;
				for (OtherY=MarkYNr+1; OtherY<NrOfMarkY; OtherY++)
					if (Y <= MarkY[OtherY])  break;
				if (MarkY[OtherY] != Y)
				{
					fprintf(stderr, "Odd, tried to find Y marker %d but only found[%d]=%d\n", Y, OtherY,MarkY[OtherY]);
					exit(6);
				}
				for (Step=MarkYNr; Step<OtherY; Step++)
					Left[Step][MarkXNr] = Seq;
				MarkYNr = OtherY;
				break;
			case '0':  // Right
				X+=Len;
				for (OtherX=MarkXNr+1; OtherX<NrOfMarkX; OtherX++)
					if (X <= MarkX[OtherX])  break;
				if (MarkX[OtherX] != X)
				{
					fprintf(stderr, "Odd, tried to find X marker %d but only found[%d]=%d\n", X, OtherX,MarkX[OtherX]);
					exit(6);
				}
				for (Step=MarkXNr; Step<OtherX; Step++)
					Up[MarkYNr][Step] = Seq;
				MarkXNr = OtherX;
				break;
		} /* switch (Dir) */
	} /* for (TaskNr) */

  /* Field exam */
  int FeelX, FeelY, FirstSeq, PrevSeq, Score;
  int Histogram[9]={0};
	unsigned long long Area;
	unsigned long long NegSum=0,PosSum=0;
  for (Y=0; Y<NrOfMarkY; Y++)
    for (X=0; X<NrOfMarkX; X++)
    {
      Score = 0;  Area = 0;
			int AddUpEdge=0, AddLeftEdge=0, RemoveCorner=0, AddCorner=0;
      // Send feelers in 4 directions, verify order of encountered Pipe segments
      // North
      FeelX=X; FeelY=Y;
      for (; FeelY>=0; FeelY--)
        if (Up[FeelY][FeelX])
        {
          PrevSeq = Up[FeelY][FeelX];
          FirstSeq = PrevSeq;
          break;
        }
      if (FeelY < 0)  continue;
      // East
      FeelX=X; FeelY=Y;
      for (FeelX++; FeelX<NrOfMarkX; FeelX++)
        if (Left[FeelY][FeelX])
        {
          if (Left[FeelY][FeelX] > PrevSeq)
            Score++;
          else
            Score--;
          PrevSeq = Left[FeelY][FeelX];
          break;
        }
      if (FeelX >= NrOfMarkX)  continue;
			if (FeelX == X+1)
			{
				AddLeftEdge=1;
				if (Up[FeelY][FeelX])  RemoveCorner=1;
			}
      // South
      FeelX=X; FeelY=Y;
      for (FeelY++; FeelY<NrOfMarkY; FeelY++)
        if (Up[FeelY][FeelX])
        {
          if (Up[FeelY][FeelX] > PrevSeq)
            Score++;
          else
            Score--;
          PrevSeq = Up[FeelY][FeelX];
          break;
        }
      if (FeelY >= NrOfMarkY)  continue;
			if (FeelY == Y+1)
			{
				AddUpEdge=1;
				if (Left[Y][X+1])  AddCorner=1;
			}
      // West
      FeelX=X; FeelY=Y;
      for (; FeelX>=0; FeelX--)
        if (Left[FeelY][FeelX])
        {
          if (Left[FeelY][FeelX] > PrevSeq)
            Score++;
          else
            Score--;
          PrevSeq = Left[FeelY][FeelX];
          break;
        }
      if (FeelX < 0)  continue;
      // Close the search by comparing last encountered SeqNr with the first
      if (FirstSeq > PrevSeq)
        Score++;
      else
        Score--;

      // Found a non-Pipe cell surrounded by Pipe
			Area += (((unsigned long long)(MarkX[X+1] - MarkX[X]))
					* ((unsigned long long)(MarkY[Y+1] - MarkY[Y])));
			if (AddUpEdge)   Area += (unsigned long long)(MarkX[X+1] - MarkX[X]);
			if (AddLeftEdge) Area += (unsigned long long)(MarkY[Y+1] - MarkY[Y]);
			Area -= RemoveCorner;
			Area += AddCorner;
			if (Score == -2)
				NegSum += Area;
			else if (Score == 2)
				PosSum += Area;
      //printf("Candidate[%3d,%3d] has Score %d\n", Y, X, Score);
      Histogram[4+Score]++;
    } /* for (X) */

	/* Finalisation */
  for (Score=-4; Score<=4; Score++)
    if (!Histogram[4+Score])
      printf("        ");
    else
      printf("H[%d]:%d ", Score, Histogram[4+Score]);
  putchar('\n');
	printf("NegSum %llu   PosSum %llu\n", NegSum, PosSum);
	return 0;
}
