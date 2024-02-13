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
	int Sum=0;
	char Text[10];
	int Number;
	char *Operand;
	int TokenNr;
	int BoxNr;
	int LensNr, NrOfLenses[256]={0};
	int* Focus[256];
	char** Name[256];
	for (BoxNr=0; BoxNr<256; BoxNr++)
	{
		Focus[BoxNr] = (int*)malloc(1000*sizeof(int));
		Name[BoxNr] = (char**)malloc(1000 * sizeof(char*));
		for (LensNr=0; LensNr<1000; LensNr++)
			Name[BoxNr][LensNr] = (char*)malloc(10);
	} /* for (BoxNr) */

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
		// Determine operand
		Operand = strpbrk(InputPtr, "=-");
		strcpy(Text, InputPtr);
		Text[Operand-InputPtr] = '\0';
		BoxNr = 0;
		for (InputPos=0; InputPos<strlen(Text); InputPos++)
		{
			BoxNr += Text[InputPos];
			BoxNr *= 17;
			BoxNr &= 0xFF;
		}

		// Debugging
		printf("Token %s : Name %s with hash %d, Operand %c", InputPtr, Text, BoxNr, *Operand);
		if (*Operand == '=')
		{
			if (1 != sscanf(Operand+1, "%d", &Number))
			{
				fprintf(stderr, "Could not scan FocalLength in %s\n", Operand+1);
				exit(4);
			}
			printf(", Focus %d\n", Number);
		}
		else
			putchar('\n');

		// Perform requested operation
		for (LensNr=0; LensNr<NrOfLenses[BoxNr]; LensNr++)
			if (!strcmp(Name[BoxNr][LensNr], Text))
				break;
		if (*Operand == '-')
		{
			if (LensNr >= NrOfLenses[BoxNr])
				printf("Lens %s not found among %d Lenses in Box %d\n", Text, NrOfLenses[BoxNr], BoxNr);
			else
			{
				for (; LensNr<NrOfLenses[BoxNr]-1; LensNr++)
				{
					Focus[BoxNr][LensNr] = Focus[BoxNr][LensNr+1];
					strcpy(Name[BoxNr][LensNr], Name[BoxNr][LensNr+1]);
				}
				NrOfLenses[BoxNr]--;
			}
		}
		else if (LensNr >= NrOfLenses[BoxNr]) // Adding a new Lens
		{
			Focus[BoxNr][LensNr] = Number;
			strcpy(Name[BoxNr][LensNr], Text);
			NrOfLenses[BoxNr]++;
		}
		else  // Replacing an existing Lens
			Focus[BoxNr][LensNr] = Number;

		// Debugging
		for (BoxNr=0; BoxNr<256; BoxNr++)
			if (NrOfLenses[BoxNr])
			{
				printf("Box %d:", BoxNr);
				for (LensNr=0; LensNr<NrOfLenses[BoxNr]; LensNr++)
					printf(" [%s %d]", Name[BoxNr][LensNr], Focus[BoxNr][LensNr]);
				putchar('\n');
			}

	} /* for (TokenNr) */

	/* Finalisation */
	for (BoxNr=0; BoxNr<256; BoxNr++)
		for (LensNr=0; LensNr<NrOfLenses[BoxNr]; LensNr++)
		{
			printf("Adding Box #%d x Lens #%d x Focus %d = %d to Sum\n",
					(BoxNr+1), (LensNr+1), Focus[BoxNr][LensNr],
					((BoxNr+1) * (LensNr+1) * Focus[BoxNr][LensNr]));
			Sum += ((BoxNr+1) * (LensNr+1) * Focus[BoxNr][LensNr]);
		}
  printf("After %d Tokens, found Sum %d\n", TokenNr, Sum);
	return 0;
}
