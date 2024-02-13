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
    unsigned Begin,End;   // Both included
  } tLine;
  tLine *Line = NULL;
  int LineNr, NrOfLines=0;
  // Helper function
  void AddLine(unsigned Start, unsigned Stop)
  {
    for (LineNr=0; LineNr<NrOfLines; LineNr++)
    {
      // If the new piece starts behind existing Line, or stops before it, no merger is possible
      if ((Start > Line[LineNr].End + 1) || (Stop < Line[LineNr].Begin - 1))  continue;
      // Arriving here, means that a merger is possible.
      // Just look for the minimum Begin and the maximum End.
printf("** Merging new piece <%u-%u> with existing Line <%u-%u> : make",
Start, Stop, Line[LineNr].Begin, Line[LineNr].End);
      if (Start < Line[LineNr].Begin)
        Line[LineNr].Begin = Start;
      if (Stop > Line[LineNr].End)
        Line[LineNr].End = Stop;
printf(" <%u-%u> **\n", Line[LineNr].Begin, Line[LineNr].End);
      // Stop searching further existing Lines
      break;
    }
    // Was it impossible to merge ? Then extend the array with this new piece.
    if (LineNr >= NrOfLines)
    {
      Line = (tLine*)realloc(Line, (++NrOfLines)*sizeof(tLine));
      Line[NrOfLines-1].Begin = Start;
      Line[NrOfLines-1].End = Stop;
    }
  } /* void AddLine() */

  /* Gathering data */

  // First line
  int TokenNr;
  unsigned int Scan, Start, Stop;
  int DoingStart=1;
  InputLineNr++;
  if (!fgets(InputLine, 1000, InputFile))
  {
    fprintf(stderr, "No first line found ?\n");
    exit(3);
  }
  while ((InputLine[strlen(InputLine)-1]=='\n')
      || (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
  if (strncmp(InputLine, "seeds: ", 6))
  {
    fprintf(stderr, "Expected \'seeds: \' on first line\n");
    exit(3);
  }
  for (TokenNr=0; InputPtr=strtok(TokenNr?NULL:InputLine+6, " "); TokenNr++)
  {
    if (1 != sscanf(InputPtr, "%u", &Scan))
    {
      fprintf(stderr, "Could not scan unsigned in segment #%d of InputLineNr %d : %s\n",
          TokenNr, InputLineNr, InputPtr);
      exit(5);
    }
printf("Token #%d: scanned number %u (DoingStart=%d)\n", TokenNr, Scan, DoingStart);
    if (DoingStart)
      Start = Scan;
    else
    {
      Stop = Start + Scan - 1;
      AddLine(Start, Stop);
    }
    DoingStart = 1 - DoingStart;
  }
  // Debugging
  printf("Scanned #%d Lines, last was %u-%u\n", NrOfLines, Line[NrOfLines-1].Begin, Line[NrOfLines-1].End);

  // All subsequent lines
  int AfterEmptyLine=0;
  char TypeName[10][20];
  int NrOfTypes, TypeNr;
	int SegmentNr;
  typedef struct {
    int FromType, ToType;
    int NrOfSegments;
    unsigned int FromStart[100], ToStart[100], Amount[100];
  } tMap;
  tMap Map[10];
  int NrOfMaps=-1, MapNr;
  strcpy(TypeName[0], "seed");
  NrOfTypes = 1;
	while (fgets(InputLine, 1000, InputFile))
	{
		// Bookkeeping
		InputLineNr++;
		// Strip line ending
		while ((InputLine[strlen(InputLine)-1]=='\n')
				|| (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
    if (!strlen(InputLine))
    {
      AfterEmptyLine = 1;
      // Prepare to parse a new map
      Map[++NrOfMaps].NrOfSegments = 0;
      continue;
    }
    if (AfterEmptyLine)
      // Map header
      for (TokenNr=0; InputPtr=strtok(TokenNr?NULL:InputLine, "- "); TokenNr++)
        switch (TokenNr)
        {
          case 0:
            if (strcmp(InputPtr, TypeName[NrOfTypes-1]))
            {
              fprintf(stderr, "FromType %s on InputLine #d doesn\'t match last ToType %s\n",
                  InputPtr, InputLineNr, TypeName[NrOfTypes-1]);
              exit(4);
            }
            Map[NrOfMaps].FromType = NrOfTypes-1;
            break;
          case 1:
            if (strcmp(InputPtr, "to"))
            {
              fprintf(stderr, "Expected \'to\' on InputLineNr %d middle segment\n", InputLineNr);
              exit(4);
            }
            break;
          case 2:
            strcpy(TypeName[NrOfTypes++], InputPtr);
            Map[NrOfMaps].ToType = NrOfTypes-1;
            break;
          case 3:
            if (strcmp(InputPtr, "map:"))
            {
              fprintf(stderr, "Expected \'map:\' on InputLineNr %d final segment\n", InputLineNr);
              exit(4);
            }
            break;
          default:
					{
						fprintf(stderr, "Not expecting more than 4 segments of InputLineNr %d\n", InputLineNr);
						exit(4);
					}
        } /* switch (TokenNr) */
    else
		{
			unsigned int ToStart, FromStart, Amount;
			int OtherSegmentNr;
      // Map content
      for (TokenNr=0; InputPtr=strtok(TokenNr?NULL:InputLine, "- "); TokenNr++)
      {
        if (1 != sscanf(InputPtr, "%u", &Scan))
        {
          fprintf(stderr, "Could not scan unsigned in segment #%d of InputLineNr %d : %s\n",
              TokenNr, InputLineNr, InputPtr);
          exit(5);
        }
        switch (TokenNr)
        {
          case 0:
						ToStart = Scan;       break;
          case 1:
						FromStart = Scan;     break;
          case 2:
						Amount = Scan;
						// Keep map segments sorted - insert at proper position :
						// Look for the right insertion position
						for (SegmentNr=0; SegmentNr<Map[NrOfMaps].NrOfSegments; SegmentNr++)
							if (Map[NrOfMaps].FromStart[SegmentNr] > FromStart)
								break;
printf("Map #%d: inserting new segment at position %d\n", NrOfMaps, SegmentNr);
						// Shift out all further segments by one position
						for (OtherSegmentNr=Map[NrOfMaps].NrOfSegments; OtherSegmentNr>SegmentNr; OtherSegmentNr--)
						{
							Map[NrOfMaps].ToStart[OtherSegmentNr]   = Map[NrOfMaps].ToStart[OtherSegmentNr-1];
							Map[NrOfMaps].FromStart[OtherSegmentNr] = Map[NrOfMaps].FromStart[OtherSegmentNr-1];
							Map[NrOfMaps].Amount[OtherSegmentNr]    = Map[NrOfMaps].Amount[OtherSegmentNr-1];
						}
						// Insert the new segment at the vacated position now
						Map[NrOfMaps].ToStart[SegmentNr]   = ToStart;
						Map[NrOfMaps].FromStart[SegmentNr] = FromStart;
						Map[NrOfMaps].Amount[SegmentNr]    = Amount;
						// Register the expansion of the array
            Map[NrOfMaps].NrOfSegments++;
            break;
          default:
					{
						fprintf(stderr, "Not expecting more than 4 segments of InputLineNr %d\n", InputLineNr);
						exit(4);
					}
        } /* switch (TokenNr) */
      } /* for (TokenNr) */
		} /* else AfterEmptyLine */
		AfterEmptyLine = 0;
	} /* while (fgets) */

	// At the very end, NrOfMaps is still one-off
	NrOfMaps++;
	printf("%d InputLines were read.\n", InputLineNr);
	// Debugging
	printf("Found %d types, last was %s\n", NrOfTypes, TypeName[NrOfTypes-1]);
	printf("Found %d maps, last had %d segments, last was To:%u From:%u Amount:%u\n",
			NrOfMaps, Map[NrOfMaps-1].NrOfSegments,
			Map[NrOfMaps-1].ToStart[Map[NrOfMaps-1].NrOfSegments-1],
			Map[NrOfMaps-1].FromStart[Map[NrOfMaps-1].NrOfSegments-1],
			Map[NrOfMaps-1].Amount[Map[NrOfMaps-1].NrOfSegments-1]);
	// Sanity checks
	if (NrOfTypes != NrOfMaps+1)
	{
		fprintf(stderr, "Expected one more Type than there are Maps\n");
		exit(6);
	}
	if (strcmp(TypeName[NrOfTypes-1], "location"))
	{
		fprintf(stderr, "That\'s not a good location, try another: %s\n", TypeName[NrOfTypes-1]);
		exit(6);
	}

	/* Operation */
	// For the existing collection of Lines,
	//   project it entirely through one Map after another,
	//   obtaining a new collection of Lines after every Map.
	tLine *PrevLine = NULL;
  int PrevLineNr, NrOfPrevLines;
  for (MapNr=0; MapNr<NrOfMaps; MapNr++)
  {
    // Preparation : copy the existing collection of Lines into a backup, then delete Lines
		NrOfPrevLines = NrOfLines;
		if (!(PrevLine = (tLine*)realloc(PrevLine, NrOfPrevLines * sizeof(tLine))))
		{
			fprintf(stderr, "Could not realloc %d PrevLines for Map #%d\n", NrOfPrevLines, MapNr);
			exit(7);
		}
		memcpy(PrevLine, Line, NrOfPrevLines * sizeof(tLine));
		free(Line);
		Line = NULL;
		NrOfLines = 0;
		// Doublecheck
		unsigned LinesSum=0;
    for (PrevLineNr=0; PrevLineNr<NrOfPrevLines; PrevLineNr++)
			LinesSum += (PrevLine[PrevLineNr].End - PrevLine[PrevLineNr].Begin + 1);
		printf("--- Entering Map #%d with %d Lines of total length %u ---\n", MapNr, NrOfPrevLines, LinesSum);
		// Now consider each PrevLine in turn, and intersect it with all segments of this map
    for (PrevLineNr=0; PrevLineNr<NrOfPrevLines; PrevLineNr++)
		{
			// Skip all segments that are irrelevant to this line
			for (SegmentNr=0; SegmentNr<Map[MapNr].NrOfSegments; SegmentNr++)
				if (Map[MapNr].FromStart[SegmentNr] + Map[MapNr].Amount[SegmentNr] > PrevLine[PrevLineNr].Begin)
					break;
			// Perhaps the next segment is beyond the entire line, then this line passes unchanged
			// Or the last segment could simply be before the entire line, same decision
			if ((SegmentNr >= Map[MapNr].NrOfSegments) ||
					(Map[MapNr].FromStart[SegmentNr] > PrevLine[PrevLineNr].End))
			{
printf("Map #%d: Line #%d <%u-%u> has no interaction with any segment\n",
MapNr, PrevLineNr, PrevLine[PrevLineNr].Begin, PrevLine[PrevLineNr].End);
AddLine(PrevLine[PrevLineNr].Begin, PrevLine[PrevLineNr].End);
				continue;
			}
			// If there *is* interaction, then determine if the first piece of Result starts unchanged
			unsigned CurrentStart = PrevLine[PrevLineNr].Begin, CurrentStop;
			if (CurrentStart < Map[MapNr].FromStart[SegmentNr])
			{
printf("Map #%d: Line #%d Starting unchanged piece <%u-%u>\n",
MapNr, PrevLineNr, CurrentStart, Map[MapNr].FromStart[SegmentNr] - 1);
				AddLine(CurrentStart, Map[MapNr].FromStart[SegmentNr] - 1);
        CurrentStart = Map[MapNr].FromStart[SegmentNr];
			}
			// This guarantees that we always start the loop with a projected piece
			int PrevLineComplete = 0;
			for (;;)
			{
				CurrentStop = Map[MapNr].FromStart[SegmentNr] + Map[MapNr].Amount[SegmentNr] - 1;
				// Add the projected piece
				if (PrevLine[PrevLineNr].End <= CurrentStop)
				{
					CurrentStop = PrevLine[PrevLineNr].End;
					PrevLineComplete = 1;
				}
printf("Map #%d: Line #%d Projected piece <%u-%u> originally was <%u-%u>\n",
MapNr, PrevLineNr,
CurrentStart - Map[MapNr].FromStart[SegmentNr] + Map[MapNr].ToStart[SegmentNr],
CurrentStop  - Map[MapNr].FromStart[SegmentNr] + Map[MapNr].ToStart[SegmentNr],
CurrentStart, CurrentStop);
				AddLine(CurrentStart - Map[MapNr].FromStart[SegmentNr] + Map[MapNr].ToStart[SegmentNr],
						     CurrentStop - Map[MapNr].FromStart[SegmentNr] + Map[MapNr].ToStart[SegmentNr]);
				if (PrevLineComplete)  break;
				// Next, there is an unchanged piece
				CurrentStart = CurrentStop + 1;   // Overflow danger
				// Look for the next segment as termination
				if (++SegmentNr >= Map[MapNr].NrOfSegments)
				{
					CurrentStop = PrevLine[PrevLineNr].End;
					PrevLineComplete = 1;
				}
				else
				{
					CurrentStop = Map[MapNr].FromStart[SegmentNr] - 1;
					if (PrevLine[PrevLineNr].End <= CurrentStop)
					{
						CurrentStop = PrevLine[PrevLineNr].End;
						PrevLineComplete = 1;
					}
				}
				// Is there actually any unchanged part in between these segments ?
				if (CurrentStop >= CurrentStart)
				{
					// Add the unchanged piece
printf("Map #%d: Line #%d Unchanged piece <%u-%u>\n",
MapNr, PrevLineNr, CurrentStart, CurrentStop);
          AddLine(CurrentStart, CurrentStop);
				}
				if (PrevLineComplete)  break;
				// Prepare for another projected piece
				CurrentStart = CurrentStop + 1;   // Overflow danger
			} /* for (;;) */
		} /* for (PrevLineNr) */
	} /* for (MapNr) */

	/* Finalisation */
	unsigned Minimum=-1;
	unsigned LinesSum=0;
	for (LineNr=0; LineNr<NrOfLines; LineNr++)
	{
		LinesSum += (Line[LineNr].End - Line[LineNr].Begin + 1);
		if (Line[LineNr].Begin < Minimum)
			Minimum = Line[LineNr].Begin;
	}
	printf("=== Concluding with %d Lines of total length %u ===\n", NrOfLines, LinesSum);
  printf("Found Minimum %u\n", Minimum);
	return 0;
}
