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
  char Grid[200][200];
  typedef struct {
    int X,Y;
    int ConnectNode[4];  // Ordered NESW
    int ConnectDir[4];   // 0 for not enforced, 1 or -1 for enforced from NodeNr to ConnectNodeNr
    int ConnectLen[4];   // -1 for impossible, 0 for not yet crawled, >0 for known length
    int Visited;
  } tNode;
  tNode *Node = (tNode*)malloc(1000 * sizeof(tNode));
  tNode *NodePtr, *OtherPtr;
  int NrOfNodes=0, NodeNr, OtherNr, ConnectNr;
  int X,Y=0, SizeX=0,SizeY,  ToX,ToY;
  int Enforced, RunLength, FromNode, ToNode;
  int Dir, StartDir, EndDir, PrevDir=StartDir, NextDir, NrOfDirs;

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
    if (!SizeX)  SizeX = InputLen;
    strcpy(Grid[Y], InputLine);
    Y++;
  } /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);
  SizeY = Y;

  // Start with just hardcoded Begin and End node
  // Start node : only connection goes South
  NodePtr = Node;
  NodePtr->X = 1; NodePtr->Y = 0;
  for (Dir=0; Dir<4; Dir++)  NodePtr->ConnectLen[Dir] = -1;
  NodePtr->ConnectLen[2] = 0;
  NodePtr->ConnectDir[2] = 0;
  // End node : only connection goes North
  NodePtr++;
  NodePtr->X = SizeX-2; NodePtr->Y = SizeY-1;
  for (Dir=0; Dir<4; Dir++)  NodePtr->ConnectLen[Dir] = -1;
  NodePtr->ConnectLen[0] = 0;
  NodePtr->ConnectDir[0] = 0;
  NrOfNodes=2;

  /* Path crawling : discover all nodes */
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
  {
    // Don't crawl from End node
    if (NodeNr==1)  continue;
    NodePtr = Node + NodeNr;
    for (StartDir=0; StartDir<4; StartDir++)
    {
      // Only investigate uncrawled directions from this Node
      if (NodePtr->ConnectLen[StartDir])  continue;
      ToX=NodePtr->X; ToY=NodePtr->Y;
      switch (StartDir)
      {
        case 0: ToY--;  break;
        case 1: ToX++;  break;
        case 2: ToY++;  break;
        case 3: ToX--;  break;
      }
      if (Grid[ToY][ToX] == '#')
      {
        // No movement possible in this direction
        NodePtr->ConnectLen[StartDir] = -1;
        continue; /* for (StartDir) */
      }
      // Movement possible along yet unknown path.
      // Embark on this journey until another node is encountered.
      RunLength = 1; Enforced = 0; FromNode = NodeNr;
      PrevDir = StartDir;
      for(;;)
      {
        // So we have arrived on a new spot.
        X=ToX; Y=ToY;
        // Check if this happens to be the End node.
        if ((Node[1].X == X) && (Node[1].Y == Y))  break;
        // Check for direction enforcement signals
        if (Grid[Y][X] != '.')
        {
          switch (PrevDir)
          {
            case 0:
              if      (Grid[Y][X] == '^')  Enforced = 1;
              else if (Grid[Y][X] == 'v')  Enforced = -1;
              else
              {
                fprintf(stderr, "Signal [%d,%d]%c unexpected with PrevDir %d\n", Y,X,Grid[Y][X], PrevDir);
                exit(3);
              }
              break;
            case 1:
              if      (Grid[Y][X] == '>')  Enforced = 1;
              else if (Grid[Y][X] == '<')  Enforced = -1;
              else
              {
                fprintf(stderr, "Signal [%d,%d]%c unexpected with PrevDir %d\n", Y,X,Grid[Y][X], PrevDir);
                exit(3);
              }
              break;
            case 2:
              if      (Grid[Y][X] == 'v')  Enforced = 1;
              else if (Grid[Y][X] == '^')  Enforced = -1;
              else
              {
                fprintf(stderr, "Signal [%d,%d]%c unexpected with PrevDir %d\n", Y,X,Grid[Y][X], PrevDir);
                exit(3);
              }
              break;
            case 3:
              if      (Grid[Y][X] == '<')  Enforced = 1;
              else if (Grid[Y][X] == '>')  Enforced = -1;
              else
              {
                fprintf(stderr, "Signal [%d,%d]%c unexpected with PrevDir %d\n", Y,X,Grid[Y][X], PrevDir);
                exit(3);
              }
              break;
          } /* switch (PrevDir) */
        } /* if (Grid[Y][X] != '.') */
        // Investigate how many NextDirs are available from this new spot
        NrOfDirs = 0;
        int CandDir, CandX, CandY;
        for (NextDir=0; NextDir<4; NextDir++)
        {
          // Don't consider going back where we came from
          if (NextDir == ((PrevDir+2)%4))  continue;
          ToX=X; ToY=Y;
          switch (NextDir)
          {
            case 0: ToY--;  break;
            case 1: ToX++;  break;
            case 2: ToY++;  break;
            case 3: ToX--;  break;
          }
          if (Grid[ToY][ToX] != '#')
          {
            NrOfDirs++;  // Accessible direction
            CandDir = NextDir;
            CandX = ToX;
            CandY = ToY;
          }
        } /* for (NextDir) */
        if (NrOfDirs > 1)  break;  // Arrived at a new Node
        // Just one continuation Dir possible ? Follow it !
        PrevDir = CandDir;  ToX = CandX; ToY = CandY;
        RunLength++;
      } /* forever following a connection */
      // Arriving here, means that we have arrived at another Node. Was it known already ?
      for (ToNode=0; ToNode<NrOfNodes; ToNode++)
        if ((Node[ToNode].X == X) && (Node[ToNode].Y == Y))  break;
      OtherPtr = Node + ToNode;
      EndDir = ((PrevDir+2)%4);
      // Have we actually discovered a new node ?
      if (ToNode >= NrOfNodes)
      {
        // Initialize this new node
        OtherPtr->X = X; OtherPtr->Y = Y;
        printf("Discovered Node[%d] at [%d,%d]\n", ToNode, Y,X);
        memset(OtherPtr->ConnectLen, 0, 4 * sizeof(int));
        NrOfNodes++;
      }
      // Store all the findings into both Nodes at either side of this connection
      printf("Establishing From[%d]-Dir[%d] To[%d]-Dir[%d] Enforced %d RunLength %d\n",
          FromNode, StartDir, ToNode, EndDir, Enforced, RunLength);
      NodePtr->ConnectNode[StartDir] = ToNode;
      OtherPtr->ConnectNode[EndDir] = FromNode;
      if (ToNode > FromNode)
      {
        NodePtr->ConnectDir[StartDir] = Enforced;
        OtherPtr->ConnectDir[EndDir] = Enforced;
      }
      else
      {
        NodePtr->ConnectDir[StartDir] = -Enforced;
        OtherPtr->ConnectDir[EndDir] = -Enforced;
      }
      NodePtr->ConnectLen[StartDir] = RunLength;
      OtherPtr->ConnectLen[EndDir] = RunLength;
    } /* for (StartDir) */
  } /* for (NodeNr) */
  printf("Discovered %d Nodes\n", NrOfNodes);

#if 0
  // Debugging
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)
  {
    NodePtr = Node + NodeNr;
    printf("Node[%d] at [%d,%d] :\n", NodeNr, NodePtr->Y, NodePtr->X);
    for (Dir=0; Dir<4; Dir++)
      if (NodePtr->ConnectLen[Dir] > 0)
      {
        OtherNr = NodePtr->ConnectNode[Dir];
        OtherPtr = Node + OtherNr;
        printf("  .. by Dir[%d] connects to Other[%d] at [%d,%d] with RunLength %d Enforced %d\n",
            Dir, OtherNr, OtherPtr->Y, OtherPtr->X, NodePtr->ConnectLen[Dir], NodePtr->ConnectDir[Dir]);
      }
  } /* for (NodeNr) */
#endif

  /* Operation */
  // Traditional exhaustive search through this crawled graph
	int Choice[40]={-1};
	int ChoiceNr=0;
	int NrOfPaths=0;
  // Remember which path we have traversed so far
  int PathNode[40]={0};
  // Monitor if we've been through a Node already
  for (NodeNr=0; NodeNr<NrOfNodes; NodeNr++)  Node[NodeNr].Visited = 0;
  // Starting situation
  NodePtr = Node; NodePtr->Visited = 1; RunLength = 0; 
  // Bookkeeping
  int Max=0;

	for (;;)
	{
		// --- Part 1 : making a new Choice at the current ChoiceNr ---
		// Always try to increase the Choice at the current ChoiceNr
		int MaxChoice = 4;  // Default
		for (++Choice[ChoiceNr]; Choice[ChoiceNr]<MaxChoice; Choice[ChoiceNr]++)
    {
      if (NodePtr->ConnectLen[Choice[ChoiceNr]] <= 0)  continue;
      ToNode = NodePtr->ConnectNode[Choice[ChoiceNr]];
      OtherPtr = Node + ToNode;
      // Don't go to a Node that has already been visited
      if (OtherPtr->Visited)  continue;
      // Do NOT Respect enforced directions
      //if (NodePtr->ConnectDir[Choice[ChoiceNr]] * (ToNode - (NodePtr-Node)) < 0)  continue;
      break;
    }

		// --- Part 2 : consider backtracking
		if (Choice[ChoiceNr] >= MaxChoice)
		{
			// Backtracking indeed needed
			if (!ChoiceNr)  break;
      NodePtr->Visited = 0;
      NodePtr = Node + PathNode[--ChoiceNr];
      RunLength -= NodePtr->ConnectLen[Choice[ChoiceNr]];
			continue;
		}

		// --- Part 3 : registering the new Choice
    PathNode[ChoiceNr+1] = NodePtr->ConnectNode[Choice[ChoiceNr]];
    RunLength += NodePtr->ConnectLen[Choice[ChoiceNr]];
    if (PathNode[ChoiceNr+1] == 1)
    {
      // End node reached
			printf("Path complete : RunLength %d\n", RunLength);
      if (RunLength > Max)  Max = RunLength;
			NrOfPaths++;
			// And go back to reconsider this last successful Choice
      RunLength -= NodePtr->ConnectLen[Choice[ChoiceNr]];
    }
    else
    {
      NodePtr = Node + PathNode[ChoiceNr+1];
      NodePtr->Visited = 1;
			// The next step of the path starts with a clean slate
			Choice[++ChoiceNr] = -1;
    }
	} /* forever */
	printf("Found a total of %d paths\n", NrOfPaths);

  /* Finalisation */
  printf("Found Max %d\n", Max);
	return 0;
}
