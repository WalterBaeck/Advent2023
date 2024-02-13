#include <stdio.h>
#include <stdlib.h>

int main(int Arguments, char* Argument[])
{
	int Choice[2]={-1};
	int ChoiceNr=0, NrOfChoices=2;
	int NrOfPaths=0;

	for (;;)
	{
		// --- Part 1 : making a new Choice at the current ChoiceNr ---
		// Always try to increase the Choice at the current ChoiceNr
		int MaxChoice = 10;  // Default
		// Exceptions to this default : not going beyond 26 possibilities
		if (ChoiceNr && (Choice[0]==2))
			MaxChoice = 6;
		else if (!ChoiceNr)
			MaxChoice = 3;
		for (++Choice[ChoiceNr]; Choice[ChoiceNr]<MaxChoice; Choice[ChoiceNr]++)
		{
			// Any additional constraints should be verified here
			// In case of success :
			break;
		}

		// --- Part 2 : consider backtracking
		if (Choice[ChoiceNr] >= MaxChoice)
		{
			// Backtracking indeed needed
			if (!ChoiceNr)  break;
			// Undo any consequences of the last Choice here
			// And then :
			ChoiceNr--;
			continue;
		}

		// --- Part 3 : registering the new Choice
		if (++ChoiceNr >= NrOfChoices)
		{
			// Report this complete chain of Choices
			printf("Path complete : %d%d\n", Choice[0], Choice[1]);
			NrOfPaths++;
			// And go back to reconsider this last successful Choice
			ChoiceNr--;
		}
		else
			// The next step of the path starts with a clean slate
			Choice[ChoiceNr] = -1;
	} /* forever */

	printf("Found a total of %d paths\n", NrOfPaths);
	return 0;
}
