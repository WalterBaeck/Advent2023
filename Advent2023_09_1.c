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
  int TokenNr, Index, Number, NewDiff;
  int Diff[100];  // Use Diff[0] to store the actual input numbers
  int Sum = 0;

  /* Operation */
	while (fgets(InputLine, 1000, InputFile))
	{
    printf("=== InputLine #%d ===\n", InputLineNr);
		// Bookkeeping
		InputLineNr++;
		// Strip line ending
		while ((InputLine[strlen(InputLine)-1]=='\n')
				|| (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
    if (!strlen(InputLine))  break;

    // Compute a growing triangle of Diffs with each new number on the inputline
    // Just keep track of the most recent 'edge' of the triangle, don't store it entirely
    for (TokenNr=0; InputPtr=strtok(TokenNr?NULL:InputLine, " "); TokenNr++)
    {
      if (1 != sscanf(InputPtr, "%d", &Number))
      {
        fprintf(stderr, "Could not scan number #%d on InputLine #%d\n", TokenNr, InputLineNr);
        exit(3);
      }
      for (Index=0; Index<TokenNr; Index++)
        if (!Index)
        {
          NewDiff = Number - Diff[0];
          printf("Inputnumber #%d : subtracting %d - %d = NewDiff %d\n",
              TokenNr, Number, Diff[0], NewDiff);
          Diff[0] = Number;
          Number = NewDiff;
        }
        else
        {
          printf("Diff row #%d: subtracting %d - %d = NewDiff %d\n",
              Index, Number, Diff[Index], Number - Diff[Index]);
          NewDiff = Number - Diff[Index];
          Diff[Index] = Number;
          Number = NewDiff;
        }
      // At the tip of the triangle, store a Diff in a new row
      Diff[Index] = Number;
      printf("+Tip+ Storing Number %d at row %d\n", Number, Index);
    } /* for (TokenNr) */

    // Doublecheck convergence
    if (Number)
    {
      fprintf(stderr, "After %d numbers, tip of Diff triangle on InputLine #%d is nonzero %d\n",
          TokenNr, InputLineNr);
      exit(4);
    }

    // When the triangle is complete, work backwards to predict the next input Number
    for (Number=0; Index; Index--)
      Number += Diff[Index-1];
    printf("After %d numbers on InputLine #%d, predict next to be %d\n", TokenNr, InputLineNr, Number);
    Sum += Number;
  } /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);

	/* Finalisation */
  printf("Found Sum %d\n", Sum);
	return 0;
}
