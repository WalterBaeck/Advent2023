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
	// Element variables
	typedef enum {eFlipFlop, eNandGate, eBroadcast} tType;
	char TypeName[3][12]={"FlipFlop", "NandGate", "Broadcast"};
	typedef enum {eLow, eHigh} tPulse;
	typedef struct {
		char Name[12];
		tType Type;
		tPulse Receive[100];
		int From[100];  // Note for each incoming Pulse, where it came from
		int NrOfReceives, NrOfTransmits;
		tPulse Transmit[100];
		// Fanin
		int Input[10];
		int NrOfInputs;
		// Fanout
		char TargetName[10][12];
		int Target[10];
		int NrOfTargets;
		// Support for FlipFlop
		tPulse State;
		// Support for NandGate
		tPulse LastInput[10];
	} tElement;
	tElement Element[100],*ElementPtr;
	int ElementNr, NrOfElements=0, StartElement;

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
		if (!(InputPtr = strstr(InputLine, " ->")))
		{
			fprintf(stderr, "Could not find \' ->\' on InputLine #%d\n", InputLineNr);
			exit(3);
		}
		// Get name and type
		if (!strncmp(InputLine, "broadcaster", 11))
		{
			Element[NrOfElements].Type = eBroadcast;
			StartElement = NrOfElements;
			strcpy(Element[NrOfElements].Name, "broadcaster");
		}
		else
		{
			strncpy(Element[NrOfElements].Name, InputLine+1, InputPtr-InputLine-1);
			Element[NrOfElements].Name[InputPtr-InputLine-1] = '\0';
			if (*InputLine == '%')       Element[NrOfElements].Type = eFlipFlop;
			else if (*InputLine == '&')  Element[NrOfElements].Type = eNandGate;
			else
			{
				fprintf(stderr, "Could not find Type on InputLine #%d\n", InputLineNr);
				exit(3);
			}
		}
		// Get the Target list
		Element[NrOfElements].NrOfTargets = 0;
		for (TokenNr=0; InputPtr = strtok(TokenNr?NULL:InputPtr+3, ","); TokenNr++)
		{
			if (*InputPtr != ' ')
			{
				fprintf(stderr, "Token[%d]:%s on InputLine #%d does not start with space ?\n",
						TokenNr, InputPtr, InputLineNr);
				exit(3);
			}
			strcpy(Element[NrOfElements].TargetName[Element[NrOfElements].NrOfTargets], InputPtr+1);
			Element[NrOfElements].NrOfTargets++;
		}
		// Element parsing is now complete
		Element[NrOfElements].NrOfInputs = 0;
		NrOfElements++;
  } /* while (fgets) */
	printf("%d InputLines were read.\n", InputLineNr);

	// Re-parse TargetNames
	int OtherElement,InputNr,TargetNr;
	int ReceiveNr, TransmitNr;
	int HighFanin[10];
	int HighFaninNr,NrOfHighFanin=0;
	int* HighFaninLoop[10];
	int HighFaninPeriod[10];
	int HighFaninCount[10];
	for (ElementNr=0,ElementPtr=Element; ElementNr<NrOfElements; ElementNr++,ElementPtr++)
		for (TargetNr=0; TargetNr<ElementPtr->NrOfTargets; TargetNr++)
		{
			for (OtherElement=0; OtherElement<NrOfElements; OtherElement++)
				if (!strcmp(Element[OtherElement].Name, ElementPtr->TargetName[TargetNr]))
				{
					// Put the OtherElement in the Fanout list
					ElementPtr->Target[TargetNr] = OtherElement;
					// Also update the OtherElement's Fanin
					Element[OtherElement].Input[Element[OtherElement].NrOfInputs] = ElementNr;
					Element[OtherElement].NrOfInputs++;
					if (Element[OtherElement].NrOfInputs > 10)
					{
						fprintf(stderr, "Element[%d]:%s has Fanin>10\n", ElementNr, ElementPtr->Name);
						exit(3);
					}
					break;
				}
			if (OtherElement >= NrOfElements)
			{
				printf("Target[%d]:%s of Element[%d]:%s was not found in the list\n",
						TargetNr, ElementPtr->TargetName[TargetNr], ElementNr, ElementPtr->Name);
				ElementPtr->Target[TargetNr] = -1;
			}
		} /* for (TargetNr) */
	for (ElementNr=0,ElementPtr=Element; ElementNr<NrOfElements; ElementNr++,ElementPtr++)
		if (ElementPtr->NrOfInputs > 4)
		{
			printf("High Fanin detected (%d) on Element[%d]:%s Type %s\n",
					ElementPtr->NrOfInputs, ElementNr, ElementPtr->Name, TypeName[ElementPtr->Type]);
			HighFanin[NrOfHighFanin] = ElementNr;
			HighFaninLoop[NrOfHighFanin] = (int*)malloc(1e6 * sizeof(int));
			HighFaninPeriod[NrOfHighFanin] = 0;
			HighFaninCount[NrOfHighFanin] = 0;
			NrOfHighFanin++;
		}

	// Set initial State
	for (ElementNr=0,ElementPtr=Element; ElementNr<NrOfElements; ElementNr++,ElementPtr++)
	{
		ElementPtr->NrOfReceives = 0;
		ElementPtr->NrOfTransmits = 0;
		if (ElementPtr->Type == eFlipFlop)
			ElementPtr->State = eLow;
		for (InputNr=0; InputNr<ElementPtr->NrOfInputs; InputNr++)
			ElementPtr->LastInput[InputNr] = eLow;
	}

	/* Operation */
	unsigned long long CycleNr=0;
	int NrOfLowTransmissions=0,NrOfHighTransmissions=0;
	for (CycleNr=0; ; CycleNr++)
	{
//		printf("=============   Cycle %llu   =============\n", CycleNr+1);
		// Every Cycle starts with the Broadcast element sending a Low Pulse
		ElementPtr = Element + StartElement;
		ElementPtr->Transmit[0] = eLow;
		ElementPtr->NrOfTransmits = 1;
		NrOfLowTransmissions++;
		// Keep performing Clocks until no further Pulses are in any Transmit queue
		for (;;)
		{
//			printf(" - - -    Clock    - - -\n");
			int NrOfTransmissions=0;
			for (ElementNr=0,ElementPtr=Element; ElementNr<NrOfElements; ElementNr++,ElementPtr++)
			{
				// 1a. Process all the Transmit queues into the Receive queues of their Target Elements
				for (TransmitNr=0; TransmitNr<ElementPtr->NrOfTransmits; TransmitNr++)
				{
					NrOfTransmissions++;
					for (TargetNr=0; TargetNr<ElementPtr->NrOfTargets; TargetNr++)
					{
						if (ElementPtr->Transmit[TransmitNr] == eLow)
							NrOfLowTransmissions++;
						else
							NrOfHighTransmissions++;
						// Is this actually a known Target ?
						if ((OtherElement = ElementPtr->Target[TargetNr]) >= 0)
						{
//printf("1a.El[%d]:%s transmits %d to Other[%d]:%s\n",
//		ElementNr, ElementPtr->Name, ElementPtr->Transmit[TransmitNr], OtherElement, Element[OtherElement].Name);
							Element[OtherElement].Receive[Element[OtherElement].NrOfReceives] = ElementPtr->Transmit[TransmitNr];
							Element[OtherElement].From[Element[OtherElement].NrOfReceives] = ElementNr;
							Element[OtherElement].NrOfReceives++;
						}
//					else
//printf("1a.El[%d]:%s transmits %d into the unknown\n",  ElementNr, ElementPtr->Name, ElementPtr->Transmit[TransmitNr]);
					}
				}
				// 1b. Clear all Transmit queues
				ElementPtr->NrOfTransmits = 0;
			} /* for (ElementNr) */
			if (!NrOfTransmissions)  break;  // No more Clocks needed
			// Now perform the logical operation of each Element
			for (ElementNr=0,ElementPtr=Element; ElementNr<NrOfElements; ElementNr++,ElementPtr++)
			{
				// 2a. Process all the Receive queues
				switch (ElementPtr->Type)
				{
					case eFlipFlop:
						for (ReceiveNr=0; ReceiveNr<ElementPtr->NrOfReceives; ReceiveNr++)
							// Only Low incoming pulses trigger any action from a FlipFlop,
							//   regardless of who the sender was
							if (ElementPtr->Receive[ReceiveNr] == eLow)
							{
								ElementPtr->State = (1 - ElementPtr->State);
//printf("2a.FF[%d]:%s receives 0 from El[%d]:%s, flips to %d and sends it\n",
//		ElementNr, ElementPtr->Name, ElementPtr->From[ReceiveNr], Element[ElementPtr->From[ReceiveNr]].Name,
//		ElementPtr->State);
								ElementPtr->Transmit[ElementPtr->NrOfTransmits] = ElementPtr->State;
								ElementPtr->NrOfTransmits++;
							}
						break;
					case eNandGate:
						for (ReceiveNr=0; ReceiveNr<ElementPtr->NrOfReceives; ReceiveNr++)
						{
							// Match the source of this Pulse with an Input from the Fanin list
							for (InputNr=0; InputNr<ElementPtr->NrOfInputs; InputNr++)
								if (ElementPtr->Input[InputNr] == ElementPtr->From[ReceiveNr])
									break;
							// Update the LastInput for this Fanin item
							ElementPtr->LastInput[InputNr] = ElementPtr->Receive[ReceiveNr];
							// Now consider all LastInputs for any Low
							for (InputNr=0; InputNr<ElementPtr->NrOfInputs; InputNr++)
								if (ElementPtr->LastInput[InputNr] == eLow)  break;
							tPulse Result = eLow;
							if (InputNr < ElementPtr->NrOfInputs)  Result=eHigh;
							ElementPtr->Transmit[ElementPtr->NrOfTransmits] = Result;
//printf("2a.Nand[%d]:%s receives %d from El[%d]:%s on its Input #%d (of %d) and sends %d\n",
//		ElementNr, ElementPtr->Name, ElementPtr->Receive[ReceiveNr],
//		ElementPtr->From[ReceiveNr], Element[ElementPtr->From[ReceiveNr]].Name,
//		InputNr, ElementPtr->NrOfInputs, ElementPtr->Transmit[ElementPtr->NrOfTransmits]);
              ElementPtr->NrOfTransmits++;

							// Monitor the HighHanin Elements
							for (HighFaninNr=0; HighFaninNr<NrOfHighFanin; HighFaninNr++)
								if (ElementNr == HighFanin[HighFaninNr])  break;
							if (HighFaninNr<NrOfHighFanin)
							{
								HighFaninLoop[HighFaninNr][HighFaninCount[HighFaninNr]++] = Result;

#if 1
								if (Result == eLow)
								{
									printf("Low seen from HighFanin El[%d]:%s after %d pulses in Cycle %d\n",
											ElementNr, ElementPtr->Name, HighFaninCount[HighFaninNr], CycleNr+1);
									HighFaninPeriod[HighFaninNr] = CycleNr+1;
								}
#endif
#if 1
								int CycleIndex, LowSeen=0;
								for (CycleIndex=0; CycleIndex<(HighFaninCount[HighFaninNr]/2); CycleIndex++)
								{
									if (HighFaninLoop[HighFaninNr][CycleIndex] !=
											HighFaninLoop[HighFaninNr][CycleIndex+(HighFaninCount[HighFaninNr]/2)])  break;
									if (HighFaninLoop[HighFaninNr][CycleIndex] == eLow)  LowSeen=1;
								}
								if ((CycleIndex >= (HighFaninCount[HighFaninNr]/2)) && (LowSeen) && (!HighFaninPeriod[HighFaninNr]))
								{
									printf("Loop %d detected on HighFanin El[%d]:%s in Cycle %d\n",
											HighFaninCount[HighFaninNr]/2, HighFanin[HighFaninNr], ElementPtr->Name, CycleNr+1);
									//HighFaninPeriod[HighFaninNr] = HighFaninCount[HighFaninNr]/2;
#endif
#if 0
					for (CycleIndex=0; CycleIndex<8192; CycleIndex++)
					{
						putchar(HighFaninLoop[HighFaninNr][CycleIndex]==eLow?' ':'.');
						if ((CycleIndex&0x3F)==0x3F)
							putchar('\n');
						if (CycleIndex==4095)
							printf("___________________________\n");
					}
#endif
#if 1
								}
#endif
							}
						}
						break;
				} /* switch (Type) */
				// 2b. Clear all Receive queues
				ElementPtr->NrOfReceives = 0;
			} /* for (ElementNr) */
		} /* forever Clocks */

		for (HighFaninNr=0; HighFaninNr<NrOfHighFanin; HighFaninNr++)
			if (!HighFaninPeriod[HighFaninNr])  break;
		if (HighFaninNr >= NrOfHighFanin)  break;

		//if (CycleNr > 20000)  break;
	}	/* for (CycleNr) */

	/* Finalisation */
	unsigned long long Product = 1;
	for (HighFaninNr=0; HighFaninNr<NrOfHighFanin; HighFaninNr++)
		Product *= HighFaninPeriod[HighFaninNr];
	printf("Product of %d loops is %llu\n", NrOfHighFanin, Product);
	return 0;
}
