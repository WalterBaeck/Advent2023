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
  long long Position[1000][3];
  int Velocity[1000][3];
  int TokenNr;
  char Text[20];
  int NrOfStones, StoneNr=0, OtherNr;

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
    InputPtr = strchr(InputLine, '@');
    // First handle Velocity part, as it is smaller in stringlength
    strcpy(Text, InputPtr+1);
    for (TokenNr=0; InputPtr=strtok(TokenNr?NULL:Text, ","); TokenNr++)
      if (1 != sscanf(InputPtr, "%d", &Velocity[StoneNr][TokenNr]))
      {
        fprintf(stderr, "Could not scan Velocity in Token[%d]:%s on InputLine #%d\n",
            TokenNr, InputPtr, InputLineNr);
        exit(3);
      }
    // Now cut away the separating '@' character
    InputPtr = strchr(InputLine, '@');
    *InputPtr = '\0';
    for (TokenNr=0; InputPtr=strtok(TokenNr?NULL:InputLine, ","); TokenNr++)
      if (1 != sscanf(InputPtr, "%lld", &Position[StoneNr][TokenNr]))
      {
        fprintf(stderr, "Could not scan Position in Token[%d]:%s on InputLine #%d\n",
            TokenNr, InputPtr, InputLineNr);
        exit(3);
      }
    StoneNr++;
  } /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);
  NrOfStones = StoneNr;

  /* Operation */
  int Sum=0;
  long long Min=200000000000000, Max=400000000000000;
  if (InputLineNr < 10) { Min=7; Max=27; }
  for (StoneNr=0; StoneNr<NrOfStones; StoneNr++)
    for (OtherNr=StoneNr+1; OtherNr<NrOfStones; OtherNr++)
    {
      // To find the linear equation y=a.x+b with given x0,y0 and dx,dy :
      // slope a=dy/dx and offset b=y0-a.x0
      double Slope0,Slope1, Offset0,Offset1;
      Slope0 = ((double)Velocity[StoneNr][1]) / Velocity[StoneNr][0];
      Slope1 = ((double)Velocity[OtherNr][1]) / Velocity[OtherNr][0];
      Offset0 = Position[StoneNr][1] - Slope0 * Position[StoneNr][0];
      Offset1 = Position[OtherNr][1] - Slope1 * Position[OtherNr][0];
      // To find the intersecting x coordinate of two linear equations
      //    y=a1.x+b1
      //    y=a2.x+b2     b1-b2
      // compute :     x=-------
      //                  a2-a1
      if (Slope0 == Slope1)
      {
        // Avoid division by zero
        printf("Stones #%d and #%d have parallel paths, will not intersect\n", StoneNr, OtherNr);
        continue;
      }
      double Xsect = (Offset0 - Offset1) / (Slope1 - Slope0);
      // And find the intersecting y coordinate by filling this back into either linear equation
      double Ysect = Slope0 * Xsect + Offset0;

#if 0
      // Debugging
      printf("**Stone[%d] Pos[%lld,%lld],Vel[%d,%d] and [%d] Pos[%lld,%lld],Vel[%d,%d]\n",
          StoneNr,
          Position[StoneNr][0], Position[StoneNr][1], Velocity[StoneNr][0], Velocity[StoneNr][1],
          OtherNr,
          Position[OtherNr][0], Position[OtherNr][1], Velocity[OtherNr][0], Velocity[OtherNr][1]);
      printf("  Slope0=%.2f Offset0=%.2f    Slope1=%.2f Offset1=%.2f\n",
          Slope0, Offset0, Slope1, Offset1);
#endif


      // Is the intersection point in the past or the future ?
      if (((Xsect - Position[StoneNr][0]) * Velocity[StoneNr][0]) < 0)
      {
        printf("Stones #%d and #%d intersect in the past for #%d\n", StoneNr, OtherNr, StoneNr);
        continue;
      }
      // This needs to be verified for BOTH equations
      if (((Xsect - Position[OtherNr][0]) * Velocity[OtherNr][0]) < 0)
      {
        printf("Stones #%d and #%d intersect in the past for #%d\n", StoneNr, OtherNr, OtherNr);
        continue;
      }

      // Finally, the intersection needs to be within the prescribed boundaries
      if ((Xsect < Min) || (Xsect > Max))
      {
        printf("Intersection [x=%.2f,y=%.2f] violates X boundaries\n", Xsect, Ysect);
        continue;
      }
      if ((Ysect < Min) || (Ysect > Max))
      {
        printf("Intersection [x=%.2f,y=%.2f] violates Y boundaries\n", Xsect, Ysect);
        continue;
      }

      // OK, all conditions have been fulfilled
      printf("OK Stones #%d and #%d   Intersection [x=%.2f,y=%.2f]\n", StoneNr, OtherNr, Xsect, Ysect);
      Sum++;
    } /* for (OtherNr) */

  /* Finalisation */
  printf("Found Sum %d\n", Sum);
	return 0;
}
