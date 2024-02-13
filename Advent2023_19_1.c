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
	int TokenNr, Number, Sum=0;
	char Text[10];
	int ParsingWorkflows = 1;
	// Workflow variables
	typedef struct {
		char Property;
		int Threshold, Larger;
		char TargetName[10];
	} tRule;
	typedef struct {
		char Name[10];
		int NrOfRules;
		tRule Rule[10];
	} tWorkflow;
	tWorkflow Workflow[1000];
	int WorkflowNr, NrOfWorkflows=0;
	// Parts variables
	int x[1000],m[1000],a[1000],s[1000];
	int PartNr, NrOfParts=0;

  /* Data gathering */
	while (fgets(InputLine, 1000, InputFile))
	{
		// Bookkeeping
		InputLineNr++;
		// Strip line ending
		while ((InputLine[strlen(InputLine)-1]=='\n')
				|| (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
    InputLen = strlen(InputLine);
    if (!InputLen)
		{
			ParsingWorkflows = 0;
			continue;
		}
		if (ParsingWorkflows)
		{
			// Get the name
			if (!(InputPtr = strchr(InputLine, '{')))
			{
				fprintf(stderr, "Could not find opening parenthesis on InputLine #%d\n", InputLineNr);
				exit(3);
			}
			strncpy(Workflow[NrOfWorkflows].Name, InputLine, InputPtr-InputLine);
			Workflow[NrOfWorkflows].Name[InputPtr-InputLine] = '\0';
			if (InputLine[InputLen-1] != '}')
			{
				fprintf(stderr, "Could not find closing parenthesis on InputLine #%d\n", InputLineNr);
				exit(3);
			}
			InputLine[InputLen-1] = '\0';  // Delete the closing parenthesis
			Workflow[NrOfWorkflows].NrOfRules = 0;
			for (TokenNr=0; InputPtr=strtok(TokenNr?NULL:InputPtr+1, ","); TokenNr++)
			{
				if ((strlen(InputPtr) < 2) || (!strpbrk(InputPtr, "<>")))
				{
					strcpy(Workflow[NrOfWorkflows].Rule[Workflow[NrOfWorkflows].NrOfRules].TargetName, InputPtr);
					// This catch-all rule should always meet the condition, setup a dummy
					Workflow[NrOfWorkflows].Rule[Workflow[NrOfWorkflows].NrOfRules].Property = 'x';
					Workflow[NrOfWorkflows].Rule[Workflow[NrOfWorkflows].NrOfRules].Larger = 1;
					Workflow[NrOfWorkflows].Rule[Workflow[NrOfWorkflows].NrOfRules].Threshold = 0;
				}
				else
				{
					Workflow[NrOfWorkflows].Rule[Workflow[NrOfWorkflows].NrOfRules].Property = *InputPtr;
					if (InputPtr[1] == '>')
						Workflow[NrOfWorkflows].Rule[Workflow[NrOfWorkflows].NrOfRules].Larger = 1;
					else
						Workflow[NrOfWorkflows].Rule[Workflow[NrOfWorkflows].NrOfRules].Larger = 0;
					InputPos = strchr(InputPtr, ':') - InputPtr;
					strncpy(Text, InputPtr+2, InputPos-2);
					Text[InputPos-2] = '\0';
					if (1 != sscanf(Text, "%d", &Number))
					{
						fprintf(stderr, "Could not parse threshold number in Token #%d : %s of workflow #%d on InputLine #%d\n",
								TokenNr, InputPtr, NrOfWorkflows, InputLineNr);
						exit(3);
					}
					Workflow[NrOfWorkflows].Rule[Workflow[NrOfWorkflows].NrOfRules].Threshold = Number;
					strncpy(Workflow[NrOfWorkflows].Rule[Workflow[NrOfWorkflows].NrOfRules].TargetName,
							InputPtr + InputPos + 1, strlen(InputPtr) - InputPos - 1);
					Workflow[NrOfWorkflows].Rule[Workflow[NrOfWorkflows].NrOfRules].TargetName[strlen(InputPtr) - InputPos - 1]
						= '\0';
				}
				Workflow[NrOfWorkflows].NrOfRules++;
			}
			NrOfWorkflows++;
		} /* if (ParsingWorkflows) */
		else
		{
			// Parse this part
			if ((*InputLine != '{') || (InputLine[InputLen-1] != '}'))
			{
				fprintf(stderr, "No surrounding parentheses around Part on InputLine #%d\n", InputLineNr);
				exit(3);
			}
			for (TokenNr=0; InputPtr=strtok(TokenNr?NULL:InputLine+1, ","); TokenNr++)
			{
				if (InputPtr[1] != '=')
				{
					fprintf(stderr, "No equal sign in Token #%d of part #%d on InputLine #%d\n",
							TokenNr, NrOfParts, InputLineNr);
					exit(3);
				}
				if (1 != sscanf(InputPtr+2, "%d", &Number))
				{
					fprintf(stderr, "Could not parse Number in Token #%d : %s of part #%d on InputLine #%d\n",
							TokenNr, InputPtr+2, NrOfParts, InputLineNr);
					exit(3);
				}
				switch (*InputPtr)
				{
					case 'x' : x[NrOfParts]=Number;  break;
					case 'm' : m[NrOfParts]=Number;  break;
					case 'a' : a[NrOfParts]=Number;  break;
					case 's' : s[NrOfParts]=Number;  break;
					default: 
						fprintf(stderr, "Unrecognised letter %c in Token #%d : %s of part #%d on InputLine #%d\n",
								*InputPtr, TokenNr, InputPtr, NrOfParts, InputLineNr);
						exit(3);
				}
			}
			NrOfParts++;
		} /* not ParsingWorkflows */
  } /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);
	printf("Found %d Workflows and %d Parts\n", NrOfWorkflows, NrOfParts);

	/* Operation */
	int RuleNr, StartWorkflowNr;
	int PartProperty;
	tWorkflow* WorkflowPtr;
	// Look for the entry Workflow once, store this result for every part
	for (WorkflowNr=0; WorkflowNr<NrOfWorkflows; WorkflowNr++)
		if (!strcmp(Workflow[WorkflowNr].Name, "in"))  break;
	if (WorkflowNr >= NrOfWorkflows)
	{
		fprintf(stderr, "No \'in\' workflow found ?\n");
		exit(4);
	}
	StartWorkflowNr = WorkflowNr;
	for (PartNr=0; PartNr<NrOfParts; PartNr++)
	{
		int Accepted=0;
		WorkflowPtr = Workflow + StartWorkflowNr;
		// Keep following workflows, for this particular part
		for (;;)
		{
			for (RuleNr=0; RuleNr < WorkflowPtr->NrOfRules; RuleNr++)
			{
				int Fulfilled=0;
				switch (WorkflowPtr->Rule[RuleNr].Property)
				{
					case 'x': PartProperty = x[PartNr];  break;
					case 'm': PartProperty = m[PartNr];  break;
					case 'a': PartProperty = a[PartNr];  break;
					case 's': PartProperty = s[PartNr];  break;
				}
				if (WorkflowPtr->Rule[RuleNr].Larger &&
						(PartProperty > WorkflowPtr->Rule[RuleNr].Threshold))  Fulfilled = 1;
				else if ((!WorkflowPtr->Rule[RuleNr].Larger) &&
						(PartProperty < WorkflowPtr->Rule[RuleNr].Threshold))  Fulfilled = 1;
				if (!Fulfilled)  continue; // goto next Rule
				// Condition was met - jump to appropriate new workflow
				//   but consider 2 special workflow names first
				if (!strcmp(WorkflowPtr->Rule[RuleNr].TargetName, "R"))
				{
					WorkflowPtr = NULL;
					break;
				}
				if (!strcmp(WorkflowPtr->Rule[RuleNr].TargetName, "A"))
				{
					Accepted = 1;
					WorkflowPtr = NULL;
					break;
				}
				for (WorkflowNr=0; WorkflowNr<NrOfWorkflows; WorkflowNr++)
					if (!strcmp(Workflow[WorkflowNr].Name, WorkflowPtr->Rule[RuleNr].TargetName))  break;
				if (WorkflowNr >= NrOfWorkflows)
				{
					fprintf(stderr, "Could not find workflow %s ?\n", WorkflowPtr->Rule[RuleNr].TargetName);
					exit(4);
				}
				WorkflowPtr = Workflow + WorkflowNr;
				break;
			} /* for (Rule) */
			if (!WorkflowPtr)  break;
		} /* forever following new workflows for this particular part */
		if (Accepted)
		{
			Sum += x[PartNr];
			Sum += m[PartNr];
			Sum += a[PartNr];
			Sum += s[PartNr];
		}
	} /* for (PartNr) */

	/* Finalisation */
	printf("Found Sum %d\n", Sum);
	return 0;
}
