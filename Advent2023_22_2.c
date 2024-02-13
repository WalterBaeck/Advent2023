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
  int Pos[1500][3][2];
  int Orig[1500][3][2];
  int BlockNr,NrOfBlocks=0;
  int OtherNr;
  int X,Y,Z, Dim,Index;
  int TokenNr;
  char Text[15];
  int TotalSupport[1500]={0};
  int Fallen[1500];

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
    if (!(InputPtr = strchr(InputLine,'~')))
    {
      fprintf(stderr, "Could not find tilde on InputLine #%d\n", InputLineNr);
      exit(2);
    }
    strncpy(Text, InputLine, InputPtr-InputLine);
    Text[InputPtr-InputLine] = '\0';
    for (TokenNr=0; InputPtr=strtok(TokenNr?NULL:Text,","); TokenNr++)
      if (1 != sscanf(InputPtr, "%d", &Pos[NrOfBlocks][TokenNr][0]))
      {
        fprintf(stderr, "Could not scan TokenNr[%d]=%s on InputLine #%d first half\n",
            TokenNr, InputPtr, InputLineNr);
        exit(3);
      }
    InputPtr = strchr(InputLine,'~');
    strncpy(Text, InputPtr+1, InputLen-(InputPtr-InputLine));
    Text[InputLen-(InputPtr-InputLine)] = '\0';
    for (TokenNr=0; InputPtr=strtok(TokenNr?NULL:Text,","); TokenNr++)
      if (1 != sscanf(InputPtr, "%d", &Pos[NrOfBlocks][TokenNr][1]))
      {
        fprintf(stderr, "Could not scan TokenNr[%d]=%s on InputLine #%d second half\n",
            TokenNr, InputPtr, InputLineNr);
        exit(3);
      }
    NrOfBlocks++;
  } /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);

  /* Operation */
  int Max[3]={0};
  for (BlockNr=0; BlockNr<NrOfBlocks; BlockNr++)
    for (Dim=0; Dim<3; Dim++)
      if (Pos[BlockNr][Dim][1] > Max[Dim])
        Max[Dim] = Pos[BlockNr][Dim][1];
  printf("Max coord:");
  for (Dim=0; Dim<3; Dim++)  printf(" %d,", Max[Dim]);
  putchar('\n');

  // Play Blockout
  int Blockout(int DeleteNr)
  {
    int AnyFalls = 0;
    for (Z=1; Z<=Max[2]; Z++)
      for (BlockNr=0; BlockNr<NrOfBlocks; BlockNr++)
      {
        if (BlockNr == DeleteNr)  continue;
        if (Pos[BlockNr][2][0] == Z)
        {
          int HighestObstacle = 0;
          for (OtherNr=0; OtherNr<NrOfBlocks; OtherNr++)
            if (Pos[OtherNr][2][1] < Z)
            {
              if (OtherNr == DeleteNr)  continue;
              // Does mutual [X,Y] plane overlap anywhere ?
              // This requires coinciding X AND Y coordinates
              int OverlappingXY = 1;
              for (Dim=0; Dim<2; Dim++)
                if ((Pos[OtherNr][Dim][0] > Pos[BlockNr][Dim][1]) ||
                    (Pos[OtherNr][Dim][1] < Pos[BlockNr][Dim][0]))
                  OverlappingXY = 0;
              if (!OverlappingXY)  continue;
              // Upper boundary of this Other block could support falling Block
              if (HighestObstacle < Pos[OtherNr][2][1])
                HighestObstacle = Pos[OtherNr][2][1];
            }
          // Block could now drop unto this highest obstacle
          int Distance = Pos[BlockNr][2][0]-1 - HighestObstacle;
          if (Distance)
          {
            for (Index=0; Index<2; Index++)
              Pos[BlockNr][2][Index] -= Distance;
            Fallen[BlockNr] = 1;
            AnyFalls = 1;
          }
        } /* if (Pos[BlockNr][2][0] == Z) */
      } /* for (BlockNr) */
    return AnyFalls;
  } /* int Blockout(int DeleteNr) */

  // Initially, don't delete any blocks, just make the setup settle
  Blockout(NrOfBlocks);
  Max[2] = 0;
  for (BlockNr=0; BlockNr<NrOfBlocks; BlockNr++)
    if (Pos[BlockNr][2][1] > Max[2])
      Max[2] = Pos[BlockNr][2][1];
  printf("After Blockout, MaxZ is now reduced to %d\n", Max[2]);

  // Store this situation
  for (BlockNr=0; BlockNr<NrOfBlocks; BlockNr++)
    for (Dim=0; Dim<3; Dim++)
      for (Index=0; Index<2; Index++)
        Orig[BlockNr][Dim][Index] = Pos[BlockNr][Dim][Index];

  int ToDelete;
  for (ToDelete=0; ToDelete<NrOfBlocks; ToDelete++)
  {
    // Revert to settled initial setup
    for (BlockNr=0; BlockNr<NrOfBlocks; BlockNr++)
      for (Dim=0; Dim<3; Dim++)
        for (Index=0; Index<2; Index++)
          Pos[BlockNr][Dim][Index] = Orig[BlockNr][Dim][Index];
    memset(Fallen, 0, NrOfBlocks * sizeof(int));
    // Now play Blockout as if the ToDelete actually disappears
    Blockout(ToDelete);
    if (Blockout(ToDelete))
      printf("2nd Blockout(%d) still makes further blocks fall ?\n", ToDelete);
    // Check the amount of Fallen for this ToDelete
    for (BlockNr=0; BlockNr<NrOfBlocks; BlockNr++)
      if (Fallen[BlockNr])
        TotalSupport[ToDelete]++;
  }

  /* Finalisation */
  int Sum=0;
  for (BlockNr=0; BlockNr<NrOfBlocks; BlockNr++)
  {
    if (TotalSupport[BlockNr])
      printf("deleting Block[%d] would make %d others fall\n", BlockNr, TotalSupport[BlockNr]);
    Sum += TotalSupport[BlockNr];
  }
  printf("Found Sum %d\n", Sum);
	return 0;
}
