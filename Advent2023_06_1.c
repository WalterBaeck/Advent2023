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
  int TokenNr, NrOfTokens;
  int Time[100], Distance[100];

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
  if (strncmp(InputLine, "Time:", 5))
  {
    fprintf(stderr, "Expected \'Time:\' on first line\n");
    exit(3);
  }
  for (TokenNr=0; InputPtr=strtok(TokenNr?NULL:InputLine+5, " "); TokenNr++)
    if (1 != sscanf(InputPtr, "%u", &Time[TokenNr]))
    {
      fprintf(stderr, "Could not scan Time #%d from %s\n", TokenNr, InputPtr);
      exit(4);
    }
  NrOfTokens = TokenNr;
  // Debugging
  printf("Scanned #%d times, last was %u\n", NrOfTokens, Time[NrOfTokens-1]);
  // Second line
  InputLineNr++;
  if (!fgets(InputLine, 1000, InputFile))
  {
    fprintf(stderr, "No second line found ?\n");
    exit(3);
  }
  while ((InputLine[strlen(InputLine)-1]=='\n')
      || (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
  if (strncmp(InputLine, "Distance:", 9))
  {
    fprintf(stderr, "Expected \'Distance:\' on first line\n");
    exit(3);
  }
  for (TokenNr=0; InputPtr=strtok(TokenNr?NULL:InputLine+9, " "); TokenNr++)
    if (1 != sscanf(InputPtr, "%u", &Distance[TokenNr]))
    {
      fprintf(stderr, "Could not scan Distance #%d from %s\n", TokenNr, InputPtr);
      exit(4);
    }
  // Debugging
  printf("Scanned #%d distances, last was %u\n", TokenNr, Distance[TokenNr-1]);
  if (TokenNr != NrOfTokens)
  {
    fprintf(stderr, "Mismatch between NrOfTimes %d and NrOfDistances %d\n", NrOfTokens, TokenNr);
    exit(5);
  }

	/* Operation */
  unsigned Product = 1;
  int Slowest,Fastest;
  for (TokenNr=0; TokenNr<NrOfTokens; TokenNr++)
  {
    // First determine the Slowest possible pace that will still beat the record
    Slowest = (Distance[TokenNr]+1) / Time[TokenNr];
    // Compensate for the lost time to reach this Slowest speed
    while (Slowest * (Time[TokenNr]-Slowest) < Distance[TokenNr]+1)
      Slowest++;

    // Then determine the fastest possible speed, which is simply Time-1
    Fastest = Time[TokenNr]-1;
    // Compensate for the time required to actually cover the distance
    while (Fastest * (Time[TokenNr]-Fastest) < Distance[TokenNr]+1)
      Fastest--;

    printf("Race #%d with Time=%d and Dist=%d can be beaten from speeds %d to %d\n",
        TokenNr+1, Time[TokenNr], Distance[TokenNr], Slowest, Fastest);

    Product *= (Fastest - Slowest + 1);
  } /* for (TokenNr) */

	/* Finalisation */
  printf("Found Product %u\n", Product);
	return 0;
}
