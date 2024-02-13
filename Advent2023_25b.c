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
  int TokenNr;
  char Text[4];
  int Index, MaxIndex=0;
  typedef struct {
    char Name[4];
    int NrOfLinks, Link[10], LinkNr[10];
    int Cost;
    int Color;
  } tNode;
  tNode Node[1500];
  tNode *NodePtr, *OtherPtr;
  int NrOfNodes=0, NodeNr, OtherNr;
  typedef struct {
    int Node[2];
    int Cut;
    int Weight;
  } tLink;
  tLink Link[5000];
  int NrOfLinks=0, LinkNr;
  memset(Link, 0, 5000 * sizeof(tLink));

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
    if (InputLine[3] != ':')
    {
      fprintf(stderr, "Expected colon at pos[3] of InputLine #%d, got %c instead\n",
          InputLineNr, InputLine[3]);
      exit(3);
    }
    // Left-hand side
    strncpy(Text, InputLine, 3);
    Text[3] = '\0';
    for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
      if (!strcmp(Node[NodeNr].Name, Text))  break;
    NodePtr = Node + NodeNr;
    if (NodeNr >= NrOfNodes)
    {
      // Create a new Node with this name
      strcpy(NodePtr->Name, Text);
      NodePtr->NrOfLinks = 0;
      NrOfNodes++;
    }
    // Right-hand side
    for (TokenNr=0; InputPtr=strtok(TokenNr?NULL:InputLine+5, " "); TokenNr++)
    {
      for (OtherNr=0; OtherNr<NrOfNodes; OtherNr++)
        if (!strcmp(Node[OtherNr].Name, InputPtr))  break;
      OtherPtr = Node + OtherNr;
      if (OtherNr >= NrOfNodes)
      {
        // Create a new Node with this name
        strcpy(OtherPtr->Name, InputPtr);
        OtherPtr->NrOfLinks = 0;
        NrOfNodes++;
      }
      // Establish the link for both Nodes involved
      Link[NrOfLinks].Node[0] = NodeNr;
      Link[NrOfLinks].Node[1] = OtherNr;
      NodePtr->Link[NodePtr->NrOfLinks] = OtherNr;
      NodePtr->LinkNr[NodePtr->NrOfLinks] = NrOfLinks;
      OtherPtr->Link[OtherPtr->NrOfLinks] = NodeNr;
      OtherPtr->LinkNr[OtherPtr->NrOfLinks] = NrOfLinks;
      if (NrOfLinks++ > 5000)
      {
        fprintf(stderr, "More than 5000 Links ?\n");
        exit(4);
      }
      NodePtr->NrOfLinks++;
      OtherPtr->NrOfLinks++;
    } /* for (TokenNr) */
  } /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);
  printf("Found %d Nodes and %d Links\n", NrOfNodes, NrOfLinks);

#if 0
  // Debugging
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
  {
    NodePtr = Node + NodeNr;
    printf("[%2d]:%s %d Links : {%s", NodeNr, NodePtr->Name, NodePtr->NrOfLinks,
        Node[NodePtr->Link[0]].Name);
    for (LinkNr=1; LinkNr<NodePtr->NrOfLinks; LinkNr++)
      printf(", %s", Node[NodePtr->Link[LinkNr]].Name);
    putchar('}');
    putchar('\n');
  }

  for (LinkNr=0; LinkNr<NrOfLinks; LinkNr++)
    printf("Link[%2d]%s-%s\n", LinkNr, Node[Link[LinkNr].Node[0]].Name,
        Node[Link[LinkNr].Node[1]].Name);
#endif


  /* Operation */
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
  {
  printf("===== NodeNr %4d =====\n", NodeNr);
  // Dijkstra :
  // compute the cost of reaching each node within the entire graph, from a starting point.
  // Remaining eligible target nodes have Color=0, final cost has been decided if Color=1.
  int Target[1500];
  int NrOfTargets=0, TargetNr, ShiftNr;
  for (OtherNr=0; OtherNr<NrOfNodes; OtherNr++)
  {
    Node[OtherNr].Color= 0;
    Node[OtherNr].Cost = 10000;
  }
  OtherPtr=Node+NodeNr;  OtherPtr->Cost=0;  OtherPtr->Color=1;
  for(;;)
  {
    // 1. For all undecided nodes connected to the freshly decided Node :
    for (LinkNr=0; LinkNr<OtherPtr->NrOfLinks; LinkNr++)
      if (!Node[OtherPtr->Link[LinkNr]].Color)
      {
        // 1a. Add them to the Target list (if not yet in there)
        for (TargetNr=0; TargetNr<NrOfTargets; TargetNr++)
          if (Target[TargetNr] == OtherPtr->Link[LinkNr])  break;
        if (TargetNr >= NrOfTargets)
          Target[NrOfTargets++] = OtherPtr->Link[LinkNr];
        // 1b. Update the cost estimation (still not final !)
        if (OtherPtr->Cost + 1 < Node[OtherPtr->Link[LinkNr]].Cost)
          Node[OtherPtr->Link[LinkNr]].Cost = OtherPtr->Cost + 1;
      }
    if (!NrOfTargets)  break;
    // 2. Go over the current Target list, and pick a new Node to finalize.
    // For Dijkstra algorithm it's essential that the lowest-cost Target is chosen.
    int MinCost = 10000, MinTargetNr;
    for (TargetNr=0; TargetNr<NrOfTargets; TargetNr++)
      if (Node[Target[TargetNr]].Cost < MinCost)
      {
        MinCost = Node[Target[TargetNr]].Cost;
        MinTargetNr = TargetNr;
        OtherNr = Target[TargetNr];
      }
    // 3a. The cost estimation for the chosen Target is now considered final.
    OtherPtr = Node + OtherNr;
    OtherPtr->Color = 1;
//printf("Node[%d]:%s final cost decision = %d\n", OtherNr, OtherPtr->Name, OtherPtr->Cost);
    // 3b. Remove this node out of the eligible Target list
    for (ShiftNr=MinTargetNr; ShiftNr<NrOfTargets-1; ShiftNr++)
      Target[ShiftNr] = Target[ShiftNr+1];
    NrOfTargets--;
  } /* forever Dijkstra */

#if 1
  // Let's get a histogram of these distance costs
  int Histo[1500];
  memset(Histo, 0, 1500 * sizeof(int));
  for (OtherNr=0; OtherNr<NrOfNodes; OtherNr++)
  {
    Index = Node[OtherNr].Cost;
    Histo[Index]++;
    if (Index > MaxIndex)
      MaxIndex = Index;
  }
  for (Index=0; Index<=MaxIndex; Index++)
    printf("%2d nodes with distance cost %2d\n", Histo[Index], Index);
#endif

  // For every 2 nodes, try to find the shortest path between them.
  // The Dijkstra results help to abort a fruitless search early.
  // Give all links within that shortest path a weight increase.
  for (OtherNr=NodeNr+1; OtherNr<NrOfNodes; OtherNr++)
  {
    int Choice[1500]={-1};
    int ChoiceNr=0;
    int PathNode[1500];

    int CurrentNr;
    CurrentNr = NodeNr;
    PathNode[0] = NodeNr;
    tNode *CurrentPtr = Node + CurrentNr;

    for (;;)
    {
      // --- Part 1 : making a new Choice at the current ChoiceNr ---
      // Always try to increase the Choice at the current ChoiceNr
      for (++Choice[ChoiceNr]; Choice[ChoiceNr]<CurrentPtr->NrOfLinks; Choice[ChoiceNr]++)
      {
        if ((ChoiceNr+1 >= Node[OtherNr].Cost) &&
            (CurrentPtr->Link[Choice[ChoiceNr]] != OtherNr))  continue;
        break;
      }

      // --- Part 2 : consider backtracking
      if (Choice[ChoiceNr] >= CurrentPtr->NrOfLinks)
      {
        // Backtracking indeed needed
        if (!ChoiceNr)
        {
          fprintf(stderr, "Shortest path Node[%d]->Other[%d] not found ?\n", NodeNr, OtherNr);
          exit(5);
        }
        //printf("Backtracking from %s ", CurrentPtr->Name);
        CurrentNr = PathNode[--ChoiceNr];
        CurrentPtr = Node + CurrentNr;
        //printf("to %s \n", CurrentPtr->Name);
        continue;
      }

      // --- Part 3 : registering the new Choice
      PathNode[ChoiceNr+1] = CurrentPtr->Link[Choice[ChoiceNr]];
      if (CurrentPtr->Link[Choice[ChoiceNr]] == OtherNr)
        // Found the shortest path !
        break;
      else
      {
        //printf("Advancing from %s ", CurrentPtr->Name);
        CurrentNr = CurrentPtr->Link[Choice[ChoiceNr]];
        CurrentPtr = Node + CurrentNr;
        //printf("to %s\n", CurrentPtr->Name);
        // The next step of the path starts with a clean slate
        Choice[++ChoiceNr] = -1;
        PathNode[ChoiceNr] = CurrentNr;
      }
    } /* forever pathfinding */

//    printf("MinPathLen %d between [%d]:%s and [%d]:%s  ",
//        Node[OtherNr].Cost, NodeNr, Node[NodeNr].Name, OtherNr, Node[OtherNr].Name);

    // Mark the shortest path Node -> Other
    tNode *PrevPtr = Node + NodeNr;
    for (int ChNr=0; ChNr<=ChoiceNr; ChNr++)
    {
      Link[PrevPtr->LinkNr[Choice[ChNr]]].Weight++;
//      printf(" %s-%s", PrevPtr->Name, Node[PrevPtr->Link[Choice[ChNr]]].Name);
      PrevPtr = Node + PrevPtr->Link[Choice[ChNr]];
    }
//    putchar('\n');

  } /* for (OtherNr) */
  } /* for (NodeNr) */

  // List all these weights, and while doing this, retain most popular 3
  int WeightSum = 0;
  int Max[3]={0}, MaxLinkNr[3];
  for (LinkNr=0; LinkNr<NrOfLinks; LinkNr++)
  {
    printf("Link[%2d]%s-%s : weight %d\n", LinkNr, Node[Link[LinkNr].Node[0]].Name,
        Node[Link[LinkNr].Node[1]].Name, Link[LinkNr].Weight);
    WeightSum += Link[LinkNr].Weight;
    for (Index=3; Index; Index--)
      if (Link[LinkNr].Weight > Max[Index-1])
      {
        // Shift lower values out right
        for (MaxIndex=3; MaxIndex>Index; MaxIndex--)
        {
          Max[MaxIndex-1] = Max[MaxIndex-2];
          MaxLinkNr[MaxIndex-1] = MaxLinkNr[MaxIndex-2];
        }
        Max[Index-1] = Link[LinkNr].Weight;
        MaxLinkNr[Index-1] = LinkNr;
      }
  } /* for (LinkNr) */
  printf("WeightSum : %d\n", WeightSum);
  for (Index=0; Index<3; Index++)
    printf("Link[%d]%s-%s has weight %d\n", MaxLinkNr[Index],
        Node[Link[MaxLinkNr[Index]].Node[0]].Name,
        Node[Link[MaxLinkNr[Index]].Node[1]].Name,
        Link[MaxLinkNr[Index]].Weight);


  /* Finalisation */
  // Now that the lifeblood links of these conjoined twins are known, SAW them apart
  for (Index=0; Index<3; Index++)
    Link[MaxLinkNr[Index]].Cut = 1;

  // Dijkstra again:
  // compute the cost of reaching each node within the entire graph, from a starting point.
  // Pick Node[0] arbitraily as starting point
  // Remaining eligible target nodes have Color=0, final cost has been decided if Color=1.
  NodeNr=0;
  int Target[1500];
  int NrOfTargets=0, TargetNr, ShiftNr;
  for (OtherNr=0; OtherNr<NrOfNodes; OtherNr++)
  {
    Node[OtherNr].Color= 0;
    Node[OtherNr].Cost = 10000;
  }
  OtherPtr=Node+NodeNr;  OtherPtr->Cost=0;  OtherPtr->Color=1;
  for(;;)
  {
    // 1. For all undecided nodes connected to the freshly decided Node :
    for (LinkNr=0; LinkNr<OtherPtr->NrOfLinks; LinkNr++)
      if ((!Node[OtherPtr->Link[LinkNr]].Color) &&
          (!Link[OtherPtr->LinkNr[LinkNr]].Cut))
      {
        // 1a. Add them to the Target list (if not yet in there)
        for (TargetNr=0; TargetNr<NrOfTargets; TargetNr++)
          if (Target[TargetNr] == OtherPtr->Link[LinkNr])  break;
        if (TargetNr >= NrOfTargets)
          Target[NrOfTargets++] = OtherPtr->Link[LinkNr];
        // 1b. Update the cost estimation (still not final !)
        if (OtherPtr->Cost + 1 < Node[OtherPtr->Link[LinkNr]].Cost)
          Node[OtherPtr->Link[LinkNr]].Cost = OtherPtr->Cost + 1;
      }
    if (!NrOfTargets)  break;
    // 2. Go over the current Target list, and pick a new Node to finalize.
    // For Dijkstra algorithm it's essential that the lowest-cost Target is chosen.
    int MinCost = 10000, MinTargetNr;
    for (TargetNr=0; TargetNr<NrOfTargets; TargetNr++)
      if (Node[Target[TargetNr]].Cost < MinCost)
      {
        MinCost = Node[Target[TargetNr]].Cost;
        MinTargetNr = TargetNr;
        OtherNr = Target[TargetNr];
      }
    // 3a. The cost estimation for the chosen Target is now considered final.
    OtherPtr = Node + OtherNr;
    OtherPtr->Color = 1;
//printf("Node[%d]:%s final cost decision = %d\n", OtherNr, OtherPtr->Name, OtherPtr->Cost);
    // 3b. Remove this node out of the eligible Target list
    for (ShiftNr=MinTargetNr; ShiftNr<NrOfTargets-1; ShiftNr++)
      Target[ShiftNr] = Target[ShiftNr+1];
    NrOfTargets--;
  } /* forever Dijkstra */

  // We now have two isolated sets : Nodes reachable from Node[0], and unreachables
  int Reachable=0, Unreachable=0;
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
    if (Node[NodeNr].Color)
      Reachable++;
    else
      Unreachable++;
  printf("From Node[0] Reachable %d x Unreachable %d = %d\n",
      Reachable, Unreachable, Reachable * Unreachable);

	return 0;
}
