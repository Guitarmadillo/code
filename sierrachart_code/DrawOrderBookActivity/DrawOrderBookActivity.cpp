#include "sierrachart.h"

SCDLLName("Draw Real-Time OrderBook Activity Example")
SCSFExport scsf_DrawOrderBookActivity(SCStudyInterfaceRef sc)
{
	// Study inputs 
	int InputIndex = 0;
	SCInputRef Input_ResetValueACSButtonNumber = sc.Input[InputIndex++];

	/* SCSubgraphRef s_Display = sc.Subgraph[0]; */ 

	SCSubgraphRef Subgraph_Open = sc.Subgraph[0];
	SCSubgraphRef Subgraph_High = sc.Subgraph[1];
	SCSubgraphRef Subgraph_Low = sc.Subgraph[2];
	SCSubgraphRef Subgraph_Close = sc.Subgraph[3];

	SCString msg;


	if(sc.SetDefaults)
	{
		sc.GraphName = "Draw Order Book Activity Example";

		sc.UsesMarketDepthData = 1; 
		/* sc.MaintainHistoricalMarketDepthData = 1; // for historical data (not used in this example) */
		sc.MaintainAdditionalChartDataArrays = 1; // needed for getting accurate bar end times 
												  // intraday storage time unit
												  // must be 1 second or 1 tick
												  //
		sc.ValueFormat = 0; // simple integer no decimal places since this study is a counter 
		/* 0 = Zero decimal places. Example output: 1 */
		/* 1 = One decimal place. Example output: 0.1 */
		/* 2 = Two decimal places. Example output: 0.01 */

		sc.GraphRegion = 1; // For Testing 

		sc.GraphDrawType = GDT_CANDLESTICK; // necessary for display as bars 
											
		sc.AutoLoop = 0; // we will do the looping since it is working with real-time data 

		// subgraph settings 

		Subgraph_Open.Name = "Open";
		Subgraph_Open.PrimaryColor = RGB(0,255,0);
		Subgraph_Open.SecondaryColorUsed = 1;  
		Subgraph_Open.SecondaryColor = RGB(0,127,0);
		Subgraph_Open.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_Open.LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
		Subgraph_Open.DrawZeros = true;

		Subgraph_High.Name = "High";
		Subgraph_High.PrimaryColor = RGB(0,255,0);
		Subgraph_High.SecondaryColorUsed = 1;  
		Subgraph_High.SecondaryColor = RGB(255,0,0);
		Subgraph_High.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_High.LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
		Subgraph_High.DrawZeros = true;

		Subgraph_Low.Name  = "Low";
		Subgraph_Low.PrimaryColor = RGB(255,0,0);
		Subgraph_Low.SecondaryColorUsed = 1;  
		Subgraph_Low.SecondaryColor = RGB(255,0,0);
		Subgraph_Low.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_Low.LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
		Subgraph_Low.DrawZeros = true;

		Subgraph_Close.Name = "Close";
		Subgraph_Close.PrimaryColor = RGB(255,0,0);
		Subgraph_Close.SecondaryColorUsed = 1;  
		Subgraph_Close.SecondaryColor = RGB(0,255,0);
		Subgraph_Close.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_Close.LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
		Subgraph_Close.DrawZeros = true;

		// Use the labels color 
		Subgraph_Close.UseLabelsColor = 1;
		Subgraph_Close.LabelsColor = RGB(255,255,0);

		// input settings 

		Input_ResetValueACSButtonNumber.Name = "Reset Value ACS Control Bar Button #";
		Input_ResetValueACSButtonNumber.SetInt(150);
		Input_ResetValueACSButtonNumber.SetIntLimits(1, MAX_ACS_CONTROL_BAR_BUTTONS);
	}


	// declare arrays to store the data in memory between calls to the function 
    float *p_bidQuantities = (float *)sc.GetPersistentPointer(0);
    float *p_askQuantities = (float *)sc.GetPersistentPointer(1); 

	// Used for Real-Time Updates 
	int& OrderBookEventCounter = sc.GetPersistentIntFast(2);

	if(sc.LastCallToFunction)
	{
		// reset this as precaution
		OrderBookEventCounter = 0;

		// FREE MEMORY ON LAST CALL TO FUNCTION 
		if(p_bidQuantities != 0)
		{
			sc.FreeMemory( p_bidQuantities);
			sc.SetPersistentPointer(0, NULL);
		}
		if(p_askQuantities != 0)
		{
			sc.FreeMemory( p_askQuantities);
			sc.SetPersistentPointer(1, NULL);
		}

		return; // must return the function here 
	}

	// CODE FOR GRABBING MARKET DEPTH DATA 
	// THIS VARIABLE REPRESENTS THE DEPTH LEVELS ON THE CHART WHICH IS CONTROLLED BY 
	// CHART SETTINGS -> MARKET DEPTH > MAXIMUM NUMBER OF MARKET DEPTH LEVELS
	// IF 0 is set, it defaults to what is being obtained from the Server 
	int MaximumMarketDepthLevels = sc.GetMaximumMarketDepthLevels();

	// debug max depth levels 
	/* msg.Format("Max Mkt Depth Levels: %d", MaximumMarketDepthLevels); */
	/* sc.AddMessageToLog(msg,1); */

	// some precaution (that I don't fully understand) it was in an SC example 
	if (MaximumMarketDepthLevels > sc.ArraySize)
	{
		MaximumMarketDepthLevels = sc.ArraySize;
	}

	//For safety: bomb out until data is done being downloaded 
	if (sc.ChartIsDownloadingHistoricalData(sc.ChartNumber))
	{
		return;
	}

	// ACS BUTTON (may deprecate) 
	// Handle ACS Button events 
	if(sc.MenuEventID != 0)
	{
		// User pressed the reset button
		if(sc.MenuEventID == Input_ResetValueACSButtonNumber.GetInt())
		{
			const int ButtonState = (sc.PointerEventType == SC_ACS_BUTTON_ON) ? 1 : 0;
			if (ButtonState == 1)	
			
			// reset the button
			sc.SetCustomStudyControlBarButtonEnable(Input_ResetValueACSButtonNumber.GetInt(), 0);

			// reset the real-time counter 
			OrderBookEventCounter = 0;

			// clear subgraphs at every index (for example only)
			for(int BarIndex = 0; BarIndex < sc.ArraySize; BarIndex++)
			{
				Subgraph_Open[BarIndex] = 0;
				Subgraph_High[BarIndex] = 0;
				Subgraph_Low[BarIndex] = 0;
				Subgraph_Close[BarIndex] = 0;
			}
		}
	}

	// REALTIME CODE STARTS HERE 
	// ALLOCATE MEMORY FOR PERSISTENT ARRAYS 
    if (p_bidQuantities == NULL) 
	{
        p_bidQuantities = (float *) sc.AllocateMemory( 1024 * sizeof(float) );
        sc.SetPersistentPointer(0, p_bidQuantities);
    }
    if (p_askQuantities == NULL) 
	{
        p_askQuantities = (float *) sc.AllocateMemory( 1024 * sizeof(float) );
        sc.SetPersistentPointer(1, p_askQuantities);
    }

	// seeing what these values are to start 
	/* msg.Format("Debug bid: %d Ask: %d", p_bidQuantities[0], p_askQuantities[0]); */
	/* sc.AddMessageToLog(msg,1); */

	// Get number of levels to compute 
	int BidArrayLevels = min(MaximumMarketDepthLevels, sc.GetBidMarketDepthNumberOfLevels());
	int AskArrayLevels = min(MaximumMarketDepthLevels, sc.GetAskMarketDepthNumberOfLevels());

	// Initial Call to the function, different behaviour than the subsequent calls 
	// THIS WAS MOSTLY FOR DEBUGGING 
	if(p_bidQuantities[0] == 0 && p_askQuantities[0] == 0)
	{
		// GET BID MARKET DATA 
		for (int Level = 0; Level < BidArrayLevels; Level++)
		{
			// depth object
			s_MarketDepthEntry DepthEntry;
			sc.GetBidMarketDepthEntryAtLevel(DepthEntry, Level);
			
			// save quantities to our array
			p_bidQuantities[Level] = DepthEntry.Quantity;

			/* sc.AddMessageToLog("BID DATA SAVED!",1); */
		}

		// GET ASK MARKET DATA 
		for (int Level = 0; Level < AskArrayLevels; Level++)
		{
			s_MarketDepthEntry DepthEntry;
			sc.GetAskMarketDepthEntryAtLevel(DepthEntry, Level);

			// save quantities to our array
			p_askQuantities[Level] = DepthEntry.Quantity;

			/* sc.AddMessageToLog("ASK DATA SAVED!",1); */
		}

		// debug: this should only print one time 
		/* sc.AddMessageToLog("we get here" , 1); */
	}
	else // subsequent calls 
	{
		/* sc.AddMessageToLog("subsequent calls we get here",1); */
		// GET BID MARKET DATA
		for (int Level = 0; Level < BidArrayLevels; Level++)
		{
			// depth object
			s_MarketDepthEntry DepthEntry;
			sc.GetBidMarketDepthEntryAtLevel(DepthEntry, Level);
			
			/* msg.Format("debug bid array level: %f current quantity: %f", p_bidQuantities[Level], */ 
			/* DepthEntry.Quantity); */
			/* sc.AddMessageToLog(msg,1); */

			// check quantity at price vs was was previously saved at this price 
			// Note: Type must be the same (float) for check to work 
			if(DepthEntry.Quantity != p_bidQuantities[Level])
			{
				// there has been a change at this market depth level 
				//
				// append to our counter 
				OrderBookEventCounter++;
				/* OrderBookEventCounter += 1; */
				//
				// overwrite with the new quantity in our array 
				p_bidQuantities[Level] = DepthEntry.Quantity;
			}
		}

		// GET ASK MARKET DATA 
		for (int Level = 0; Level < AskArrayLevels; Level++)
		{
			s_MarketDepthEntry DepthEntry;
			sc.GetAskMarketDepthEntryAtLevel(DepthEntry, Level);

			/* msg.Format("debug ask array level: %f current quantity: %f", p_askQuantities[Level], */ 
			/* DepthEntry.Quantity); */
			/* sc.AddMessageToLog(msg,1); */

			// check quantity at price vs was was previously saved at this price 
			// Note: Type must be the same (float) for check to work 
			if(DepthEntry.Quantity != p_askQuantities[Level])
			{
				// there has been a change at this market depth level 
				//
				// subtract from our counter 
				OrderBookEventCounter--;
				
				// overwrite with the new quantity in our array 
				p_askQuantities[Level] = DepthEntry.Quantity;
			}
		}

		// debug output value 
		/* msg.Format("Value: %d", OrderBookEventCounter); */
		/* sc.AddMessageToLog(msg,1); */

		// Realtime Only: Write our value into subgraph arrays
		Subgraph_Close[sc.ArraySize-1] = OrderBookEventCounter;

		// LOGIC TO PRINT CORRECT HIGH AND LOW VALUES 
		// Compute Open High Low Close Values 
		if(Subgraph_High[sc.ArraySize-1] < OrderBookEventCounter || Subgraph_High[sc.ArraySize-1] == 0)
		{
			Subgraph_High[sc.ArraySize-1] = OrderBookEventCounter;
		}
		if(Subgraph_Low[sc.ArraySize-1] > OrderBookEventCounter || Subgraph_Low[sc.ArraySize-1] == 0)
		{
			Subgraph_Low[sc.ArraySize-1] = OrderBookEventCounter;
		}

		// Set the bar open to the previous bar's close 
		if(Subgraph_Close[sc.ArraySize-2] != 0)
		{
			Subgraph_Open[sc.ArraySize-1] = Subgraph_Close[sc.ArraySize-2];
		}
	}

	// Activity on the bid makes our output number go positive
	// Activie on the offer makes our output number go negative. 
	//
	// The goal of this study is to observe if activity on the bid/offer correlates
	// to any consistent behaviour, also relative to methods we already use and
	// understand
	//
}
