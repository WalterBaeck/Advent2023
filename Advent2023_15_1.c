#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int Arguments, char* Argument[])
{
	FILE* InputFile = stdin;
	char* InputLine = NULL;
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
	InputLine = (char*)malloc(30000);
	int Sum=0, Hash=0;
	int TokenNr;

  /* Gathering data */
	if (!fgets(InputLine, 30000, InputFile))
	{
		fprintf(stderr, "Could not read InputLine\n");
		exit(3);
	}
	// Strip line ending
	while ((InputLine[strlen(InputLine)-1]=='\n')
			|| (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
	InputLen = strlen(InputLine);
	printf("InputLine of %d chars read\n", InputLen);

	/* Operation */
	for (TokenNr=0; InputPtr=strtok(TokenNr?NULL:InputLine, ","); TokenNr++)
	{
		Hash = 0;
		for (InputPos=0; InputPos<strlen(InputPtr); InputPos++)
		{
			Hash += InputPtr[InputPos];
			Hash *= 17;
			Hash &= 0xFF;
		}

		// Debugging
		printf("Token %s yields hash %d\n", InputPtr, Hash);
		Sum += Hash;
	} /* for (TokenNr) */

  printf("After %d Tokens, found Sum %d\n", TokenNr, Sum);
	return 0;
}
