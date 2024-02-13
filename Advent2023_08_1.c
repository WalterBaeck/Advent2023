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
	char Path[300];
	typedef struct {
		char Name[4], LeftName[4], RightName[4];
		int LeftNr, RightNr;
	} tNode;
	tNode *Node = (tNode*)malloc(2000 * sizeof(tNode));
	int NodeNr, NrOfNodes=0;

  /* Gathering data */

	// First line contains the Path
	fgets(InputLine, 1000, InputFile);
	// Strip line ending
	while ((InputLine[strlen(InputLine)-1]=='\n')
			|| (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
	strcpy(Path, InputLine);
	InputLineNr++;

	// Second line is a blank separator
	fgets(InputLine, 1000, InputFile);
	// Strip line ending
	while ((InputLine[strlen(InputLine)-1]=='\n')
			|| (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
	if (strlen(InputLine))
	{
		fprintf(stderr, "Expected a blank 2nd InputLine instead of %s\n", InputLine);
		exit(3);
	}
	InputLineNr++;

	// All further lines contain Nodes
	while (fgets(InputLine, 1000, InputFile))
	{
		// Bookkeeping
		InputLineNr++;
		// Strip line ending
		while ((InputLine[strlen(InputLine)-1]=='\n')
				|| (InputLine[strlen(InputLine)-1]=='\r'))  InputLine[strlen(InputLine)-1] ='\0';
    if (!strlen(InputLine))  break;
		// Parse this Node
		if (3 != sscanf(InputLine, "%3s = (%3s, %3s)",
						Node[NrOfNodes].Name, Node[NrOfNodes].LeftName, Node[NrOfNodes].RightName))
		{
			fprintf(stderr, "Could not scan 3 NodeNames in InputLine #%d : %s\n", InputLineNr, InputLine);
			exit(4);
		}
		NrOfNodes++;
	} /* while (fgets) */
	printf("Last node %s had Left %s and Right %s\n",
			Node[NrOfNodes-1].Name, Node[NrOfNodes-1].LeftName, Node[NrOfNodes-1].RightName);
	printf("%d InputLines were read.\n", InputLineNr);
	// Parse nodenames to form the full graph
	for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
	{
		for (Node[NodeNr].LeftNr=0; Node[NodeNr].LeftNr<NrOfNodes; Node[NodeNr].LeftNr++)
			if (!strcmp(Node[NodeNr].LeftName, Node[Node[NodeNr].LeftNr].Name))  break;
		if (Node[NodeNr].LeftNr >= NrOfNodes)
		{
			fprintf(stderr, "Could not find NodeNr #%d : %s Left node %s\n",
					NodeNr, Node[NodeNr].Name, Node[NodeNr].LeftName);
			exit(5);
		}
		for (Node[NodeNr].RightNr=0; Node[NodeNr].RightNr<NrOfNodes; Node[NodeNr].RightNr++)
			if (!strcmp(Node[NodeNr].RightName, Node[Node[NodeNr].RightNr].Name))  break;
		if (Node[NodeNr].RightNr >= NrOfNodes)
		{
			fprintf(stderr, "Could not find NodeNr #%d : %s Right node %s\n",
					NodeNr, Node[NodeNr].Name, Node[NodeNr].RightName);
			exit(5);
		}
	} /* for (NodeNr) */

	/* Operation */
	for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
		if (!strcmp(Node[NodeNr].Name, "AAA"))  break;
	// In Position !
	int StepNr=0;
	for (;;)
	{
		if (!strcmp(Node[NodeNr].Name, "ZZZ"))  break;
		if (Path[StepNr % strlen(Path)] == 'L')
			NodeNr = Node[NodeNr].LeftNr;
		else
			NodeNr = Node[NodeNr].RightNr;
		StepNr++;
		//printf("Wound up at Node #%d : %s\n", NodeNr, Node[NodeNr].Name);
	}

	/* Finalisation */
  printf("Found NrOfSteps %d\n", StepNr);
	return 0;
}
