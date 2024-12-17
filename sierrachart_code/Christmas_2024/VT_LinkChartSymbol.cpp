#include "sierrachart.h"

SCDLLName("VerrilloTrading - Link Chart Symbol")
SCSFExport scsf_LinkChartSymbol(SCStudyInterfaceRef sc)
{
	//
	// -------------------------------------------------------------------------
	int InputIndex = 0;
	SCInputRef i_ChartNumberToMonitor = sc.Input[InputIndex++];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Link Chart Symbol To Target Chart";
		sc.ValueFormat = sc.BaseGraphValueFormat;
		sc.GraphRegion = 0;
		sc.AutoLoop		= 0;
		sc.UpdateAlways = 1;

		sc.StudyDescription = 
			
		"This Advanced Custom Study was written by Christian at VerrilloTrading during December of 2024."	
		"<br><br>"	
		"<strong><u>Study Description:</u></strong>"
		"<br><br>"	
		"This study changes the symbol of the current chart to the symbol of the target chart that is set in the input setting of the study. "
		"<br><br>"	
		"The purpose of this study is to solve an issue when users configure historical and intraday charts within the same chartbook and they want to use chart linking to keep the historical charts linked, and the intraday chrats linked. "
		"<br><br>"
		"If a user runs that type of configuration, it becomes impossible to use chart linking to automaticlly change the symbol of the intraday chart if the historical chart symbol is changed."
		"<br><br>"
		"This study works the same as if the two charts were part of the same chart linking group with the symbol linked. Except that it bypasses chart linking and handles the symbol switching on it's own. "
		"<br><br>"	
		"----"
		"<br>"
		"First it is important to understand how the study works one way. Then Bidirectional Symbol Switching will be explained."
		"<br>"
		"<strong><u>Simple Unidirectional Symbol Switching:</u></strong> "
		"<br><br>"
		"1. The Sierra Chart chartbook contains two charts, Chart #1 and Chart #2."
		"<br>"
		"2. This study is placed on Chart #1."
		"<br>"
		"3. The input setting <strong>Chart Number To Monitor Symbol Change</strong> is set to target Chart #2."
		"<br>"
		"<br>"
		"4. This means that when the symbol on Chart #2 is changed to a new symbol, the symbol on Chart #1 will change to match the symbol on Chart #2."
		"<br>"
		"This is fine except that if the user decides to change the symbol on Chart #1 instead. The symbol of Chart #2 will not automatically change to match the symbol on Chart #1 unless we use the Bidirectional method."
		"<br>"
		"<br>"
		"<strong><u>Bidirectional Chart Switching Configuration:</u></strong>"
		"<br>"
		"<br>"
		"Bidirectional Symbol Switching refers to when the symbol is changed on either Chart #1 or Chart #2. The other chart also changes it's symbol."
		"<br>"
		"Bidirectional Symbol Switching is supported here if this study is placed on both charts that you want to keep the symbols linked. The charts must be set to target eachother using the input setting <strong>Chart Number To Monitor Symbol Change</strong>."
		"<br>"
		"<br>"
		"1. This functionality is enabled by default. All that is required is that the study is placed on both charts that you want to automatically switch when one chart or the other is changed."
		"<br>"
		"<br>"
		"2. In order to do this, the study needs to be placed on both Chart #1 and Chart #2. The input setting <strong>Chart Number To Monitor Symbol Change </strong> for Chart #1 will be set a target chart #2. For Chart #2 it should be set to target Chart #1."
		
		"<br>"
		"<br>"
		"This Advanced Custom Study is open source and free to use."
		"<br>"
		"<br>"
		"For any inquiries or issues please send them to support@verrillotrading.com."
		"<br>"
		"<br>"
		"Kind Regards,"
		"<br>"
		"<br>"
		"-Christian"
		"<br>"
		"<br>"
		"VerrilloTrading, Content Creator, Programmer";

		// Input Configuration
		i_ChartNumberToMonitor.Name = "Chart Number To Monitor Symbol Change";
		i_ChartNumberToMonitor.SetChartNumber(20); // higher number then will set manually 
		i_ChartNumberToMonitor.SetDescription("Set this to the Chart Number that will be monitored for a Symbol change");
	}

	// logging object 
	SCString msg; 

	// remember the target chart symbol and last current symbol 
	SCString& TargetChartSymbolMemory = sc.GetPersistentSCString(0);
	SCString& CurrentChartSymbolMemory = sc.GetPersistentSCString(1);

	// Get current chart Symbol 
	SCString CurrentSymbol = sc.Symbol;

	// SCString CurrentSymbol = sc.GetChartSymbol(sc.ChartNumber);
	
	// Get the symbol of the chart they inputted to monitor 
	SCString TargetChartSymbol = sc.GetChartSymbol(i_ChartNumberToMonitor.GetChartNumber());
	if(TargetChartSymbol != "") // returns true as long as the function was called on a valid chart number 
	{
		// do processing 
		//
		// Initialize persistent variables first call only 
		if(TargetChartSymbolMemory == "")
		{
			TargetChartSymbolMemory = TargetChartSymbol;
			CurrentChartSymbolMemory = CurrentSymbol;
			return; 
		}

		if(TargetChartSymbol == TargetChartSymbolMemory && CurrentSymbol != CurrentChartSymbolMemory)
		{
			// set in memory our new chart symbol
			CurrentChartSymbolMemory = CurrentSymbol;

			if(CurrentSymbol != TargetChartSymbol)
			{
				// Current Symbol was changed if we get here 
				// sc.AddMessageToLog("current chart symbol was changed",1);

				// If we also have this study on the target chart targeting the current chart,
				// The target chart is going to pick up this chart as having it's symbol changed 
				// and it is going to change it's own symbol. 
				//
				// end the function here;
				return;

				// Part of the reason why we decided to handle it this way is because sc
				// does not have an ACSIL function named sc.SetChartSymbol to set the symbol
				// of another chart in the chartbook. 
			}
		}
		else if(TargetChartSymbolMemory != TargetChartSymbol && CurrentSymbol == CurrentChartSymbolMemory)
		{
			// The Target Chart symbol was changed
			//
			// Remember the target chart symbol in memory
			TargetChartSymbolMemory = TargetChartSymbol;

			if(CurrentSymbol != TargetChartSymbol)
			{
				// Get bar period to append the correct file extension
				n_ACSIL::s_BarPeriod r_BarPeriod;
				sc.GetBarPeriodParameters(r_BarPeriod);

				if(r_BarPeriod.ChartDataType == 1)
				{
					// We are set to a daily chart therefore use .dly
					// as file extension
					
					SCString DataFileName = TargetChartSymbol + ".dly";

					// Change the data file (to change the symbol)
					sc.DataFile = DataFileName;

				}
				else if(r_BarPeriod.ChartDataType == 2)
				{
					// We are set to an intraday chart therefore use
					// .scid 

					SCString DataFileName = TargetChartSymbol + ".scid";

					// Change the data file (to change the symbol)
					sc.DataFile = DataFileName;
				}
			}
		}
	}
	else
	{
		// string empty therefore chart does not exist 
		return;
	}
}
