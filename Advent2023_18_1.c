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
	int **Seq = (int**)malloc(2000*sizeof(int*));
  int X,Y;
  int StartX=1000,StartY=1000;
  for (Y=0; Y<2000; Y++)
  {
    Seq[Y] = (int*)malloc(2000*sizeof(int));
    memset(Seq[Y], 0, 2000 * sizeof(int));
  }
	int TokenNr, Step, Len;
	char Dir;
  int LoopLen=1;
	X = StartX; Y = StartY;
  // From here, keep following the pipe until we're back at starting point

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
				case 0:  Dir=*InputPtr;  break;
				case 1:
					if (1 != sscanf(InputPtr, "%d", &Len))
					{
						fprintf(stderr, "Could not scan Length from %s in InputLine #%d\n", InputPtr, InputLineNr);
						exit(3);
					}
					break;
				case 2: break;  // Disregard RGB code
			}
		for (Step=0; Step<Len; Step++)
		{
			switch (Dir)
			{
				case 'U': Y--; break;
				case 'L': X--; break;
				case 'D': Y++; break;
				case 'R': X++; break;
        default:
          fprintf(stderr, "Unrecognized char %c at [%d,%d] ?\n",
              InputLine[X], Y, X);
          exit(5);
      } /* switch (Dir) */
			if ((X<0) || (X>=2000) || (Y<0) || (Y>=2000))
			{
				fprintf(stderr, "Out of bounds [%d,%d] at InputLine #%d\n", Y,X, InputLineNr);
				exit(4);
			}
			Seq[Y][X] = LoopLen++;
		} /* for (Step) */
  } /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);
	// Verification
	if ((X!=StartX) || (Y!=StartY))
	{
		fprintf(stderr, "Not back at starting point [%d,%d] but at [%d,%d] instead ?\n",
				StartY,StartX, Y,X);
		exit(4);
	}
	LoopLen--;
	printf("Found LoopLen %d\n", LoopLen);

  /* Field exam */
  int FeelX, FeelY, FirstSeq, PrevSeq, Score;
  int Histogram[9]={0};
  for (Y=0; Y<2000; Y++)
    for (X=0; X<2000; X++)
    {
      Score = 0;
      // Send feelers in 4 directions, verify order of encountered Pipe segments
      if (Seq[Y][X])  continue;
      // North
      FeelX=X; FeelY=Y;
      for (FeelY--; FeelY>=0; FeelY--)
        if (Seq[FeelY][FeelX])
        {
          PrevSeq = Seq[FeelY][FeelX];
          FirstSeq = PrevSeq;
          break;
        }
      if (FeelY < 0)  continue;
      // East
      FeelX=X; FeelY=Y;
      for (FeelX++; FeelX<2000; FeelX++)
        if (Seq[FeelY][FeelX])
        {
          if (Seq[FeelY][FeelX] > PrevSeq)
            Score++;
          else
            Score--;
          PrevSeq = Seq[FeelY][FeelX];
          break;
        }
      if (FeelX >= 2000)  continue;
      // South
      FeelX=X; FeelY=Y;
      for (FeelY++; FeelY<2000; FeelY++)
        if (Seq[FeelY][FeelX])
        {
          if (Seq[FeelY][FeelX] > PrevSeq)
            Score++;
          else
            Score--;
          PrevSeq = Seq[FeelY][FeelX];
          break;
        }
      if (FeelY >= 2000)  continue;
      // West
      FeelX=X; FeelY=Y;
      for (FeelX--; FeelX>=0; FeelX--)
        if (Seq[FeelY][FeelX])
        {
          if (Seq[FeelY][FeelX] > PrevSeq)
            Score++;
          else
            Score--;
          PrevSeq = Seq[FeelY][FeelX];
          break;
        }
      if (FeelX < 0)  continue;
      // Close the search by comparing last encountered SeqNr with the first
      if (FirstSeq > PrevSeq)
        Score++;
      else
        Score--;

      // Found a non-Pipe cell surrounded by Pipe
      //printf("Candidate[%3d,%3d] has Score %d\n", Y, X, Score);
      Histogram[4+Score]++;
    } /* for (X) */

	/* Finalisation */
  for (Score=-4; Score<=4; Score++)
    if (!Histogram[4+Score])
      printf("        ");
    else
      printf("H[%d]:%d<Sum=%d> ", Score, Histogram[4+Score], LoopLen+Histogram[4+Score]);
  putchar('\n');
	return 0;
}
