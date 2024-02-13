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
  int TokenNr, Number;
  unsigned long long Time=0, Distance=0;

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
  for (TokenNr=0; InputPtr=strtok(TokenNr?NULL:InputLine+5, " \t"); TokenNr++)
  {
    InputLen = strlen(InputPtr);
    for (InputPos=0; InputPos<InputLen; InputPos++)  Time*=10;
    if (1 != sscanf(InputPtr, "%d", &Number))
    {
      fprintf(stderr, "Could not scan Time #%d from %s\n", TokenNr, InputPtr);
      exit(4);
    }
    Time += Number;
  } /* for (TokenNr) */
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
  {
    InputLen = strlen(InputPtr);
    for (InputPos=0; InputPos<InputLen; InputPos++)  Distance*=10;
    if (1 != sscanf(InputPtr, "%u", &Number))
    {
      fprintf(stderr, "Could not scan Distance #%d from %s\n", TokenNr, InputPtr);
      exit(4);
    }
    Distance += Number;
  } /* for (TokenNr) */

	/* Operation */
  unsigned long long Slowest,Fastest;

  // First determine the Slowest possible pace that will still beat the record
  Slowest = (Distance+1) / Time;
  // Compensate for the lost time to reach this Slowest speed
  while (Slowest * (Time-Slowest) < Distance+1)
    Slowest++;

  // Then determine the fastest possible speed, which is simply Time-1
  Fastest = Time-1;
  // Compensate for the time required to actually cover the distance
  while (Fastest * (Time-Fastest) < Distance+1)
    Fastest--;

  printf("Time=%llu and Dist=%llu can be beaten from speeds %llu to %llu\n",
      Time, Distance, Slowest, Fastest);

	/* Finalisation */
  printf("Found #possibilities %llu\n", Fastest-Slowest+1);
	return 0;
}
