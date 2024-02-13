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
	int PathLen;
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
	PathLen = strlen(Path);
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
	int Current[1000];
	int CurrentNr, NrOfCurrent=0;
	for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
		if (Node[NodeNr].Name[2] == 'A')
			Current[NrOfCurrent++] = NodeNr;
	printf("PathLen %d  --  Found %d starting nodes\n", PathLen, NrOfCurrent);
	// In Position !

	// Analyzing cycles for each of the starting nodes
	int StepNr;
	unsigned long long Product=1;
	typedef struct {
		int NodeNr, StepNr;
		int Period;
	} tWaypoint;
	tWaypoint* Waypoint = (tWaypoint*)malloc(1000 * sizeof(tWaypoint));
	int WaypointNr, NrOfWaypoints;
	for (CurrentNr=0; CurrentNr<NrOfCurrent; CurrentNr++)
	{
		NodeNr = Current[CurrentNr];
		printf("== Starting %d Node #%d : %s ==\n", CurrentNr, NodeNr, Node[NodeNr].Name);
		NrOfWaypoints=0;
		for (StepNr=0; StepNr<1000000;)
		{
			// Assessment of this step
			if (Node[NodeNr].Name[2] == 'Z')
			{
				// Have we arrived at this Waypoint before ?
				for (WaypointNr=0; WaypointNr<NrOfWaypoints; WaypointNr++)
					if (Waypoint[WaypointNr].NodeNr == NodeNr)
						break;
				if (WaypointNr<NrOfWaypoints)
				{
					// YES we have been here before
					int Period = StepNr - Waypoint[WaypointNr].StepNr;
					if (!Waypoint[WaypointNr].Period)
					{
						// No Period was established yet
						printf("At StepNr %d for WaypointNr %d %s we establish Period %d\n",
								StepNr, WaypointNr, Node[NodeNr].Name, Period);
						// Puzzle seems constructed such that :
						// 1. All loops repeat perfectly from the start, without any 'prequel' path into the loop
						// 2. All loop periods are a Prime * PathLen
						// 3. Each loop has a unique such Prime
						// Therefore the answer can be found by multiplying all these Primes, and then just once PathLen
						Product *= (Period/PathLen);
					}
					else if (Waypoint[WaypointNr].Period != Period)
						printf("Breaking previous period %d at StepNr %d for WaypointNr %d %s, now becomes %d\n",
								Waypoint[WaypointNr].Period, StepNr, WaypointNr, Node[NodeNr].Name, Period);
					Waypoint[WaypointNr].StepNr = StepNr;
					Waypoint[WaypointNr].Period = Period;
				}
				else
				{
					// NO this is the first time here
					// Add this waypoint
					Waypoint[NrOfWaypoints].NodeNr = NodeNr;
					Waypoint[NrOfWaypoints].StepNr = StepNr;
					Waypoint[NrOfWaypoints].Period = 0;
					NrOfWaypoints++;
				} /* if Waypoint known */
			} /* if Z found */
			// Moving ahead
			if (Path[StepNr % PathLen] == 'L')
				NodeNr = Node[NodeNr].LeftNr;
			else
				NodeNr = Node[NodeNr].RightNr;
			// Bookkeeping
			//if (!((++StepNr)&0xFFFFFF))
			StepNr++;
		} /*for (StepNr) */
	} /* for (CurrentNr) */

	/* Finalisation */
  printf("Found ProductOfPrimes %llu * PathLen %d = %llu\n", Product, PathLen, Product*PathLen);
	return 0;
}
