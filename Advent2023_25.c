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
  typedef struct {
    char Name[4];
    int NrOfLinks, Link[10], LinkNr[10];
    int Color;
  } tNode;
  tNode Node[1500];
  tNode *NodePtr, *OtherPtr;
  int NrOfNodes=0, NodeNr, OtherNr;
  typedef struct {
    int Node[2];
    int Weight;
    int Cut;
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
#endif


  /* Operation */
  // For every 2 nodes, try to find the shortest path between them.
  // Give all links within that shortest path a weight increase.
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
    for (OtherNr=NodeNr+1; OtherNr<NrOfNodes; OtherNr++)
    {
	int Choice[1500]={-1};
	int ChoiceNr=0;
  int PathNode[1500];
	int MinPathLen=10000;
  int MinPathLink[1500];

  int CurrentNr;
  for (CurrentNr=0; CurrentNr<NrOfNodes; CurrentNr++) Node[CurrentNr].Color=0;
  Node[NodeNr].Color = 1;
  CurrentNr = NodeNr;
  PathNode[0] = NodeNr;
  tNode *CurrentPtr = Node + CurrentNr;

	for (;;)
	{
		// --- Part 1 : making a new Choice at the current ChoiceNr ---
		// Always try to increase the Choice at the current ChoiceNr
		// Exceptions to this default : not going beyond 26 possibilities
		for (++Choice[ChoiceNr]; Choice[ChoiceNr]<CurrentPtr->NrOfLinks; Choice[ChoiceNr]++)
      if (!Node[CurrentPtr->Link[Choice[ChoiceNr]]].Color)  break;

		// --- Part 2 : consider backtracking
		if (Choice[ChoiceNr] >= CurrentPtr->NrOfLinks)
		{
			// Backtracking indeed needed
			if (!ChoiceNr)  break;
			CurrentPtr->Color = 0;
//printf("Backtracking from %s ", CurrentPtr->Name);
      CurrentNr = PathNode[--ChoiceNr];
      CurrentPtr = Node + CurrentNr;
//printf("to %s \n", CurrentPtr->Name);
			continue;
		}

		// --- Part 3 : registering the new Choice
    PathNode[ChoiceNr+1] = CurrentPtr->Link[Choice[ChoiceNr]];
		if (CurrentPtr->Link[Choice[ChoiceNr]] == OtherNr)
		{
      // Consider this complete path
      if (ChoiceNr+1 < MinPathLen)
      {
        MinPathLen = ChoiceNr+1;
        tNode *PrevPtr = Node + NodeNr;
        for (int ChNr=0; ChNr<=ChoiceNr; ChNr++)
        {
          MinPathLink[ChNr] = PrevPtr->LinkNr[Choice[ChNr]];
          PrevPtr = Node + PrevPtr->Link[Choice[ChNr]];
        }
      }
		}
		else
    {
//printf("Advancing from %s ", CurrentPtr->Name);
      CurrentNr = CurrentPtr->Link[Choice[ChoiceNr]];
      CurrentPtr = Node + CurrentNr;
//printf("to %s\n", CurrentPtr->Name);
			CurrentPtr->Color = 1;
			// The next step of the path starts with a clean slate
			Choice[++ChoiceNr] = -1;
      PathNode[ChoiceNr] = CurrentNr;
    }
	} /* forever */

  printf("MinPathLen %d between [%d]:%s and [%d]:%s\n  ",
      MinPathLen, NodeNr, Node[NodeNr].Name, OtherNr, Node[OtherNr].Name);
  for (int Step=0; Step<MinPathLen; Step++)
  {
    printf(" %s-%s",
        Node[Link[MinPathLink[Step]].Node[0]].Name,
        Node[Link[MinPathLink[Step]].Node[1]].Name);
    Link[MinPathLink[Step]].Weight++;
  }
  putchar('\n');
  if (MinPathLen == 10000)  exit(6);

    } /* for (OtherNr) */

  int WeightSum = 0;
  for (LinkNr=0; LinkNr<NrOfLinks; LinkNr++)
  {

    printf("Link[%2d]%s-%s : weight %d\n", LinkNr, Node[Link[LinkNr].Node[0]].Name,
        Node[Link[LinkNr].Node[1]].Name, Link[LinkNr].Weight);
    WeightSum += Link[LinkNr].Weight;
  }
  printf("WeightSum : %d\n", WeightSum);

  /* Finalisation */
	return 0;
}
