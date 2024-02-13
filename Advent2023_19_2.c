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
	char Text[10];
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
  } /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);
	printf("Found %d Workflows\n", NrOfWorkflows);

	/* Operation */
	// We start out with a four-dimensional hypercube of size 4000 along x,m,a,s axes
	//   which enters the 'in' workflow. Each condition slices the current hypercube
	//   neatly into two parts : the unfulfilled part goes on to the next condition,
	//   and the fulfilling part is thrown onto a stack for later processing.
	// No overlaps are allowed, and no gaps either - both sliced hypercubes together,
	//   should take up the exact same volume as the original current hypercube.
	typedef struct {
		int From[4],To[4];   // Strictly higher than From, smaller-or-equal to To
		char TargetName[10];
	} tCube;
	tCube *Cube, *CubePtr;
	Cube = (tCube*)malloc(10000 * sizeof(tCube));
	int CubeNr, NrOfCubes, Dim;
	// Initial situation
	for (Dim=0; Dim<4; Dim++)
	{
		Cube->From[Dim] = 0;
		Cube->To[Dim] = 4000;
	}
	strcpy(Cube->TargetName, "in");
	NrOfCubes = 1;

	unsigned long long Volume, Sum=0;
	// Forever keep handling hypercubes from the stack : always take the firstmost cube
	while (NrOfCubes)
	{
		// Handle trivial workflows
		if (!strcmp(Cube->TargetName, "A"))
		{
			// This entire cube has been Accepted.
			// Add its volume to the Sum.
			Volume = 1;
			for (Dim=0; Dim<4; Dim++)
				Volume *= (Cube->To[Dim] - Cube->From[Dim]);
			Sum += Volume;
		}
		if ((!strcmp(Cube->TargetName, "A")) || (!strcmp(Cube->TargetName, "R")))
		{
			// Remove the firstmost cube from the stack
			for (CubeNr=1; CubeNr<NrOfCubes; CubeNr++)
				memcpy(Cube+CubeNr-1, Cube+CubeNr, sizeof(tCube));
			NrOfCubes--;
			continue;
		}
		// Now deal with a regular workflow
		tWorkflow* WorkflowPtr;
		for (WorkflowNr=0,WorkflowPtr=Workflow; WorkflowNr<NrOfWorkflows; WorkflowNr++, WorkflowPtr++)
			if (!strcmp(Workflow[WorkflowNr].Name, Cube->TargetName))  break;
		if (WorkflowNr >= NrOfWorkflows)
		{
			fprintf(stderr, "No workflow \'%s\' found ?\n", Cube->TargetName);
			exit(4);
		}
		int RuleNr, RuleDim;
		tRule *RulePtr;
		for (RuleNr=0,RulePtr=WorkflowPtr->Rule; RuleNr < WorkflowPtr->NrOfRules; RuleNr++,RulePtr++)
		{
			switch (RulePtr->Property)
			{
				case 'x': RuleDim = 0;  break;
				case 'm': RuleDim = 1;  break;
				case 'a': RuleDim = 2;  break;
				case 's': RuleDim = 3;  break;
			}

#if 0
			printf("Cube[0 from %d]: x[%d-%d] m[%d-%d] a[%d-%d] s[%d-%d] WF:%s\n", NrOfCubes,
					Cube->From[0], Cube->To[0], Cube->From[1], Cube->To[1], 
					Cube->From[2], Cube->To[2], Cube->From[3], Cube->To[3], Cube->TargetName);
			printf("Rule[%d from %d]: Dim[%d] %c %d ?\n", RuleNr, WorkflowPtr->NrOfRules, RuleDim,
					RulePtr->Larger ? '>' : '<', RulePtr->Threshold);
#endif

			// Check for trivial cases first
			if ((( RulePtr->Larger) && (Cube->From[RuleDim] >= RulePtr->Threshold)) ||
					((!RulePtr->Larger) && (Cube->To[RuleDim]    < RulePtr->Threshold)))
			{
				// The entire incoming cube fulfills the condition.
				// Keep it in place, just change the targetname, and quit this workflow
				strcpy(Cube->TargetName, RulePtr->TargetName);
				break;  /* for (RuleNr) */
			}
			if ((( RulePtr->Larger) && (Cube->To[RuleDim]   <= RulePtr->Threshold)) ||
					((!RulePtr->Larger) && (Cube->From[RuleDim] >= RulePtr->Threshold-1)))
				// The entire incoming cube fails the condition.
				// It should continue unchanged into the next Rule.
				continue;  /* for (RuleNr) */
			// An actual cube slicing occurs.
			// Create a new cube on the stack, with th fulfilling part
			CubePtr = Cube + NrOfCubes;
			memcpy(CubePtr, Cube, sizeof(tCube));
			if (RulePtr->Larger)
			{
				CubePtr->From[RuleDim] = RulePtr->Threshold;
				// Also modify the original cube which fails the condition
				Cube->To[RuleDim]      = RulePtr->Threshold;
			}
			else
			{
				CubePtr->To[RuleDim]   = RulePtr->Threshold-1;
				// Also modify the original cube which fails the condition
				Cube->From[RuleDim]    = RulePtr->Threshold-1;
			}
			strcpy(CubePtr->TargetName, RulePtr->TargetName);
			NrOfCubes++;
		} /* for (RuleNr) */
	} /* while (NrOfCubes) */

	/* Finalisation */
	printf("Found Sum %llu\n", Sum);
	return 0;
}
