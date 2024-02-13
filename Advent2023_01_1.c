#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int Arguments, char* Argument[])
{
	FILE* InputFile = stdin;
	char InputLine[100];
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
		fprintf(stderr, "%s [InputFile]\n", Argument[0]);
		fprintf(stderr, "\twith no [InputFile] argument, data will be read from stdin\n");
		exit(1);
	}

	/* Initialisation */
	int Sum=0;
	int FirstDigit, LastDigit;

	/* Operation */
	while (fgets(InputLine, 100, InputFile))
	{
		// Bookkeeping
		InputLineNr++;
		// Strip line ending
		while ((InputLine[strlen(InputLine)-1]=='\n')
				|| (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
		InputLen = strlen(InputLine);
		FirstDigit = -1;
		for (InputPos=0,InputPtr=InputLine; InputPos<InputLen; InputPos++,InputPtr++)
			if ((*InputPtr >= '0') && (*InputPtr <= '9'))
			{
				LastDigit = *InputPtr - '0';
				if (FirstDigit < 0)  FirstDigit=LastDigit;
			}
		// Debugging
		//printf("Line %d: FirstDigit %d LastDigit %d\n", InputLineNr, FirstDigit, LastDigit);
		Sum += 10*FirstDigit + LastDigit;
	} /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);

	/* Finalisation */
	printf("Sum found: %d\n", Sum);
	return 0;
}
