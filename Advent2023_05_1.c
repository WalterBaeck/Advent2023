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
  int TokenNr;
  unsigned int Seed[100], Scan;
  int NrOfSeeds=0, SeedNr;
  char TypeName[10][20];
  int NrOfTypes, TypeNr;
  typedef struct {
    int FromType, ToType;
    int NrOfSegments;
    unsigned int FromStart[100], ToStart[100], Amount[100];
  } tMap;
  tMap Map[10];
  int NrOfMaps=-1, MapNr;
  unsigned Minimum=-1, AfterEmptyLine;

  /* Gathering data */
  // First line
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
    if (1 != sscanf(InputPtr, "%u", &Seed[NrOfSeeds++]))
    {
      fprintf(stderr, "Could not scan Seed #%d from %s\n", NrOfSeeds, InputPtr);
      exit(4);
    }
  // Debugging
  printf("Scanned #%d seeds, last was %u\n", NrOfSeeds, Seed[NrOfSeeds-1]);
  strcpy(TypeName[0], "seed");
  NrOfTypes = 1;
  // All subsequent lines
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
            Map[NrOfMaps].ToStart[Map[NrOfMaps].NrOfSegments] = Scan;
            break;
          case 1:
            Map[NrOfMaps].FromStart[Map[NrOfMaps].NrOfSegments] = Scan;
            break;
          case 2:
            Map[NrOfMaps].Amount[Map[NrOfMaps].NrOfSegments] = Scan;
            Map[NrOfMaps].NrOfSegments++;
            break;
          default:
            {
              fprintf(stderr, "Not expecting more than 4 segments of InputLineNr %d\n", InputLineNr);
              exit(4);
            }
        } /* switch (TokenNr) */
      } /* for (TokenNr) */
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
  for (SeedNr=0; SeedNr<NrOfSeeds; SeedNr++)
  {
    unsigned Number = Seed[SeedNr];
    for (MapNr=0; MapNr<NrOfMaps; MapNr++)
    {
      int SegmentNr;
      for (SegmentNr=0; SegmentNr<Map[MapNr].NrOfSegments; SegmentNr++)
        if ((Number >= Map[MapNr].FromStart[SegmentNr]) &&
            (Number < Map[MapNr].FromStart[SegmentNr] + Map[MapNr].Amount[SegmentNr]))
        {
          Number -= Map[MapNr].FromStart[SegmentNr];
          Number += Map[MapNr].ToStart[SegmentNr];
          // Debugging
          printf("In map #%d From(%s) To(%s) at segment #%d found new Number %u\n",
              MapNr, TypeName[Map[MapNr].FromType], TypeName[Map[MapNr].ToType], SegmentNr, Number);
          printf("\tTo:%u From:%u Amount:%u\n",
              Map[MapNr].ToStart[SegmentNr], Map[MapNr].FromStart[SegmentNr],
              Map[MapNr].Amount[SegmentNr]);
          break;
        } /* if segment found */
    } /* for (MapNr) */
    // Debugging
    printf("Seed #%d ended up at location %u\n", SeedNr, Number);
    if (Number < Minimum)
      Minimum = Number;
  } /* for (SeedNr) */

	/* Finalisation */
  printf("Found Minimum %d\n", Minimum);
	return 0;
}
