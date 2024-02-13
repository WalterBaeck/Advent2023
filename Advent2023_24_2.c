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
  int Dir;
  for (Dir=0; Dir<3; Dir++)
  {
    printf("===== Axis[%d] ======\n", Dir);
    // Find timelines moving parallel for this coordinate
    for (StoneNr=0; StoneNr<NrOfStones; StoneNr++)
      for (OtherNr=StoneNr+1; OtherNr<NrOfStones; OtherNr++)
        if (Velocity[StoneNr][Dir] == Velocity[OtherNr][Dir])
        {
          long long Diff = Position[StoneNr][Dir] - Position[OtherNr][Dir];
          if (Diff < 0)  Diff = -Diff;
//        printf("[%d] and [%d] moving at same speed, diff is %lld\n", StoneNr, OtherNr, Diff);
        }
  }

  // This study shows that stones 140 and 191 have a parallel timeline in Y *and* Z coordinates.
  // A large common factor emerges in the distances between them :
  long long Large = (Position[140][1] - Position[191][1])/265;
  printf("---------------------------------------------\n");
  printf("Y-diff [140]<->[191] = %lld = %lld = 265 * %lld\n",
      Position[140][1] - Position[191][1], 265 * Large, Large);
  printf("Z-diff [140]<->[191] = %lld = %lld = 83 * %lld\n",
      Position[140][2] - Position[191][2], 83 * Large, Large);
  printf("---------------------------------------------\n");
  // Therefore, the Rock will travel during Large timesteps,
  // and bridge the gap between both stones by Y=265,Z=83 in every timestep.
  //
  // This allows to focus in the X timeline of both Stones ; these are not parallel.
  // The rock will also take Large timesteps to travel between these X timelines.
  // Unlike the parallel Y,Z timelines, the gap between the X timelines will become
  // smaller, then intersect, and eventually become larger again.
  printf("[191] has X=%lld at t=0, Large later [140] will have x=%lld, difference %lld\n",
      Position[191][0], Position[140][0]+Velocity[140][0]*Large,
      Position[191][0] - (Position[140][0]+Velocity[140][0]*Large));
  printf("Crossing this gap in Large timesteps requires %.2f per timestep\n",
      (Position[191][0] - (Position[140][0]+Velocity[140][0]*Large)) / (double)Large);
  printf("+++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("Different situation at t=1e11 :\n");
  printf("[191] has X=%lld, Large later [140] will have x=%lld, difference %lld\n",
      Position[191][0]+Velocity[191][0]*(long long)1e11,
      Position[140][0]+Velocity[140][0]*((long long)1e11 + Large),
      Position[191][0]+Velocity[191][0]*(long long)1e11 -
      (Position[140][0]+Velocity[140][0]*((long long)1e11 + Large)));
  printf("Crossing this gap in Large timesteps requires %.2f per timestep\n",
      (Position[191][0]+Velocity[191][0]*(long long)1e11 -
          (Position[140][0]+Velocity[140][0]*((long long)1e11 + Large))) / (double)Large);
  printf("+++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("Yet another situation at t=2e11 :\n");
  printf("[191] has X=%lld, Large later [140] will have x=%lld, difference %lld\n",
      Position[191][0]+Velocity[191][0]*(long long)2e11,
      Position[140][0]+Velocity[140][0]*((long long)2e11 + Large),
      Position[191][0]+Velocity[191][0]*(long long)2e11 -
      (Position[140][0]+Velocity[140][0]*((long long)2e11 + Large)));
  printf("Crossing this gap in Large timesteps requires %.2f per timestep\n",
      (Position[191][0]+Velocity[191][0]*(long long)2e11 -
          (Position[140][0]+Velocity[140][0]*((long long)2e11 + Large))) / (double)Large);

  // This leads the way to a solution !
  printf("+++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("Gap per each of Large timesteps, between [191].X at t and [140].X at t+Large:\n");
  printf("%.2f - (t/Large)*%d\n",
      (Position[191][0] - Position[140][0])/(double)Large - Velocity[140][0],
      Velocity[140][0] - Velocity[191][0]);
  // Solve this equation at 500 outcomes, backwards to t
  for (int Outcome=99; Outcome>=-400; Outcome--)
  {
    long long t = (Position[191][0] - Position[140][0] - Large * (Velocity[140][0] + Outcome)) /
      (Velocity[140][0] - Velocity[191][0]);
    if (Position[140][0]+Velocity[140][0]*(t + Large) == 
        Position[191][0]+Velocity[191][0]*t - Outcome*Large)
    {
      printf("=============================================\n");
      printf("At t=%lld [191].X=%lld ; At t+Large=%lld [140].X=%lld\n", 
          t, Position[191][0]+Velocity[191][0]*t,
          t + Large, Position[140][0]+Velocity[140][0]*(t + Large));
      printf("  Diff=%lld or %.2f*Large ; ",
          Position[191][0]+Velocity[191][0]*t - (Position[140][0]+Velocity[140][0]*(t + Large)),
          (Position[191][0]+Velocity[191][0]*t - (Position[140][0]+Velocity[140][0]*(t + Large))) /
          (double)Large);
      printf("Rock.X at t+Large=%lld misses by %lld\n",
          Position[191][0]+Velocity[191][0]*t - Outcome*Large,
          Position[140][0]+Velocity[140][0]*(t + Large) - 
          (Position[191][0]+Velocity[191][0]*t - Outcome*Large));
      // Successfull integer bridge between [140] and [191]
      long long RockPos[3];
      int RockVel[3];
      RockPos[0] = Position[191][0]+(Velocity[191][0]+Outcome)*t;
      RockPos[1] = Position[191][1]-265*t;
      RockPos[2] = Position[191][2]-83*t;
      RockVel[0] = -Outcome;
      RockVel[1] = 265 + Velocity[191][1];
      RockVel[2] =  83 + Velocity[191][2];
      printf("Rock at t=0 [ %lld %lld %lld ] velocity [ %d %d %d ]\n",
          RockPos[0], RockPos[1], RockPos[2], RockVel[0], RockVel[1], RockVel[2]);
      // Doublecheck
      printf("  At t=%lld [191] is at [ %lld %lld %lld ]\n  while Rock at [ %lld %lld %lld ]\n",
          t, Position[191][0]+Velocity[191][0]*t, Position[191][1]+Velocity[191][1]*t,
          Position[191][2]+Velocity[191][2]*t,
          RockPos[0]+RockVel[0]*t, RockPos[1]+RockVel[1]*t, RockPos[2]+RockVel[2]*t);
      printf("  At t+Large=%lld [140] is at [ %lld %lld %lld ]\n  while Rock at [ %lld %lld %lld ]\n",
          t+Large, Position[140][0]+Velocity[140][0]*(t+Large),
          Position[140][1]+Velocity[140][1]*(t+Large), Position[140][2]+Velocity[140][2]*(t+Large),
          RockPos[0]+RockVel[0]*(t+Large), RockPos[1]+RockVel[1]*(t+Large),
          RockPos[2]+RockVel[2]*(t+Large));

      // Does this solution also intersect the path of the Zero stone ?
      long long ZeroTime = (Position[0][0] - RockPos[0]) / (RockVel[0] - Velocity[0][0]);
      if (Position[0][1] + ZeroTime*Velocity[0][1] != RockPos[1] + ZeroTime*RockVel[1])
        continue;
      if (Position[0][2] + ZeroTime*Velocity[0][2] != RockPos[2] + ZeroTime*RockVel[2])
        continue;
      printf("*********************************\n");
      printf("*  Sum  %lld  *\n",
          RockPos[0] + RockPos[1] + RockPos[2]);
      printf("*********************************\n");
    }
  } /* for (Outcome) */

  /* Finalisation */
	return 0;
}
