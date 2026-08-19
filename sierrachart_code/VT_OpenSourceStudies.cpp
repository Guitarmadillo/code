#include <boost/format.hpp>
#include <curl/curl.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <thread>
#include "json.hpp" // convenience 
					//
#include "VT_SCHelpers.h"
#include "sierrachart.h"

// This code is free to use and study for all Sierra Chart users
//
// Please consider supporting this work with a one time donation 
// https://verrillotrading.com/tips
//
// or simply by sending a thank you email to support@verrillotrading.com
//
// Consider getting added to our list of SC Study users, 
// https://verrillotrading.com/sc-open-source-studies-access
//
// Be first to know when new updates or services are released 
// including first priority support in the case of any issues that are found. 
//
// The version of this code is for github and does not recieve updates as
// frequently as the code on our local repository. 
//
// Compiling this file requires that you install and include curl and any other
// libraries that are included. 
//
// Some helper functions may be defined in these studies that are defined in a
// different source/header pair, these additional helpers are not open source. 
//
// To obtain access to these studies immediately, use this page:
// https://verrillotrading.com/sc-open-source-studies-access
//
// Kind regards, 
//
// - Christian
// VerrilloTrading, Content Creator - Developer
// support@verrillotrading.com
//
// Wed Aug 19 01:04:52 PM EDT 2026
int ver = 94;

SCDLLName("VerrilloTrading - Open Source Studies")
SCSFExport scsf_LinkChartSymbol(SCStudyInterfaceRef sc)
{
	// -------------------------------------------------------------------------
	int InputIndex = 0;
	SCInputRef i_ChartNumberToMonitor = sc.Input[InputIndex++];

	if(sc.SetDefaults)
	{
		std::string StudyGraphName = "Helpers: Link Chart Symbol To Target Chart Symbol | v" + std::to_string(ver) + " |";
		sc.GraphName = StudyGraphName.c_str();
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

	if(sc.IsUserAllowedForSCDLLName == false)
	{
		if(sc.Index == 0)
		{
		  sc.AddMessageToLog("You are not allowed to use this study. Please contact support@verrillotrading.com to obtain free access.",1);
		}
    	return;
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
std::vector<int> FindDuplicateStudiesInSameChartbook_V2(SCStudyInterfaceRef sc, const char* StudyName, SCString& msg)
{
	// same as FindDuplicateStudyiesInSameChartbook except returns a vector
	// containing the chart numbers where that study exists 
	//
	// Get the highest chart number in the current chartbook 
	int highest_chart_num = sc.GetHighestChartNumberUsedInChartBook();

	// get the current chart number for reference
	int this_chart_num = sc.ChartNumber;

	// vector used to save the chart numbers of charts in current chartbook
	std::vector <int> chart_numbers;

	// Go through each chart number from 1 to highest chart number to 
	// determine the chart numbers that exist and save those. 
	for(int ChartNumber = 1; ChartNumber <= highest_chart_num; ChartNumber++)
	{
		// returns true if chart number exists in current chartbook, empty string refers to current chartbook
		if(sc.IsChartNumberExist(ChartNumber, ""))
		{
			// add this chart number to the vector 
			chart_numbers.push_back(ChartNumber);
		}
	}	

	// vector used to save the chart numbers where we find instances of the study 
	std::vector <int> chart_numbers_where_study_is_found;
	
	// iterate through the existing number of charts with chart numbers as values  
	for(int i = 0; i < chart_numbers.size(); i++)
	{
		// Check if the study with this name is found on this chart number
		// arguments: (chart number, study name as a string, search for study short name instead)
		int is_study_found = sc.GetStudyIDByName(chart_numbers[i], StudyName, 0);

		// If the study is found and if the chart it was found on is not the current chart
		if(is_study_found != 0 && chart_numbers[i] != this_chart_num)
		{
			chart_numbers_where_study_is_found.push_back(chart_numbers[i]);
			// This should only return true if the study exists on two or more chartbooks
			// print which charts where the study is found
			// msg.Format("A duplicate is found on chart #%d. Reduce the number of studies "
			// "per chartbook to one unless you wish to recieve duplicate alerts.", chart_numbers[i]);
			// sc.AddMessageToLog(msg,1);
		}
	}

	// the scope calling this function will check the size of the returned vector
	return chart_numbers_where_study_is_found;
}
SCSFExport scsf_DrawPriorDaysLevels(SCStudyInterfaceRef sc)
{
	int InputIndex = 0;

	// In order to detect input changes made by other instances of Prior Day Levels within the same chartbook
	// we need to save the input index of each study input. 

	// num days 
	int NumDaysInputIndex = InputIndex;
	SCInputRef i_NumDaysToCalculate = sc.Input[InputIndex++];

	// Draw Levels of Saturday 
	int DrawWeekendsInputIndex = InputIndex;
	SCInputRef i_DrawLevelsOnSaturday = sc.Input[InputIndex++];

	// DST on off 
	int DSTInputIndex = InputIndex;
	SCInputRef i_IsDaylightSavings = sc.Input[InputIndex++];

	// User configurable session times 
	int SessionStartTimeInputIndex = InputIndex;
	SCInputRef i_SessionStartTime = sc.Input[InputIndex++];

	int SessionEndTimeInputIndex = InputIndex;
	SCInputRef i_SessionEndTime = sc.Input[InputIndex++];

	int UseEveningSessionInputIndex = InputIndex;
	SCInputRef i_UseEveningSession = sc.Input[InputIndex++];

	int EveningSessionStartTimeInputIndex = InputIndex;
	SCInputRef i_EveningSessionStartTime = sc.Input[InputIndex++];

	int EveningSessionEndTimeInputIndex = InputIndex;
	SCInputRef i_EveningSessionEndTime = sc.Input[InputIndex++];

	// on off inputs 
	int DrawSessionOpenInputIndex = InputIndex;
	SCInputRef i_DrawSessionOpen = sc.Input[InputIndex++];

	int DrawSessionCloseInputIndex = InputIndex;
	SCInputRef i_DrawSessionClose = sc.Input[InputIndex++];

	// Sun Jan  4 02:27:18 PM EST 2026
	//
	// us stocks open close
	int DrawStocksOpenInputIndex = InputIndex;
	SCInputRef i_DrawStocksOpen = sc.Input[InputIndex++];

	int DrawStocksCloseInputIndex = InputIndex;
	SCInputRef i_DrawStocksClose = sc.Input[InputIndex++];

	// futures open close 
	int DrawGlobexOpenInputIndex = InputIndex;
	SCInputRef i_DrawGlobexOpen = sc.Input[InputIndex++];

	int DrawGlobexCloseInputIndex = InputIndex;
	SCInputRef i_DrawGlobexClose = sc.Input[InputIndex++];

	// eu open close 
	int DrawEuropeOpenInputIndex = InputIndex;
	SCInputRef i_DrawEuropeOpen = sc.Input[InputIndex++];

	int DrawEuropeCloseInputIndex = InputIndex;
	SCInputRef i_DrawEuropeClose = sc.Input[InputIndex++];


	// NY Midnight Price
	int DrawMidnightInputIndex = InputIndex;
	SCInputRef i_DrawMidnightPrice = sc.Input[InputIndex++];
	
	// 00:00 UTC
	// 00:00 UTC is also China Open
	int DrawUTCInputIndex = InputIndex;
	SCInputRef i_DrawUTCOpenPrice = sc.Input[InputIndex++];

	// NAH TODO: China open close? 
	// until 06:00–07:00 UTC (JAPAN)
	//~08:00

	// TODO: Show Line Short Name Text (O, C, H, L, Midnight, CNO, CNC, EUO, EUC)

	// GRAPHICS SETTINGS 
	// Intraday High Low 
	int IntradayHighLowColor_InputIndex = InputIndex;
	SCInputRef i_IntradayHighLowColor = sc.Input[InputIndex++];

	int IntradayHighLowLineStyle_InputIndex = InputIndex;
	SCInputRef i_IntradayHighLowLineStyle = sc.Input[InputIndex++];

	int IntradayHighLowLineWidth_InputIndex = InputIndex;
	SCInputRef i_IntradayHighLowLineWidth = sc.Input[InputIndex++];

	// Session Open Close 
	// -----
	int SessionOpenColor_InputIndex = InputIndex;
	SCInputRef i_SessionOpenColor = sc.Input[InputIndex++];

	int SessionOpenLineStyle_InputIndex = InputIndex;
	SCInputRef i_SessionOpenLineStyle = sc.Input[InputIndex++];

	int SessionOpenLineWidth_InputIndex = InputIndex;
	SCInputRef i_SessionOpenLineWidth = sc.Input[InputIndex++];

	int SessionCloseColor_InputIndex = InputIndex;
	SCInputRef i_SessionCloseColor = sc.Input[InputIndex++];

	int SessionCloseLineStyle_InputIndex = InputIndex;
	SCInputRef i_SessionCloseLineStyle = sc.Input[InputIndex++];

	int SessionCloseLineWidth_InputIndex = InputIndex;
	SCInputRef i_SessionCloseLineWidth = sc.Input[InputIndex++];
	// -----

	// US Stocks Open Close 
	// ---
	int StocksOpenColor_InputIndex = InputIndex;
	SCInputRef i_USStocksOpenPriceColor = sc.Input[InputIndex++];

	int StocksOpenLineStyle_InputIndex = InputIndex;
	SCInputRef i_USStocksOpenPriceLineStyle = sc.Input[InputIndex++];

	int StocksOpenLineWidth_InputIndex = InputIndex;
	SCInputRef i_USStocksOpenPriceLineWidth = sc.Input[InputIndex++];

	int StocksCloseColor_InputIndex = InputIndex;
	SCInputRef i_USStocksClosePriceColor = sc.Input[InputIndex++];

	int StocksCloseLineStyle_InputIndex = InputIndex;
	SCInputRef i_USStocksClosePriceLineStyle = sc.Input[InputIndex++];
	
	int StocksCloseLineWidth_InputIndex = InputIndex;
	SCInputRef i_USStocksClosePriceLineWidth = sc.Input[InputIndex++];
	// ---
	
	// cme open close 
	// --- 
	int GlobexOpenColor_InputIndex = InputIndex;
	SCInputRef i_CMEOpenPriceColor = sc.Input[InputIndex++];

	int GlobexOpenLineStyle_InputIndex = InputIndex;
	SCInputRef i_CMEOpenPriceLineStyle = sc.Input[InputIndex++];

	int GlobexOpenLineWidth_InputIndex = InputIndex;
	SCInputRef i_CMEOpenPriceLineWidth = sc.Input[InputIndex++];

	int GlobexCloseColor_InputIndex = InputIndex;
	SCInputRef i_CMEClosePriceColor = sc.Input[InputIndex++];

	int GlobexCloseLineStyle_InputIndex = InputIndex;
	SCInputRef i_CMEClosePriceLineStyle = sc.Input[InputIndex++];

	int GlobexCloseLineWidth_InputIndex = InputIndex;
	SCInputRef i_CMEClosePriceLineWidth = sc.Input[InputIndex++];
	// ---
	
	// eu open close 
	// ---
	int EuropeOpenColor_InputIndex = InputIndex;
	SCInputRef i_EuropeOpenColor = sc.Input[InputIndex++];

	int EuropeOpenLineStyle_InputIndex = InputIndex;
	SCInputRef i_EuropeOpenLineStyle = sc.Input[InputIndex++];

	int EuropeOpenLineWidth_InputIndex = InputIndex;
	SCInputRef i_EuropeOpenLineWidth = sc.Input[InputIndex++];

	int EuropeCloseColor_InputIndex = InputIndex;
	SCInputRef i_EuropeCloseColor = sc.Input[InputIndex++];

	int EuropeCloseLineStyle_InputIndex = InputIndex;
	SCInputRef i_EuropeCloseLineStyle = sc.Input[InputIndex++];

	int EuropeCloseLineWidth_InputIndex = InputIndex;
	SCInputRef i_EuropeCloseLineWidth = sc.Input[InputIndex++];
	// --- 

	// extra levels 
	// ---
	int MidnightColor_InputIndex = InputIndex;
	SCInputRef i_MidnightPriceColor = sc.Input[InputIndex++];

	int MidnightLineStyle_InputIndex = InputIndex;
	SCInputRef i_MidnightPriceLineStyle = sc.Input[InputIndex++];

	int MidnightLineWidth_InputIndex = InputIndex;
	SCInputRef i_MidnightPriceLineWidth = sc.Input[InputIndex++];

	int UTCColor_InputIndex = InputIndex;
	SCInputRef i_UTCOpenPriceColor = sc.Input[InputIndex++];

	int UTCLineStyle_InputIndex = InputIndex;
	SCInputRef i_UTCOpenPriceLineStyle = sc.Input[InputIndex++];

	int UTCLineWidth_InputIndex = InputIndex;
	SCInputRef i_UTCOpenPriceLineWidth = sc.Input[InputIndex++];
	// ---

	// ACS Buttons 
	int ACSButtonToDrawLevels_InputIndex = InputIndex;
	SCInputRef i_ACSButtonToDrawLevels = sc.Input[InputIndex++];

	int ACSButtonToHideLevels_InputIndex = InputIndex;
	SCInputRef i_ACSButtonToHideLevels = sc.Input[InputIndex++];

	// Enable Alerts for various levels  
	//
	int EnableNewHighLowAlert_InputIndex = InputIndex;
	SCInputRef i_EnableNewHighLowAlert = sc.Input[InputIndex++];

	int EnableSessionOpenAlert_InputIndex = InputIndex;
	SCInputRef i_EnableSessionOpenAlert = sc.Input[InputIndex++];

	int EnableSessionCloseAlert_InputIndex = InputIndex;
	SCInputRef i_EnableSessionCloseAlert = sc.Input[InputIndex++];

	int EnableStocksOpenPriceAlert_InputIndex = InputIndex;
	SCInputRef i_EnableStocksOpenPriceAlert = sc.Input[InputIndex++];

	int EnableStocksClosePriceAlert_InputIndex = InputIndex;
	SCInputRef i_EnableStocksClosePriceAlert = sc.Input[InputIndex++];

	int EnableCMEOpenPriceAlert_InputIndex = InputIndex;
	SCInputRef i_EnableCMEOpenPriceAlert = sc.Input[InputIndex++];

	int EnableCMEClosePriceAlert_InputIndex = InputIndex;
	SCInputRef i_EnableCMEClosePriceAlert = sc.Input[InputIndex++];

	int EnableEuropeOpenPriceAlert_InputIndex = InputIndex;
	SCInputRef i_EnableEuropeOpenPriceAlert = sc.Input[InputIndex++];

	int EnableEuropeClosePriceAlert_InputIndex = InputIndex;
	SCInputRef i_EnableEuropeClosePriceAlert = sc.Input[InputIndex++];

	int EnableMidnightPriceAlert_InputIndex = InputIndex;
	SCInputRef i_EnableMidnightPriceAlert = sc.Input[InputIndex++];

	int EnableUTCPriceAlert_InputIndex = InputIndex;
	SCInputRef i_EnableUTCMidnightPriceAlert = sc.Input[InputIndex++];

	// Alert Number 
	int AlertNumberForAlerts_InputIndex = InputIndex;
	SCInputRef i_AlertNumberForCrossAlerts = sc.Input[InputIndex++];

	// Alert once per bar for all alerts 
	int AlertOncePerBar_InputIndex = InputIndex;
	SCInputRef i_AlertOnlyOncePerBar = sc.Input[InputIndex++];

	// This is the only input where we will not change it if it gets changed from another study instance 
	int DetectChangesInputIndex = InputIndex;
	SCInputRef i_DetectChangesFromOtherStudyInstance = sc.Input[InputIndex++];

	int FontSizeInputIndex = InputIndex;
	SCInputRef i_FontSizeForLineText = sc.Input[InputIndex++];

	// defaults
	int StudyDisplayOrder = 1;
	if(sc.SetDefaults)
	{
		std::string StudyGraphName = "Analysis - Draw Prior Days Levels | v" + std::to_string(ver) + " |";
		sc.GraphName = StudyGraphName.c_str();
		sc.ValueFormat = sc.BaseGraphValueFormat;
		sc.GraphRegion = 0;
		sc.AutoLoop		= 0; // manual loop 
		
		// Update Interval is Controlled in Chart Settings -> Performance ->
		// ACSIL Performance -> Minimum Chart Update Interval in Milliseconds
		// For ACSIL UpdateAlways. 
		sc.UpdateAlways = 1; 
							 
		// Necessary for obtaining the Bar End Time 
		sc.MaintainAdditionalChartDataArrays = 1;

		i_DetectChangesFromOtherStudyInstance.Name = "Commit Changes Made From Other Study Instances";
		i_DetectChangesFromOtherStudyInstance.SetYesNo(1);
		i_DetectChangesFromOtherStudyInstance.SetDescription("Detects changes made by other instances of this study within the same chart book, and commits them to this instance.");
		i_DetectChangesFromOtherStudyInstance.DisplayOrder = StudyDisplayOrder++;

		i_NumDaysToCalculate.Name = "Number of Days Back to Draw Levels";
		i_NumDaysToCalculate.SetInt(5);
		i_NumDaysToCalculate.SetIntLimits(0,30);
		i_NumDaysToCalculate.SetDescription("Controls the Number of Days Back the Study draws the Levels. A setting of 0 draws only for the current day.");
		i_NumDaysToCalculate.DisplayOrder = StudyDisplayOrder++;

		i_DrawLevelsOnSaturday.Name = "Draw Levels on Saturday";
		i_DrawLevelsOnSaturday.SetYesNo(0);
		i_DrawLevelsOnSaturday.SetDescription("Controls whether levels are drawn on Saturday. This is for usage with Cryptocurrency symbols that continue trading on Saturday. This input has no effect on Sunday levels, these will be drawn regardless.");
		i_DrawLevelsOnSaturday.DisplayOrder = StudyDisplayOrder++;

		i_IsDaylightSavings.Name = "Adjust Study Times for Daylight Savings Time";
		i_IsDaylightSavings.SetYesNo(0);
		i_IsDaylightSavings.SetDescription("Enable this if you are in a location that observes Daylight Savings Time and the current date is between March and November when Daylight Savings Time is observed");
		i_IsDaylightSavings.DisplayOrder = StudyDisplayOrder++;
 
		// User controlled session times (Initially obtained from the Chart)
		//
		// sc.StartTime1 (RTH Open)
		// sc.StartTime2 (RTH Close)
		//
		// sc.EndTime1 (ETH Open)
		// sc.EndTime2 (ETH Close)
		i_SessionStartTime.Name = "Session Start Time (Read/Write)";
		i_SessionStartTime.SetTime(sc.StartTime1); // Set the time for the input to 8:30
		i_SessionStartTime.SetDescription("This input sets Chart Settings >> Session Times >> Session Start Time for the Chart. "
			"The value of this input will also change when the setting is changed by the user in Chart Settings.");
		i_SessionStartTime.DisplayOrder = StudyDisplayOrder++;
													  
		i_SessionEndTime.Name = "Session End Time (Read/Write)";
		i_SessionEndTime.SetTime(sc.EndTime1); // 
		i_SessionEndTime.SetDescription("This input sets Chart Settings >> Session Times >> Session End Time for the Chart. "
			"The value of this input will also change when the setting is changed by the user in Chart Settings.");
		i_SessionEndTime.DisplayOrder = StudyDisplayOrder++;

		i_UseEveningSession.Name = "Use Evening Session (Read/Write)"; 
		i_UseEveningSession.SetYesNo(sc.UseSecondStartEndTimes);
		i_UseEveningSession.SetDescription("This input sets Chart Settings >> Session Times >> Use Evening Session for the Chart. "
			"The value of this input will also change when the setting is changed by the user in Chart Settings.");
		i_UseEveningSession.DisplayOrder = StudyDisplayOrder++;

		i_EveningSessionStartTime.Name = "Evening Session Start Time (Read/Write)";
		i_EveningSessionStartTime.SetTime(sc.StartTime2);
		i_EveningSessionStartTime.SetDescription("This input sets Chart Settings >> Session Times >> Evening Start Time for the Chart. "
			"The value of this input will also change when the setting is changed by the user in Chart Settings.");
		i_EveningSessionStartTime.DisplayOrder = StudyDisplayOrder++;

		i_EveningSessionEndTime.Name = "Evening Session End Time (Read/Write)";
		i_EveningSessionEndTime.SetTime(sc.EndTime2);
		i_EveningSessionEndTime.SetDescription("This input sets Chart Settings >> Session Times >> Evening End Time for the Chart"
			"The value of this input will also change when the setting is changed by the user in Chart Settings.");
		i_EveningSessionEndTime.DisplayOrder = StudyDisplayOrder++;
		//
		// Session Open Time (Set in Chart Settings -> Session Times)
		i_DrawSessionOpen.Name = "Draw Session Open Price";
		i_DrawSessionOpen.SetYesNo(1);
		i_DrawSessionOpen.SetDescription("Enable Drawing the Session Open Price.");
		i_DrawSessionOpen.DisplayOrder = StudyDisplayOrder++;

		// Session Close Time (Set in Chart Settings -> Session Times)
		i_DrawSessionClose.Name = "Draw Session Close Price";
		i_DrawSessionClose.SetYesNo(1);
		i_DrawSessionClose.SetDescription("Enable Drawing the Session Close Price.");
		i_DrawSessionClose.DisplayOrder = StudyDisplayOrder++;

		// Stocks Open
		i_DrawStocksOpen.Name = "Draw US Stock Market Open Price";
		i_DrawStocksOpen.SetYesNo(1);
		i_DrawStocksOpen.SetDescription("Draw US Stock Market Open Price When Session Open Not 9:30 ET.");
		i_DrawStocksOpen.DisplayOrder = StudyDisplayOrder++;

		// Stocks Close
		i_DrawStocksClose.Name = "Draw US Stock Market Close Price";
		i_DrawStocksClose.SetYesNo(1);
		i_DrawStocksClose.SetDescription("Draw US Stock Market Close Price When Session Close Not 16:00 ET.");
		i_DrawStocksClose.DisplayOrder = StudyDisplayOrder++;

		// Globex Open
		i_DrawGlobexOpen.Name = "Draw CME Futures Open Price";
		i_DrawGlobexOpen.SetYesNo(1);
		i_DrawGlobexOpen.SetDescription("Enable Drawing the Globex Futures Market Opening Price");
		i_DrawGlobexOpen.DisplayOrder = StudyDisplayOrder++;

		// Globex Close 5pm ET
		i_DrawGlobexClose.Name = "Draw CME Futures Close Price";
		i_DrawGlobexClose.SetYesNo(1);
		i_DrawGlobexClose.SetDescription("Enable Drawing the CME Globex Close Price.");
		i_DrawGlobexClose.DisplayOrder = StudyDisplayOrder++;

		// Europe Open (3AM ET)
		i_DrawEuropeOpen.Name = "Draw Europe Open Price";
		i_DrawEuropeOpen.SetYesNo(1);
		i_DrawEuropeOpen.SetDescription("Enable Drawing the Europe Session Open Price. 3AM New York Time");
		i_DrawEuropeOpen.DisplayOrder = StudyDisplayOrder++;

		// Europe Close (11:30AM ET)
		i_DrawEuropeClose.Name = "Draw Europe Close Price";
		i_DrawEuropeClose.SetYesNo(1);
		i_DrawEuropeClose.SetDescription("Enable Drawing the Europe Session Close Price. 11:30 AM New York Time");
		i_DrawEuropeClose.DisplayOrder = StudyDisplayOrder++;

		// Extra Levels / Open Close Times 
		// Midnight Price 
		i_DrawMidnightPrice.Name = "Draw New York Midnight Price";
		i_DrawMidnightPrice.SetYesNo(1);
		i_DrawMidnightPrice.SetDescription("Enable Drawing the New York Midnight Price.");
		i_DrawMidnightPrice.DisplayOrder = StudyDisplayOrder++;

		i_DrawUTCOpenPrice.Name = "Draw UTC 00:00 Open Price";
		i_DrawUTCOpenPrice.SetYesNo(1);
		i_DrawUTCOpenPrice.SetDescription("Enable Drawing the UTC 00:00 Open Price.");
		i_DrawUTCOpenPrice.DisplayOrder = StudyDisplayOrder++;

		// LINE SETTINGS 
		//
		i_FontSizeForLineText.Name = "Font Size For Line Text";
		i_FontSizeForLineText.SetInt(10);
		i_FontSizeForLineText.SetIntLimits(3,40);
		i_FontSizeForLineText.DisplayOrder = StudyDisplayOrder++;

		// PD HIGH LOW Line Color & Style inputs 
		i_IntradayHighLowColor.Name = "Intraday High/Low Line Color";
		i_IntradayHighLowColor.SetColor(128,255,255); // baby blue 
		i_IntradayHighLowColor.SetDescription("Line Color for High/Low Line Drawings");
		i_IntradayHighLowColor.DisplayOrder = StudyDisplayOrder++;
													  
		i_IntradayHighLowLineStyle.Name = "Intraday High/Low Line Style";
		i_IntradayHighLowLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_IntradayHighLowLineStyle.SetCustomInputIndex(0);
		i_IntradayHighLowLineStyle.SetDescription("Line Style for High/Low Line Drawings");
		i_IntradayHighLowLineStyle.DisplayOrder = StudyDisplayOrder++;

		i_IntradayHighLowLineWidth.Name = "Intraday High/Low Line Width";
		i_IntradayHighLowLineWidth.SetInt(2);
		i_IntradayHighLowLineWidth.SetIntLimits(1,20);
		i_IntradayHighLowLineWidth.SetDescription("Line Width for High/Low Line Drawings");
		i_IntradayHighLowLineWidth.DisplayOrder = StudyDisplayOrder++;

		// Session Open
		i_SessionOpenColor.Name = "Session Open Price Color";
		i_SessionOpenColor.SetColor(128,255,128); // light green
		i_SessionOpenColor.SetDescription("Line Color for Session Open Drawings");
		i_SessionOpenColor.DisplayOrder = StudyDisplayOrder++;
												  
		i_SessionOpenLineStyle.Name = "Session Open Price Line Style";
		i_SessionOpenLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_SessionOpenLineStyle.SetCustomInputIndex(4);
		i_SessionOpenLineStyle.SetDescription("Line Style for Session Open Drawings");
		i_SessionOpenLineStyle.DisplayOrder = StudyDisplayOrder++;

		i_SessionOpenLineWidth.Name = "Session Open Price Line Width";
		i_SessionOpenLineWidth.SetInt(3);
		i_SessionOpenLineWidth.SetIntLimits(1,20);
		i_SessionOpenLineWidth.SetDescription("Line Width for Session Open Drawings");
		i_SessionOpenLineWidth.DisplayOrder = StudyDisplayOrder++;

		// Session Close 
		i_SessionCloseColor.Name = "Session Close Price Color";
		i_SessionCloseColor.SetColor(255,128, 192); // pink
		i_SessionCloseColor.SetDescription("Line Color for Session Close Drawings");
		i_SessionCloseColor.DisplayOrder = StudyDisplayOrder++;

		i_SessionCloseLineStyle.Name = "Session Close Price Line Style";
		i_SessionCloseLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_SessionCloseLineStyle.SetCustomInputIndex(4);
		i_SessionCloseLineStyle.SetDescription("Line Style for Session Close Drawings");
		i_SessionCloseLineStyle.DisplayOrder = StudyDisplayOrder++;

		i_SessionCloseLineWidth.Name = "Session Close Price Line Width";
		i_SessionCloseLineWidth.SetInt(3);
		i_SessionCloseLineWidth.SetIntLimits(1,20);
		i_SessionCloseLineWidth.SetDescription("Line Width for Session Close Drawings");
		i_SessionCloseLineWidth.DisplayOrder = StudyDisplayOrder++;


		// US Stocks Open Price 
		i_USStocksOpenPriceColor.Name = "US Stocks Open Price Color";
		i_USStocksOpenPriceColor.SetColor(0,128,0); // lighter green
		i_USStocksOpenPriceColor.SetDescription("Line Color for US Stocks Open Price Drawings");
		i_USStocksOpenPriceColor.DisplayOrder = StudyDisplayOrder++;
													 
		i_USStocksOpenPriceLineStyle.Name = "US Stocks Open Price Line Style";
		i_USStocksOpenPriceLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_USStocksOpenPriceLineStyle.SetCustomInputIndex(4);
		i_USStocksOpenPriceLineStyle.SetDescription("Line Style for US Stocks Open Price Drawings");
		i_USStocksOpenPriceLineStyle.DisplayOrder = StudyDisplayOrder++;

		i_USStocksOpenPriceLineWidth.Name = "US Stocks Open Price Line Width";
		i_USStocksOpenPriceLineWidth.SetInt(3);
		i_USStocksOpenPriceLineWidth.SetIntLimits(1,20);
		i_USStocksOpenPriceLineWidth.SetDescription("Line Width for US Stocks Open Price Drawings");
		i_USStocksOpenPriceLineWidth.DisplayOrder = StudyDisplayOrder++;

		// US Stocks Close Price
		i_USStocksClosePriceColor.Name = "US Stocks Close Price Color";
		i_USStocksClosePriceColor.SetColor(255,0, 128); // darker cherry red 
		i_USStocksClosePriceColor.SetDescription("Line Color for US Stocks Close Price Drawings");
		i_USStocksClosePriceColor.DisplayOrder = StudyDisplayOrder++;
													 
		i_USStocksClosePriceLineStyle.Name = "US Stocks Close Price Line Style";
		i_USStocksClosePriceLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_USStocksClosePriceLineStyle.SetCustomInputIndex(4);
		i_USStocksClosePriceLineStyle.SetDescription("Line Style for US Stocks Close Price Drawings");
		i_USStocksClosePriceLineStyle.DisplayOrder = StudyDisplayOrder++;

		i_USStocksClosePriceLineWidth.Name = "US Stocks Close Price Line Width";
		i_USStocksClosePriceLineWidth.SetInt(3);
		i_USStocksClosePriceLineWidth.SetIntLimits(1,20);
		i_USStocksClosePriceLineWidth.SetDescription("Line Width for US Stocks Close Price Drawings");
		i_USStocksClosePriceLineWidth.DisplayOrder = StudyDisplayOrder++;
		//
		// CME Open price 
		i_CMEOpenPriceColor.Name = "CME Futures Open Price Color";
		i_CMEOpenPriceColor.SetColor(255,255,255); // white
		i_CMEOpenPriceColor.SetDescription("Line Color for CME Open Price Drawings");
		i_CMEOpenPriceColor.DisplayOrder = StudyDisplayOrder++;
													 
		i_CMEOpenPriceLineStyle.Name = "CME Futures Open Price Line Style";
		i_CMEOpenPriceLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_CMEOpenPriceLineStyle.SetCustomInputIndex(4);
		i_CMEOpenPriceLineStyle.SetDescription("Line Style for CME Open Price Drawings");
		i_CMEOpenPriceLineStyle.DisplayOrder = StudyDisplayOrder++;

		i_CMEOpenPriceLineWidth.Name = "CME Futures Open Price Line Width";
		i_CMEOpenPriceLineWidth.SetInt(3);
		i_CMEOpenPriceLineWidth.SetIntLimits(1,20);
		i_CMEOpenPriceLineWidth.SetDescription("Line Width for CME Open Price Drawings");
		i_CMEOpenPriceLineWidth.DisplayOrder = StudyDisplayOrder++;

		// CME Close price 
		i_CMEClosePriceColor.Name = "CME Futures Close Price Color";
		i_CMEClosePriceColor.SetColor(0,255,255); // darker cyan (dash dot) 
		i_CMEClosePriceColor.SetDescription("Line Color for CME Close Price Drawings");
		i_CMEClosePriceColor.DisplayOrder = StudyDisplayOrder++;
													 
		i_CMEClosePriceLineStyle.Name = "CME Futures Close Price Line Style";
		i_CMEClosePriceLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_CMEClosePriceLineStyle.SetCustomInputIndex(4);
		i_CMEClosePriceLineStyle.SetDescription("Line Style for CME Close Price Drawings");
		i_CMEClosePriceLineStyle.DisplayOrder = StudyDisplayOrder++;

		i_CMEClosePriceLineWidth.Name = "CME Futures Close Price Line Width";
		i_CMEClosePriceLineWidth.SetInt(3);
		i_CMEClosePriceLineWidth.SetIntLimits(1,20);
		i_CMEClosePriceLineWidth.SetDescription("Line Width for CME Close Price Drawings");
		i_CMEClosePriceLineWidth.DisplayOrder = StudyDisplayOrder++;

		// EU Open Price 
		i_EuropeOpenColor.Name = "Europe Open Price Color";
		i_EuropeOpenColor.SetColor(0,0,255); // navy blue dash dot) 
		i_EuropeOpenColor.SetDescription("Line Color for EU Open Price Drawings");
		i_EuropeOpenColor.DisplayOrder = StudyDisplayOrder++;
		
		i_EuropeOpenLineStyle.Name = "Europe Open Price Line Style";
		i_EuropeOpenLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_EuropeOpenLineStyle.SetCustomInputIndex(4);
		i_EuropeOpenLineStyle.SetDescription("Line Style for EU Open Price Drawings");
		i_EuropeOpenLineStyle.DisplayOrder = StudyDisplayOrder++;

		i_EuropeOpenLineWidth.Name = "Europe Open Price Line Width";
		i_EuropeOpenLineWidth.SetInt(3);
		i_EuropeOpenLineWidth.SetIntLimits(1,20);
		i_EuropeOpenLineWidth.SetDescription("Line Width for EU Open Price Drawings");
		i_EuropeOpenLineWidth.DisplayOrder = StudyDisplayOrder++;

		// EU Close Price 
		i_EuropeCloseColor.Name = "Europe Close Price Color";
		i_EuropeCloseColor.SetColor(128,0,128); // navy blue 
		i_EuropeCloseColor.SetDescription("Line Color for EU Close Price Drawings");
		i_EuropeCloseColor.DisplayOrder = StudyDisplayOrder++;
												
		i_EuropeCloseLineStyle.Name = "Europe Close Price Line Style";
		i_EuropeCloseLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_EuropeCloseLineStyle.SetCustomInputIndex(4);
		i_EuropeCloseLineStyle.SetDescription("Line Style for EU Close Price Drawings");
		i_EuropeCloseLineStyle.DisplayOrder = StudyDisplayOrder++;

		i_EuropeCloseLineWidth.Name = "Europe Close Price Line Width";
		i_EuropeCloseLineWidth.SetInt(3);
		i_EuropeCloseLineWidth.SetIntLimits(1,20);
		i_EuropeCloseLineWidth.SetDescription("Line Width for EU Close Price Drawings");
		i_EuropeCloseLineWidth.DisplayOrder = StudyDisplayOrder++;

		// Midnight Price 
		i_MidnightPriceColor.Name = "Midnight Price Color";
		i_MidnightPriceColor.SetColor(255,0,255); // magenta ish (dash dot) 
		i_MidnightPriceColor.SetDescription("Line Color for Midnight Price Drawings");
		i_MidnightPriceColor.DisplayOrder = StudyDisplayOrder++;
												  
		i_MidnightPriceLineStyle.Name = "Midnight Price Line Style";
		i_MidnightPriceLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_MidnightPriceLineStyle.SetCustomInputIndex(4);
		i_MidnightPriceLineStyle.SetDescription("Line Style for Midnight Price Drawings");
		i_MidnightPriceLineStyle.DisplayOrder = StudyDisplayOrder++;

		i_MidnightPriceLineWidth.Name = "Midnight Price Line Width";
		i_MidnightPriceLineWidth.SetInt(3);
		i_MidnightPriceLineWidth.SetIntLimits(1,20);
		i_MidnightPriceLineWidth.SetDescription("Line Width for Midnight Price Drawings");
		i_MidnightPriceLineWidth.DisplayOrder = StudyDisplayOrder++;

		// UTC Open Price
		i_UTCOpenPriceColor.Name = "UTC Open Price Color";
		i_UTCOpenPriceColor.SetColor(255,145,33); // Orange 
		i_UTCOpenPriceColor.SetDescription("Line Color for UTC Open Price Drawings");
		i_UTCOpenPriceColor.DisplayOrder = StudyDisplayOrder++;
												  
		i_UTCOpenPriceLineStyle.Name = "UTC Open Price Line Style";
		i_UTCOpenPriceLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_UTCOpenPriceLineStyle.SetCustomInputIndex(4);
		i_UTCOpenPriceLineStyle.SetDescription("Line Style for UTC Open Price Drawings");
		i_UTCOpenPriceLineStyle.DisplayOrder = StudyDisplayOrder++;

		i_UTCOpenPriceLineWidth.Name = "UTC Open Price Line Width";
		i_UTCOpenPriceLineWidth.SetInt(3);
		i_UTCOpenPriceLineWidth.SetIntLimits(1,20);
		i_UTCOpenPriceLineWidth.SetDescription("Line Width for UTC Open Price Drawings");
		i_UTCOpenPriceLineWidth.DisplayOrder = StudyDisplayOrder++;

		// ACS Button inputs 
		// i_ACSButtonToDrawLevels.Name = "(Debugging only) ACS Button To ReDraw Prior Day Levels";
		// i_ACSButtonToDrawLevels.SetInt(110);
		// i_ACSButtonToDrawLevels.SetIntLimits(0, 150);
		// i_ACSButtonToDrawLevels.SetDescription("ACS Button Number that was used to redraw the drawings during debugging");

		i_ACSButtonToHideLevels.Name = "ACS Button To Hide Prior Day Levels";
		i_ACSButtonToHideLevels.SetInt(111);
		i_ACSButtonToHideLevels.SetIntLimits(0,150);
		i_ACSButtonToHideLevels.SetDescription("ACS Button Number to be used to temporarily Hide/Show the drawings created by this study.");
		i_ACSButtonToHideLevels.DisplayOrder = StudyDisplayOrder++;

		// Alert Inputs 
		i_EnableNewHighLowAlert.Name = "Alert When Crossing Previous High or Low";
		i_EnableNewHighLowAlert.SetYesNo(1);
		i_EnableNewHighLowAlert.SetDescription("Enable Study Alert when there is a new High/Low of Current Day");
		i_EnableNewHighLowAlert.DisplayOrder = StudyDisplayOrder++;

		i_EnableSessionOpenAlert.Name = "Alert When Crossing Previous Session Open";
		i_EnableSessionOpenAlert.SetYesNo(0);
		i_EnableSessionOpenAlert.SetDescription("Enable Study Alert When TCrossingest Previous Open Price");
		i_EnableSessionOpenAlert.DisplayOrder = StudyDisplayOrder++;

		i_EnableSessionCloseAlert.Name = "Alert When Crossing Previous Session Close";
		i_EnableSessionCloseAlert.SetYesNo(0);
		i_EnableSessionCloseAlert.SetDescription("Enable Study Alert When Crossing Previous Close Price");
		i_EnableSessionCloseAlert.DisplayOrder = StudyDisplayOrder++;

		i_EnableStocksOpenPriceAlert.Name = "Alert When Crossing Previous US Stocks Open Price";
		i_EnableStocksOpenPriceAlert.SetYesNo(0);
		i_EnableStocksOpenPriceAlert.SetDescription("Enable Study Alert When Clossing US Stocks Open Price");
		i_EnableStocksOpenPriceAlert.DisplayOrder = StudyDisplayOrder++;

		i_EnableStocksClosePriceAlert.Name = "Alert When Crossing Previous US Stocks Close Price";
		i_EnableStocksClosePriceAlert.SetYesNo(0);
		i_EnableStocksClosePriceAlert.SetDescription("Enable Study Alert When Crossing US Previous Stocks Close Price");
		i_EnableStocksClosePriceAlert.DisplayOrder = StudyDisplayOrder++;

		i_EnableCMEOpenPriceAlert.Name = "Alert When Crossing Previous CME Open Price";
		i_EnableCMEOpenPriceAlert.SetYesNo(0);
		i_EnableCMEOpenPriceAlert.SetDescription("Enable Study Alert When Crossing Previous CME Open Price");
		i_EnableCMEOpenPriceAlert.DisplayOrder = StudyDisplayOrder++;

		i_EnableCMEClosePriceAlert.Name = "Alert When Crossing Previous CME Close Price";
		i_EnableCMEClosePriceAlert.SetYesNo(0);
		i_EnableCMEClosePriceAlert.SetDescription("Enable Study Alert When Crossing Previous CME Close Price");
		i_EnableCMEClosePriceAlert.DisplayOrder = StudyDisplayOrder++;

		i_EnableEuropeOpenPriceAlert.Name = "Alert When Crossing Previous Europe Open Price";
		i_EnableEuropeOpenPriceAlert.SetYesNo(0);
		i_EnableEuropeOpenPriceAlert.SetDescription("Enable Study Alert When Crossing Previous Europe Open Price");
		i_EnableEuropeOpenPriceAlert.DisplayOrder = StudyDisplayOrder++;

		i_EnableEuropeClosePriceAlert.Name = "Alert When Crossing Previous Europe Close Price";
		i_EnableEuropeClosePriceAlert.SetYesNo(0);
		i_EnableEuropeClosePriceAlert.SetDescription("Enable Study Alert When Crossing Previous Europe Close Price");
		i_EnableEuropeClosePriceAlert.DisplayOrder = StudyDisplayOrder++;

		i_EnableCMEClosePriceAlert.Name = "Alert When Crossing Previous CME Close Price";
		i_EnableCMEClosePriceAlert.SetYesNo(0);
		i_EnableCMEClosePriceAlert.SetDescription("Enable Study Alert When Crossing Previous CME Close Price");
		i_EnableCMEClosePriceAlert.DisplayOrder = StudyDisplayOrder++;

		i_EnableMidnightPriceAlert.Name = "Alert When Crossing Previous Midnight Price";
		i_EnableMidnightPriceAlert.SetYesNo(1);
		i_EnableMidnightPriceAlert.SetDescription("Enable Study Alert When Crossing Previous Midnight Price");
		i_EnableMidnightPriceAlert.DisplayOrder = StudyDisplayOrder++;

		i_EnableUTCMidnightPriceAlert.Name = "Alert When Crossing Previous UTC Midnight Price";
		i_EnableUTCMidnightPriceAlert.SetYesNo(1);
		i_EnableUTCMidnightPriceAlert.SetDescription("Enable Study Alert When Crossing Previous UTC Midnight Price");
		i_EnableUTCMidnightPriceAlert.DisplayOrder = StudyDisplayOrder++;

		// Alert Number for the alert
		// Alert Numbers and sounds are configured in 
		// Global Settings > General Settings > Alerts 
		i_AlertNumberForCrossAlerts.Name = "Alert Number To Use for Study Alerts";
		i_AlertNumberForCrossAlerts.SetInt(4);
		i_AlertNumberForCrossAlerts.SetIntLimits(1,200);
		i_AlertNumberForCrossAlerts.SetDescription("Alert Number To Use for Alerts Generated By This Study ");
		i_AlertNumberForCrossAlerts.DisplayOrder = StudyDisplayOrder++;

		// Alert Only Once per bar or everytime there is a new high or low of day. 
		//
		// The user would set this to 0 if they wanted to recieve many alerts when
		// there is a new high or low. In the case that they want to be notified 
		// if the market is moving. 
		i_AlertOnlyOncePerBar.Name = "Alert Only Once Per Bar";
		i_AlertOnlyOncePerBar.SetYesNo(1);
		i_AlertOnlyOncePerBar.SetDescription("It is recommended to leave this input enabled and only disable it if you want to recieve many alerts when a new high/low is made or when a level is crossed.");
		i_AlertOnlyOncePerBar.DisplayOrder = StudyDisplayOrder++;

		// Study Description
		sc.StudyDescription = "This study was written by Christian at VerrilloTrading in December of 2024. "
			"<br><br>"
			"This is version 2 which was released on Feburary 2, 2026"
			"<br><br>"
			"This study shows prior day levels on the chart for a number of days going back. These levels include the session Open Close High Low, and additional levels like New York midnight price, EU open and close prices, and CME Globex close price."
			"<br><br>"
			"Please view this video for a demonstration on how to use this study: <a href = https://youtu.be/-kRpjkp2WDw?si=3u4yganP8uOTOeNH target=_blank rel=noopener noreferrer >Holiday Gift For Sierra Chart Traders</a>"
			"<br><br>"
			"The levels that this study draws are drawn automatically when the study is added to the chart, or when any input setting is changed in the study. There is no need to use the redraw button that was mentioned in the video. This button may be deprecated and no longer in the study."
			"<br><br>"
			"The study will draw the Session Open and Session Close, which are controlled in Chart Settings > Session Times. The session High and Low are calculated using the highest and lowest prices for the day."

			"<br><br>"
			"The additional levels that this study draws or overnight levels that compliment and provide context to the NY Regular trading hours session. The times of these levels are hard coded in the study in UTC time and then converted to the Time Zone of the chart."
			"<br><br>"
			"<strong>Suggestions Regarding The Chart Time Zone:</strong>"
			"<br><br>"
			"It is recommended to set the Time Zone of your chart explicitly using Chart Settings > Session Times > Time Zone. As opposed to setting it to <u>Use Global Time Zone</u>. One reason for this is because when this setting is changed it automatically changes the session start and end times for the chart. Making it easier if you move to a different Time Zone and need to change the Session Times for the Chart. In addition to this a user might consider linking the Session Time and Time Zone for some charts in your chartbook that use the same Session Times. This is controlled using Chart Settings > Linking > Chart Linking."

			"<br><br>"
			"<strong>Hide/Show Prior Day Levels:</strong>"
			"<br><br>"
			"There is support for hiding the drawings that this study draws. This is done using the Input Setting named <strong>ACS Button To Hide Prior Day Levels</strong> to select which Advanced Custom Study button will be used to hide or show the drawings. By default they are always shown, until the study is removed from the chart. The lines can also be deleted but will be redrawn if the chart is recalculated by performing Chart > Reload and Recalculate"
			"<br><br>"
			"<strong>Using Different Intraday Storage Time Units:</strong>"
			"<br><br>"
			"The study now supports using different Intra Day Storage Time Units. Please contact us if you see a problem or if levels are not being drawn. We tested it using Intraday Storage Time units of 5 and lower"
			"<br><br>"
			"<strong>Steps to Ensure the CME Globex Close Price is Drawn:</strong>"
			"<br><br>"
			"The CME Globex close price will not be drawn in real-time when the futures market close. If you need it to be drawn you can perform Chart > Reload and Recalculate after the market closes and it will be drawn."
			"<br><br>"
			"It is no longer necessary to enable this setting: Chart Settings > Bar Period > Include Columns With No Data. In fact we recommend leaving it disabled as it keeps the chart cleaner"
			"<br><br>"
			"It is recommended to disable this setting: Chart Settings > Session Times > Apply Intra Day Session Times To Intraday Chart.  "
			"<br><br>"
			"Because otherwise everytime the Chart Symbol is changed, the Session Times may also get modified."
			"<br><br>"
			"<strong>New High/Low of Day and Level Cross Alerts:</strong>"
			"<br><br>"
			"This study support sending a study alert when a new high or low price of day is made. It now supports alerts when additional levels from the study are crossed."
			"<br>"
			"It is possible to forward these alerts directly to a Telegram chat using either the VerrilloTrading <a href = https://youtu.be/EQZI9pBtDrE target=_blank rel=noopener noreferrer >Telegram Chart Drawing Alerts</a> study or using the built-in Sierra Chart method."
			"<br><br>"
			"<strong>General Notes:</strong>"
			"<br><br>"
			"This study has only been tested with Denali Exchange Data Feed on Sierra Chart and not other Data feeds like IQFeed."
			"<br><br>"
			"If you encounter issues with any levels not displaying, you can report this to support@verrillotrading.com. This study is open source and there will not be any free ongoing development of it, with the exception of a few items we may add. There is no guarantee we will be able to get to any support requests, or if we will consider additional feature requests. "
			"<br><br>"
			"I genuinely hope you enjoy the study,"
			"<br><br>"
			"Happy Trading, "
			"<br><br>"
			"-Christian"
			"<br><br>"
			"VerrilloTrading, Content Creator, Programmer"
			"";

		return;
	}

	if(sc.IsUserAllowedForSCDLLName == false)
	{
		if(sc.Index == 0)
		{
		  sc.AddMessageToLog("You are not allowed to use this study. Please contact support@verrillotrading.com to obtain free access.",1);
		}
    	return;
	} 

	// do not calculate if chart is still downloading data 
	if (sc.ChartIsDownloadingHistoricalData(sc.ChartNumber))
	{
		// sc.AddMessageToLog(
		// 	"Chart is downloading historical data. Wait for it to finish.", 1);
		return;
	}

	// logging object 
	SCString msg;

	// Index to assist with persistent variable numbers 
	int PersistentVariableIndex = 0;

	// Used for remembering if the drawings are hidden 
	int& HideLevels = sc.GetPersistentIntFast(PersistentVariableIndex++);

	// Other Persistent Variables 
	// 
	// used to remember price levels for triggering realtime alerts 
	double& LowOfDayMemory = sc.GetPersistentDoubleFast(PersistentVariableIndex++);
	double& HighOfDayMemory = sc.GetPersistentDoubleFast(PersistentVariableIndex++);

	// These are used to remember the levels drawn in real-time 
	// In order to not draw them again and also modify if necessary
	int& LowOfDayLineNumberMemory = sc.GetPersistentIntFast(PersistentVariableIndex++);
	int& HighOfDayLineNumberMemory = sc.GetPersistentIntFast(PersistentVariableIndex++);

	int& SessionOpenLineNumberMemory = sc.GetPersistentIntFast(PersistentVariableIndex++);
	int& SessionCloseLineNumberMemory = sc.GetPersistentIntFast(PersistentVariableIndex++);

	int& USStocksOpenLineNumberMemory = sc.GetPersistentIntFast(PersistentVariableIndex++);
	int& USStocksCloseLineNumberMemory = sc.GetPersistentIntFast(PersistentVariableIndex++);

	int& CMEOpenLineNumberMemory = sc.GetPersistentIntFast(PersistentVariableIndex++);
	int& CMECloseLineNumberMemory = sc.GetPersistentIntFast(PersistentVariableIndex++);

	int& EUOpenPriceLineNumberMemory = sc.GetPersistentIntFast(PersistentVariableIndex++);
	int& EUClosePriceLineNumberMemory = sc.GetPersistentIntFast(PersistentVariableIndex++);

	int& MidnightPriceLineNumberMemory = sc.GetPersistentIntFast(PersistentVariableIndex++);

	int& UTCOpenPriceLineNumberMemory = sc.GetPersistentIntFast(PersistentVariableIndex++);

	// memory for session times 
	int& SessionStartTimeMemory = sc.GetPersistentIntFast(PersistentVariableIndex++);
	int& SessionEndTimeMemory = sc.GetPersistentIntFast(PersistentVariableIndex++);
	int& UseEveningSessionMemory = sc.GetPersistentIntFast(PersistentVariableIndex++);
	int& EveningStartTimeMemory = sc.GetPersistentIntFast(PersistentVariableIndex++);
	int& EveningEndTimeMemory = sc.GetPersistentIntFast(PersistentVariableIndex++);

	// Persistent Vector used to save what lines we have drawn, in order
	// to ensure correct cleanup behaviour 
    std::vector<int>* p_LineNumbers = reinterpret_cast<std::vector<int>*>
		(sc.GetPersistentPointer(100));

	// msg.Format("session start mem: %d", SessionStartTimeMemory, UseEveningSessionMemory);
	// sc.AddMessageToLog(msg,1);

	// First time initialization
    if (p_LineNumbers == NULL) 
	{
		// initialize 
        p_LineNumbers = new std::vector<int>;
        sc.SetPersistentPointer(100, p_LineNumbers);
    }
    else 
	{
		// If not null, just do nothing because we will recalc only on a press
		// of acs button after deleting the drawings 
    }

	// INIT SESSION TIMES MEMORY (Assumes the user would never set those end times to 0, which makes no sense)
	if(SessionEndTimeMemory == 0 && EveningEndTimeMemory == 0)
	{
		// init persistent session times 
		SessionStartTimeMemory = sc.StartTime1;
		SessionEndTimeMemory = sc.EndTime1;

		UseEveningSessionMemory = sc.UseSecondStartEndTimes;

		EveningStartTimeMemory = sc.StartTime2;
		EveningEndTimeMemory = sc.EndTime2;
	}

	// handle last call to function
	if(sc.LastCallToFunction)
	{
		if(p_LineNumbers->size() != 0)
		{
			// LOGIC FOR DELETE ALL DRAWINGS IN PERSISTENT VECTOR AND CLEAR VECTOR 
			// sc.AddMessageToLog("we get here last call",1);
			// int NumDrawings = 0;
			for(int i = 0; i < p_LineNumbers->size(); i++)
			{
				sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, (*p_LineNumbers)[i]);
				// {
				// 	msg.Format("LineNumber: %d Deleted!", (*p_LineNumbers)[i]);
				// 	sc.AddMessageToLog(msg,1);

				// }
				// else
				// {
				// 	sc.AddMessageToLog("not deleted!",1);
				// }
			}
			// msg.Format("Vector Size: %d", p_LineNumbers->size());
			// sc.AddMessageToLog(msg,1);

			// clear the vector 
			p_LineNumbers->clear();

			// Then Delete the pointer 
			delete p_LineNumbers;
			sc.SetPersistentPointer(100, NULL);
		}

		// Reset Persistent Variables 
		LowOfDayMemory = 0;
		HighOfDayMemory = 0;
		LowOfDayLineNumberMemory = 0;
		HighOfDayLineNumberMemory = 0;

		SessionOpenLineNumberMemory = 0;
		SessionCloseLineNumberMemory = 0;

		USStocksOpenLineNumberMemory = 0;
		USStocksCloseLineNumberMemory = 0;

		CMEOpenLineNumberMemory = 0;
		CMECloseLineNumberMemory = 0;

		EUOpenPriceLineNumberMemory = 0;
		EUClosePriceLineNumberMemory = 0;

		MidnightPriceLineNumberMemory = 0;
		UTCOpenPriceLineNumberMemory = 0;
		return; // so necessary
	}

	// handle full chart recalculation
	if(sc.IsFullRecalculation)
	{
		// sc.AddMessageToLog("Full recalc!" ,1);
		// empty the vector here and call sc.MenuEventID to redraw all levels 
		if(p_LineNumbers->size() != 0)
		{
			// LOGIC FOR DELETE ALL DRAWINGS IN PERSISTENT VECTOR AND CLEAR VECTOR 
			for(int i = 0; i < p_LineNumbers->size(); i++)
			{
				sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, (*p_LineNumbers)[i]);
			}

			// clear the vector 
			p_LineNumbers->clear();
		}

		// find all other instances of this study in this chartbook
		std::vector<int> ChartsWhereStudyIsFound = FindDuplicateStudiesInSameChartbook_V2(sc, sc.GraphName.GetChars(), msg);
		if(ChartsWhereStudyIsFound.size() != 0)
		{
			// iterate over the list of charts where the study is found 
			for(int i = 0; i < ChartsWhereStudyIsFound.size(); i++)
			{
				// get the study ID of the study in question (other instance of this study)
				int StudyID = sc.GetStudyIDByName(ChartsWhereStudyIsFound[i], sc.GraphName.GetChars(), 0);

				// check this chart's value for Detect Changes Yes No input value. 
				int IntegerValue = -1;
				int GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DetectChangesInputIndex, IntegerValue);

				// If the user has enabled detecting study input changes for the study instance in question
				if(IntegerValue == 1)
				{
					// Detect and Process study input changes here 
					//
					// NUM DAYS TO CALCULATE
					int IntegerValue = -1;
					int GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, NumDaysInputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_NumDaysToCalculate.GetInt())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, NumDaysInputIndex, i_NumDaysToCalculate.GetInt());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// DST ON OFF 
					// check this chart's value for day light savings yes no input value. 
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DSTInputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_IsDaylightSavings.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DSTInputIndex, i_IsDaylightSavings.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}

					// DRAW WEEKENDS
					// Check this chart's value for draw weekend yes no input value. 
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawWeekendsInputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_DrawLevelsOnSaturday.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawWeekendsInputIndex, i_DrawLevelsOnSaturday.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}

					// Session Start Times
					// NOTE: JUST RELY ON CHART LINKING SESSION TIMES FOR THE MOMENT, Instead of changing them dynamically
					//
					// double TimeValue = -1;
					// GetInput = sc.GetChartStudyInputFloat(ChartsWhereStudyIsFound[i], StudyID, SessionStartTimeInputIndex, TimeValue);

					// msg.Format("Time From Float: %ld, Current Study Input: %ld", TimeValue, i_SessionStartTime.GetTime());
					// sc.AddMessageToLog(msg,1);
					// //
					// // If it does not match what is set in this study, set it for that instance 
					// if(TimeValue != i_SessionStartTime.GetTime())
					// {
					// 	int SetInput = sc.SetChartStudyInputFloat(ChartsWhereStudyIsFound[i], StudyID, SessionStartTimeInputIndex, i_SessionStartTime.GetTime());

					// 	// trigger a chart recalc on the other chart and the
					// 	// persistent variable would change to match the
					// 	// changed input 
					// 	sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
					// 	//
					// 	// Once we have changed the input there is no need to
					// 	// keep iterating over the chart list
					// }

					// DRAW SESSION OPEN
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawSessionOpenInputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_DrawSessionOpen.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawSessionOpenInputIndex, i_DrawSessionOpen.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}

					//
					// DRAW SESSION CLOSE
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawSessionCloseInputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_DrawSessionClose.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawSessionCloseInputIndex, i_DrawSessionClose.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// DRAW US STOCK OPEN
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawStocksOpenInputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_DrawStocksOpen.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawStocksOpenInputIndex, i_DrawStocksOpen.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// DRAW US STOCK CLOSE
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawStocksCloseInputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_DrawStocksClose.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawStocksCloseInputIndex, i_DrawStocksClose.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// DRAW CME OPEN
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawGlobexOpenInputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_DrawGlobexOpen.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawGlobexOpenInputIndex, i_DrawGlobexOpen.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// DRAW CME CLOSE
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawGlobexCloseInputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_DrawGlobexClose.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawGlobexCloseInputIndex, i_DrawGlobexClose.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// DRAW EUROPE OPEN
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawEuropeOpenInputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_DrawEuropeOpen.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawEuropeOpenInputIndex, i_DrawEuropeOpen.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// DRAW EUROPE CLOSE 
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawEuropeCloseInputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_DrawEuropeClose.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawEuropeCloseInputIndex, i_DrawEuropeClose.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// DRAW NY MIDNGIHT
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawMidnightInputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_DrawMidnightPrice.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawMidnightInputIndex, i_DrawMidnightPrice.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// DRAW UTC MIDNIGHT
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawUTCInputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_DrawUTCOpenPrice.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, DrawUTCInputIndex, i_DrawUTCOpenPrice.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}

					// GRAPHICS SETTINGS 
					// Font Size For Line Drawings 
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, FontSizeInputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_FontSizeForLineText.GetInt())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, FontSizeInputIndex, i_FontSizeForLineText.GetInt());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// Intraday Low Low Line Color
					int ColorValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, IntradayHighLowColor_InputIndex, ColorValue);

					// If it does not match what is set in this study, set it for that instance 
					if(ColorValue != i_IntradayHighLowColor.GetColor())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							IntradayHighLowColor_InputIndex, i_IntradayHighLowColor.GetColor());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					
					// Intraday High Low Line Style
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, IntradayHighLowLineStyle_InputIndex, IntegerValue);

					// msg.Format("StringIntValueFromOtherStudy: %d String Value in Current Study: %d", 
					// IntegerValue, i_IntradayHighLowLineStyle.GetIndex());
					// sc.AddMessageToLog(msg,1);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_IntradayHighLowLineStyle.GetIndex())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							IntradayHighLowLineStyle_InputIndex, i_IntradayHighLowLineStyle.GetIndex());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Intraday High Low Line Width
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, IntradayHighLowLineWidth_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_IntradayHighLowLineWidth.GetInt())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							IntradayHighLowLineWidth_InputIndex, i_IntradayHighLowLineWidth.GetInt());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}

					// Session open color (int)
					ColorValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, SessionOpenColor_InputIndex, ColorValue);

					// If it does not match what is set in this study, set it for that instance 
					if(ColorValue != i_SessionOpenColor.GetColor())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							SessionOpenColor_InputIndex, i_SessionOpenColor.GetColor());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// Session open line style
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, SessionOpenLineStyle_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_SessionOpenLineStyle.GetIndex())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							SessionOpenLineStyle_InputIndex, i_SessionOpenLineStyle.GetIndex());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Session open line width (int)
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, SessionOpenLineWidth_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_SessionOpenLineWidth.GetInt())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							SessionOpenLineWidth_InputIndex, i_SessionOpenLineWidth.GetInt());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}

					//
					// Session close color (int)
					ColorValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, SessionCloseColor_InputIndex, ColorValue);

					// If it does not match what is set in this study, set it for that instance 
					if(ColorValue != i_SessionCloseColor.GetColor())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							SessionCloseColor_InputIndex, i_SessionCloseColor.GetColor());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// Session close line style
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, SessionCloseLineStyle_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_SessionCloseLineStyle.GetIndex())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							SessionCloseLineStyle_InputIndex, i_SessionCloseLineStyle.GetIndex());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Session close line width (int)
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, SessionCloseLineWidth_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_SessionCloseLineWidth.GetInt())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							SessionCloseLineWidth_InputIndex, i_SessionCloseLineWidth.GetInt());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Stocks open color (int)
					ColorValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, StocksOpenColor_InputIndex, ColorValue);

					// If it does not match what is set in this study, set it for that instance 
					if(ColorValue != i_USStocksOpenPriceColor.GetColor())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							StocksOpenColor_InputIndex, i_USStocksOpenPriceColor.GetColor());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// Stocks open style
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, StocksOpenLineStyle_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_USStocksOpenPriceLineStyle.GetIndex())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							StocksOpenLineStyle_InputIndex, i_USStocksOpenPriceLineStyle.GetIndex());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}

					// Stocks open width (int)
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, StocksOpenLineWidth_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_USStocksOpenPriceLineWidth.GetInt())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							StocksOpenLineWidth_InputIndex, i_USStocksOpenPriceLineWidth.GetInt());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Stocks close color (int)
					ColorValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, StocksCloseColor_InputIndex, ColorValue);

					// If it does not match what is set in this study, set it for that instance 
					if(ColorValue != i_USStocksClosePriceColor.GetColor())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							StocksCloseColor_InputIndex, i_USStocksClosePriceColor.GetColor());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// Stocks close line style
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, StocksCloseLineStyle_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_USStocksClosePriceLineStyle.GetIndex())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							StocksCloseLineStyle_InputIndex, i_USStocksClosePriceLineStyle.GetIndex());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Stocks close width (int)
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, StocksCloseLineWidth_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_USStocksClosePriceLineWidth.GetInt())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							StocksCloseLineWidth_InputIndex, i_USStocksClosePriceLineWidth.GetInt());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Globex open color (int)
					ColorValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, GlobexOpenColor_InputIndex, ColorValue);

					// If it does not match what is set in this study, set it for that instance 
					if(ColorValue != i_CMEOpenPriceColor.GetColor())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							GlobexOpenColor_InputIndex, i_CMEOpenPriceColor.GetColor());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// Globex open Line style
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, GlobexOpenLineStyle_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_CMEOpenPriceLineStyle.GetIndex())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							GlobexOpenLineStyle_InputIndex, i_CMEOpenPriceLineStyle.GetIndex());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Globex open width (int)
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, GlobexOpenLineWidth_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_CMEOpenPriceLineWidth.GetInt())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							GlobexOpenLineWidth_InputIndex, i_CMEOpenPriceLineWidth.GetInt());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Globex close color (int)
					ColorValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, GlobexCloseColor_InputIndex, ColorValue);

					// If it does not match what is set in this study, set it for that instance 
					if(ColorValue != i_CMEClosePriceColor.GetColor())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							GlobexCloseColor_InputIndex, i_CMEClosePriceColor.GetColor());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// Globex close Line style
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, GlobexCloseLineStyle_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_CMEClosePriceLineStyle.GetIndex())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							GlobexCloseLineStyle_InputIndex, i_CMEClosePriceLineStyle.GetIndex());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Globex close width (int)
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, GlobexCloseLineWidth_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_CMEClosePriceLineWidth.GetInt())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							GlobexCloseLineWidth_InputIndex, i_CMEClosePriceLineWidth.GetInt());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Europe Open color (int)
					ColorValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EuropeOpenColor_InputIndex, ColorValue);

					// If it does not match what is set in this study, set it for that instance 
					if(ColorValue != i_EuropeOpenColor.GetColor())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							EuropeOpenColor_InputIndex, i_EuropeOpenColor.GetColor());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// Europe Open Line style
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EuropeOpenLineStyle_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_EuropeOpenLineStyle.GetIndex())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							EuropeOpenLineStyle_InputIndex, i_EuropeOpenLineStyle.GetIndex());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Europe Open width (int)
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EuropeOpenLineWidth_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_EuropeOpenLineWidth.GetInt())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							EuropeOpenLineWidth_InputIndex, i_EuropeOpenLineWidth.GetInt());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Europe Close color (int)
					ColorValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EuropeCloseColor_InputIndex, ColorValue);

					// If it does not match what is set in this study, set it for that instance 
					if(ColorValue != i_EuropeCloseColor.GetColor())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							EuropeCloseColor_InputIndex, i_EuropeCloseColor.GetColor());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// Europe Close Line style
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EuropeCloseLineStyle_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_EuropeCloseLineStyle.GetIndex())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							EuropeCloseLineStyle_InputIndex, i_EuropeCloseLineStyle.GetIndex());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Europe Close width (int)
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EuropeCloseLineWidth_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_EuropeCloseLineWidth.GetInt())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							EuropeCloseLineWidth_InputIndex, i_EuropeCloseLineWidth.GetInt());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// NY Midnight color (int)
					ColorValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, MidnightColor_InputIndex, ColorValue);

					// If it does not match what is set in this study, set it for that instance 
					if(ColorValue != i_MidnightPriceColor.GetColor())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							MidnightColor_InputIndex, i_MidnightPriceColor.GetColor());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// NY Midnight line style
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, MidnightLineStyle_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_MidnightPriceLineStyle.GetIndex())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							MidnightLineStyle_InputIndex, i_MidnightPriceLineStyle.GetIndex());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// NY Midnight width (int)
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, MidnightLineWidth_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_MidnightPriceLineWidth.GetInt())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							MidnightLineWidth_InputIndex, i_MidnightPriceLineWidth.GetInt());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// UTC Midnight color (int)
					ColorValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, UTCColor_InputIndex, ColorValue);

					// If it does not match what is set in this study, set it for that instance 
					if(ColorValue != i_UTCOpenPriceColor.GetColor())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							UTCColor_InputIndex, i_UTCOpenPriceColor.GetColor());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//  UTC Midnight Line style
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, UTCLineStyle_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_UTCOpenPriceLineStyle.GetIndex())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							UTCLineStyle_InputIndex, i_UTCOpenPriceLineStyle.GetIndex());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// UTC Midnight width (int)
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, UTCLineWidth_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_UTCOpenPriceLineWidth.GetInt())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							UTCLineWidth_InputIndex, i_UTCOpenPriceLineWidth.GetInt());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// ACS Button to Hide PD Levels (Int)
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, ACSButtonToHideLevels_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_ACSButtonToHideLevels.GetInt())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							ACSButtonToHideLevels_InputIndex, i_ACSButtonToHideLevels.GetInt());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// ALERTS
					// PD High Low Alert Yes No
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EnableNewHighLowAlert_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_EnableNewHighLowAlert.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EnableNewHighLowAlert_InputIndex, i_EnableNewHighLowAlert.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Session Open Alert Yes No
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EnableSessionOpenAlert_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_EnableSessionOpenAlert.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							EnableSessionOpenAlert_InputIndex, i_EnableSessionOpenAlert.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// Session Close Alert Yes No
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EnableSessionCloseAlert_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_EnableSessionCloseAlert.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							EnableSessionCloseAlert_InputIndex, i_EnableSessionCloseAlert.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Stocks Open Alert Yes No
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EnableStocksOpenPriceAlert_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_EnableStocksOpenPriceAlert.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							EnableStocksOpenPriceAlert_InputIndex, i_EnableStocksOpenPriceAlert.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// Stocks Close Alert Yes No
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EnableStocksClosePriceAlert_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_EnableStocksClosePriceAlert.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							EnableStocksClosePriceAlert_InputIndex, i_EnableStocksClosePriceAlert.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					//
					// Globex Open Alert Yes No
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EnableCMEOpenPriceAlert_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_EnableCMEOpenPriceAlert.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							EnableCMEOpenPriceAlert_InputIndex, i_EnableCMEOpenPriceAlert.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// Globex Close Alert Yes No
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EnableCMEClosePriceAlert_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_EnableCMEClosePriceAlert.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							EnableCMEClosePriceAlert_InputIndex, i_EnableCMEClosePriceAlert.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Europe Open Alert Yes No
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EnableEuropeOpenPriceAlert_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_EnableEuropeOpenPriceAlert.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							EnableEuropeOpenPriceAlert_InputIndex, i_EnableEuropeOpenPriceAlert.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}

					// Europe Close Alert Yes No
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EnableEuropeClosePriceAlert_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_EnableEuropeClosePriceAlert.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							EnableEuropeClosePriceAlert_InputIndex, i_EnableEuropeClosePriceAlert.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// NY Midnight Alert Yes No
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EnableMidnightPriceAlert_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_EnableMidnightPriceAlert.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							EnableMidnightPriceAlert_InputIndex, i_EnableMidnightPriceAlert.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					// UTC Midnight Open Alert Yes No
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, EnableUTCPriceAlert_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_EnableUTCMidnightPriceAlert.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							EnableUTCPriceAlert_InputIndex, i_EnableUTCMidnightPriceAlert.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// ---
					// Alert Number for Study Alerts (int)
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, AlertNumberForAlerts_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_AlertNumberForCrossAlerts.GetInt())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							AlertNumberForAlerts_InputIndex, i_AlertNumberForCrossAlerts.GetInt());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
					//
					// Alert Only Once Per Bar Yes No
					IntegerValue = -1;
					GetInput = sc.GetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, AlertOncePerBar_InputIndex, IntegerValue);

					// If it does not match what is set in this study, set it for that instance 
					if(IntegerValue != i_AlertOnlyOncePerBar.GetBoolean())
					{
						int SetInput = sc.SetChartStudyInputInt(ChartsWhereStudyIsFound[i], StudyID, 
							AlertOncePerBar_InputIndex, i_AlertOnlyOncePerBar.GetBoolean());

						// trigger a chart recalc on the other chart and the
						// persistent variable would change to match the
						// changed input 
						sc.RecalculateChart(ChartsWhereStudyIsFound[i]);
						//
						// Once we have changed the input there is no need to
						// keep iterating over the chart list
					}
				}
				else
				{
					// did not enable detecting for this chart, continue to next chart 
					continue;
				}
			}
		}

		// Reset Persistent Variables 
		LowOfDayMemory = 0;
		HighOfDayMemory = 0;
		LowOfDayLineNumberMemory = 0;
		HighOfDayLineNumberMemory = 0;

		SessionOpenLineNumberMemory = 0;
		SessionCloseLineNumberMemory = 0;

		USStocksOpenLineNumberMemory = 0;
		USStocksCloseLineNumberMemory = 0;

		CMEOpenLineNumberMemory = 0;
		CMECloseLineNumberMemory = 0;

		EUOpenPriceLineNumberMemory = 0;
		EUClosePriceLineNumberMemory = 0;

		MidnightPriceLineNumberMemory = 0;
		UTCOpenPriceLineNumberMemory = 0;

		// only execute this if our persistent variables have already been initialized from 0  
		if(SessionEndTimeMemory != 0 && EveningEndTimeMemory != 0)
		{
			// -------------------
			// LOOK FOR INPUT OR CHART SETTINGS CHANGES TO SESSION TIMES 
			// GET THE SESSION START TIME FROM CHART SETTINGS 
			//
			// Chart Settings 
			int SessionStartTime = sc.StartTime1;
			int SessionEndTime = sc.EndTime1;
			int UseEveningSession = sc.UseSecondStartEndTimes;
			int EveningStartTime = sc.StartTime2;
			int EveningEndTime = sc.EndTime2;

			// START TIME 
			// The change came from chart settings 
			if(SessionStartTime != SessionStartTimeMemory)
			{
				// change study input setting to match 
				i_SessionStartTime.SetTime(SessionStartTime);

				// Update Memory 
				SessionStartTimeMemory = SessionStartTime;
			}
			// change came from study input setting
			else if(i_SessionStartTime.GetTime() != SessionStartTimeMemory)
			{
				// change chart settings to match 
				sc.StartTime1 = i_SessionStartTime.GetTime();
				
				// Update Memory 
				SessionStartTimeMemory = i_SessionStartTime.GetTime();
			}

			// END TIME 
			if(SessionEndTime != SessionEndTimeMemory)
			{
				// change study input setting to match 
				i_SessionEndTime.SetTime(SessionEndTime);

				// Update Memory 
				SessionEndTimeMemory = SessionEndTime;
			}
			// change came from study input setting
			else if(i_SessionEndTime.GetTime() != SessionEndTimeMemory)
			{
				// change chart settings to match 
				sc.EndTime1 = i_SessionEndTime.GetTime();
				
				// Update Memory 
				SessionEndTimeMemory = i_SessionEndTime.GetTime();
			}
			
			// USE EVENING SESSION (YES NO)
			if(UseEveningSession != UseEveningSessionMemory)
			{
				// change study input setting to match chart settings 
				i_UseEveningSession.SetYesNo(UseEveningSession);

				// Update Memory 
				UseEveningSessionMemory = UseEveningSession;
			}
			// change came from study input setting
			else if(i_UseEveningSession.GetBoolean() != UseEveningSessionMemory)
			{
				// change chart settings to match 
				sc.UseSecondStartEndTimes = i_UseEveningSession.GetBoolean();
				
				// Update Memory 
				UseEveningSessionMemory = i_UseEveningSession.GetBoolean();
			}

			// EVENING START TIME 
			// The change came from chart settings 
			if(EveningStartTime != EveningStartTimeMemory)
			{
				// change study input setting to match 
				i_EveningSessionStartTime.SetTime(EveningStartTime);

				// Update Memory 
				EveningStartTimeMemory = EveningStartTime;
			}
			// change came from study input setting
			else if(i_EveningSessionStartTime.GetTime() != EveningStartTimeMemory)
			{
				// change chart settings to match 
				sc.StartTime2 = i_EveningSessionStartTime.GetTime();
				
				// Update Memory 
				EveningStartTimeMemory = i_EveningSessionStartTime.GetTime();
			}

			// EVENING END TIME 
			if(EveningEndTime != EveningEndTimeMemory)
			{
				// change study input setting to match 
				i_EveningSessionEndTime.SetTime(EveningEndTime);

				// Update Memory 
				EveningEndTimeMemory = EveningEndTime;
			}
			// change came from study input setting
			else if(i_EveningSessionEndTime.GetTime() != EveningEndTimeMemory)
			{
				// change chart settings to match 
				sc.EndTime2 = i_EveningSessionEndTime.GetTime();
				
				// Update Memory 
				EveningEndTimeMemory = i_EveningSessionEndTime.GetTime();
			}

		}
		return;
	}

	//-----------------------------------------------------------
	// Set the Alert Only Once per Bar Option
	sc.AlertOnlyOncePerBar = i_AlertOnlyOncePerBar.GetBoolean();
	//-----------------------------------------------------------

	// Get the last price for line positioning
	double Last = sc.GetLastPriceForTrading();

	// Obtain the session start and end times from the chart directly 
	
	// sc structure to store chart session times 
	n_ACSIL::s_ChartSessionTimes r_ChartSessionTimes;

	SCDateTime SessionStartTime; // Session Start 
								 // 8AM currencies 
								 //
	SCDateTime SessionEndTime; // Session End,
							   // 3pm ET for Currencies, and bonds
							   // 4pm for Stocks
	SCDateTime EveningStartTime;
	SCDateTime EveningEndTime;

	int UseEveningSessionTimes;
	int NewBarAtSessionStart;
	int LoadWeekendDataSetting;

	if(sc.GetSessionTimesFromChart(sc.ChartNumber, r_ChartSessionTimes))
	{
		// Populate date time objects for session times 
		SessionStartTime = r_ChartSessionTimes.StartTime;
		SessionEndTime = r_ChartSessionTimes.EndTime;

		// Session Open and Close already account for daylight savings
		// automatically since we get them from the chart and Sierra Chart
		// adjusts for this. 

		EveningStartTime = r_ChartSessionTimes.EveningStartTime;
		EveningEndTime = r_ChartSessionTimes.EveningEndTime;

		// ints
		UseEveningSessionTimes = r_ChartSessionTimes.UseEveningSessionTimes;
		NewBarAtSessionStart = r_ChartSessionTimes.NewBarAtSessionStart;
		LoadWeekendDataSetting = r_ChartSessionTimes.LoadWeekendDataSetting;
	}

	// This function is supposed to return the time of the chart so it
	// adjusts to chart replay too.
	SCDateTime CurrentTime = sc.GetCurrentDateTime();

	int CurrentDay = CurrentTime.GetDate();

	if(CurrentDay == 0)
	{
		// This resolves a glitch when using chart replay 
		return;

		// When doing a chart replay and the replay is paused and the button is
		// pressed to go back a bar. the CurrentDay variable returns a 0 which
		// represents 0 days from epoch. When this vlue is returned we just
		// quit the study function until it returns a valid day number again. 
		//
		// Or else it results in many drawings being drawn incorrectly due to the Day being wrong.
	}

	// SCString ChartTZ = sc.GetChartTimeZone(sc.ChartNumber);
	// msg.Format("Chart Timezone: %s", ChartTZ.GetChars());
	// sc.AddMessageToLog(msg,1);

	//  we start by putting in those times in UTC time then converting them to the chart timezone 
	//
	//  8:00 AM London Time = EU OPEN
	//  16:30 PM London Time = EU CLose 
	//
	//	Right now UTC Time is same as london time 
	//	This may change during daylight savings time 
	//
	//	22:00 PM UTC time = CME Futures Close 
	//
	// Set those specific times using either a sierra chart function or manually 
	// convert the date time of the record in
	
	// additional times 
	SCDateTime USStocksOpenTime, 
			   USStocksCloseTime, 
			   CMEOpenTime, 
			   CMECloseTime, 
			   EUOpenTime, 
			   EUCloseTime, 
			   MidnightPriceTime, 
			   UTCOpenPriceTime;
										   
	// Maybe instead of hardcoding these we can set them relative to the session open or close times. 
	// 
	// For now I will adjust all of these times down by one hour and use the input setting to account for this 
	USStocksOpenTime.SetDate(CurrentDay);
	USStocksCloseTime.SetDate(CurrentDay);
	CMEOpenTime.SetDate(CurrentDay);
	CMECloseTime.SetDate(CurrentDay);
	EUOpenTime.SetDate(CurrentDay);
	EUCloseTime.SetDate(CurrentDay);
	MidnightPriceTime.SetDate(CurrentDay);
	UTCOpenPriceTime.SetDate(CurrentDay);

	// TODO: Better algo to determine if we are in dst or not 
	//
	if(i_IsDaylightSavings.GetBoolean())
	{
		// Adjusted times for daylight savings time 
		USStocksOpenTime.SetTimeHMS(13,30,0); // 13:30 PM UTC Time
		USStocksCloseTime.SetTimeHMS(20,0,0); // 20:00 PM UTC Time
											  //
		CMEOpenTime.SetTimeHMS(22,0,0); // 22:00 PM UTC Time 
		CMECloseTime.SetTimeHMS(21,0,0); // 21:00 PM UTC Time 
										 //
		EUOpenTime.SetTimeHMS(7,0,0); // 7:00 AM UTC Time 
		EUCloseTime.SetTimeHMS(15,30,0); // 15:30 PM UTC Time  
										 //
		MidnightPriceTime.SetTimeHMS(4,0,0); // 4:00 AM UTC Time (04:00:00)
											 //
		UTCOpenPriceTime.SetTimeHMS(0,0,0); // 
		// utc the same daylight savings or not 
	}
	else
	{
		USStocksOpenTime.SetTimeHMS(14,30,0); // 14:30 PM UTC Time
		USStocksCloseTime.SetTimeHMS(21,0,0); // 21:00 PM UTC Time
											  //
		CMEOpenTime.SetTimeHMS(23,0,0); // 23:00 PM UTC Time 
		CMECloseTime.SetTimeHMS(22,0,0); // 22:00 PM UTC Time 
										 //
		EUOpenTime.SetTimeHMS(8,0,0); // 8:00 AM UTC Time 
		EUCloseTime.SetTimeHMS(16,30,0); // 16:30 PM UTC Time  
										 //
		MidnightPriceTime.SetTimeHMS(5,0,0); // 5:00 AM UTC Time (05:00:00)
											 //
		// Either 0,0,0 or 23,0,0, because chart timezone is impacted by dst
		UTCOpenPriceTime.SetTimeHMS(23,0,0); // 0:00 AM UTC Time (00:00:00)
	}
	
	// UTC back to chart timezone 

	SCDateTimeMS ConvertedUSStocksOpenTime = 
		sc.ConvertDateTimeToChartTimeZone(USStocksOpenTime, TIMEZONE_UTC);

	SCDateTimeMS ConvertedUSStocksCloseTime = 
		sc.ConvertDateTimeToChartTimeZone(USStocksCloseTime, TIMEZONE_UTC);

	SCDateTimeMS ConvertedCMEOpenTime = 
		sc.ConvertDateTimeToChartTimeZone(CMEOpenTime, TIMEZONE_UTC);

	SCDateTimeMS ConvertedCMECloseTime = 
		sc.ConvertDateTimeToChartTimeZone(CMECloseTime, TIMEZONE_UTC);

	SCDateTimeMS ConvertedEUOpenTime = 
		sc.ConvertDateTimeToChartTimeZone(EUOpenTime, TIMEZONE_UTC);

	SCDateTimeMS ConvertedEUCloseTime = 
		sc.ConvertDateTimeToChartTimeZone(EUCloseTime, TIMEZONE_UTC);

	SCDateTimeMS ConvertedMidnightPrice = 
		sc.ConvertDateTimeToChartTimeZone(MidnightPriceTime, TIMEZONE_UTC);
	
	SCDateTimeMS ConvertedUTCOpenPrice = 
		sc.ConvertDateTimeToChartTimeZone(UTCOpenPriceTime, TIMEZONE_UTC);

	// Calculate the starting day for high/lows
	int NumDaysToCalculate = i_NumDaysToCalculate.GetInt();

	// get the start dates for calculation
	int HighLowStartDate = CurrentDay - NumDaysToCalculate;
	
	// debug 
	// msg.Format("Current Day: %d HighLow Start Date: %d NumDaysCalculateHighLow: %d", 
	// CurrentDay, HighLowStartDate, NumDaysToCalculate);
	// sc.AddMessageToLog(msg,1);

	// Assign that day to SCDateTime variables so we can call the next function 
	SCDateTime HighLowStartDateTime(HighLowStartDate,0);

	// I will use this function start iterating from the correct bar index for each day 
	int HighLowStartDateTimeIndex = 
		sc.GetContainingIndexForSCDateTime(sc.ChartNumber, HighLowStartDateTime);

	// DONE: Keep track of the day, when the day moves to the next day, stop
	// redrawing the old high lows and start calculating for a new high low 
	//
	// The high lows should only be drawn once the function has iterated through all the bars 
	// of a given day . 

	// for reset purposes later
	double DefaultHighValue = -999999999999;
	double DefaultLowValue = 999999999999;

	// Arbitrary high and low start values, could probably do something
	// arbitrary start values to ensure we can handle a wide variately of
	// prices including negative priced markets.
	double Low = DefaultLowValue;
	double High = DefaultHighValue;

	// remember the bar indexes 
	int LowIndex = 0;
	int HighIndex = 0;

	int LastDrawnHighLowDate = 0; 
	bool HighLowPriceDrawn = 0;

	// SCDateTime Date (number of days since 1899) 
	// used to determine if we need to be looking for open/close prices 
	int CurrentBarDate;
	int CurrentBarStartTime = 0;
	int CurrentBarEndTime = 0;

	// ---------------------------------------
	// Session Open Time Setup 
	// Get the Time in Seconds Number for Session Start  
	int SessionStartTimeInSeconds = SessionStartTime.GetTimeInSeconds();
	int LastDrawnOpenDate = 0; 
	bool SessionOpenPriceDrawn = 0;
	// -------------------------
	//
	// Session Close Time Setup 
	//
	// Get the Time in Seconds Number for Session End Time 
	int SessionEndTimeInSeconds = SessionEndTime.GetTimeInSeconds();
	int LastDrawnSessionCloseDate = 0; 
	bool SessionClosePriceDrawn = 0;

	// If session end time is set to 1 second before a stop of the hour, make it top of the hour,
	// to greatly facilitate our drawings 
	if (SessionEndTimeInSeconds % 100 == 99)
	{
		SessionEndTimeInSeconds++;
	}

	// -----------------------------------
	//  Variables for US Stocks Open Time 
	int USStocksOpenTimeInSeconds = ConvertedUSStocksOpenTime.GetTimeInSeconds();
	int LastDrawnUSStocksOpenDate = 0; 
	bool USStocksOpenPriceDrawn = 0;
	// -----------------------------------
	//  Variables for US Stocks Close Time 
	int USStocksCloseTimeInSeconds = ConvertedUSStocksCloseTime.GetTimeInSeconds();
	int LastDrawnUSStocksCloseDate = 0; 
	bool USStocksClosePriceDrawn = 0;
	// -----------------------------------
	//  Variables for CME Open Time 
	int CMEOpenTimeInSeconds = ConvertedCMEOpenTime.GetTimeInSeconds();
	int LastDrawnCMEOpenDate = 0; 
	bool CMEOpenPriceDrawn = 0;
	//-------------------------------------
	//  Variables for CME Close Time 
	int CMECloseTimeInSeconds = ConvertedCMECloseTime.GetTimeInSeconds();
	int LastDrawnCMECloseDate = 0; 
	bool CMEClosePriceDrawn = 0;
	// ------------------------------------
	//  Variables for EU Open time 
	int EUOpenTimeInSeconds = ConvertedEUOpenTime.GetTimeInSeconds();
	int LastDrawnEUOpenDate = 0; 
	bool EUOpenPriceDrawn = 0;
	// ------------------------------------
	// Variables for EU Close time 
	int EUCloseTimeInSeconds = ConvertedEUCloseTime.GetTimeInSeconds();
	int LastDrawnEUCloseDate = 0; 
	bool EUClosePriceDrawn = 0;
	// ------------------------------------
	// Variables for Midnight Price 
	int MidnightPriceInSeconds = ConvertedMidnightPrice.GetTimeInSeconds();
	int LastDrawnMidnightPriceDate = 0; 
	bool MidnightPriceDrawn = 0;
	// -----------------------------------
	// Variables for UTC Open Price 
	int UTCOpenPriceInSeconds = ConvertedUTCOpenPrice.GetTimeInSeconds();
	int LastDrawnUTCOpenPriceDate = 0; 
	bool UTCOpenPriceDrawn = 0;
	//
	// Variable used for passing in line names for line labels 
	SCString LineName = "";

	
	// LINES GET DRAWN IF THEY DO NOT EXIST OR IF ACS BUTTON IS EXPLICITLY PRESSED 
	// ACS REDRAW BUTTON SCOPE
	// Pressing this ACS Button will Draw or Delete and Redraw all lines that
	// this study drew
	if (sc.MenuEventID != 0 && sc.MenuEventID == i_ACSButtonToDrawLevels.GetInt()
		|| p_LineNumbers->size() == 0)
	{
		// Logic to reset ACS button 
		const int ButtonState = (sc.PointerEventType == SC_ACS_BUTTON_ON) ? 1 : 0;
		if (ButtonState == 1)
		{
			// Turn off the button right away when pressed 
			sc.SetCustomStudyControlBarButtonEnable(i_ACSButtonToDrawLevels.GetInt(), 0);
		}

		// LOGIC FOR DELETE ALL DRAWINGS IN PERSISTENT VECTOR AND CLEAR VECTOR 
		if(p_LineNumbers->size() != 0)
		{
			// int NumDrawings = 0;
			for(int i = 0; i < p_LineNumbers->size(); i++)
			{
				sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, (*p_LineNumbers)[i]);
				// NumDrawings++;
			}

			// clear the vector 
			p_LineNumbers->clear();
		}

		// TESTING CODE HERE THAT RUNS ON EVERY TICK 
		//-------------------
		// // get the session start and end time variables 
		// int st1 = sc.StartTime1;
		// int et1 = sc.EndTime1;

		// // TODO: Use Evening Session Yes No 
		// if(sc.UseSecondStartEndTimes)
		// {
		// 	// handle handle overnight start end times 
		// 	int st2 = sc.StartTime2;
		// 	int et2 = sc.EndTime2;

		// }

		// // Set the time into a SCDateTime variable for display 
		// SCDateTime StartTime;
		// StartTime.SetTime(st1);

		// // define integers to obtain our time 
		// int Hour, Minute, Second;
		// StartTime.GetTimeHMS(Hour, Minute, Second);

		// // Format the text for the image file name 
		// // The source string combined with the current date time 
		// msg.Format("start time: %d, converted start time: %d-%d-%d", st1, Hour, Minute, Second);

		// // msg.Format("start time: %d end time: %d", st1, et1);
		// sc.AddMessageToLog(msg,1);


		// New Loop Iterating over a set number of days for drawing historical high lows maing use of this function
		// sc.GetOHLCForDate(double Date, float& Open, float& High, float& Low, float& Close)
		for(int Index = HighLowStartDate; Index < CurrentDay+1; Index++)
		{
			// This loop setup only iterates over historical days not the curreny day because we handle that one separately. 
			//
			// Get DateTime from Day 
			// set day and start time 
			SCDateTime HighLowStartDateTime(Index,0);

			// reset these 
			HighIndex = 0;
			LowIndex = 0;

			// msg.Format("Day: %d CurrentDay: %d", Index, CurrentDay);
			// sc.AddMessageToLog(msg,1);

			// Get day of the week 
			SCDateTime TradingDayDate(sc.GetTradingDayDate(Index));
			int DayOfWeek = TradingDayDate.GetDayOfWeek();

			// msg.Format("DayOfWeek: %d", DayOfWeek);
			// sc.AddMessageToLog(msg,1);
			//
			// Get OHLC RTH Hours 
			float Open, High, Low, Close, NextOpen;
			// sc.GetOHLCForDate(HighLowStartDateTime, Open, High, Low, Close);

			// Find the bar index of the high and low 
			//
			// This code needs to be moved to the bottom after we draw our levels. 
			// skip saturday 
			//
			// SCDateTime HighLowStartDateTime(CurrentBarDate -2,0);

			// set day end time 
			SCDateTime DayEndTime;
			DayEndTime.SetTimeHMS(23,59,59);
			SCDateTime HighLowEndDateTime(Index, DayEndTime.GetTimeInSeconds()); 

			// DEBUG 
			// get the year month and day from SCDateTimeVariable
			int Y, M, D, h, m, s;
			HighLowStartDateTime.GetDateTimeYMDHMS(Y,M,D, h, m, s);
			// msg.Format("Day: %d Day of Week Start DateTime: %d %d_%d_%d-%d_%d_%d",Index, DayOfWeek, Y,M,D, h, m, s);
			// sc.AddMessageToLog(msg,1);

			HighLowEndDateTime.GetDateTimeYMDHMS(Y,M,D, h, m, s);

			// msg.Format("Day: %d Day of Week End DateTime: %d %d_%d_%d-%d_%d_%d",Index, DayOfWeek, Y,M,D, h, m, s);
			// sc.AddMessageToLog(msg,1);

			// Better function can get high low for total day instead of just rth 
			sc.GetOHLCOfTimePeriod(HighLowStartDateTime, HighLowEndDateTime, Open, High, Low, Close, NextOpen);

			// Day Start Bar Index 
			int DayStartBarIndex = sc.GetContainingIndexForSCDateTime(sc.ChartNumber, HighLowStartDateTime);

			// Get Day End Bar Index 
			int DayEndBarIndex = sc.GetContainingIndexForSCDateTime(sc.ChartNumber, HighLowEndDateTime);

			// iterate over the chart bars for this day to find the bar indexes that contain high and lows 
			for(int i = DayStartBarIndex; i < DayEndBarIndex+1; i++)
			{
				// get the high of the day (get the first one in case we have a double top)
				if(sc.High[i] == High && HighIndex == 0)
				{
					// we found the index of that high price 
					HighIndex = i;
				}

				// get the low of the day (get the first one in case we have a double bottom)
				if(sc.Low[i] == Low && LowIndex == 0)
				{
					LowIndex = i;
				}
			}

			// Could use this to examine the high and low outside of RTH and determine of those levels should replace our high lows: 
			// sc.GetOHLCOfTimePeriod()
			//
			// i_DrawOverightHighLowsSeparate
			// i_UseOvernightHighLow

			// Debug 
			// msg.Format("Day: %d Day of Week %d O: %f, H: %f, L: %f , C: %f LowIndex: %d HighIndex: %d",Index, DayOfWeek, Open,High,Low,Close, LowIndex, HighIndex);
			// sc.AddMessageToLog(msg,1);

			int SecType = sc.SecurityType();
			// Different logic for different sec types Because some don't show any data for Saturday Sunday 

			// 1 futures: Skip Saturday 
			// 2 stock: Skip Saturday and Sunday 
			// 3 forex/cfds/crypto: Skip Saturday (if input is enabled) but always draw on Sunday 
			//
			// DONE: TEST NEW LOGIC ON DIFFERENT SYMBOLS GOING INTO WEEKEND DATA 
			if(SecType == 1) // futures 
			{
				if(DayOfWeek == SATURDAY)
				{
					// sc.AddMessageToLog("IS saturday!",1);

					// Simply don't draw
					// Index++;
					continue;
				}
				// else if(DayOfWeek == SUNDAY)
				// {


				// }
			}
			else if(SecType == 2) // stock 
			{
				if(DayOfWeek == SATURDAY)
				{
					// sc.AddMessageToLog("Stock IS saturday!",1);

					// Simply don't draw
					// Index++;
					continue;
				}
				else if(DayOfWeek == SUNDAY)
				{
					// sc.AddMessageToLog("Stock IS Sunday!",1);
					// Simply don't draw
					continue;
				}

			}
			else if(SecType == 3) // forex 
			{
				// Looks good 
				if(i_DrawLevelsOnSaturday.GetBoolean() == false)
				{
					if(DayOfWeek == SATURDAY)
					{
						// sc.AddMessageToLog("IS saturday!",1);

						// Simply don't draw
						// Index++;
						continue;
					}
					// else if(DayOfWeek == SUNDAY)
					// {
					// 	// sc.AddMessageToLog("IS Sunday!",1);
					// }
				}
				else
				{
					// do nothing and keep drawing on saturday 
				}
			}

			// draw High and Low 
			s_UseTool LowOfDay;

			// User Drawn Drawing
			LowOfDay.AddAsUserDrawnDrawing = 1;
			LowOfDay.AllowCopyToOtherCharts = 1;
			LowOfDay.LockDrawing = 1;

			// LowOfDay.AllowSaveToChartbook = 1;
			LowOfDay.ChartNumber = sc.ChartNumber;

			LowOfDay.LineNumber = -1;  

			LowOfDay.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
			LowOfDay.LineWidth = i_IntradayHighLowLineWidth.GetInt();
			LowOfDay.LineStyle = static_cast<SubgraphLineStyles>(i_IntradayHighLowLineStyle.GetIndex());
			LowOfDay.DisplayHorizontalLineValue = 1;
			LowOfDay.TransparentLabelBackground = 1;

			// Price Value
			LowOfDay.BeginValue = Low;
			LowOfDay.EndValue = LowOfDay.BeginValue;

			// to check this 
			// GET BAR INDEX 
			LowOfDay.BeginIndex = LowIndex;
			LowOfDay.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

			LowOfDay.AddMethod = UTAM_ADD_OR_ADJUST;
			LowOfDay.Region = sc.GraphRegion;
			LowOfDay.Color = i_IntradayHighLowColor.GetColor();

			LowOfDay.FontSize = i_FontSizeForLineText.GetInt();

			// Code used for Fixing Line Labels 

			// Date Time Object 
			SCDateTime DrawingStartDate;

			// Convert bar index into a date time variable, number of days since 1899
			DrawingStartDate = sc.BaseDateTimeIn[LowIndex].GetDate();
			
			// get the year month and day from SCDateTimeVariable
			int Year, Month, Day;
			DrawingStartDate.GetDateYMD(Year,Month,Day);

			// Assign the line name for this drawing and pass to function
			LineName = "L";

			// fix line labels 
			VT_FixLineLabels(Month, Day, LowOfDay, msg, Last,LineName);

			// draw 
			sc.UseTool(LowOfDay);

			// remember 
			p_LineNumbers->push_back(LowOfDay.LineNumber);

			// save low of day into memory to be alerted on new low of day 
			LowOfDayLineNumberMemory = LowOfDay.LineNumber;
			LowOfDayMemory = LowOfDay.BeginValue;

			// sc.AddMessageToLog("Low drawn!",1);
			//
			// msg.Format("Low Drawn! Index: %d Price: %f HighLowStartDateTimeIndex: %d, Index: %d", LowIndex, LowOfDay.BeginValue, HighLowStartDateTimeIndex, Index);
			// sc.AddMessageToLog(msg,1);


			// ---------------------------------------------------------------------
			// Now do High of day 
			s_UseTool HighOfDay;

			// User Drawn Drawing
			HighOfDay.AddAsUserDrawnDrawing = 1;
			HighOfDay.AllowCopyToOtherCharts = 1;
			HighOfDay.LockDrawing = 1;
			//
			// HighOfDay.AllowSaveToChartbook = 1;
			//
			HighOfDay.ChartNumber = sc.ChartNumber;
			//
			HighOfDay.LineNumber = -1;  

			HighOfDay.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
			HighOfDay.LineWidth = i_IntradayHighLowLineWidth.GetInt();
			HighOfDay.LineStyle = static_cast<SubgraphLineStyles>(i_IntradayHighLowLineStyle.GetIndex());
			HighOfDay.DisplayHorizontalLineValue = 1;
			HighOfDay.TransparentLabelBackground = 1;

			// Price Value 
			HighOfDay.BeginValue = High;
			HighOfDay.EndValue = HighOfDay.BeginValue;

			// GET BAR INDEX 
			HighOfDay.BeginIndex = HighIndex;
			HighOfDay.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

			HighOfDay.AddMethod = UTAM_ADD_OR_ADJUST;
			HighOfDay.Region = sc.GraphRegion;

			HighOfDay.Color = i_IntradayHighLowColor.GetColor();
			HighOfDay.FontSize = i_FontSizeForLineText.GetInt();

			// Code used for Fixing Line Labels 
			
			// Convert bar index into a date time variable, number of days since 1899
			DrawingStartDate = sc.BaseDateTimeIn[HighIndex].GetDate();
			
			// get the year month and day from SCDateTimeVariable
			DrawingStartDate.GetDateYMD(Year,Month,Day);

			// Assign the line name for this drawing and pass to function
			LineName = "H";

			// Call fix line labels 
			VT_FixLineLabels(Month, Day, HighOfDay, msg, Last, LineName);
			// draw 
			sc.UseTool(HighOfDay);
			// remember 
			p_LineNumbers->push_back(HighOfDay.LineNumber);

			// save low of day into memory to be alerted on new low of day 
			HighOfDayLineNumberMemory = HighOfDay.LineNumber;
			HighOfDayMemory = HighOfDay.BeginValue;

			// sc.AddMessageToLog("High drawn!",1);
			// ESSENTIAL RESET VARIABLES 
			// Set variables to ensure we only get here again on the next day 
			// LastDrawnHighLowDate = sc.BaseDateTimeIn[Index].GetDate();
			// HighLowPriceDrawn = 1;

		}

		// we need this for certain close prices 
		int IDSTU = sc.IntradayDataStorageTimeUnit;

		// Get bar period to get the number of seconds per bar for a time based chart 
		n_ACSIL::s_BarPeriod r_BarPeriod;
		sc.GetBarPeriodParameters(r_BarPeriod);

		int SecondsPerBar = 0;
		// if it is a time based chart 
		if(r_BarPeriod.IntradayChartBarPeriodType == 0)
		{
			SecondsPerBar = r_BarPeriod.IntradayChartBarPeriodParameter1;
		}
		// We are set to a daily chart therefore use .dly
		// NOTE: The code for HIGH LOW is newer and simpler than the code in
		// this following for loop. 
		//
		// This for loop iterates over every chart bar in our range to achieve
		// bar level precision. Complications can occur with weekend levels but
		// right now we have no observed any. 
		//
		// Iterate from the starting point until end of the chart 
		for(int Index = HighLowStartDateTimeIndex; Index < sc.ArraySize; Index++)
		{
			// Get the date
			CurrentBarDate = sc.BaseDateTimeIn.DateAt(Index);

			// Get Start and End Time of the Current Bar 
			CurrentBarStartTime = sc.BaseDateTimeIn.TimeAt(Index);
			CurrentBarEndTime = sc.BaseDataEndDateTime.TimeAt(Index);

			// useful debug 
			// msg.Format("CME Close Time Seconds: %d CurrentBarStartTime: %d CurrentBarEndTime: %d", 
			// 	CMECloseTimeInSeconds, CurrentBarStartTime,CurrentBarEndTime);
			// sc.AddMessageToLog(msg,1);

			// ----------------------------------------------------------------------
			// START SESSION OPEN TIME LOGIC 
			if(i_DrawSessionOpen.GetInt() == 1)
			{
				// if user chose to enable Saturday levels or not 
				if(i_DrawLevelsOnSaturday.GetBoolean() == false)
				{
					// if we are saturday 
					if(sc.BaseDateTimeIn[Index].IsSaturday() == 1)
					{
						if(CurrentBarDate >= LastDrawnOpenDate)
						{
							// NECESSARY: only increment indexes if CurrentBarDate is Greater than or Equal to Last Drawn Open Date 
							//
							// Increment and set
							LastDrawnOpenDate++;
							SessionOpenPriceDrawn = 1;
							Index++;

							// msg.Format("1 Current Bar Date: %d Last Drawn Date: %d", CurrentBarDate, LastDrawnOpenDate);
							// sc.AddMessageToLog(msg,1);
							continue;
						}
					}
				}
				// logic to reset looking for new session open price 
				if(CurrentBarDate > LastDrawnOpenDate && SessionOpenPriceDrawn == 1)
				{
					SessionOpenPriceDrawn = 0;
				}

				// only looks for the openprice if the open price 
				// on the day has not already been drawn 
				if(SessionOpenPriceDrawn == 0)
				{
					// msg.Format("2 Current Bar Date: %d", CurrentBarDate);
					// msg.Format("2 Current Bar Date: %d Last Drawn Date: %d", CurrentBarDate, LastDrawnOpenDate);
					// sc.AddMessageToLog(msg,1);
					// Check if Session End Time exists within the boundaries of our bar 
					bool IsWithinBarTime = 
					(SessionStartTimeInSeconds >= CurrentBarStartTime && 
					 SessionStartTimeInSeconds <= CurrentBarEndTime);

					if(IsWithinBarTime)
					{
						// we're on current settlement bar

						// Draw it using our regular drawing tool 
						s_UseTool SessionOpen;

						// User Drawn Drawings 
						SessionOpen.AddAsUserDrawnDrawing = 1;
						SessionOpen.AllowCopyToOtherCharts = 1;
						SessionOpen.LockDrawing = 1;

						// SessionOpen.AllowSaveToChartbook = 1;
						SessionOpen.ChartNumber = sc.ChartNumber;
						SessionOpen.LineNumber = -1;  

						SessionOpen.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;

						SessionOpen.LineWidth = i_SessionOpenLineWidth.GetInt();
						SessionOpen.LineStyle = static_cast<SubgraphLineStyles>(i_SessionOpenLineStyle.GetIndex());
						SessionOpen.DisplayHorizontalLineValue = 1;

						// Price Value 
						SessionOpen.BeginValue = sc.Open[Index];
						SessionOpen.EndValue = SessionOpen.BeginValue;

						// GET BAR INDEX 
						SessionOpen.BeginIndex = Index;
						SessionOpen.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

						SessionOpen.AddMethod = UTAM_ADD_OR_ADJUST;
						SessionOpen.Region = sc.GraphRegion;

						SessionOpen.Color = i_SessionOpenColor.GetColor();
						SessionOpen.FontSize = i_FontSizeForLineText.GetInt();

						// Code used for Fixing Line Labels 
						
						// Convert bar index into a date time variable, number of days since 1899
						SCDateTime DrawingStartDate = sc.BaseDateTimeIn[Index].GetDate();
						
						// get the year month and day from SCDateTimeVariable
						int Year, Month, Day;
						DrawingStartDate.GetDateYMD(Year,Month,Day);

						// Assign the line name for this drawing and pass to function
						LineName = "";

						// Call fix line labels 
						VT_FixLineLabels(Month, Day, SessionOpen, msg, Last, LineName);

						sc.UseTool(SessionOpen);
						p_LineNumbers->push_back(SessionOpen.LineNumber);

						//-----------------------------------
						// ESSENTIAL RESET VARIABLES 
						// Set variables to ensure we only get here again on the next day 
						LastDrawnOpenDate = sc.BaseDateTimeIn[Index].GetDate();
						SessionOpenPriceDrawn = 1;

						// if this drawing is on the current day
						if(CurrentDay == DrawingStartDate && SessionStartTimeInSeconds != 0)
						{
							// This drawing takes place on current day therefore we should remember its line number 
							// This is to inform the real-time code that the drawing has already been drawn. 

							SessionOpenLineNumberMemory = SessionOpen.LineNumber;
						}
					}
				}
				// Insert code here for weekend handling reset 
				// Check input setting for draw on weekend
			}
			// END SESSION OPEN LOGIC 
			//
			// LOGIC FOR Session Close Price 
			if(i_DrawSessionClose.GetInt() == 1)
			{
				// if we are on a new day and settlement price has 
				// been drawn, we need to reset this variable,
				// so we can again be looking for the settlement price on the new day 
				if(CurrentBarDate > LastDrawnSessionCloseDate && SessionClosePriceDrawn == 1)
				{
					SessionClosePriceDrawn = 0;
					// sc.AddMessageToLog("we get here reset SessionClosePriceDrawn",1);
				}

				// only looks for the settlemnt price if the settlement price 
				// on the day has not already been drawn 
				if(SessionClosePriceDrawn == 0)
				{
					// Check if Session End Time exists within the boundaries of our bar 
					bool IsWithinBarTime = 
					(SessionEndTimeInSeconds >= CurrentBarStartTime && 
					 SessionEndTimeInSeconds <= CurrentBarEndTime);

					if(IsWithinBarTime)
					{
						// we're on current settlement bar

						// Draw it using our regular drawing tool 
						s_UseTool SessionClose;

						// User Drawn Drawings 
						//
						SessionClose.AddAsUserDrawnDrawing = 1;
						SessionClose.AllowCopyToOtherCharts = 1;
						SessionClose.LockDrawing = 1;

						// SessionClose.AllowSaveToChartbook = 1;
						SessionClose.ChartNumber = sc.ChartNumber;
						SessionClose.LineNumber = -1;  

						SessionClose.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
						SessionClose.LineWidth = i_SessionCloseLineWidth.GetInt();
						SessionClose.LineStyle = static_cast<SubgraphLineStyles>(i_SessionCloseLineStyle.GetIndex());
						SessionClose.DisplayHorizontalLineValue = 1;

						// Price Value 
						SessionClose.BeginValue = sc.Close[Index-1];
						SessionClose.EndValue = SessionClose.BeginValue;

						// GET BAR INDEX 
						SessionClose.BeginIndex = Index -1;
						SessionClose.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

						SessionClose.AddMethod = UTAM_ADD_OR_ADJUST;
						SessionClose.Region = sc.GraphRegion;

						SessionClose.Color = i_SessionCloseColor.GetColor();

						SessionClose.FontSize = i_FontSizeForLineText.GetInt();

						// Code used for Fixing Line Labels 
						
						// Convert bar index into a date time variable, number of days since 1899
						SCDateTime DrawingStartDate = sc.BaseDateTimeIn[Index].GetDate();
						
						// get the year month and day from SCDateTimeVariable
						int Year, Month, Day;
						DrawingStartDate.GetDateYMD(Year,Month,Day);

						// Assign the line name for this drawing and pass to function
						LineName = "";

						// Call fix line labels 
						VT_FixLineLabels(Month, Day, SessionClose, msg, Last, LineName);

						sc.UseTool(SessionClose);
						p_LineNumbers->push_back(SessionClose.LineNumber);

						//-----------------------------------
						// ESSENTIAL RESET VARIABLES 
						// Set variables to ensure we only get here again on the next day 
						LastDrawnSessionCloseDate = sc.BaseDateTimeIn[Index].GetDate();
						SessionClosePriceDrawn = 1;


						// if this drawing is on the current day
						if(CurrentDay == DrawingStartDate && SessionEndTimeInSeconds != 0)
						{
							// This drawing takes place on current day
							// therefore we should remember its line number 
							// This is to inform the real-time code that the
							// drawing has already been drawn. 

							SessionCloseLineNumberMemory = SessionClose.LineNumber;
						}
					}
					//----------------------------
					// End Session Close Price Logic 
				}
			}

			// START US STOCKS OPEN LOGIC 
			if(i_DrawStocksOpen.GetInt() == 1)
			{
				// START US Stocks Open LOGIC 
				if(CurrentBarDate > LastDrawnUSStocksOpenDate && USStocksOpenPriceDrawn == 1)
				{
					USStocksOpenPriceDrawn = 0;
				}

				// only looks for the open price if the open price 
				// on the day has not already been drawn, DON'T DRAW THIS ON WEEKEND 
				if(USStocksOpenPriceDrawn == 0 && sc.BaseDateTimeIn[Index].IsSunday() != 1 
					&& sc.BaseDateTimeIn[Index].IsSaturday() != 1)
				{
					// ONLY DRAW THIS IF SESSION START TIME IS NOT ALREADY SET TO THIS TIME 
					if(SessionStartTimeInSeconds != USStocksOpenTimeInSeconds)
					{
						// Check if Session End Time exists within the boundaries of our bar 
						bool IsWithinBarTime = 
						(USStocksOpenTimeInSeconds >= CurrentBarStartTime && 
						 USStocksOpenTimeInSeconds <= CurrentBarEndTime);

						bool IsWithinBarTimeCME = 
						(USStocksOpenTimeInSeconds == CurrentBarStartTime && 
						 CurrentBarEndTime == 0 
						 || USStocksOpenTimeInSeconds -1 == CurrentBarStartTime && CurrentBarEndTime == 0);

						if(IsWithinBarTime || IsWithinBarTimeCME)
						{
							// we're on current settlement bar

							// Draw it using our regular drawing tool 
							s_UseTool USStocksOpen;

							// User Drawn Drawings 
							USStocksOpen.AddAsUserDrawnDrawing = 1;
							USStocksOpen.AllowCopyToOtherCharts = 1;
							USStocksOpen.LockDrawing = 1;

							// USStocksOpen.AllowSaveToChartbook = 1;
							USStocksOpen.ChartNumber = sc.ChartNumber;
							USStocksOpen.LineNumber = -1;  

							USStocksOpen.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
							USStocksOpen.LineWidth = i_USStocksOpenPriceLineWidth.GetInt();
							USStocksOpen.LineStyle = static_cast<SubgraphLineStyles>(i_USStocksOpenPriceLineStyle.GetIndex());
							USStocksOpen.DisplayHorizontalLineValue = 1;

							// Price Value 
							USStocksOpen.BeginValue = sc.Open[Index];
							USStocksOpen.EndValue = USStocksOpen.BeginValue;

							// GET BAR INDEX 
							USStocksOpen.BeginIndex = Index;
							USStocksOpen.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;
							USStocksOpen.AddMethod = UTAM_ADD_OR_ADJUST;
							USStocksOpen.Region = sc.GraphRegion;
							USStocksOpen.Color = i_USStocksOpenPriceColor.GetColor();
							USStocksOpen.FontSize = i_FontSizeForLineText.GetInt();

							// Code used for Fixing Line Labels 
							
							// Convert bar index into a date time variable, number of days since 1899
							SCDateTime DrawingStartDate = sc.BaseDateTimeIn[Index].GetDate();
							
							// get the year month and day from SCDateTimeVariable
							int Year, Month, Day;
							DrawingStartDate.GetDateYMD(Year,Month,Day);

							// Assign the line name for this drawing and pass to function
							LineName = "";

							// Call fix line labels 
							VT_FixLineLabels(Month, Day, USStocksOpen, msg, Last, LineName);

							// draw it 
							sc.UseTool(USStocksOpen);

							// remember line number 
							p_LineNumbers->push_back(USStocksOpen.LineNumber);


							// ESSENTIAL RESET VARIABLES 
							// Set variables to ensure we only get here again on the next day 
							LastDrawnUSStocksOpenDate = sc.BaseDateTimeIn[Index].GetDate();
							USStocksOpenPriceDrawn = 1;

							if(CurrentDay == DrawingStartDate && USStocksOpenTimeInSeconds != 0)
							{
								// This drawing takes place on current day therefore we should remember its line number 
								// This is to inform the real-time code that the drawing has already been drawn. 

								USStocksOpenLineNumberMemory = USStocksOpen.LineNumber;
							}
						}
					}
					else
					{
						// ESSENTIAL RESET VARIABLES 
						// Set variables to ensure we only get here again on the next day 
						LastDrawnUSStocksOpenDate = sc.BaseDateTimeIn[Index].GetDate();
						USStocksOpenPriceDrawn = 1;

						// debug 
						// msg.Format("Open Times are the same, no drawing took place",1);
						// sc.AddMessageToLog(msg,1);
					}
				}
			}
			// END US STOCKS OPEN LOGIC 
			//
			//
			// START US STOCKS CLOSE LOGIC 
			if(i_DrawStocksClose.GetInt() == 1)
			{
				// START US Stocks Close LOGIC 
				if(CurrentBarDate > LastDrawnUSStocksCloseDate && USStocksClosePriceDrawn == 1)
				{
					USStocksClosePriceDrawn = 0;
				}

				// only looks for the open price if the open price 
				// on the day has not already been drawn, DON'T DRAW THIS ON WEEKEND 
				if(USStocksClosePriceDrawn == 0 && sc.BaseDateTimeIn[Index].IsSunday() != 1 
					&& sc.BaseDateTimeIn[Index].IsSaturday() != 1)
				{
					// ONLY DRAW THIS IF SESSION END TIME IS NOT ALREADY SET TO THIS TIME 
					if(SessionEndTimeInSeconds != USStocksCloseTimeInSeconds)
					{
						// Check if Session End Time exists within the boundaries of our bar 
						bool IsWithinBarTime = 
						(USStocksCloseTimeInSeconds >= CurrentBarStartTime && 
						 USStocksCloseTimeInSeconds <= CurrentBarEndTime);

						bool IsWithinBarTimeCME = 
						(USStocksCloseTimeInSeconds == CurrentBarStartTime && 
						 CurrentBarEndTime == 0 
						 || USStocksCloseTimeInSeconds -1 == CurrentBarStartTime && CurrentBarEndTime == 0);

						if(IsWithinBarTime || IsWithinBarTimeCME)
						{
							// we're on current settlement bar

							// Draw it using our regular drawing tool 
							s_UseTool USStocksClose;

							// User Drawn Drawings 
							USStocksClose.AddAsUserDrawnDrawing = 1;
							USStocksClose.AllowCopyToOtherCharts = 1;
							USStocksClose.LockDrawing = 1;

							// USStocksClose.AllowSaveToChartbook = 1;
							USStocksClose.ChartNumber = sc.ChartNumber;
							USStocksClose.LineNumber = -1;  

							USStocksClose.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
							USStocksClose.LineWidth = i_USStocksClosePriceLineWidth.GetInt();
							USStocksClose.LineStyle = static_cast<SubgraphLineStyles>(i_USStocksClosePriceLineStyle.GetIndex());
							USStocksClose.DisplayHorizontalLineValue = 1;

							// Price Value 
							USStocksClose.BeginValue = sc.Close[Index];
							USStocksClose.EndValue = USStocksClose.BeginValue;

							// GET BAR INDEX 
							USStocksClose.BeginIndex = Index;
							USStocksClose.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

							USStocksClose.AddMethod = UTAM_ADD_OR_ADJUST;
							USStocksClose.Region = sc.GraphRegion;

							USStocksClose.Color = i_USStocksClosePriceColor.GetColor();
							USStocksClose.FontSize = i_FontSizeForLineText.GetInt();

							// Code used for Fixing Line Labels 
							
							// Convert bar index into a date time variable, number of days since 1899
							SCDateTime DrawingStartDate = sc.BaseDateTimeIn[Index].GetDate();
							
							// get the year month and day from SCDateTimeVariable
							int Year, Month, Day;
							DrawingStartDate.GetDateYMD(Year,Month,Day);

							// Assign the line name for this drawing and pass to function
							LineName = "";

							// Call fix line labels 
							VT_FixLineLabels(Month, Day, USStocksClose, msg, Last, LineName);

							// draw it 
							sc.UseTool(USStocksClose);

							// remember line number 
							p_LineNumbers->push_back(USStocksClose.LineNumber);

							// ESSENTIAL RESET VARIABLES 
							// Set variables to ensure we only get here again on the next day 
							LastDrawnUSStocksCloseDate = sc.BaseDateTimeIn[Index].GetDate();
							USStocksClosePriceDrawn = 1;

							if(CurrentDay == DrawingStartDate && USStocksCloseTimeInSeconds != 0)
							{
								// This drawing takes place on current day therefore we should remember its line number 
								// This is to inform the real-time code that the drawing has already been drawn. 

								USStocksCloseLineNumberMemory = USStocksClose.LineNumber;
							}
						}
					}
					else
					{
						// ESSENTIAL RESET VARIABLES 
						// Set variables to ensure we only get here again on the next day 
						LastDrawnUSStocksCloseDate = sc.BaseDateTimeIn[Index].GetDate();
						USStocksClosePriceDrawn = 1;

						// debug 
						// msg.Format("Close Times are the same, no drawing took place",1);
						// sc.AddMessageToLog(msg,1);

					}
				}
			}
			// END US STOCKS CLOSE LOGIC 
			//
			// START CME OPEN LOGIC 
			if(i_DrawGlobexOpen.GetInt() == 1)
			{
				// START CME Open LOGIC 
				if(CurrentBarDate > LastDrawnCMEOpenDate && CMEOpenPriceDrawn == 1)
				{
					CMEOpenPriceDrawn = 0;
				}

				// only looks for the open price if the open price 
				// on the day has not already been drawn, ALLOW DRAWING ON SUNDAY 
				// if(CMEOpenPriceDrawn == 0 && sc.BaseDateTimeIn[Index].IsSunday() != 1
				// 	&& sc.BaseDateTimeIn[Index].IsSaturday() != 1)
				if(CMEOpenPriceDrawn == 0 && sc.BaseDateTimeIn[Index].IsSaturday() != 1)
				{
					// Check if Session End Time exists within the boundaries of our bar 
					bool IsWithinBarTime = 
					(CMEOpenTimeInSeconds >= CurrentBarStartTime && 
					 CMEOpenTimeInSeconds <= CurrentBarEndTime);

					bool IsWithinBarTimeCME = 
					(CMEOpenTimeInSeconds == CurrentBarStartTime && 
					 CurrentBarEndTime == 0 
					 || CMEOpenTimeInSeconds -1 == CurrentBarStartTime && CurrentBarEndTime == 0);

					if(IsWithinBarTime || IsWithinBarTimeCME)
					{
						// we're on current settlement bar

						// Draw it using our regular drawing tool 
						s_UseTool CMEOpen;

						// User Drawn Drawings 
						CMEOpen.AddAsUserDrawnDrawing = 1;
						CMEOpen.AllowCopyToOtherCharts = 1;
						CMEOpen.LockDrawing = 1;

						// CMEOpen.AllowSaveToChartbook = 1;
						CMEOpen.ChartNumber = sc.ChartNumber;
						CMEOpen.LineNumber = -1;  

						CMEOpen.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
						CMEOpen.LineWidth = i_CMEOpenPriceLineWidth.GetInt();
						CMEOpen.LineStyle = static_cast<SubgraphLineStyles>(i_CMEOpenPriceLineStyle.GetIndex());
						CMEOpen.DisplayHorizontalLineValue = 1;

						// Price Value 
						CMEOpen.BeginValue = sc.Open[Index];
						CMEOpen.EndValue = CMEOpen.BeginValue;

						// GET BAR INDEX 
						CMEOpen.BeginIndex = Index;
						CMEOpen.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

						CMEOpen.AddMethod = UTAM_ADD_OR_ADJUST;
						CMEOpen.Region = sc.GraphRegion;

						CMEOpen.Color = i_CMEOpenPriceColor.GetColor();
						CMEOpen.FontSize = i_FontSizeForLineText.GetInt();

						// Code used for Fixing Line Labels 
						
						// Convert bar index into a date time variable, number of days since 1899
						SCDateTime DrawingStartDate = sc.BaseDateTimeIn[Index].GetDate();
						
						// get the year month and day from SCDateTimeVariable
						int Year, Month, Day;
						DrawingStartDate.GetDateYMD(Year,Month,Day);

						// Assign the line name for this drawing and pass to function
						LineName = "";

						// Call fix line labels 
						VT_FixLineLabels(Month, Day, CMEOpen, msg, Last, LineName);

						// draw it 
						sc.UseTool(CMEOpen);

						// remember line number 
						p_LineNumbers->push_back(CMEOpen.LineNumber);

						// ESSENTIAL RESET VARIABLES 
						// Set variables to ensure we only get here again on the next day 
						LastDrawnCMEOpenDate = sc.BaseDateTimeIn[Index].GetDate();
						CMEOpenPriceDrawn = 1;

						if(CurrentDay == DrawingStartDate && CMEOpenTimeInSeconds != 0)
						{
							// This drawing takes place on current day therefore we should remember its line number 
							// This is to inform the real-time code that the drawing has already been drawn. 

							CMEOpenLineNumberMemory = CMEOpen.LineNumber;
						}
					}
				}
			}
			// END CME OPEN LOGIC 
			//
			// START CME CLOSE LOGIC 
			if(i_DrawGlobexClose.GetInt() == 1)
			{
				// START CME CLOSE LOGIC 
				if(CurrentBarDate > LastDrawnCMECloseDate && CMEClosePriceDrawn == 1)
				{
					CMEClosePriceDrawn = 0;
				}

				// only looks for the open price if the open price 
				// on the day has not already been drawn 
				if(CMEClosePriceDrawn == 0 &&  sc.BaseDateTimeIn[Index].IsSaturday() != 1)
				{
					// Check if Session End Time exists within the boundaries of our bar 
					bool IsWithinBarTime = 
					(CMECloseTimeInSeconds >= CurrentBarStartTime && 
					 CMECloseTimeInSeconds <= CurrentBarEndTime);

					bool IsWithinEndTimeMinusStorageTimeUnit = 
					(CurrentBarEndTime >= CMECloseTimeInSeconds - IDSTU
					 && CurrentBarEndTime <= CMECloseTimeInSeconds);

					// if(CurrentBarStartTime == CMECloseTimeInSeconds - SecondsPerBar)

					bool IsWithinBarTimeCME = 
					(CMECloseTimeInSeconds == CurrentBarStartTime && CurrentBarEndTime == 0 
					 || CMECloseTimeInSeconds -1 == CurrentBarStartTime && CurrentBarEndTime == 0
					 || IsWithinEndTimeMinusStorageTimeUnit
					 || CurrentBarStartTime == CMECloseTimeInSeconds - SecondsPerBar); 
					// second last one is the one that returns true when I have
					// storage time unit set to 5 seconds 
					//
					// otherwise also check if the bar start time is equal to the close time minus seconds per bar 

					if(IsWithinBarTime || IsWithinBarTimeCME)
					{
						// we're on current settlement bar

						// Draw it using our regular drawing tool 
						s_UseTool CMEClose;

						// User Drawn Drawings 
						CMEClose.AddAsUserDrawnDrawing = 1;
						CMEClose.AllowCopyToOtherCharts = 1;
						CMEClose.LockDrawing = 1;

						// CMEClose.AllowSaveToChartbook = 1;
						CMEClose.ChartNumber = sc.ChartNumber;
						CMEClose.LineNumber = -1;  

						CMEClose.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
						CMEClose.LineWidth = i_CMEClosePriceLineWidth.GetInt();
						CMEClose.LineStyle = static_cast<SubgraphLineStyles>(i_CMEClosePriceLineStyle.GetIndex());
						CMEClose.DisplayHorizontalLineValue = 1;

						// Price Value 
						CMEClose.BeginValue = sc.Close[Index];
						CMEClose.EndValue = CMEClose.BeginValue;

						// GET BAR INDEX 
						CMEClose.BeginIndex = Index;
						CMEClose.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

						CMEClose.AddMethod = UTAM_ADD_OR_ADJUST;
						CMEClose.Region = sc.GraphRegion;

						CMEClose.Color = i_CMEClosePriceColor.GetColor();
						CMEClose.FontSize = i_FontSizeForLineText.GetInt();

						// Code used for Fixing Line Labels 
						
						// Convert bar index into a date time variable, number of days since 1899
						SCDateTime DrawingStartDate = sc.BaseDateTimeIn[Index].GetDate();
						
						// get the year month and day from SCDateTimeVariable
						int Year, Month, Day;
						DrawingStartDate.GetDateYMD(Year,Month,Day);

						// Assign the line name for this drawing and pass to function
						LineName = "";

						// Call fix line labels 
						VT_FixLineLabels(Month, Day, CMEClose, msg, Last, LineName);

						// draw it 
						sc.UseTool(CMEClose);

						// remember line number 
						p_LineNumbers->push_back(CMEClose.LineNumber);

						// ESSENTIAL RESET VARIABLES 
						// Set variables to ensure we only get here again on the next day 
						LastDrawnCMECloseDate = sc.BaseDateTimeIn[Index].GetDate();
						CMEClosePriceDrawn = 1;

						if(CurrentDay == DrawingStartDate && CMECloseTimeInSeconds != 0)
						{
							// This drawing takes place on current day therefore we should remember its line number 
							// This is to inform the real-time code that the drawing has already been drawn. 

							CMECloseLineNumberMemory = CMEClose.LineNumber;
						}
					}
				}
			}
			// END CME CLOSE LOGIC

			// START EU OPEN PRICE LOGIC 
			if(i_DrawEuropeOpen.GetInt() == 1)
			{
				if(CurrentBarDate > LastDrawnEUOpenDate && EUOpenPriceDrawn == 1)
				{
					EUOpenPriceDrawn = 0;
				}

				// only looks for the open price if the open price 
				// on the day has not already been drawn 
				if(EUOpenPriceDrawn == 0)
				{
					// Check if Session End Time exists within the boundaries of our bar 
					bool IsWithinBarTime = 
					(EUOpenTimeInSeconds >= CurrentBarStartTime && 
					 EUOpenTimeInSeconds <= CurrentBarEndTime);

					if(IsWithinBarTime)
					{
						// we're on current settlement bar

						// Draw it using our regular drawing tool 
						s_UseTool EUOpen;

						// User Drawn Drawings 
						EUOpen.AddAsUserDrawnDrawing = 1;
						EUOpen.AllowCopyToOtherCharts = 1;
						EUOpen.LockDrawing = 1;

						// EUOpen.AllowSaveToChartbook = 1;
						EUOpen.ChartNumber = sc.ChartNumber;
						EUOpen.LineNumber = -1;  

						EUOpen.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
						EUOpen.LineWidth = i_EuropeOpenLineWidth.GetInt();
						EUOpen.LineStyle = static_cast<SubgraphLineStyles>(i_EuropeOpenLineStyle.GetIndex());
						EUOpen.DisplayHorizontalLineValue = 1;

						// Price Value 
						EUOpen.BeginValue = sc.Close[Index-1];
						EUOpen.EndValue = EUOpen.BeginValue;

						// GET BAR INDEX 
						EUOpen.BeginIndex = Index;
						EUOpen.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

						EUOpen.AddMethod = UTAM_ADD_OR_ADJUST;
						EUOpen.Region = sc.GraphRegion;

						EUOpen.Color = i_EuropeOpenColor.GetColor();
						EUOpen.FontSize = i_FontSizeForLineText.GetInt();

						// Code used for Fixing Line Labels 
						
						// Convert bar index into a date time variable, number of days since 1899
						SCDateTime DrawingStartDate = sc.BaseDateTimeIn[Index].GetDate();
						
						// get the year month and day from SCDateTimeVariable
						int Year, Month, Day;
						DrawingStartDate.GetDateYMD(Year,Month,Day);

						// Assign the line name for this drawing and pass to function
						LineName = "";

						// Call fix line labels 
						VT_FixLineLabels(Month, Day, EUOpen, msg, Last, LineName);

						// draw it 
						sc.UseTool(EUOpen);

						// remember line number 
						p_LineNumbers->push_back(EUOpen.LineNumber);

						//-----------------------------------
						// ESSENTIAL RESET VARIABLES 
						// Set variables to ensure we only get here again on the next day 
						LastDrawnEUOpenDate = sc.BaseDateTimeIn[Index].GetDate();
						EUOpenPriceDrawn = 1;

						if(CurrentDay == DrawingStartDate && EUOpenTimeInSeconds != 0)
						{
							// This drawing takes place on current day therefore we should remember its line number 
							// This is to inform the real-time code that the drawing has already been drawn. 
							EUOpenPriceLineNumberMemory = EUOpen.LineNumber;
						}
					}
				}
			}
			// END EU OPEN PRICE LOGIC 
			
			// START EU CLOSE PRICE LOGIC 
			if(i_DrawEuropeClose.GetInt() == 1)
			{
				if(CurrentBarDate > LastDrawnEUCloseDate && EUClosePriceDrawn == 1)
				{
					EUClosePriceDrawn = 0;
				}

				// only looks for the openprice if the open price 
				// on the day has not already been drawn 
				if(EUClosePriceDrawn == 0)
				{
					// Check if Session End Time exists within the boundaries of our bar 
					bool IsWithinBarTime = 
					(EUCloseTimeInSeconds >= CurrentBarStartTime && 
					 EUCloseTimeInSeconds <= CurrentBarEndTime);

					if(IsWithinBarTime)
					{
						// we're on current settlement bar

						// Draw it using our regular drawing tool 
						s_UseTool EUClose;

						// User Drawn Drawings 
						EUClose.AddAsUserDrawnDrawing = 1;
						EUClose.AllowCopyToOtherCharts = 1;
						EUClose.LockDrawing = 1;

						// EUClose.AllowSaveToChartbook = 1;
						EUClose.ChartNumber = sc.ChartNumber;
						EUClose.LineNumber = -1;  

						EUClose.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
						EUClose.LineWidth = i_EuropeCloseLineWidth.GetInt();
						EUClose.LineStyle = static_cast<SubgraphLineStyles>(i_EuropeCloseLineStyle.GetIndex());
						EUClose.DisplayHorizontalLineValue = 1;

						// Price Value 
						EUClose.BeginValue = sc.Open[Index];
						EUClose.EndValue = EUClose.BeginValue;

						// GET BAR INDEX 
						EUClose.BeginIndex = Index;
						EUClose.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

						EUClose.AddMethod = UTAM_ADD_OR_ADJUST;
						EUClose.Region = sc.GraphRegion;

						EUClose.Color = i_EuropeCloseColor.GetColor();
						EUClose.FontSize = i_FontSizeForLineText.GetInt();

						// Code used for Fixing Line Labels 
						
						// Convert bar index into a date time variable, number of days since 1899
						SCDateTime DrawingStartDate = sc.BaseDateTimeIn[Index].GetDate();
						
						// get the year month and day from SCDateTimeVariable
						int Year, Month, Day;
						DrawingStartDate.GetDateYMD(Year,Month,Day);

						// Assign the line name for this drawing and pass to function
						LineName = "";

						// Call fix line labels 
						VT_FixLineLabels(Month, Day, EUClose, msg, Last, LineName);

						// draw it 
						sc.UseTool(EUClose);

						// remember line number 
						p_LineNumbers->push_back(EUClose.LineNumber);

						//-----------------------------------
						// ESSENTIAL RESET VARIABLES 
						// Set variables to ensure we only get here again on the next day 
						// LastDrawnEUCloseDate = sc.BaseDateTimeIn[Index].GetDate();
						LastDrawnCMECloseDate = sc.BaseDateTimeIn[Index].GetDate();
						EUClosePriceDrawn = 1;

						if(CurrentDay == DrawingStartDate && EUCloseTimeInSeconds != 0)
						{
							// This drawing takes place on current day therefore we should remember its line number 
							// This is to inform the real-time code that the drawing has already been drawn. 

							EUClosePriceLineNumberMemory = EUClose.LineNumber;
						}
					}
				}
			}
			// END EU CLOSE PRICE LOGIC 
			//
			// START MIDNIGHT PRICE LOGIC 
			if(i_DrawMidnightPrice.GetInt() == 1)
			{
				if(CurrentBarDate > LastDrawnMidnightPriceDate && MidnightPriceDrawn == 1)
				// if(LastDrawnMidnightPriceDate < CurrentBarDate && MidnightPriceDrawn == 1)
				{
					MidnightPriceDrawn = 0;
				}

				// msg.Format("MidnightPriceInSeconds: %d, CurrentBarStartTime: %d, CurrentBarEndTime: %d",
				// 	MidnightPriceInSeconds, CurrentBarStartTime, CurrentBarEndTime);
				// sc.AddMessageToLog(msg,1);

				// only looks to draw midnight price if the midnight price 
				// on the day has not already been drawn 
				if(MidnightPriceDrawn == 0)
				{
					// special logic for midnight price compared to other levels 
					// Might still present problems with intraday storage time unit 
					if(CurrentBarStartTime == MidnightPriceInSeconds) 
					{
						// Go back a bar and draw the midnight price
						//
						// Debug 

						// Draw it using our regular drawing tool 
						s_UseTool MidnightPrice;

						// User Drawn Drawings 
						MidnightPrice.AddAsUserDrawnDrawing = 1;
						MidnightPrice.AllowCopyToOtherCharts = 1;
						MidnightPrice.LockDrawing = 1;

						// MidnightPrice.AllowSaveToChartbook = 1;
						MidnightPrice.ChartNumber = sc.ChartNumber;
						MidnightPrice.LineNumber = -1;  

						MidnightPrice.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
						MidnightPrice.LineWidth = i_MidnightPriceLineWidth.GetInt();
						// MidnightPrice.LineStyle = LINESTYLE_DASHDOTDOT;
						MidnightPrice.LineStyle = static_cast<SubgraphLineStyles>(i_MidnightPriceLineStyle.GetIndex());
						MidnightPrice.DisplayHorizontalLineValue = 1;

						// Price Value 
						// set value to be close of the previous bar 
						MidnightPrice.BeginValue = sc.Close[Index-1];

						// set end value to the beginning value 
						MidnightPrice.EndValue = MidnightPrice.BeginValue;

						// GET BAR INDEX 
						MidnightPrice.BeginIndex = Index;
						MidnightPrice.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

						MidnightPrice.AddMethod = UTAM_ADD_OR_ADJUST;
						MidnightPrice.Region = sc.GraphRegion;

						MidnightPrice.Color = i_MidnightPriceColor.GetColor();
						MidnightPrice.FontSize = i_FontSizeForLineText.GetInt();

						// Code used for Fixing Line Labels 
						
						// Convert bar index into a date time variable, number of days since 1899
						SCDateTime DrawingStartDate = sc.BaseDateTimeIn[Index].GetDate();
						
						// get the year month and day from SCDateTimeVariable
						int Year, Month, Day;
						DrawingStartDate.GetDateYMD(Year,Month,Day);

						// Assign the line name for this drawing and pass to function
						LineName = "";

						// Call fix line labels 
						VT_FixLineLabels(Month, Day, MidnightPrice, msg, Last, LineName);

						sc.UseTool(MidnightPrice);
						p_LineNumbers->push_back(MidnightPrice.LineNumber);

						//-----------------------------------
						// ESSENTIAL RESET VARIABLES 
						// Set variables to ensure we only get here again on the next day 
						LastDrawnMidnightPriceDate = sc.BaseDateTimeIn[Index].GetDate();
						MidnightPriceDrawn = 1;

						// NOTE: The second condition here adds support for users that have timezone
						// set differeant than NY time. Because in NY time the
						// Midnight time in seconds in 0 therefore it is a new day
						// already so the real-time code below can handle the drawing.
						//
						// In other timezones MidnightPriceinSeconds will not be 0 therefore
						// this will be the way of handling it. 
						if(CurrentDay == DrawingStartDate && MidnightPriceInSeconds != 0)
						{
							// This drawing takes place on current day therefore we should remember its line number 
							// This is to inform the real-time code that the drawing has already been drawn. 
							MidnightPriceLineNumberMemory = MidnightPrice.LineNumber;
						}
					}
				}
			}
			// END MIDNIGHT PRICE LOGIC 
			//
			// START UTC Open PRICE LOGIC 
			if(i_DrawUTCOpenPrice .GetInt() == 1)
			{
				if(CurrentBarDate > LastDrawnUTCOpenPriceDate && UTCOpenPriceDrawn == 1)
				// if(LastDrawnUTCOpenPriceDate < CurrentBarDate && UTCOpenPriceDrawn == 1)
				{
					UTCOpenPriceDrawn = 0;
				}

				// msg.Format("UTCOpenPriceInSeconds: %d, CurrentBarStartTime: %d, CurrentBarEndTime: %d",
				// 	UTCOpenPriceInSeconds, CurrentBarStartTime, CurrentBarEndTime);
				// sc.AddMessageToLog(msg,1);

				// only looks to draw midnight price if the midnight price 
				// on the day has not already been drawn 
				if(UTCOpenPriceDrawn == 0)
				{
					// special logic for midnight price compared to other levels 
					// Might still present problems with intraday storage time unit 
					if(CurrentBarStartTime == UTCOpenPriceInSeconds) 
					{
						// Go back a bar and draw the midnight price
						//
						// Debug 

						// Draw it using our regular drawing tool 
						s_UseTool UTCOpenPrice;

						// User Drawn Drawings 
						UTCOpenPrice.AddAsUserDrawnDrawing = 1;
						UTCOpenPrice.AllowCopyToOtherCharts = 1;
						UTCOpenPrice.LockDrawing = 1;

						// UTCOpenPrice.AllowSaveToChartbook = 1;
						UTCOpenPrice.ChartNumber = sc.ChartNumber;
						UTCOpenPrice.LineNumber = -1;  

						UTCOpenPrice.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
						UTCOpenPrice.LineWidth = i_UTCOpenPriceLineWidth.GetInt();
						// UTCOpenPrice.LineStyle = LINESTYLE_DASHDOTDOT;
						UTCOpenPrice.LineStyle = static_cast<SubgraphLineStyles>(i_UTCOpenPriceLineStyle.GetIndex());
						UTCOpenPrice.DisplayHorizontalLineValue = 1;

						// Price Value 
						// set value to be close of the previous bar 
						UTCOpenPrice.BeginValue = sc.Close[Index-1];

						// set end value to the beginning value 
						UTCOpenPrice.EndValue = UTCOpenPrice.BeginValue;

						// GET BAR INDEX 
						UTCOpenPrice.BeginIndex = Index;
						UTCOpenPrice.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

						UTCOpenPrice.AddMethod = UTAM_ADD_OR_ADJUST;
						UTCOpenPrice.Region = sc.GraphRegion;

						UTCOpenPrice.Color = i_UTCOpenPriceColor.GetColor();
						UTCOpenPrice.FontSize = i_FontSizeForLineText.GetInt();

						// Code used for Fixing Line Labels 
						
						// Convert bar index into a date time variable, number of days since 1899
						SCDateTime DrawingStartDate = sc.BaseDateTimeIn[Index].GetDate();
						
						// get the year month and day from SCDateTimeVariable
						int Year, Month, Day;
						DrawingStartDate.GetDateYMD(Year,Month,Day);

						// Assign the line name for this drawing and pass to function
						LineName = "";

						// Call fix line labels 
						VT_FixLineLabels(Month, Day, UTCOpenPrice, msg, Last, LineName);

						sc.UseTool(UTCOpenPrice);
						p_LineNumbers->push_back(UTCOpenPrice.LineNumber);

						//-----------------------------------
						// ESSENTIAL RESET VARIABLES 
						// Set variables to ensure we only get here again on the next day 
						LastDrawnUTCOpenPriceDate = sc.BaseDateTimeIn[Index].GetDate();
						UTCOpenPriceDrawn = 1;

						// NOTE: The second condition here adds support for users that have timezone
						// set differeant than NY time. Because in NY time the
						// Midnight time in seconds in 0 therefore it is a new day
						// already so the real-time code below can handle the drawing.
						//
						// In other timezones UTCOpenPriceinSeconds will not be 0 therefore
						// this will be the way of handling it. 
						if(CurrentDay == DrawingStartDate && UTCOpenPriceInSeconds != 0)
						{
							// This drawing takes place on current day therefore we should remember its line number 
							// This is to inform the real-time code that the drawing has already been drawn. 
							UTCOpenPriceLineNumberMemory = UTCOpenPrice.LineNumber;
						}
					}
				}
			}
			// END UTC Open PRICE LOGIC 
			 //
			// if(isSunday)
			// {
			// 	// This code needs to be moved to the bottom after we draw our levels. 
			// 	// skip saturday 
			// 	//
			// 	SCDateTime HighLowStartDateTime(CurrentBarDate -2,0);

			// 	// move index 
			// 	// Index = sc.GetContainingIndexForSCDateTime(sc.ChartNumber, HighLowStartDateTime);

			// 	// debug 
			// 	msg.Format("SUNDAY SecType: %d Current Bar Date: %d Last Drawn Date: %d", SecType, CurrentBarDate, LastDrawnHighLowDate);
			// 	sc.AddMessageToLog(msg,1);

			// 	// set variables 
			// 	HighLowPriceDrawn = 1;
			// 	// LastDrawnHighLowDate = CurrentBarDate-3;
			// 	LastDrawnHighLowDate = CurrentBarDate ;

			// 	continue;
			// }
		}

		// debug midnight price 
		// msg.Format("NumResets: %d CurrentBarDate: %d LastDrawnMidnightPriceDate: %d",
		// 	NumResets, CurrentBarDate, LastDrawnMidnightPriceDate);
		// sc.AddMessageToLog(msg,1);
		//
		//
		// DRAWING HIGH LOW HERE IS NO LONGER NECESSARY BECAUSE OUR RECENT HIGH OF DAY GETS DRAWN BY THE LOOP ABOVE 
	}
	// End ACS Button press 

	// START REALTIME LOGIC 
	//
	// Go over array of levels saved from this study 
	//
	// If we reclaim prior day high or low, midnight price etc. 
	//
	// We can iterate through all saved levels in vector and compare which are
	// closest to last price and monitor those.
	

	s_UseTool Line;

	// Get the current chart symbol for alert text 
	SCString CurrentSymbol = sc.Symbol;

	// Text for formatting alert text 
	SCString AlertText;

	// Check levels in array for realtime tests 
	//
	// msg.Format("Array Size: %d", p_LineNumbers->size());
	// sc.AddMessageToLog(msg,1);
	//
	// This for loop 
	for(int i = 0; i < p_LineNumbers->size();i++)
	{
		if(sc.GetUserDrawnDrawingByLineNumber(sc.ChartNumber, (*p_LineNumbers)[i], Line))
		// if(sc.GetACSDrawingByLineNumber(sc.ChartNumber, (*p_LineNumbers)[i], Line))
		{
			// Check for levels based on their color and draw style 
			//
			// SESSION OPEN LINE PARAMETERS 
			if(Line.Color == i_SessionOpenColor.GetColor()
				&& Line.LineStyle == static_cast<SubgraphLineStyles>(i_SessionOpenLineStyle.GetIndex())
				&& Line.LineWidth == i_SessionOpenLineWidth.GetInt())
			{
				// This line is a prior session open 
				// Check if the user enabled alerts for those levels 
				if(i_EnableSessionOpenAlert.GetBoolean())
				{
					// Cross From below 
					// Bar Open Price is less than Line
					// Last price is greater than or equal to Line
					if(sc.Open[sc.ArraySize-1] < Line.BeginValue
						&& sc.Close[sc.ArraySize-1] >= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing Session Open Price (%f) From Below!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 

					}
					// Cross From Above  
					// Bar Open Price is greater than Line 
					// Last Price is less than or equal to Line 
					else if(sc.Open[sc.ArraySize-1] > Line.BeginValue
						&& sc.Close[sc.ArraySize-1] <= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing Session Open Price (%f) From Above!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 
					}
				}
				// else
				// {
				// 	// They did not enable alert 

				// }


			}
			// SESSION CLOSE LINE PARAMETERS 
			else if(Line.Color == i_SessionCloseColor.GetColor()
				&& Line.LineStyle == static_cast<SubgraphLineStyles>(i_SessionCloseLineStyle.GetIndex())
				&& Line.LineWidth == i_SessionCloseLineWidth.GetInt())
			{
				// This line is a prior session close 
				// Check if the user enabled alerts for those levels 
				if(i_EnableSessionCloseAlert.GetBoolean())
				{
					// Cross From below 
					// Bar Open Price is less than Line
					// Last price is greater than or equal to Line
					if(sc.Open[sc.ArraySize-1] < Line.BeginValue
						&& sc.Close[sc.ArraySize-1] >= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing Session Close Price (%f) From Below!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 

					}
					// Cross From Above  
					// Bar Open Price is greater than Line 
					// Last Price is less than or equal to Line 
					else if(sc.Open[sc.ArraySize-1] > Line.BeginValue
						&& sc.Close[sc.ArraySize-1] <= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing Session Close Price (%f) From Above!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 
					}
				}
			}
			// US STOCKS OPEN LINE PARAMETERS 
			else if(Line.Color == i_USStocksOpenPriceColor.GetColor()
				&& Line.LineStyle == static_cast<SubgraphLineStyles>(i_USStocksOpenPriceLineStyle.GetIndex())
				&& Line.LineWidth == i_USStocksOpenPriceLineWidth.GetInt())
			{
				// This line is a prior US Stocks open 
				// Check if the user enabled alerts for those levels 
				if(i_EnableStocksOpenPriceAlert.GetBoolean())
				{
					// Cross From below 
					// Bar Open Price is less than Line
					// Last price is greater than or equal to Line
					if(sc.Open[sc.ArraySize-1] < Line.BeginValue
						&& sc.Close[sc.ArraySize-1] >= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing US Stocks Open Price (%f) From Below!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 

					}
					// Cross From Above  
					// Bar Open Price is greater than Line 
					// Last Price is less than or equal to Line 
					else if(sc.Open[sc.ArraySize-1] > Line.BeginValue
						&& sc.Close[sc.ArraySize-1] <= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing US Stocks Open Price (%f) From Above!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 
					}
				}
			}
			// US STOCKS CLOSE LINE PARAMETERS 
			else if(Line.Color == i_USStocksClosePriceColor.GetColor()
				&& Line.LineStyle == static_cast<SubgraphLineStyles>(i_USStocksClosePriceLineStyle.GetIndex())
				&& Line.LineWidth == i_USStocksClosePriceLineWidth.GetInt())
			{
				// This line is a prior US Stocks close 
				// Check if the user enabled alerts for those levels 
				if(i_EnableStocksClosePriceAlert.GetBoolean())
				{
					// Cross From below 
					// Bar Open Price is less than Line
					// Last price is greater than or equal to Line
					if(sc.Open[sc.ArraySize-1] < Line.BeginValue
						&& sc.Close[sc.ArraySize-1] >= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing US Stocks Close Price (%f) From Below!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 

					}
					// Cross From Above  
					// Bar Open Price is greater than Line 
					// Last Price is less than or equal to Line 
					else if(sc.Open[sc.ArraySize-1] > Line.BeginValue
						&& sc.Close[sc.ArraySize-1] <= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing US Stocks Close Price (%f) From Above!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 
					}
				}
			}
			// CME OPEN LINE PARAMETERS 
			else if(Line.Color == i_CMEOpenPriceColor.GetColor()
				&& Line.LineStyle == static_cast<SubgraphLineStyles>(i_CMEOpenPriceLineStyle.GetIndex())
				&& Line.LineWidth == i_CMEOpenPriceLineWidth.GetInt())
			{
				// This line is a prior CME open 
				// Check if the user enabled alerts for those levels 
				if(i_EnableCMEOpenPriceAlert.GetBoolean())
				{
					// Cross From below 
					// Bar Open Price is less than Line
					// Last price is greater than or equal to Line
					if(sc.Open[sc.ArraySize-1] < Line.BeginValue
						&& sc.Close[sc.ArraySize-1] >= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing CME Open Price (%f) From Below!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 

					}
					// Cross From Above  
					// Bar Open Price is greater than Line 
					// Last Price is less than or equal to Line 
					else if(sc.Open[sc.ArraySize-1] > Line.BeginValue
						&& sc.Close[sc.ArraySize-1] <= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing CME Open Price (%f) From Above!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 
					}
				}
			}
			// CME CLOSE LINE PARAMETERS 
			else if(Line.Color == i_CMEClosePriceColor.GetColor()
				&& Line.LineStyle == static_cast<SubgraphLineStyles>(i_CMEClosePriceLineStyle.GetIndex())
				&& Line.LineWidth == i_CMEClosePriceLineWidth.GetInt())
			{
				// This line is a prior CME Close  
				// Check if the user enabled alerts for those levels 
				if(i_EnableCMEClosePriceAlert.GetBoolean())
				{
					// Cross From below 
					// Bar Open Price is less than Line
					// Last price is greater than or equal to Line
					if(sc.Open[sc.ArraySize-1] < Line.BeginValue
						&& sc.Close[sc.ArraySize-1] >= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing CME Close Price (%f) From Below!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 

					}
					// Cross From Above  
					// Bar Open Price is greater than Line 
					// Last Price is less than or equal to Line 
					else if(sc.Open[sc.ArraySize-1] > Line.BeginValue
						&& sc.Close[sc.ArraySize-1] <= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing CME Close Price (%f) From Above!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 
					}
				}
			}
			// EUROPE OPEN LINE PARAMETERS 
			else if(Line.Color == i_EuropeOpenColor.GetColor()
				&& Line.LineStyle == static_cast<SubgraphLineStyles>(i_EuropeOpenLineStyle.GetIndex())
				&& Line.LineWidth == i_EuropeOpenLineWidth.GetInt())
			{
				// This line is a prior Europe open 
				// Check if the user enabled alerts for those levels 
				if(i_EnableEuropeOpenPriceAlert.GetBoolean())
				{
					// Cross From below 
					// Bar Open Price is less than Line
					// Last price is greater than or equal to Line
					if(sc.Open[sc.ArraySize-1] < Line.BeginValue
						&& sc.Close[sc.ArraySize-1] >= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing Europe Open Price (%f) From Below!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 

					}
					// Cross From Above  
					// Bar Open Price is greater than Line 
					// Last Price is less than or equal to Line 
					else if(sc.Open[sc.ArraySize-1] > Line.BeginValue
						&& sc.Close[sc.ArraySize-1] <= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing Europe Open Price (%f) From Above!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 
					}
				}
			}
			// EUROPE CLOSE LINE PARAMETERS 
			else if(Line.Color == i_EuropeCloseColor.GetColor()
				&& Line.LineStyle == static_cast<SubgraphLineStyles>(i_EuropeCloseLineStyle.GetIndex())
				&& Line.LineWidth == i_EuropeCloseLineWidth.GetInt())
			{
				// This line is a prior Europe Close  
				// Check if the user enabled alerts for those levels 
				if(i_EnableEuropeClosePriceAlert.GetBoolean())
				{
					// Cross From below 
					// Bar Open Price is less than Line
					// Last price is greater than or equal to Line
					if(sc.Open[sc.ArraySize-1] < Line.BeginValue
						&& sc.Close[sc.ArraySize-1] >= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing Europe Close Price (%f) From Below!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 

					}
					// Cross From Above  
					// Bar Open Price is greater than Line 
					// Last Price is less than or equal to Line 
					else if(sc.Open[sc.ArraySize-1] > Line.BeginValue
						&& sc.Close[sc.ArraySize-1] <= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing Europe Close Price (%f) From Above!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 
					}
				}
			}
			// NY MIDNIGHT LINE PARAMETERS 
			else if(Line.Color == i_MidnightPriceColor.GetColor()
				&& Line.LineStyle == static_cast<SubgraphLineStyles>(i_MidnightPriceLineStyle.GetIndex())
				&& Line.LineWidth == i_MidnightPriceLineWidth.GetInt())
			{
				// This line is a prior NY Midnight Price 
				// Check if the user enabled alerts for those levels 
				if(i_EnableMidnightPriceAlert.GetBoolean())
				{
					// Cross From below 
					// Bar Open Price is less than Line
					// Last price is greater than or equal to Line
					if(sc.Open[sc.ArraySize-1] < Line.BeginValue
						&& sc.Close[sc.ArraySize-1] >= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing Midnight Price (%f) From Below!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 

					}
					// Cross From Above  
					// Bar Open Price is greater than Line 
					// Last Price is less than or equal to Line 
					else if(sc.Open[sc.ArraySize-1] > Line.BeginValue
						&& sc.Close[sc.ArraySize-1] <= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing Midnight Price (%f) From Above!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 
					}
				}
			}
			// UTC MIDNIGHT LINE PARAMETERS 
			else if(Line.Color == i_UTCOpenPriceColor.GetColor()
				&& Line.LineStyle == static_cast<SubgraphLineStyles>(i_UTCOpenPriceLineStyle.GetIndex())
				&& Line.LineWidth == i_UTCOpenPriceLineWidth.GetInt())
			{
				// This line is a prior UTC Open
				// Check if the user enabled alerts for those levels 
				if(i_EnableUTCMidnightPriceAlert.GetBoolean())
				{
					// Cross From below 
					// Bar Open Price is less than Line
					// Last price is greater than or equal to Line
					if(sc.Open[sc.ArraySize-1] < Line.BeginValue
						&& sc.Close[sc.ArraySize-1] >= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing UTC Open Price (%f) From Below!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 

					}
					// Cross From Above  
					// Bar Open Price is greater than Line 
					// Last Price is less than or equal to Line 
					else if(sc.Open[sc.ArraySize-1] > Line.BeginValue
						&& sc.Close[sc.ArraySize-1] <= Line.BeginValue)
					{
						// Format alert text 
						AlertText.Format("%s Crossing UTC Open Price (%f) From Above!", CurrentSymbol.GetChars(), Line.BeginValue) ;

						// Create the alert 
						sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
							sc.ArraySize-1, AlertText); 
					}
				}
			}

		}
	}

	// debug 
	// msg.Format("DEBUG: low Memory: %f, Low Line Number: %d, High Memory: %f, High Line Number: %d", LowOfDayMemory, LowOfDayLineNumberMemory, HighOfDayMemory, HighOfDayLineNumberMemory);
	// sc.AddMessageToLog(msg,1);
	//
	// msg.Format("last bar index: %d", sc.ArraySize-1);
	// sc.AddMessageToLog(msg,1);

	// check for new low of day 
	if(sc.Low[sc.ArraySize-1] < LowOfDayMemory)
	{
		// Remember the new low 
		LowOfDayMemory = sc.Low[sc.ArraySize-1];
		
		// drawing object 
		s_UseTool LowOfDay;

		// Possible Need to Get the drawing by the line number since it is user drawn 
		// use saved line number 
		sc.GetUserDrawnDrawingByLineNumber(sc.ChartNumber, LowOfDayLineNumberMemory, LowOfDay);
		// if(sc.GetACSDrawingByLineNumber(sc.ChartNumber, LowOfDayLineNumberMemory,LowOfDay))

		// These three lines are crucial to a successful user drawn drawing modification
		LowOfDay.Clear(); // crucial 
		LowOfDay.LineNumber = LowOfDayLineNumberMemory;  // line number must be set explicitly
		LowOfDay.AddAsUserDrawnDrawing = 1; // Third crucial line 
		LowOfDay.LockDrawing = 1;

		// Update Price
		LowOfDay.BeginValue = sc.Low[sc.ArraySize-1];
		LowOfDay.EndValue = LowOfDay.BeginValue;

		// to check this 
		// GET BAR INDEX 
		LowOfDay.BeginIndex = sc.ArraySize-1;
		LowOfDay.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

		LowOfDay.AddMethod = UTAM_ADD_OR_ADJUST;
		LowOfDay.FontSize = i_FontSizeForLineText.GetInt();

		// draw 
		sc.UseTool(LowOfDay);

		// if alerts are enabled 
		if(i_EnableNewHighLowAlert.GetBoolean())
		{
			// Format Alert Text 
			AlertText.Format("%s New Low Of Day! Price: (%f)", CurrentSymbol.GetChars(), LowOfDay.BeginValue) ;

			// Create New Low of day alert 
			sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
				sc.ArraySize-1, AlertText); 
		}
	}
	// check for new high of day 
	if(sc.High[sc.ArraySize-1] > HighOfDayMemory)
	{
		// remember the new high 
		HighOfDayMemory = sc.High[sc.ArraySize-1];

		// redraw the drawing 
		s_UseTool HighOfDay;

		sc.GetUserDrawnDrawingByLineNumber(sc.ChartNumber, HighOfDayLineNumberMemory, HighOfDay);
		// if(sc.GetACSDrawingByLineNumber(sc.ChartNumber, HighOfDayLineNumberMemory,HighOfDay))

		// These three lines are crucial to a successful user drawn drawing modification
		HighOfDay.Clear();
		HighOfDay.LineNumber = HighOfDayLineNumberMemory;  
		HighOfDay.AddAsUserDrawnDrawing = 1;
		HighOfDay.LockDrawing = 1;

		// Update Price   
		HighOfDay.BeginValue = sc.High[sc.ArraySize-1];
		HighOfDay.EndValue = HighOfDay.BeginValue;

		// Update Bar Index 
		HighOfDay.BeginIndex = sc.ArraySize-1;
		HighOfDay.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

		HighOfDay.FontSize = i_FontSizeForLineText.GetInt();

		// draw
		sc.UseTool(HighOfDay);

		// if alerts are enabled 
		if(i_EnableNewHighLowAlert.GetBoolean())
		{
			// Format Alert Text 
			AlertText.Format("%s New High of Day! Price: (%f)", CurrentSymbol.GetChars(), HighOfDay.BeginValue) ;

			// Create New High of day alert 
			sc.SetAlert(i_AlertNumberForCrossAlerts.GetInt(), 
				sc.ArraySize-1, AlertText); 
		}
	}

	// --------------------------------------------------------------------------------
	// Draw Real-Time session LEVELS as they happen 
	
	// Get Most recent bar start and end time 
	CurrentBarStartTime = sc.BaseDateTimeIn.TimeAt(sc.ArraySize-1);
	CurrentBarEndTime = sc.BaseDataEndDateTime.TimeAt(sc.ArraySize-1);

	// SESSION OPEN
	// Input enabled and logic to determine if we are on the right bar 
	if(i_DrawSessionOpen.GetBoolean() && 
		CurrentBarStartTime == SessionStartTimeInSeconds)
	{
		// Line Number will be saved if it was already drawn 
		if(SessionOpenLineNumberMemory == 0)
		{
			// Draw the session open price 
			s_UseTool SessionOpen;

			// User Drawn Drawings 
			SessionOpen.AddAsUserDrawnDrawing = 1;
			SessionOpen.AllowCopyToOtherCharts = 1;
			SessionOpen.LockDrawing = 1;
			// SessionOpen.AllowSaveToChartbook = 1;
			SessionOpen.ChartNumber = sc.ChartNumber;
			SessionOpen.LineNumber = -1;  

			SessionOpen.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;

			SessionOpen.LineWidth = i_SessionOpenLineWidth.GetInt();
			SessionOpen.LineStyle = static_cast<SubgraphLineStyles>(i_SessionOpenLineStyle.GetIndex());
			SessionOpen.DisplayHorizontalLineValue = 1;

			// Price Value 
			SessionOpen.BeginValue = sc.Open[sc.ArraySize-1];
			SessionOpen.EndValue = SessionOpen.BeginValue;

			// GET BAR INDEX 
			SessionOpen.BeginIndex = sc.ArraySize-1;
			SessionOpen.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

			SessionOpen.AddMethod = UTAM_ADD_OR_ADJUST;
			SessionOpen.Region = sc.GraphRegion;

			SessionOpen.Color = i_SessionOpenColor.GetColor();
			SessionOpen.FontSize = i_FontSizeForLineText.GetInt();

			// Code used for Fixing Line Labels 
			
			// Convert bar index into a date time variable, number of days since 1899
			SCDateTime DrawingStartDate = sc.BaseDateTimeIn[sc.ArraySize-1].GetDate();
			
			// get the year month and day from SCDateTimeVariable
			int Year, Month, Day;
			DrawingStartDate.GetDateYMD(Year,Month,Day);

			// Assign the line name for this drawing and pass to function
			LineName = "";

			// Call fix line labels 
			VT_FixLineLabels(Month, Day, SessionOpen, msg, Last, LineName);

			// draw it 
			sc.UseTool(SessionOpen);

			// remember in vector 
			p_LineNumbers->push_back(SessionOpen.LineNumber);

			// Remember the Line Number so our historical drawings do not redraw it 
			SessionOpenLineNumberMemory = SessionOpen.LineNumber;
		}
	}

	// SESSION ClOSE
	// Input enabled and logic to determine if we are on the right bar 
	if(i_DrawSessionClose.GetBoolean() && 
		CurrentBarStartTime == SessionEndTimeInSeconds)
	{
		// Line Number will be saved if it was already drawn 
		if(SessionCloseLineNumberMemory == 0)
		{
			// Draw it using our regular drawing tool 
			s_UseTool SessionClose;

			// User Drawn Drawings 
			SessionClose.AddAsUserDrawnDrawing = 1;
			SessionClose.AllowCopyToOtherCharts = 1;
			SessionClose.LockDrawing = 1;

			// SessionClose.AllowSaveToChartbook = 1;
			SessionClose.ChartNumber = sc.ChartNumber;
			SessionClose.LineNumber = -1;  

			SessionClose.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
			SessionClose.LineWidth = i_SessionOpenLineWidth.GetInt();
			SessionClose.LineStyle = static_cast<SubgraphLineStyles>(i_SessionCloseLineStyle.GetIndex());
			SessionClose.DisplayHorizontalLineValue = 1;

			// Price Value 
			SessionClose.BeginValue = sc.Close[sc.ArraySize-2];
			SessionClose.EndValue = SessionClose.BeginValue;

			// GET BAR INDEX 
			SessionClose.BeginIndex = sc.ArraySize-2;
			SessionClose.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

			SessionClose.AddMethod = UTAM_ADD_OR_ADJUST;
			SessionClose.Region = sc.GraphRegion;

			SessionClose.Color = i_SessionCloseColor.GetColor();
			SessionClose.FontSize = i_FontSizeForLineText.GetInt();

			// Code used for Fixing Line Labels 
			
			// Convert bar index into a date time variable, number of days since 1899
			SCDateTime DrawingStartDate = sc.BaseDateTimeIn[sc.ArraySize-1].GetDate();
			
			// get the year month and day from SCDateTimeVariable
			int Year, Month, Day;
			DrawingStartDate.GetDateYMD(Year,Month,Day);

			// Assign the line name for this drawing and pass to function
			LineName = "";

			// Call fix line labels 
			VT_FixLineLabels(Month, Day, SessionClose, msg, Last, LineName);

			sc.UseTool(SessionClose);
			p_LineNumbers->push_back(SessionClose.LineNumber);

			// Remember the Line Number so our historical drawings do not redraw it 
			SessionCloseLineNumberMemory = SessionClose.LineNumber;
		}
	}

	// US Stocks OPEN
	// Input enabled and logic to determine if we are on the right bar 
	if(i_DrawStocksOpen.GetBoolean() && 
		CurrentBarStartTime == USStocksOpenTimeInSeconds)
	{
		// Line Number will be saved if it was already drawn 
		if(USStocksOpenLineNumberMemory == 0)
		{
			// Draw the session open price 
			s_UseTool StocksOpen;

			// User Drawn Drawings 
			StocksOpen.AddAsUserDrawnDrawing = 1;
			StocksOpen.AllowCopyToOtherCharts = 1;
			StocksOpen.LockDrawing = 1;

			// StocksOpen.AllowSaveToChartbook = 1;
			StocksOpen.ChartNumber = sc.ChartNumber;
			StocksOpen.LineNumber = -1;  

			StocksOpen.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;

			StocksOpen.LineWidth = i_USStocksOpenPriceLineWidth.GetInt();
			StocksOpen.LineStyle = static_cast<SubgraphLineStyles>(i_USStocksOpenPriceLineStyle.GetIndex());
			StocksOpen.DisplayHorizontalLineValue = 1;

			// Price Value 
			StocksOpen.BeginValue = sc.Open[sc.ArraySize-1];
			StocksOpen.EndValue = StocksOpen.BeginValue;

			// GET BAR INDEX 
			StocksOpen.BeginIndex = sc.ArraySize-1;
			StocksOpen.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

			StocksOpen.AddMethod = UTAM_ADD_OR_ADJUST;
			StocksOpen.Region = sc.GraphRegion;

			StocksOpen.Color = i_USStocksOpenPriceColor.GetColor();
			StocksOpen.FontSize = i_FontSizeForLineText.GetInt();

			// Code used for Fixing Line Labels 
			
			// Convert bar index into a date time variable, number of days since 1899
			SCDateTime DrawingStartDate = sc.BaseDateTimeIn[sc.ArraySize-1].GetDate();
			
			// get the year month and day from SCDateTimeVariable
			int Year, Month, Day;
			DrawingStartDate.GetDateYMD(Year,Month,Day);

			// Assign the line name for this drawing and pass to function
			LineName = "";

			// Call fix line labels 
			VT_FixLineLabels(Month, Day, StocksOpen, msg, Last, LineName);

			// draw it 
			sc.UseTool(StocksOpen);

			// remember in vector 
			p_LineNumbers->push_back(StocksOpen.LineNumber);

			// Remember the Line Number so our historical drawings do not redraw it 
			USStocksOpenLineNumberMemory = StocksOpen.LineNumber;
		}
	}
	
	// US Stocks ClOSE
	// Input enabled and logic to determine if we are on the right bar 
	if(i_DrawStocksClose.GetBoolean() && 
		CurrentBarStartTime == USStocksCloseTimeInSeconds)
	{
		// Line Number will be saved if it was already drawn 
		if(USStocksCloseLineNumberMemory == 0)
		{
			// Draw it using our regular drawing tool 
			s_UseTool StocksClose;

			// User Drawn Drawings 
			StocksClose.AddAsUserDrawnDrawing = 1;
			StocksClose.AllowCopyToOtherCharts = 1;
			StocksClose.LockDrawing = 1;

			// StocksClose.AllowSaveToChartbook = 1;
			StocksClose.ChartNumber = sc.ChartNumber;
			StocksClose.LineNumber = -1;  

			StocksClose.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
			StocksClose.LineWidth = i_USStocksClosePriceLineWidth.GetInt();;
			StocksClose.LineStyle = static_cast<SubgraphLineStyles>(i_USStocksClosePriceLineStyle.GetIndex());
			StocksClose.DisplayHorizontalLineValue = 1;

			// Price Value 
			StocksClose.BeginValue = sc.Close[sc.ArraySize-2];
			StocksClose.EndValue = StocksClose.BeginValue;

			// GET BAR INDEX 
			StocksClose.BeginIndex = sc.ArraySize-2;
			StocksClose.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

			StocksClose.AddMethod = UTAM_ADD_OR_ADJUST;
			StocksClose.Region = sc.GraphRegion;

			StocksClose.Color = i_USStocksClosePriceColor.GetColor();
			StocksClose.FontSize = i_FontSizeForLineText.GetInt();

			// Code used for Fixing Line Labels 
			
			// Convert bar index into a date time variable, number of days since 1899
			SCDateTime DrawingStartDate = sc.BaseDateTimeIn[sc.ArraySize-1].GetDate();
			
			// get the year month and day from SCDateTimeVariable
			int Year, Month, Day;
			DrawingStartDate.GetDateYMD(Year,Month,Day);

			// Assign the line name for this drawing and pass to function
			LineName = "";

			// Call fix line labels 
			VT_FixLineLabels(Month, Day, StocksClose, msg, Last, LineName);

			sc.UseTool(StocksClose);
			p_LineNumbers->push_back(StocksClose.LineNumber);

			// Remember the Line Number so our historical drawings do not redraw it 
			USStocksCloseLineNumberMemory = StocksClose.LineNumber;
		}
	}
	// CME OPEN  
	if(i_DrawGlobexOpen.GetBoolean() && 
		CurrentBarStartTime == CMEOpenTimeInSeconds)
	{
		// Line Number will be saved if it was already drawn 
		if(CMEOpenLineNumberMemory == 0)
		{
			// Draw it using our regular drawing tool 
			s_UseTool CMEOpen;

			// User Drawn Drawings 
			CMEOpen.AddAsUserDrawnDrawing = 1;
			CMEOpen.AllowCopyToOtherCharts = 1;
			CMEOpen.LockDrawing = 1;

			// CMEOpen.AllowSaveToChartbook = 1;
			CMEOpen.ChartNumber = sc.ChartNumber;
			CMEOpen.LineNumber = -1;  

			CMEOpen.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
			CMEOpen.LineWidth = i_CMEOpenPriceLineWidth.GetInt();
			CMEOpen.LineStyle = static_cast<SubgraphLineStyles>(i_CMEOpenPriceLineStyle.GetIndex());
			CMEOpen.DisplayHorizontalLineValue = 1;

			// Price Value 
			CMEOpen.BeginValue = sc.Open[sc.ArraySize-1];
			CMEOpen.EndValue = CMEOpen.BeginValue;

			// GET BAR INDEX 
			CMEOpen.BeginIndex = sc.ArraySize-1;
			CMEOpen.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

			CMEOpen.AddMethod = UTAM_ADD_OR_ADJUST;
			CMEOpen.Region = sc.GraphRegion;

			CMEOpen.Color = i_CMEOpenPriceColor.GetColor();
			CMEOpen.FontSize = i_FontSizeForLineText.GetInt();

			// Code used for Fixing Line Labels 
			
			// Convert bar index into a date time variable, number of days since 1899
			SCDateTime DrawingStartDate = sc.BaseDateTimeIn[sc.ArraySize-1].GetDate();
			
			// get the year month and day from SCDateTimeVariable
			int Year, Month, Day;
			DrawingStartDate.GetDateYMD(Year,Month,Day);

			// Assign the line name for this drawing and pass to function
			LineName = "";

			// Call fix line labels 
			VT_FixLineLabels(Month, Day, CMEOpen, msg, Last, LineName);

			// draw it 
			sc.UseTool(CMEOpen);

			// remember line number 
			p_LineNumbers->push_back(CMEOpen.LineNumber);

			// Remember the Line Number so our historical drawings do not redraw it 
			CMEOpenLineNumberMemory = CMEOpen.LineNumber;
		}
	}
	// CME CLOSE 
	// SOMETIMES THIS CME CLOSE WILL NOT DRAW BECAUSE OF INTRADAY STORAGE TIME UNIT
	// AND BECAUSE THERE IS USUALLY NO BAR AFTER THIS CLOSE BAR. 
	//
	// Note: This functions on most symbols but not CME Futures symbols where
	// the market close is 5pm. When the market re opens, simply recalculate
	// the chart and the historical drawing code will handle the drawing of
	// this level. 
	if(i_DrawGlobexClose.GetBoolean() && 
		CurrentBarStartTime == CMECloseTimeInSeconds)
	{
		// Line Number will be saved if it was already drawn 
		if(CMECloseLineNumberMemory == 0)
		{
			// Draw it using our regular drawing tool 
			s_UseTool CMEClose;

			// User Drawn Drawings 
			CMEClose.AddAsUserDrawnDrawing = 1;
			CMEClose.AllowCopyToOtherCharts = 1;
			CMEClose.LockDrawing = 1;

			// CMEClose.AllowSaveToChartbook = 1;
			CMEClose.ChartNumber = sc.ChartNumber;
			CMEClose.LineNumber = -1;  

			CMEClose.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
			CMEClose.LineWidth = i_CMEClosePriceLineWidth.GetInt();
			CMEClose.LineStyle = static_cast<SubgraphLineStyles>(i_CMEClosePriceLineStyle.GetIndex());
			CMEClose.DisplayHorizontalLineValue = 1;

			// Price Value 
			CMEClose.BeginValue = sc.Close[sc.ArraySize-2];
			CMEClose.EndValue = CMEClose.BeginValue;

			// GET BAR INDEX 
			CMEClose.BeginIndex = sc.ArraySize-2;
			CMEClose.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

			CMEClose.AddMethod = UTAM_ADD_OR_ADJUST;
			CMEClose.Region = sc.GraphRegion;

			CMEClose.Color = i_CMEClosePriceColor.GetColor();
			CMEClose.FontSize = i_FontSizeForLineText.GetInt();

			// Code used for Fixing Line Labels 
			
			// Convert bar index into a date time variable, number of days since 1899
			SCDateTime DrawingStartDate = sc.BaseDateTimeIn[sc.ArraySize-1].GetDate();
			
			// get the year month and day from SCDateTimeVariable
			int Year, Month, Day;
			DrawingStartDate.GetDateYMD(Year,Month,Day);

			// Assign the line name for this drawing and pass to function
			LineName = "";

			// Call fix line labels 
			VT_FixLineLabels(Month, Day, CMEClose, msg, Last, LineName);

			// draw it 
			sc.UseTool(CMEClose);

			// remember line number 
			p_LineNumbers->push_back(CMEClose.LineNumber);

			// Remember the Line Number so our historical drawings do not redraw it 
			CMECloseLineNumberMemory = CMEClose.LineNumber;
		}
	}

	// EU OPEN
	if(i_DrawEuropeOpen.GetBoolean() && 
		CurrentBarStartTime == EUOpenTimeInSeconds)
	{
		// Line Number will be saved if it was already drawn 
		if(EUOpenPriceLineNumberMemory == 0)
		{
			s_UseTool EUOpen;

			// User Drawn Drawings
			EUOpen.AddAsUserDrawnDrawing = 1;
			EUOpen.AllowCopyToOtherCharts = 1;
			EUOpen.LockDrawing = 1;

			// EUOpen.AllowSaveToChartbook = 1;
			EUOpen.ChartNumber = sc.ChartNumber;
			EUOpen.LineNumber = -1;  

			EUOpen.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
			EUOpen.LineWidth = i_EuropeOpenLineWidth.GetInt();
			EUOpen.LineStyle = static_cast<SubgraphLineStyles>(i_EuropeOpenLineStyle.GetIndex());
			EUOpen.DisplayHorizontalLineValue = 1;

			// Price Value 
			EUOpen.BeginValue = sc.Close[sc.ArraySize-2];
			EUOpen.EndValue = EUOpen.BeginValue;

			// GET BAR INDEX 
			EUOpen.BeginIndex = sc.ArraySize-2;
			EUOpen.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

			EUOpen.AddMethod = UTAM_ADD_OR_ADJUST;
			EUOpen.Region = sc.GraphRegion;

			EUOpen.Color = i_EuropeOpenColor.GetColor();
			EUOpen.FontSize = i_FontSizeForLineText.GetInt();

			// Code used for Fixing Line Labels 
			
			// Convert bar index into a date time variable, number of days since 1899
			SCDateTime DrawingStartDate = sc.BaseDateTimeIn[sc.ArraySize-1].GetDate();
			
			// get the year month and day from SCDateTimeVariable
			int Year, Month, Day;
			DrawingStartDate.GetDateYMD(Year,Month,Day);

			// Assign the line name for this drawing and pass to function
			LineName = "";

			// Call fix line labels 
			VT_FixLineLabels(Month, Day, EUOpen, msg, Last, LineName);

			// draw it 
			sc.UseTool(EUOpen);

			// remember line number 
			p_LineNumbers->push_back(EUOpen.LineNumber);

			// Remember the Line Number so our historical drawings do not redraw it 
			EUOpenPriceLineNumberMemory = EUOpen.LineNumber;
		}
	}

	// EU CLOSE
	if(i_DrawEuropeClose.GetBoolean() && 
		CurrentBarStartTime == EUCloseTimeInSeconds)
	{
		// Line Number will be saved if it was already drawn 
		if(EUClosePriceLineNumberMemory == 0)
		{
			s_UseTool EUClose;

			// User Drawn Drawings 
			EUClose.AddAsUserDrawnDrawing = 1;
			EUClose.AllowCopyToOtherCharts = 1;
			EUClose.LockDrawing = 1;
			// EUClose.AllowSaveToChartbook = 1;
			EUClose.ChartNumber = sc.ChartNumber;
			EUClose.LineNumber = -1;  

			EUClose.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
			EUClose.LineWidth = i_EuropeOpenLineWidth.GetInt();
			EUClose.LineStyle = static_cast<SubgraphLineStyles>(i_EuropeCloseLineStyle.GetIndex());
			EUClose.DisplayHorizontalLineValue = 1;

			// Price Value 
			EUClose.BeginValue = sc.Open[sc.ArraySize-1];
			EUClose.EndValue = EUClose.BeginValue;

			// GET BAR INDEX 
			EUClose.BeginIndex = sc.ArraySize-1;
			EUClose.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

			EUClose.AddMethod = UTAM_ADD_OR_ADJUST;
			EUClose.Region = sc.GraphRegion;

			EUClose.Color = i_EuropeCloseColor.GetColor();
			EUClose.FontSize = i_FontSizeForLineText.GetInt();

			// Code used for Fixing Line Labels 
			
			// Convert bar index into a date time variable, number of days since 1899
			SCDateTime DrawingStartDate = sc.BaseDateTimeIn[sc.ArraySize-1].GetDate();
			
			// get the year month and day from SCDateTimeVariable
			int Year, Month, Day;
			DrawingStartDate.GetDateYMD(Year,Month,Day);

			// Assign the line name for this drawing and pass to function
			LineName = "";

			// Call fix line labels 
			VT_FixLineLabels(Month, Day, EUClose, msg, Last, LineName);

			// draw it 
			sc.UseTool(EUClose);

			// remember line number 
			p_LineNumbers->push_back(EUClose.LineNumber);

			// Remember the Line Number so our historical drawings do not redraw it 
			EUClosePriceLineNumberMemory = EUClose.LineNumber;
		}
	}

	// Midnight Price
	if(i_DrawMidnightPrice.GetBoolean() && 
		CurrentBarStartTime == MidnightPriceInSeconds)
	{
		// Line Number will be saved if it was already drawn 
		if(MidnightPriceLineNumberMemory == 0)
		{
			// Draw it using our regular drawing tool 
			s_UseTool MidnightPrice;

			// User Drawn Drawings 
			MidnightPrice.AddAsUserDrawnDrawing = 1;
			MidnightPrice.AllowCopyToOtherCharts = 1;
			MidnightPrice.LockDrawing = 1;

			// MidnightPrice.AllowSaveToChartbook = 1;
			MidnightPrice.ChartNumber = sc.ChartNumber;
			MidnightPrice.LineNumber = -1;  

			MidnightPrice.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
			MidnightPrice.LineWidth = i_MidnightPriceLineWidth.GetInt();
			MidnightPrice.LineStyle = static_cast<SubgraphLineStyles>(i_MidnightPriceLineStyle.GetIndex());
			MidnightPrice.DisplayHorizontalLineValue = 1;

			// Price Value 
			// set value to be close of the previous bar 
			MidnightPrice.BeginValue = sc.Close[sc.ArraySize-2];

			// set end value to the beginning value 
			MidnightPrice.EndValue = MidnightPrice.BeginValue;

			// GET BAR INDEX 
			MidnightPrice.BeginIndex = sc.ArraySize-1;
			MidnightPrice.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

			MidnightPrice.AddMethod = UTAM_ADD_OR_ADJUST;
			MidnightPrice.Region = sc.GraphRegion;

			MidnightPrice.Color = i_MidnightPriceColor.GetColor();
			MidnightPrice.FontSize = i_FontSizeForLineText.GetInt();

			// Code used for Fixing Line Labels 
			
			// Convert bar index into a date time variable, number of days since 1899
			SCDateTime DrawingStartDate = sc.BaseDateTimeIn[sc.ArraySize-1].GetDate();
			
			// get the year month and day from SCDateTimeVariable
			int Year, Month, Day;
			DrawingStartDate.GetDateYMD(Year,Month,Day);

			// Assign the line name for this drawing and pass to function
			LineName = "";

			// Call fix line labels 
			VT_FixLineLabels(Month, Day, MidnightPrice, msg, Last, LineName);

			// draw it 
			sc.UseTool(MidnightPrice);

			// remember line number 
			p_LineNumbers->push_back(MidnightPrice.LineNumber);

			// Remember the Line Number so our historical drawings do not redraw it 
			MidnightPriceLineNumberMemory = MidnightPrice.LineNumber;
		}
	}
	// Midnight UTC Price
	if(i_DrawUTCOpenPrice.GetBoolean() && 
		CurrentBarStartTime == UTCOpenPriceInSeconds)
	{
		// Line Number will be saved if it was already drawn 
		if(UTCOpenPriceLineNumberMemory == 0)
		{
			// Draw it using our regular drawing tool 
			s_UseTool UTCOpen;

			// User Drawn Drawings 
			UTCOpen.AddAsUserDrawnDrawing = 1;
			UTCOpen.AllowCopyToOtherCharts = 1;
			UTCOpen.LockDrawing = 1;

			// UTCOpen.AllowSaveToChartbook = 1;
			UTCOpen.ChartNumber = sc.ChartNumber;
			UTCOpen.LineNumber = -1;  

			UTCOpen.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
			UTCOpen.LineWidth = i_UTCOpenPriceLineWidth.GetInt();
			UTCOpen.LineStyle = static_cast<SubgraphLineStyles>(i_UTCOpenPriceLineStyle.GetIndex());
			UTCOpen.DisplayHorizontalLineValue = 1;

			// Price Value 
			// set value to be close of the previous bar 
			UTCOpen.BeginValue = sc.Close[sc.ArraySize-2];

			// set end value to the beginning value 
			UTCOpen.EndValue = UTCOpen.BeginValue;

			// GET BAR INDEX 
			UTCOpen.BeginIndex = sc.ArraySize-1;
			UTCOpen.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

			UTCOpen.AddMethod = UTAM_ADD_OR_ADJUST;
			UTCOpen.Region = sc.GraphRegion;

			UTCOpen.Color = i_UTCOpenPriceColor.GetColor();
			UTCOpen.FontSize = i_FontSizeForLineText.GetInt();

			// Code used for Fixing Line Labels 
			
			// Convert bar index into a date time variable, number of days since 1899
			SCDateTime DrawingStartDate = sc.BaseDateTimeIn[sc.ArraySize-1].GetDate();
			
			// get the year month and day from SCDateTimeVariable
			int Year, Month, Day;
			DrawingStartDate.GetDateYMD(Year,Month,Day);

			// Assign the line name for this drawing and pass to function
			LineName = "";

			// Call fix line labels 
			VT_FixLineLabels(Month, Day, UTCOpen, msg, Last, LineName);

			// draw it 
			sc.UseTool(UTCOpen);

			// remember line number 
			p_LineNumbers->push_back(UTCOpen.LineNumber);

			// Remember the Line Number so our historical drawings do not redraw it 
			UTCOpenPriceLineNumberMemory = UTCOpen.LineNumber;
		}
	}
	// ACS Button to Hide Drawings 
	if (sc.MenuEventID != 0 && sc.MenuEventID == i_ACSButtonToHideLevels.GetInt())
	{
		int ButtonState = sc.GetCustomStudyControlBarButtonEnableState(i_ACSButtonToHideLevels.GetInt());
		if (ButtonState == 1 && HideLevels != 1)
		{
			// Set this to only get here once 
			HideLevels = 1;

			// grab drawings
			s_UseTool Line;
			for(int i = 0; i < p_LineNumbers->size();i++)
			{
				if(sc.GetUserDrawnDrawingByLineNumber(sc.ChartNumber, (*p_LineNumbers)[i], Line))
				// if(sc.GetACSDrawingByLineNumber(sc.ChartNumber, (*p_LineNumbers)[i], Line))
				{
					// Hide the drawing and redraw 
					Line.HideDrawing = 1;
					sc.UseTool(Line);
				}
			}
		}
		else if(ButtonState == 0 && HideLevels != 0) // drawings already hidden
		{
			// Set this to only get here once 
			HideLevels = 0;

			// grab drawings
			s_UseTool Line;
			for(int i = 0; i < p_LineNumbers->size();i++)
			{
				if(sc.GetUserDrawnDrawingByLineNumber(sc.ChartNumber, (*p_LineNumbers)[i], Line))
				// if(sc.GetACSDrawingByLineNumber(sc.ChartNumber, (*p_LineNumbers)[i], Line))
				{
					// Hide the drawing and redraw 
					Line.HideDrawing = 0;
					sc.UseTool(Line);
				}
			}
		}
	} // End ACS Button 
} // End Study 
  
// Start Telegram Study 
////////////////////////////////////////////////////////////////////////////////////////////////////////
void GetLogsFolderPath(SCStudyInterfaceRef sc, std::string& LogsFolderPath)
{
	// Get the directory of sierra data folder from a function directly into std::string
	LogsFolderPath = sc.DataFilesFolder().GetChars();

	// Get the Starting position of 'SierraChart' text from Data Folder Path 
	std::size_t start_pos = LogsFolderPath.find("SierraChart");

	// find the first backslash after SierraChart (adds support for
	// Sierrachart2,3,4, or any text succeeding it)
	std::size_t end_pos = LogsFolderPath.find('\\', start_pos); 																
	if(start_pos != std::string::npos)
	{
		// Create a string from the existing string that omits the "Data" characters
		// and adds the "Logs" characters to the end of it. 
		 
		// re assign our directory string to the new value
		LogsFolderPath = LogsFolderPath.substr(0, end_pos + 1) + "Logs";
	}
	else 
	{	
		// If we get here it means the Data Files Folder path does not have
		// SierraChart in it.  The user moved it to some other directory. 
		// This means the log file path needs to be specified through the study
		// input.
		//
		sc.AddMessageToLog("Error: text 'SierraChart' not found in Data Folder Path."
		" Please enable Study input #6 and specify the full path to Sierra Chart Install directory in Study Input #7.",1);

		LogsFolderPath = "Invalid_Path";
	}
}
void FindMostRecentFile(SCStudyInterfaceRef sc, const std::string& LogsFolderPath, int& number_of_files, 
	int64_t& Local_LastModTime, std::string& most_recent_filename)
{
	// HANDLE if the folder does not exist before accessing the directory 
	if (!std::filesystem::exists(LogsFolderPath) && !std::filesystem::is_directory(LogsFolderPath)) 
	{
		sc.AddMessageToLog("Error: Logs Folder does not exist! Check if the folder exists in SC install directory."
		" Enable Save Alerts Log to File under General Settings > Log. Then trigger a chart drawing alert and "
		"the directory should be created.",1);
		return;
	}

	// 1. Iterate through the directory to find the number of files that match
	// 2. Find the file with the most recent modification time
	for (const auto& entry : std::filesystem::directory_iterator(LogsFolderPath)) 
	{
		if (entry.is_regular_file()) 
		{
			// save the file name
			std::string filename = entry.path().filename().string();

			// Make sure the file is an Alert Log and the last 3 characters are 'log'
			// to ensure it's not a swap file or some other extension
			if(filename.find("Alert Log") != std::string::npos && 
			filename.substr(filename.length() - 3) == "log")
			{
				// count the number of files in directory
				number_of_files++; 

				// get the last write time of our file	
				std::filesystem::file_time_type CurrentModTime = std::filesystem::last_write_time(entry);

				// convert it to a time in seconds from epoch
				auto CurrentModTimePoint = std::chrono::duration_cast<std::chrono::seconds>
				(CurrentModTime.time_since_epoch()).count();

				//	if this is the first time the study function is run 
				//	save it to check on the next iteration (HANDLES only 1 file present)
				if(Local_LastModTime == 0)
				{
					// save the last modified time
					Local_LastModTime = CurrentModTimePoint;

					// save the filename so we can potentially read it later
					most_recent_filename = std::move(filename);
				}
				else 
				{
					// if there is only one FILE we will not get here!
					// We should get here on the following iterations
					
					// Check if the file modified time is more recent than the last one
					if(CurrentModTimePoint > Local_LastModTime)
					{

						// save the last modified time
						Local_LastModTime = CurrentModTimePoint;

						// save the filename
						most_recent_filename = std::move(filename);

						// debug
						/* msg.Format("print filenames: %s", filename.c_str()); */ 
						/* sc.AddMessageToLog(msg, 1); */
					}
				}
			}
		}
	}
}
int AlertSymbolExistsInChartBook(SCStudyInterfaceRef sc, SCString& AlertSymbol, SCString& msg)
{
	// Get the highest chart number in the current chartbook 
	int highest_chart_num = sc.GetHighestChartNumberUsedInChartBook();

	// get the current chart number for reference
	int this_chart_num = sc.ChartNumber;

	// vector used to save the chart numbers of charts in current chartbook
	std::vector <int> chart_numbers;

	// Go through each chart number from 1 to highest chart number to 
	// determine the chart numbers that exist and save those. 
	for(int ChartNumber = 1; ChartNumber <= highest_chart_num; ChartNumber++)
	{
		// returns true if chart number exists in current chartbook, empty string refers to current chartbook
		if(sc.IsChartNumberExist(ChartNumber, ""))
		{
			// add this chart number to the vector 
			chart_numbers.push_back(ChartNumber);
		}
	}	

	// iterate through the existing number of charts with chart numbers as values  
	for(int i = 0; i < chart_numbers.size(); i++)
	{
		// get symbol of each chart we are checking
		SCString Symbol = sc.GetChartSymbol(chart_numbers[i]);

		// Check if this symbol matches our alert symbol
		if(Symbol != "" && Symbol == AlertSymbol)
		{
			// We have a match therefore use this chart, return the chart number 
			return chart_numbers[i];

			// TODO: 2. Filter out by a specific bar period ie prefer an
			// intraday chart for chart image instead of daily chart 
		}

	}

	// default
	return 0;
}
void FindDuplicateStudiesInSameChartbook(SCStudyInterfaceRef sc, const char* StudyName, SCString& msg)
{
	// Get the highest chart number in the current chartbook 
	int highest_chart_num = sc.GetHighestChartNumberUsedInChartBook();

	// get the current chart number for reference
	int this_chart_num = sc.ChartNumber;

	// vector used to save the chart numbers of charts in current chartbook
	std::vector <int> chart_numbers;

	// Go through each chart number from 1 to highest chart number to 
	// determine the chart numbers that exist and save those. 
	for(int ChartNumber = 1; ChartNumber <= highest_chart_num; ChartNumber++)
	{
		// returns true if chart number exists in current chartbook, empty string refers to current chartbook
		if(sc.IsChartNumberExist(ChartNumber, ""))
		{
			// add this chart number to the vector 
			chart_numbers.push_back(ChartNumber);
		}
	}	

	// iterate through the existing number of charts with chart numbers as values  
	for(int i = 0; i < chart_numbers.size(); i++)
	{
		// Check if the study with this name is found on this chart number
		// arguments: (chart number, study name as a string, search for study short name instead)
		int is_study_found = sc.GetStudyIDByName(chart_numbers[i], StudyName, 0);

		// If the study is found and if the chart it was found on is not the current chart
		if(is_study_found != 0 && chart_numbers[i] != this_chart_num)
		{
			// This should only return true if the study exists on two or more chartbooks
			// print which charts where the study is found
			msg.Format("A duplicate is found on chart #%d. Reduce the number of studies "
			"per chartbook to one unless you wish to recieve duplicate alerts.", chart_numbers[i]);
			sc.AddMessageToLog(msg,1);
		}
	}
}

void ParseChartStudyAlertText(std::string& line, std::string& SourceChartImageText, int& CustomizeMessageContent, 
	SCString& sc_alert_chartbook, int& sc_alert_chart_num)
{
	// String parsing logic 
	// find various text inside the string
	//
	// Get the Source Chart Text String
	std::size_t source_start_pos = line.find("Source");
	std::size_t source_end_pos = line.find('|', source_start_pos);

	// Alert text string (different for Chart Alerts)
	std::size_t text_start_pos = line.find("Formula");
	std::size_t first_pipe_pos = line.find('|', text_start_pos);

	// set the end pos to the second pipe character to capture the alert condition that was triggered.
	// Necessary to set the starting position 1 in front or else it will just find the same one 
	std::size_t text_end_pos = line.find('|', first_pipe_pos + 1);

	// REMOVE / characters from Source Chart Text
	// check over the substring for any forward slash character and kill loop if we reach npos string position 
	for(auto i = source_end_pos; i != source_start_pos && i != std::string::npos; i--)
	{
		if(line[i] == '/')
		{
			// Replace the character at the position i with a whitespace 
			//string& replace (size_t pos, size_t len, const string& str);
        	line.replace(i, 1, " ");
		}
	}

	// Do the same for the Alert Text String
	for(auto i = text_end_pos; i != text_start_pos && i != std::string::npos; i--)
	{
		if(line[i] == '/')
		{
			// Replace the character at the position i with a whitespace 
        	line.replace(i, 1, " ");
		}
	}

	// CREATE image file text using the source chart text iterators 
	SourceChartImageText = line.substr(source_start_pos + 8, source_end_pos - (source_start_pos + 8));

	// REMOVE INVALID CHARACTERS FROM Image File Text for WINDOWS FILENAME 
	// an array of invalid characters for a windows filename
	std::array<char, 9> invalid_filename_characters = {'\\', '/', ':', '*', '?', '"', '<', '>', '|'};

	// for each of the invalid characters 
	for(int index = 0; index < invalid_filename_characters.size(); index++)
	{
		// iterate backwards through the string so we can delete characters from it 
		for(auto i = SourceChartImageText.end(); i != SourceChartImageText.begin(); i--)
		{
			// if current string character is equal to the current element in invalid characters array 
			if(*i == invalid_filename_characters[index])
			{
				// erase this character from the string
				/* SourceChartImageText.erase(i); */
				// replace the invalid character with a whitespace 
				*i = ' ';
			}
		}
	}

	
	// Determine specifically if it is a Chart or Study alert 
	// This will be done further down by comparing the source string with study text string
	
	// This iterator gives us a starting point to retrieve the Study Name (short name). 
	std::size_t is_studyalert_start_pos = line.rfind("Study:", text_start_pos);

	std::size_t study_name_start_pos = 0;
	std::size_t study_name_end_pos = 0;

	// 2024-01-05 Due to an inconsistency found with studies that have Use As Main Price
	// Graph enabled For now it is necessary to perform a check that will ensure the
	// study name is retrieved safely. 
	//
	// This variable is used to inform the program of the inconsistency 
	bool format_inconsistency = 0;

	// if the iterator returns null it means that Display As Main Price Graph was enabled for the study.
	// When that is enabled it changes the formatting of the Alert Log to not include the Study: Study Name
	// it will be necessary to format the message differently further below
	if(is_studyalert_start_pos == std::string::npos) 
	{
		// set the boolean to positive informing the study to use the separate formatting method 
		format_inconsistency = 1;
	}
	else
	{
		// if we get here we can continue to initialize the other variables safely 
		// set the starting point for the Study Name 7 characters infront of text: "Study: "
		study_name_start_pos = is_studyalert_start_pos + 7;

		// Set the end point of the substring
		study_name_end_pos = line.find('|', study_name_start_pos) - 1;
	}

	// Create our substring (safely since if they are initialized and it should not use invalid iterators)
	std::string_view study_name (line.c_str() + study_name_start_pos, study_name_end_pos - study_name_start_pos);

	// potentially have something here to take care of any special HTML text formatting
	
	// continue with the other substrings
	std::size_t bar_datetime_start_pos = line.find("Bar start:");
	std::size_t bar_datetime_end_pos = line.find('|', bar_datetime_start_pos) - 5; // removing the ms timestamp

	// only need the start position since it reads until end of the string
	std::size_t chartbook_start_pos = line.find("Chartbook:");
	
	// SAVE the alert log chartbook name independantly 
	std::string_view alert_chartbook (line.c_str() + chartbook_start_pos + 11);

	// Get the chart number where the alert originated from
	std::size_t find_chtbooknum = line.find("ChartNumber: ");
	std::size_t cht_number_start_pos = line.find(' ', find_chtbooknum) + 1;
	std::size_t cht_number_end_pos = line.find(' ', cht_number_start_pos);

	// save only the chart number itself  
	std::string_view alert_cht_number (line.c_str() + cht_number_start_pos , 
	cht_number_end_pos  - cht_number_start_pos);

	// Alert chart number must persist past the string formatting we are about to do 
	std::string alert_chart_number = std::string(alert_cht_number);

	// make sure the formula text is found, otherwise simply do nothing to the line variable for the moment
	if(text_start_pos != std::string::npos)
	{
		if(source_start_pos != std::string::npos) // safety check
		{
			// std::string_view Make reference to different sequences of characters from the underlying string
			// without copying any of the data (lightweight) 
			std::string_view source_string(line.c_str() + source_start_pos, source_end_pos - source_start_pos);

			// Determine if source string and study name string are identical. This will tell us if the alert was from
			// a Chart Alert or a Study Alert.

			// Only check if it's a Chart or Study Alert if our previous substring got initialized (aka normal behaviour)
			std::size_t find_substring_match = 0;
			if(study_name_start_pos != 0) 
			{
				// find the study_name contents inside of source_string. 
				// If a full match is found (iterator set to a valid number), this was a Chart Alert.
				find_substring_match = source_string.find(study_name);
			}

			bool is_chart_alert = 0;
			// check if the find_substring_match iterator was initialized and it was not initialized to null
			if(find_substring_match != 0 && find_substring_match != std::string::npos)
			{
				// debug 
				/* sc.AddMessageToLog("The Study Name is found within Source String, this was a Chart Alert.",1); */

				// informs the following code to format it as a chart alert instead 
				is_chart_alert = 1;

			}

			// set the alert text substring (using std::string because this string 
			// might need to be modified.
			//
			// With Chart/Study Alerts, this is where the formula is found. 
			std::string text_string (line.c_str() + text_start_pos, text_end_pos - text_start_pos);

			// find any < characters in the formula and replace them with html equivalent
			// this is a fix for where the < character is being interpreted as an opening
			// brace for html formatting. We want to display the literal character instead. 
			std::size_t find_less_than = text_string.find('<');
			if(find_less_than != std::string::npos)
			{
				// the < character is found 
				//
				// Replace all occurrences of '<' with '&lt;'
				std::size_t pos = 0;
				while ((pos = text_string.find('<', pos)) != std::string::npos) 
				{
					text_string.replace(pos, 1, "&lt;");
					pos += 4; // Move past the replaced '&lt;'
				}
			}

			// set the date_time substring	
			std::string_view bar_datetime_string (line.c_str() + bar_datetime_start_pos,
			bar_datetime_end_pos - bar_datetime_start_pos);

			// set the chartbook substring
			std::string_view chartbook_string (line.c_str() + chartbook_start_pos);

			// Get the Input selection for Customize Message Text
			int CustomizeMessageSettingIndex = CustomizeMessageContent;

			// Format the string to pass in Telegram message
			if(format_inconsistency == 1) // Will remove when SC fixes bug
			{
				// Different formatting to adjust for the lack of Study Name 
				if(CustomizeMessageSettingIndex == 0) // default include formula
				{
						// boost::format fmt = boost::format("%1%\n\n%2%\n%3%\n%4%") 
						boost::format fmt = boost::format("%1%\n%2%\n%3%\n%4%") 
						%text_string %source_string %bar_datetime_string  %chartbook_string ;

						// overwrite our line string with the formatted string
						line = fmt.str();
				}
				else if(CustomizeMessageSettingIndex == 1) // only remove formula
				{
					boost::format fmt = boost::format("%1%\n%2%\n%3%") 
					%source_string %bar_datetime_string %chartbook_string ;

					// overwrite our line string with the formatted string
					line = fmt.str();
				}
				else if(CustomizeMessageSettingIndex == 2) // same as no formula
				{
					boost::format fmt = boost::format("%1%\n%2%\n%3%") 
					%source_string %bar_datetime_string %chartbook_string ;

					// overwrite our line string with the formatted string
					line = fmt.str();
				}
			}
			else
			{
				// Formatting for Study Alert 
				if(CustomizeMessageSettingIndex == 0) // default
				{
					/* boost::format fmt = boost::format("<strong>%1%</strong>\n\n%2%\n%3%\n%4%\n%5%") */ 
					boost::format fmt = boost::format("%1%\n\n%2%\n%3%\n%4%\n%5%") 
					%study_name %text_string %source_string %bar_datetime_string %chartbook_string ;

					// overwrite our line string with the formatted string
					line = fmt.str();
				}
				else if(CustomizeMessageSettingIndex == 1) // only remove formula
				{
					/* boost::format fmt = boost::format("<strong>%1%</strong>\n\n%2%\n%3%\n%4%") */ 
					boost::format fmt = boost::format("%1%\n\n%2%\n%3%\n%4%") 
					%study_name %source_string %bar_datetime_string %chartbook_string ;

					// overwrite our line string with the formatted string
					line = fmt.str();
				}
				else if(CustomizeMessageSettingIndex == 2) // only include Study Name
				{
					/* boost::format fmt = boost::format("<strong>%1%</strong>") */ 
					boost::format fmt = boost::format("%1%") 
					%study_name ;

					// overwrite our line string with the formatted string
					line = fmt.str();
				}
			}
		}
	}

	// Convert Chartbook name as std::string_view into SCString (fancy 1 liner)
	sc_alert_chartbook = std::string(alert_chartbook).c_str();

	// Cast/Convert chart number stringview into int 
	sc_alert_chart_num = std::stoi(std::string(alert_cht_number));
}

void ParseChartDrawingAlertText(std::string& line, std::string& SourceChartImageText, int& CustomizeMessageContent, SCString& sc_alert_chartbook, int& sc_alert_chart_num, SCStudyInterfaceRef sc)
{
	SCString msg;
	// String parsing for Chart Drawing Alerts
	// find various text inside the string that was obtained from the alert log file 
	//
	// Get the Source Chart Text String
	std::size_t source_start_pos = line.find("Source");
	std::size_t source_end_pos = line.find('|', source_start_pos);

	// Alert text string
	std::size_t text_start_pos = line.find("Chart Drawing");
	std::size_t text_end_pos = line.find('|', text_start_pos);

	// REMOVE / from Source Chart Text
	// check over the substring for any forward slash character and kill loop if we reach npos string position 
	for(auto i = source_end_pos; i != source_start_pos && i != std::string::npos; i--)
	{
		if(line[i] == '/')
		{
			// Replace the character at the position i with a whitespace 
			//string& replace (size_t pos, size_t len, const string& str);
        	line.replace(i, 1, " ");
		}
	}

	// do the same for the Alert Text String
	for(auto i = text_end_pos; i != text_start_pos && i != std::string::npos; i--)
	{
		if(line[i] == '/')
		{
			// Replace the character at the position i with a whitespace 
        	line.replace(i, 1, " ");
		}
	}

	// CREATE image file text using the source chart text iterators 
	SourceChartImageText = line.substr(source_start_pos + 8, source_end_pos - (source_start_pos + 8));

	// REMOVE INVALID CHARACTERS FROM Image File Text for WINDOWS FILENAME 
	// an array of invalid characters for a windows filename
	std::array<char, 9> invalid_filename_characters = {'\\', '/', ':', '*', '?', '"', '<', '>', '|'};

	// for each of the invalid characters 
	for(int index = 0; index < invalid_filename_characters.size(); index++)
	{
		// iterate backwards through the string so we can delete characters from it 
		for(auto i = SourceChartImageText.end(); i != SourceChartImageText.begin(); i--)
		{
			// if current string character is equal to the current element in invalid characters array 
			if(*i == invalid_filename_characters[index])
			{
				// erase this character from the string
				/* SourceChartImageText.erase(i); */
				// replace the invalid character with a whitespace 
				*i = ' ';
			}
		}
	}

	// ADD FIX FOR UNIQUE ALERT NAME 
	if(text_start_pos != std::string::npos)
	{
		// MONOSPACE FORMATTING TO THE PRICE in HTML FORMAT
		// Get Iterators to the Price (there are two prices in the alert text)
		
		// We start from the end of the string and work our way back (safest way)
		std::size_t price2_end_pos = line.rfind(')', text_end_pos);
		std::size_t price2_start_pos = line.rfind('(', price2_end_pos);

		// Insert text for price2 formatting 
		line.insert(price2_end_pos, "</code>");
		line.insert(price2_start_pos + 1, "<code>"); 

		// reset the iterator for price2_start_pos in order to safely get price1 iterators
		price2_start_pos = line.rfind('(', text_end_pos);

		// Get the iterators for price1
		std::size_t price1_end_pos = line.rfind(')', price2_start_pos);
		std::size_t price1_start_pos = line.rfind('(', price1_end_pos);
		
		// Insert text for price1 formatting
		line.insert(price1_end_pos, "</code>");
		line.insert(price1_start_pos + 1, "<code>"); 

		// once done inserting the text, RESET Alert Text End Position to the new correct iterator
		text_end_pos = line.find('|', text_start_pos);
	}

	// continue with the other substrings
	std::size_t bar_datetime_start_pos = line.find("Bar date-time");

	// possibly check if first iterator is valid before finding the end point 
	std::size_t bar_datetime_end_pos = line.find('|', bar_datetime_start_pos);

	// if Bar date-time text was not found, default to using DateTime
	if(bar_datetime_start_pos == std::string::npos)
	{
		bar_datetime_start_pos = line.find("DateTime");
		bar_datetime_end_pos = line.find('|', bar_datetime_start_pos);
	}

	// only need the start position since it reads until end of the string
	std::size_t chartbook_start_pos = line.find("Chartbook");
	
	// SAVE the alert log chartbook name independantly 
	std::string_view alert_chartbook (line.c_str() + chartbook_start_pos + 11);

	// Get the chart number where the alert originated from
	std::size_t find_chtbooknum = line.find("ChartNumber: ");
	std::size_t cht_number_start_pos = line.find(' ', find_chtbooknum) + 1;
	std::size_t cht_number_end_pos = line.find(' ', cht_number_start_pos);

	std::string_view alert_cht_number (line.c_str() + cht_number_start_pos , 
	cht_number_end_pos  - cht_number_start_pos);

	// Alert chart number must persist past the string formatting we are about to do 
	std::string alert_chart_number = std::string(alert_cht_number);

	// Chart Drawing Alerts are simpler than Study Alerts and do not have a formatting option 
	if(source_start_pos != std::string::npos) // safety check
	{
		if(text_start_pos != std::string::npos)
		{
			// make reference to different sequences of characters from the underlying string
			// without copying any of the data (lightweight) 
			std::string_view source_string(line.c_str() + source_start_pos, source_end_pos - source_start_pos);

			std::string_view text_string (line.c_str() + text_start_pos, text_end_pos - text_start_pos);

			std::string_view bar_datetime_string (line.c_str() + bar_datetime_start_pos,
			bar_datetime_end_pos - bar_datetime_start_pos);
			std::string_view chartbook_string (line.c_str() + chartbook_start_pos);

			// Format the string to pass in Telegram message
			boost::format fmt = boost::format("%1%\n\n%2%\n%3%\n%4%") %text_string 
			%bar_datetime_string %chartbook_string %source_string ;

			// overwrite our line string with the formatted string
			line = fmt.str();
		}
		else
		{
			//Mon Oct 27 01:20:37 PM EDT 2025
			// DIFFERENT FORMATTING FOR UNIQUE STUDY NAME SETTING 
			// alert text start pos variable was not valid therefore unique study
			// name option was enabled, use different formatting 
			//
			// make reference to different sequences of characters from the underlying string
			// without copying any of the data (lightweight) 
			std::string_view source_string(line.c_str() + source_start_pos, source_end_pos - source_start_pos);

			std::string_view bar_datetime_string (line.c_str() + bar_datetime_start_pos,
			bar_datetime_end_pos - bar_datetime_start_pos);
			std::string_view chartbook_string (line.c_str() + chartbook_start_pos);

			// Format the string to pass in Telegram message
			boost::format fmt = boost::format("Chart Drawing Alert\n%1%\n%2%\n%3%")
			%bar_datetime_string %chartbook_string %source_string ;

			// overwrite our line string with the formatted string
			line = fmt.str();
		}
	}

	// Convert Chartbook name as std::string_view into SCString (fancy 1 liner)
	sc_alert_chartbook = std::string(alert_chartbook).c_str();

	// Cast/Convert chart number stringview into int 
	sc_alert_chart_num = std::stoi(std::string(alert_cht_number));
}

void ParseSymbolAlertText(std::string& line, std::string& SourceChartImageText, int& CustomizeMessageContent, SCStudyInterfaceRef sc, SCString& AlertSymbol)
{
	// SCString msg;
	SCString msg; 
	// sc.AddMessageToLog("parse func we get here!" ,1);

	// String parsing for Chart Drawing Alerts
	// find various text inside the string that was obtained from the alert log file 
	//
	// Get the Source Chart Text String
	std::size_t source_start_pos = line.find("Source");
	std::size_t source_end_pos = line.find('|', source_start_pos);

	// Get the actual symbol text 
	std::size_t symbol_start_pos = line.find(' ', source_start_pos) + 1;
	std::size_t symbol_end_pos = line.find(' ', symbol_start_pos) ;

	std::string_view symbol_string(line.c_str() + symbol_start_pos, symbol_end_pos - symbol_start_pos);

	// Apply it to our existing string variable in main scope 
	AlertSymbol = std::string(symbol_string).c_str();

	// Print for debug 
	// msg.Format("Symbol string: %s", std::string(symbol_string).c_str());
	// sc.AddMessageToLog(msg,1);

	// Alert text string
	std::size_t text_start_pos = line.find("Alert Text");
	std::size_t text_end_pos = line.find('|', text_start_pos);

	// REMOVE / from Source Chart Text
	// check over the substring for any forward slash character and kill loop if we reach npos string position 
	//
	// Note: This is for safety
	for(auto i = source_end_pos; i != source_start_pos && i != std::string::npos; i--)
	{
		if(line[i] == '/')
		{
			// Replace the character at the position i with a whitespace 
			//string& replace (size_t pos, size_t len, const string& str);
        	line.replace(i, 1, " ");
		}
	}

	// do the same for the Alert Text String
	for(auto i = text_end_pos; i != text_start_pos && i != std::string::npos; i--)
	{
		if(line[i] == '/')
		{
			// Replace the character at the position i with a whitespace 
        	line.replace(i, 1, " ");
		}
	}

	// CREATE image file text using the source chart text iterators 
	// // COULD BE BOMBING OUT THE STUDY 
	SourceChartImageText = line.substr(source_start_pos + 8, source_end_pos - (source_start_pos + 8));

	// REMOVE INVALID CHARACTERS FROM Image File Text for WINDOWS FILENAME 
	// an array of invalid characters for a windows filename
	std::array<char, 9> invalid_filename_characters = {'\\', '/', ':', '*', '?', '"', '<', '>', '|'};

	// for each of the invalid characters 
	for(int index = 0; index < invalid_filename_characters.size(); index++)
	{
		// iterate backwards through the string so we can delete characters from it 
		for(auto i = SourceChartImageText.end(); i != SourceChartImageText.begin(); i--)
		{
			// if current string character is equal to the current element in invalid characters array 
			if(*i == invalid_filename_characters[index])
			{
				// erase this character from the string
				/* SourceChartImageText.erase(i); */
				// replace the invalid character with a whitespace 
				*i = ' ';
			}
		}
	}

	// MONOSPACE FORMATTING TO THE PRICE in HTML FORMAT
	// Get Iterators to the Price (there are two prices in the alert text)
	
	// We start from the end of the string and work our way back (safest way)
	std::size_t price_end_pos = line.rfind(')', text_end_pos);
	std::size_t price_start_pos = line.rfind('(', price_end_pos);

	// make sure our iterator is valid (support for unique study name)
	if(price_end_pos != std::string::npos)
	{

		/* // Insert text for price2 formatting */ 
		line.insert(price_end_pos , "</code>");
		line.insert(price_start_pos +1, "<code>");

		// reset the iterator for price_start_pos in order to safely get price2 iterators
		price_start_pos = line.rfind(')', text_end_pos);

		// Array of valid numbers (0–9)
		std::array<char, 10> valid_numbers = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

		// Find the start of price 2 (first digit after price_end_pos)
		std::size_t price2_start_pos = std::string::npos;

		for (std::size_t i = price_start_pos + 1; i < line.length(); ++i) 
		{
			if (std::find(valid_numbers.begin(), valid_numbers.end(), line[i]) != valid_numbers.end()) 
			{
				price2_start_pos = i;
				break;
			}
		}

		// Find the end of price 2 (first whitespace after price2_start_pos)
		std::size_t price2_end_pos = std::string::npos;

		if (price2_start_pos != std::string::npos) 
		{
			price2_end_pos = line.find(' ', price2_start_pos);
			if (price2_end_pos == std::string::npos) 
			{
				price2_end_pos = line.length(); // If no whitespace, use end of string
			}
		}

		// debug only 
		// std::string_view test_string(line.c_str() + price2_start_pos, price2_end_pos - price2_start_pos);
		// msg.Format("test string: %s", std::string(test_string).c_str());
		// sc.AddMessageToLog(msg,1);

		// Insert formatting for price 2 (if found)
		if (price2_start_pos != std::string::npos) 
		{
			line.insert(price2_end_pos, "</code>");
			line.insert(price2_start_pos , "<code>");
		}
	}


	// Date Time Substring
	// TODO: Convert date time into chart timezone from UTC since it is automatically in UTC for some reason. 
	std::size_t bar_datetime_start_pos = line.find("DateTime");
	// std::size_t bar_datetime_end_pos = line.find('|', bar_datetime_start_pos);
	//
	// Date time is last in Symbol Alert format so we can take the end of the string 
	std::size_t bar_datetime_end_pos = line.length() - 1;

	// only need the start position since it reads until end of the string
	// NO CHARTBOOK IN SYMBOL ALERTS 
	/* std::size_t chartbook_start_pos = line.find("Chartbook"); */
	
	// SAVE the alert log chartbook name independantly 
	/* std::string_view alert_chartbook (line.c_str() + chartbook_start_pos + 11); */

	// Get the chart number where the alert originated from
	/* std::size_t find_chtbooknum = line.find("ChartNumber: "); */
	/* std::size_t cht_number_start_pos = line.find(' ', find_chtbooknum) + 1; */
	/* std::size_t cht_number_end_pos = line.find(' ', cht_number_start_pos); */

	/* std::string_view alert_cht_number (line.c_str() + cht_number_start_pos , */ 
	/* cht_number_end_pos  - cht_number_start_pos); */

	// Alert chart number must persist past the string formatting we are about to do 
	/* std::string alert_chart_number = std::string(alert_cht_number); */

	// Chart Drawing Alerts are simpler than Study Alerts and do not have a formatting option 
	if(source_start_pos != std::string::npos) // safety check
	{
		// make reference to different sequences of characters from the underlying string
		// without copying any of the data (lightweight) 
		std::string_view source_string(line.c_str() + source_start_pos, source_end_pos - source_start_pos);
		std::string_view text_string (line.c_str() + text_start_pos, text_end_pos - text_start_pos);

		std::string_view bar_datetime_string (line.c_str() + bar_datetime_start_pos,
		bar_datetime_end_pos - bar_datetime_start_pos);

		/* std::string_view chartbook_string (line.c_str() + chartbook_start_pos); */

		// Format the string to pass in Telegram message
		boost::format fmt = boost::format("// SC Symbol Alert //\n%1%\n%2%\n%3%") %source_string %text_string
		%bar_datetime_string ;

		/* // overwrite our line string with the formatted string */
		line = fmt.str();

		/* msg.Format("line: %s", line.c_str()); */
		// msg.Format("source: %s text: %s, datetime: %s", std::string(source_string).c_str(), std::string(text_string).c_str(), std::string(bar_datetime_string).c_str());
		// sc.AddMessageToLog(msg,1);
	}

	// Convert Chartbook name as std::string_view into SCString (fancy 1 liner)
	/* sc_alert_chartbook = std::string(alert_chartbook).c_str(); */

	// Cast/Convert chart number stringview into int 
	/* sc_alert_chart_num = std::stoi(std::string(alert_cht_number)); */
}
void SCTelegramPostRequest(SCStudyInterfaceRef sc, const SCString& host, const SCString& token, const std::string& ChatID, const std::string& line, SCString& msg)
{
	SCString method = "/sendMessage?";

	// Set our Telegram URL for the POST request 
	SCString URL = std::move(host) + std::move(token) + std::move(method);

	// std::cout << "HELLO!" << std::endl;
	// SIERRA CHART POST REQUEST WITH JSON BODY EXAMPLE 
	//
	// Create the json object for Telegram
	// For simplicity using nlohmann json
	nlohmann::json object = {
	{"chat_id", std::move(ChatID)},
	{"text", std::move(line)},
	{"parse_mode", "HTML"},	
	}; 

	// Convert nlohmann json into SCString in one line. 
	SCString query = object.dump().c_str(); 						
	
	// Set our headers 
	n_ACSIL::s_HTTPHeader headers[1];
	headers[0].Name = "Content-Type";
	headers[0].Value = "application/json";

	if(sc.MakeHTTPPOSTRequest(URL, query, headers, 1) )
	{
		// successful request 
		/* msg.Format("Response: %s", sc.HTTPResponse.GetChars()); */
		/* sc.AddMessageToLog(msg,1); */
	}
	else
	{
		// problem with the request 
		msg.Format("Error with the Request: %s", sc.HTTPResponse.GetChars());
		sc.AddMessageToLog(msg,1);
	}
}

std::string s_DebugResponse = "";
void DebugResponse(SCStudyInterfaceRef sc)
{
	SCString msg;
	if(s_DebugResponse != "")
	{
		msg.Format("RESPONSE: %s", s_DebugResponse.c_str());
		sc.AddMessageToLog(msg,1);

		s_DebugResponse = "";
	}
}
// CURL Callback function to capture the response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::stringstream* s)
{
    size_t totalSize = size * nmemb;
    s->write(static_cast<char*>(contents), totalSize);
    return totalSize;
}

// This function performs a curl synchronous request to the inputted URL
void CURLTelegramPostRequest(const SCString& URL, const std::string& ChatID, const std::string& FilePath, const std::string& caption)
{
	// How to print to SC Message log from a different thread using a pointer to the SCStudyInterfaceRef : 
	//
	// Does not always work for some reason
	//
	// grab a pointer to our SC variable to call it's methods 
  	/* s_sc *p_sc = &sc; */
	/* p_sc->AddMessageToLog("FROM CURL FUNCTION!" ,1); */

	// it is also possible to use the reference without initializing a pointer from the reference. 
	/* sc.AddMessageToLog("FROM CURL FUNCTION!" ,1); */

	// std::cout << "HELLO!" << std::endl;

	// timeout requests in case of failure to save file 
	auto start = std::chrono::steady_clock::now();
	int maxWaitTimeMillis = 20000;

	// Logic to ensure the file is finished being saved before allowing the request to go through 
	for(;;)
	{
		bool FileSavedSuccessfully = std::filesystem::exists(FilePath);
		if(FileSavedSuccessfully)
		{
			auto FileSize = std::filesystem::file_size(FilePath); // get the file size 
			Sleep(150); // quick pause 
			for(;;)
			{
				if(std::filesystem::file_size(FilePath) > FileSize) // if the current file size is greater than the saved size 
				{
					/* std::cout << "File not finished being written!" << std::endl; */
						
					// update the file size 
					FileSize = std::filesystem::file_size(FilePath);

					// pause some time 
					Sleep(100);

					// loop again 
					continue;
				}
				else
				{
					// file size was the same. should be finished writing 
					/* std::cout << "File finished being written!" << std::endl; */
					break;
				}
			}
			break; // break outer loop once inner loop breaks 
		}
		else
		{
			/* std::cout << "File not finished being saved!" << std::endl; */
			Sleep(200); // file not saved, wait for it to be saved 
			
			// if our clock has exceeded the timer, just timeout the request. 
			auto elapsedMillis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
			if (elapsedMillis > maxWaitTimeMillis) 
			{
				// debug 
				/* std::cout << "Request timed out due to file not being saved successfully!" << std::endl; */
				return ; // Timeout
			}
		}
	}

	// initialize curl 
	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL* curl = curl_easy_init();

	// Updated to new mime interface: Wed Jul 31 16:54:31 EDT 2024
	curl_mime *mime;
	curl_mimepart *part;

	// debug string 
	std::stringstream responseStream;

	if (curl) 
	{
		mime = curl_mime_init(curl);

		// add the chat ID 
		part = curl_mime_addpart(mime);
		curl_mime_data(part, ChatID.c_str(), CURL_ZERO_TERMINATED);
		curl_mime_name(part, "chat_id");

		// handle if there is a group subtopic ID in the Chat ID 
		std::size_t ThreadIDPos = ChatID.find("_");
		if(ThreadIDPos != std::string::npos)
		{
			const std::string MessageThreadID = ChatID.substr(ThreadIDPos + 1);

			// add the message thread id 
			part = curl_mime_addpart(mime);
			curl_mime_data(part, MessageThreadID.c_str(), CURL_ZERO_TERMINATED);
			curl_mime_name(part, "message_thread_id");
		}

		// add the photo 
		part = curl_mime_addpart(mime);
		curl_mime_filedata(part, FilePath.c_str());
		curl_mime_name(part, "photo");

		// add the message caption (text)
		part = curl_mime_addpart(mime);
		curl_mime_data(part, caption.c_str(), CURL_ZERO_TERMINATED);
		curl_mime_name(part, "caption");

		// add the parse mode 
		part = curl_mime_addpart(mime);
		curl_mime_data(part, "HTML", CURL_ZERO_TERMINATED);
		curl_mime_name(part, "parse_mode");

		// debug: gives more curl output from the server 
		/* curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); */

		// Declare headers 
        struct curl_slist *headers = nullptr;

        // Initialize headers as needed
        headers = curl_slist_append(headers, "Content-Type: multipart/form-data");
        /* headers = curl_slist_append(headers, "Expect:");  // Disable 100-continue */

		// set the headers 
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		// set it as a mime post request
		curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

		// Set the URL
		curl_easy_setopt(curl, CURLOPT_URL, URL.GetChars());

		// Debug  
		// Set the write function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

		// Pass a pointer to string variable to store the response
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStream);

		// Perform the HTTP POST request
		CURLcode res = curl_easy_perform(curl);

		// debug 
        /* std::cout << "Response code: " << res << std::endl; */
        /* std::cout << "Response: " << responseStream.str() << std::endl; */

		// Check for errors
		if (res != CURLE_OK) 
		{
			// debug if necessary 
			/* debug_request = "Curl request failed: " + std::string(curl_easy_strerror(res)); */
			/* msg.Format("CURL Request Error: %s", debug_request.c_str()); */
			/* sc.AddMessageToLog(msg,1); */

			/* s_DebugResponse = "CURL Request Failed: " + responseStream.str() + "\n"; */
			s_DebugResponse = "CURL Request Failed: " +  std::string(curl_easy_strerror(res)) + "\n";
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;

		}
		else
		{
			s_DebugResponse = "CURL Request Success: " + responseStream.str() + "\n";

			// Log the response
            /* std::cout << "Response code: " << res << std::endl; */
            /* std::cout << "Response: " << responseStream.str() << std::endl; */
		}

		// Clean up
		curl_easy_cleanup(curl);
		curl_mime_free(mime);
		curl_global_cleanup();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
SCSFExport scsf_TelegramDrawingAlert(SCStudyInterfaceRef sc)
{
	// Study Inputs 
	SCInputRef Input_Enabled = sc.Input[0];
	SCInputRef Input_ChatID = sc.Input[1];
	SCInputRef Input_SendChartImage = sc.Input[2];
	SCInputRef Input_ImageFolderPath = sc.Input[3];
	SCInputRef Input_UseCustomBot = sc.Input[4];
	SCInputRef Input_CustomBotToken = sc.Input[5];
	SCInputRef Input_SpecifyPath = sc.Input[6];
	SCInputRef Input_CustomFolderPath = sc.Input[7];
	SCInputRef Input_FindDuplicateStudies = sc.Input[8];
	SCInputRef Input_CustomizeMessageContent = sc.Input[9];
	SCInputRef Input_Debug = sc.Input[10];
	SCInputRef Input_ToggleAlertsACSButtonNumber = sc.Input[11];

	SCString msg; // logging object
	
	// Declare Necessary persistent variables
	// These are items that need to be remembered between calls to the study function 
	
	// Keep track of the last modification time of an alert log file. to be
	// This is used to determine if a file has been recently modified.
	std::int64_t& LastModTime = sc.GetPersistentInt64(0); 

	// keep track of the number of lines in the file
	// This is in order to only process the most recently added line (only send one alert)
	int& NumberOfLines = sc.GetPersistentInt(4); 	

	// keep track of the very first search for the file
	// This is in order to handle the behaviour for the very first time Sierra Chart is started 
	int& FirstTimeStart = sc.GetPersistentInt(5); 												  

	if (sc.SetDefaults)
	{
		// This is what appears when you select Description from the study settings
		// If you do not have the study just refer to the README file on the
		// repository which contains the identical documentation. 
		sc.StudyDescription = 
			
		"<strong><u>Info About the Code: </u></strong> "
			"<br> <br> This advanced custom study was written by VerrilloTrading in Q3-Q4 of 2023. This study is open source and it can serve as an example for how to use a Telegram Bot with Sierra Chart. To compile this study users will need to include the necessary dependencies like boost::format and CURL. I tried as much as possible to stick to the Sierra Chart libraries but some things are much more practical with other c++ libraries. For example, there are very few existing examples of HTTP POST functionality using ACSIL. The study provides an example of this in combination with <a href = https://github.com/nlohmann/json target=_blank ref=noopener noreferrer >nlohmann json</a> to prepare the request body. CURL was necessary to perform the <a href = https://core.telegram.org/bots/api#sendphoto target=_blank ref=noopener noreferrer >sendPhoto</a> Telegram method for sending multipart/form data. Anyone is welcome to repurpose the code, improve it or add functionality."

		" <br> <br> This study contains two examples of sending a http post request from Sierra Chart."
		"<br> 1. using CURL to call the Telegram <a href = https://core.telegram.org/bots/api#sendphoto target=_blank ref=noopener noreferrer >sendPhoto</a> method where the chart image is passed as multipart/form data."
		"<br> 2. using <a href = https://www.sierrachart.com/index.php?page=doc/ACSIL_Members_Functions.html#scMakeHTTPPOSTRequest target=_blank ref=noopener noreferrer >sc.MakeHTTPPOSTRequest()</a> to call Telegram <a href = https://core.telegram.org/bots/api#sendmessage target=_blank ref=noopener noreferrer >sendMessage</a> method which is a native Sierra Chart ACSIL function."

		"<br> <br> The second example is in the study in the case that the user does not want to recieve the chart screenshot. Otherwise it is meant to serve as an example."

		"<br> <br> Reasons for sharing the source code:"
		"<br> 1. Greater Transparency " 
		"<br> 2. Helps the community"
		"<br> 3. Show off skills and features in Sierra Chart"
		 
		"<br> <br> <strong><u>Info for Users: </u></strong> "
		"<br> <br> If you want to use this study and do not want to bother building it yourself you can send an email to <span style=\"background-color: MediumTurquoise; font-weight: bold;\">support@verrillotrading.com</span>"
" with your Sierra Chart username and I will add your username to the list of authorized users for this DLL. When your username is added to this list the file will appear in your Sierra Chart Data Folder the next time you start Sierra Chart."
		" The DLL has been compiled using this exact source code file **(except one change) and a build script."

		"<br> <br> ** For convenience purposes The DLL provided by VerrilloTrading provides a default bot that users can rely on instead of having to create their own bot. The bot token for that bot was removed from the repository for security reasons."

		"<br> <br> <strong><u>Description of Study Functionality:</u></strong> "

		"<br> <br> When this study is enabled and a <a href = https://www.sierrachart.com/index.php?page=doc/Tools.html#ChartDrawingAlerts target=_blank rel=noopener noreferrer >Sierra Chart Drawing Alert</a> or <a href = https://www.sierrachart.com/index.php?page=doc/StudyChartAlertsAndScanning.php target=_blank rel=noopener noreferrer >Chart/Study Alert</a> takes place, a Telegram message including a chart screenshot will be sent to the specified Telegram Chat ID. This message will contain information about the alert that took place. For a full demonstration please watch this accompanying <a href = https://youtu.be/EQZI9pBtDrE target=_blank rel=noopener noreferrer >video</a> on YouTube, and continue to follow along with this document. "

		" This study handles different cases and exceptions for when Sierra Chart generates new Alert Log Files. If you find any undefined behaviour or a replicable bug, please inform the study developer at the above email."


		"<br> <br> <strong><u>HOW TO SETUP AND USE THIS STUDY:</u></strong> "
		"<br> <br> <strong><u>Step 1:</u></strong> "
		"<br> <br> For this study to work it is necessary to <u>Enable</u> this setting in Sierra Chart:"
		"<br> <span style=\"background-color: yellow; font-weight: bold;\">Global Settings >> Log >> Save Alerts Log to File</span>"

		"<br> <br> When an Alert is triggered, a new line is written to a file in the <strong>C:\\SierraChart\\Logs</strong> directory. The path to this Folder is relative to where you have Sierra Chart installed. It is also possible that this Logs Folder does not exist yet. <span style=\"background-color: yellow; text-decoration: underline;\">Follow the next steps to ensure this folder exists.</span> "

		"<br> <br> When putting the study on the chart for the first time it will provide instructions in the Message Log. The first message will inform the user to enter the Telegram Chat ID."


		"<br> <br> <strong><u>Step 2:</u></strong> "
		"<br> <br> Get the Telegram Chat ID of where we want to send the alert. The easiest way is to use Telegram Web to get the Telegram Chat ID. Telegram Groups, Group Subtopics, Channels and Private Chats are supported. <span style=\"background-color: yellow; font-weight: bold; \">It is necessary to be on Telegram Web version A to get the Chat ID from the browser address bar.</span> The version can be selected from the settings window in the top left corner when using Telegram Web. The Chat ID for the currently open chat appears in the address bar. Try changing to different chats to see the end of the address bar change. Example: https://web.telegram.org/a/#-12345678 where '-12345567' is the Chat ID. Groups and Group Sub-Topics typically have negative numbers as the Chat ID. To get your personal chat ID, go to the Saved Messages chat. On Telegram Web this can be accessed by clicking at the top left and selecting 'Saved Messages'. The address bar should display your Private Chat ID. In order to the send messages to a Telegram Channel it is necessary to give the bot administrator permissions in the channel, specifically with the permission to send messages. "

		" <br> <br> The default bot that the study uses on Telegram is <a href = https://t.me/vt_sierrachart_sender_bot target=_blank rel=noopener noreferrer >https://t.me/vt_sierrachart_sender_bot.</a> "
		"<br> You need to message this bot with /start on Telegram or add it to your group, unless you are using your own bot. Telegram bots cannot message you first and they cannot read messages sent by other bots. "  

		"<br> <br> Users can override the default bot with their their own bot if they prefer. "

		"<br> <br> <strong><u>Step 3:</u></strong> "

	   "<br> <br> Perform the following steps after you have inputted the Telegram Chat ID in Step 2. If you still see an Error in the Message Log about not being able to find the Logs folder, perform the following steps: "

		"<br> <br> 1. Make sure you enabled the setting mentioned above named <a href = https://www.sierrachart.com/index.php?page=doc/GeneralSettings.html#SaveAlertsLogToFile target=_blank rel=noopener noreferrer >Save Alerts Log to File. </a>"
		"<br> 2. Check if the Folder 'Logs' exists in your Sierra Chart installation directory."
		"<br> 3. If the Logs folder does not exist, create a new <a href = https://www.sierrachart.com/index.php?page=doc/Tools.html#ChartDrawingAlerts target=_blank rel=noopener noreferrer >Chart Drawing Alert</a> and make sure it is triggered. Once this first alert is triggered, the folder should be created. "
		"<br> 4. Check inside the Logs folder for a file that starts with 'Alert Log' followed by the current DateTime. If you do not see this file, repeat the instructions in Step 1. "

		"<br> <br> <strong><u>Step 4:</u></strong> "

		"<br> <br> <strong>Important:</strong> Make sure Study Input #4 is set to the Image Folder Path that is set in <strong>Global Settings > Paths > Chart Image Folder.</strong> When this setting is changed in Sierra Chart, a new folder will be automatically created. It is absolutely critical that you set Study Input #4 input text to this path, or the screenshot will not work. You will get an Error showing up in the message log if the path you specified in this Study Input does not exist. The default path for this on Windows is: <strong>C:\\SierraChart\\Images</strong> If you are running on Linux through Wine or if you changed the drive letter of your C drive it might look like this: <strong>Z:\\mnt\\SierraChart\\Images</strong>. This study is confirmed to work when Sierra Chart is being run on Mac using Parallels."

		"<br> <br> For the screenshot function to work when charts are hidden behind other charts in your chartbook it is necessary to <span style=\"background-color: yellow; font-weight: bold;\">disable</span> <a href = https://www.sierrachart.com/index.php?page=doc/GeneralSettings.html#DestroyChartWindowsWhenHidden target=_blank rel=noopener noreferrer >Destroy Chart Windows When Hidden</a>."

		"<br> <br> <u><strong>STUDY AND ALERT CONFIGURATION:</strong></u> "

		"<br> <br> <u><strong>Alert Settings:</strong></u> "

		"<br> <br> Users have multiple options for how they can configure Alerts in Sierra Chart. A Chart Alert is set up by going to Chart Settings > Alert. Identical settings exist for Study Alerts which can be set up in the Study Settings for any particular study and going to the Alerts tab. To enable a Chart/Study Alert, an alert formula is necessary. Refer to <a = href https://www.sierrachart.com/index.php?page=doc/StudyChartAlertsAndScanning.php target=_blank rel=noopener noreferrer >this documentation</a> for info about Chart/Study Alerts and Formula examples. If the user wants to be repeatedly notified of an alert being triggered, they should disable <strong><i>Disable Simple Alert After Trigger</i></strong>. This will result in repeated alerts being sent to Telegram, about one second apart. If the user wants to send one alert and disable the alert when it is triggered, it is necessary to enable this setting."

		"<br> <br> <i>Chart Drawing Alerts</i> are configured by right clicking on an existing Chart Drawing and selecting <i>Alerts</i>. Similar settings exist for <i>Chart Drawing Alerts</i> where the user can choose to enable <i>Only Trigger Once</i> or <i>Alert Once Per Bar</i>."

		"<br> <br> <u><strong>Usage Across Multiple Chartbooks:</strong></u> "
		"<br> <br> <a href = https://www.sierrachart.com/index.php?page=doc/ACSIL_Members_Functions.html#scSaveChartImageToFileExtended target=_blank rel=noopener noreferrer >The screenshot function from Sierra Chart</a> is limited to taking screenshots of charts in the chartbook where the function call originates from. For this reason <strong>it is necessary to have 1 instance of this study open per chartbook</strong>. If you have 5 open chartbooks with different symbols waiting for alerts, you will need to open this study on a chart in each one of those chartbooks. When the study is active in any particular chartbook it will handle all the alerts that take place within that chartbook and provide an image of the chart as it is displayed in the chartbook. The chart can be hidden behind other charts and the screenshot will work as long as <a href = https://www.sierrachart.com/index.php?page=doc/GeneralSettings.html#DestroyChartWindowsWhenHidden target=_blank rel=noopener noreferrer >Destroy Chart Windows When Hidden</a> is <span style=\"background-color: yellow; font-weight: bold;\">disabled</span>. "

		"<br> <br> <strong><u>Using Duplicates of This Study Within the Same Chartbook:</u></strong> "
		"<br> <br> One viable use case for having more than one instance of this study open in the same chartbook is to send the same alerts to multiple Telegram chats at the same time. In this case it is necessary to configure the each study with valid settings and use a different Telegram Chat ID for each instance of the study. If the same Chat ID is used on two or more of these studies, the user will get duplicate alerts sent to the same chat. "

		"<br> <br> <strong><u>Detecting Duplicates of This Study Within the Same Chartbook:</u></strong> "
		"<br> <br> If the user added the study more than once by accident, they can enable a feature that provides the user with the chart numbers of any chart containing the Telegram Chart & Drawing Alerts study within the same chartbook. "

		"<br> <br> 1. Enable Study Input #9 which is named <u><i>Find Duplicate Studies in Current Chartbook</i></u>."
		"<br> 2. Open the Sierra Chart Message Log by going to Window > Message Log."
		"<br> 3. Create and trigger a Chart Drawing or Chart Alert and monitor the Message Log."
		"<br> 4. Look out for a message that looks like this: "
		"<br> <br> \"A duplicate is found on chart #(chart number here). Reduce the number of studies per chartbook to one unless you wish to recieve duplicate alerts.\"  "

		"<br> <br> 5. Then you can proceed to Window > Windows and Chartbooks, select the chart that has this chart number and remove the study from this chart." 

		"<br> <br> <strong><u>How to tell if Alerts are not going through to Telegram:</u></strong> "

		"<br> <br> It is fully possible that users are able to find a bug in the study where an alert is not being recieved on Telegram. One reason for this might be a particular text that is not in agreement with the Telegram Text formatting being used. "

		"<br> <br> 1. Open the SC Alert Manager by going to Window > Alert Manager."
		"<br> <br> 2. Monitor the alerts being added to the Alert Manager."
 		"<br> <br> 3. If you are encountering a particular alert that is not going through to Telegram, please inform the study developer at support@verrillotrading.com as soon as possible. "

		"<br> <br> <strong><u>Known or potential Issues :</u></strong> "

		"<br> <br> There are some settings in Sierra Chart that can change the formatting for text that is saved to the Alert Log files. "
		"<br> Here are settings that are known to cause issues with the study: "
		
		"<br> <br> 1. General Settings > Charts > Show Chart Number First on Chart Name "
		"<br> This setting should remain set to <strong><u>no</u></strong>. "

		"<br> <br> <strong><u>INFO ABOUT THE LOG FILE PATH:</u></strong> "

		"<br> <br> The following information applies to you if you have moved the Sierra Chart Data Files Folder out of the SC Installation directory. "


		"<br> <br> <strong>It is not recommended to move the location of the <a href = https://www.sierrachart.com/index.php?page=doc/GeneralSettings.html#DataFilesFolder target=_blank rel=noopener referrer >Data Files Folder</a> from the Sierra Chart installation directory.</strong> "

		"<br> <br> If you absolutely need to move this folder or have already moved it, you will need to perform this next step: "
		"<br> <br> <strong>IMPORTANT:</strong> If you have changed the location of the Data Files Folder, You will need to explicitly specify the directory of the Sierra Chart Logs folder. This folder will be located in the same directory where Sierra Chart is installed. The reason why this is necessary is because the study uses the Data Files Folder Path to get the Path to the Logs Folder. Example: if you moved the Data Folder to ANY PATH that is not the Sierra Chart Installation directory, you will now need to specify the Path of the Logs Folder which is in the directory where Sierra Chart is installed. <span style=\"background-color: yellow; text-decoration: underline;\">You need to explicitly specify the Logs Folder path in the last study input field.</span> "

		"<br> <br> The default location of this folder is: <strong>C:\\SierraChart\\Logs</strong> If you are running on Linux through Wine or if you changed the drive letter of your C drive it might look like this: <strong>Z:\\mnt\\SierraChart\\Logs</strong>"

		"<br> <br> For any issues or inquiries, send them to support@verrillotrading.com"

		"<br> <br> Thank you and enjoy the study!" 
		"<br> <br> -VerrilloTrading, Content Creator - Programmer";

		// Study Defaults
		std::string StudyGraphName = "Alerts - Send SC Alerts to Telegram | v" + std::to_string(ver) + " |";
		sc.GraphName = StudyGraphName.c_str();
		sc.AutoLoop = 0; // Manual looping
		sc.GraphRegion = 0;
		sc.ScaleRangeType= SCALE_SAMEASREGION;

		sc.UpdateAlways = 1; // not strictly necessary but can make it faster 

		Input_Enabled.Name = "Send Telegram on Alert Trigger";
		Input_Enabled.SetYesNo(1);
		Input_Enabled.SetDescription("Allows the user to turn the study on or off."); 

		Input_ChatID.Name = "Telegram Chat ID to Send Message";
		Input_ChatID.SetString("");
		Input_ChatID.SetDescription("User must provide the Telegram Chat ID for the Bot to send a message to.");

		Input_SendChartImage.Name = "Send Chart Screenshot in Telegram Message";
	   	Input_SendChartImage.SetYesNo(1);	
		Input_SendChartImage.SetDescription("Allows the user to control if a chart screenshot is sent along with the message.");

		Input_ImageFolderPath.Name = "Full Path to Chart Image Folder";
		Input_ImageFolderPath.SetString("C:\\SierraChart\\Images");
		Input_ImageFolderPath.SetDescription("User must put in the exact path to the Chart Image Folder. " 
		"It must match what is set in Global Settings > General Settings > Paths > Chart Image Folder (Action).");

		Input_UseCustomBot.Name = "Use a Different Telegram Bot";
		Input_UseCustomBot.SetYesNo(0);
		Input_UseCustomBot.SetDescription("Allows the user to specify the use of a different Telegram Bot.");

		Input_CustomBotToken.Name = "Telegram Bot Token for Use Different Bot";
		Input_CustomBotToken.SetString("");
		Input_CustomBotToken.SetDescription("If Previous Input Enabled: User must specify the Bot API token for the Bot. Bot API Tokens can be obtained by chatting with Bot Father on Telegram.");

		Input_SpecifyPath.Name = "Special case: SC Data Folder is not in Installation Directory";
		Input_SpecifyPath.SetYesNo(0);
		Input_SpecifyPath.SetDescription("Allows the user to specify if they have moved the Data Files Folder out of the Sierra Chart Installation Directory.");

		Input_CustomFolderPath.Name = "Special case: Explicit Path to Logs Directory";
		Input_CustomFolderPath.SetString("C:\\SierraChart\\Logs");
		Input_CustomFolderPath.SetDescription("If Previous Input Enabled: User must put in the exact path to the Logs Folder. This folder is found in the Sierra Chart Installation Directory. Example: if Sierra Chart is installed to Z:\\mnt\\SierraChart23\\ you would set this input to be: Z:\\mnt\\SierraChart23\\Logs");

		Input_FindDuplicateStudies.Name = "Find Duplicate Studies in Current Chartbook";
		Input_FindDuplicateStudies.SetYesNo(0);
		Input_FindDuplicateStudies.SetDescription("This input can be enabled to determine if there are duplicate instances of this study within the same chartbook. When set to yes and two or more instances of this study exist within the same chartbook, a message will be added to the Sierra Chart message log containing the chart number where the duplicate study is found."); 
		Input_CustomizeMessageContent.Name = "Remove Items from Message Text for Chart/Study Alerts";
		Input_CustomizeMessageContent.SetCustomInputStrings("Off;Remove Formula;Remove All Except Study Name");
		Input_CustomizeMessageContent.SetCustomInputIndex(0);
		Input_CustomizeMessageContent.SetDescription("This setting allows the user to remove the alert formula, or only keep the Study Name in the Telegram message text for Chart/Study Alerts."); 

		Input_Debug.Name = "Print CURL Request Response in Message Log";
		Input_Debug.SetYesNo(0);
		Input_Debug.SetDescription("This input is used to debug a http request by printing the reseponse to the SC Message log.");

		Input_ToggleAlertsACSButtonNumber.Name = "ACS Button Number To Enable/Disable Alerts";
		Input_ToggleAlertsACSButtonNumber.SetInt(50);
		Input_ToggleAlertsACSButtonNumber.SetIntLimits(1,MAX_ACS_CONTROL_BAR_BUTTONS);
		Input_ToggleAlertsACSButtonNumber.SetDescription("This button is used to enable or disable the alerts sent by this study. It controls Send Telegram On Alert Trigger. It offers a quicker way of seeing if alerts are enabled. It is faster to enable and disable alerts.");


		// Persistent variable precaution 
		if(FirstTimeStart != 0)
		{
			FirstTimeStart = 0;
		}	

		return;
		
	}
	// End Study Defaults 
	
	if(sc.IsUserAllowedForSCDLLName == false)
	{
		if(sc.Index == 0)
		{
		  sc.AddMessageToLog("You are not allowed to use this study. Please contact support@verrillotrading.com to obtain free access.",1);
		}
    	return;
	} 

	// ACS Button 
	if (sc.MenuEventID != 0 && sc.MenuEventID == Input_ToggleAlertsACSButtonNumber.GetInt())
	{
		int ButtonState = sc.GetCustomStudyControlBarButtonEnableState(Input_ToggleAlertsACSButtonNumber.GetInt());

		// If button was turned on, check our input and change it 
		if (ButtonState == 1 && Input_Enabled.GetBoolean() != 1)
		{
			// Set this to only get here once 
			Input_Enabled.SetYesNo(1); 

			// No need to recalculate chart 

			// Optional: Find instances of the study in the chartbook if there is more than one 
			// This is also convenient because any chart in the chartbook can be the currently selected chart 
			//
		}
		if(ButtonState == 0 && Input_Enabled.GetBoolean() != 0) 
		{
			// Set this to only get here once 
			Input_Enabled.SetYesNo(0); 

			// No need to recalculate chart 
		}
	} // End ACS Button 

	// Enabling and Disabling the study via the input
	// This also sets the ACS button accordingly 
	if(Input_Enabled.GetBoolean() == 0)
	{
		// debug 
		// sc.AddMessageToLog("Error: Study is disabled.",1);
		// check the state of the acs button 
		int ButtonState = sc.GetCustomStudyControlBarButtonEnableState(Input_ToggleAlertsACSButtonNumber.GetInt());

		// If button was turned on, check our input and change it 
		if(ButtonState != 0)
		{
			// turn off button 
			sc.SetCustomStudyControlBarButtonEnable(Input_ToggleAlertsACSButtonNumber.GetInt(), 0);

			// TODO: Possibly recalculate chart 
		}

		// the study is turned off so end function 
		return; 
	}
	else
	{
		// check the state of the acs button 
		int ButtonState = sc.GetCustomStudyControlBarButtonEnableState(Input_ToggleAlertsACSButtonNumber.GetInt());
		if(ButtonState != 1)
		{
			// turn on button 
			sc.SetCustomStudyControlBarButtonEnable(Input_ToggleAlertsACSButtonNumber.GetInt(), 1);
		}
	}


	// DO NOT TRIGGER STUDY IF WE ARE STILL DOWNLOADING HISTORICAL DATA
	// ON ANY CHARTS IN THE CHARTBOOK
	
	// Get the highest chart number in the current chartbook 
	int highest_chart_num = sc.GetHighestChartNumberUsedInChartBook();

	// Iterate from Chart number 1 to highest chart number 
	for(int ChartNumber = 1; ChartNumber <= highest_chart_num; ChartNumber++)
	{
		// Returns true if chart number exists in current chartbook,
		// empty string refers to current chartbook
		if(sc.IsChartNumberExist(ChartNumber, ""))
		{
			if(sc.ChartIsDownloadingHistoricalData(sc.ChartNumber))
			{
				// if the chart is downloading data, bomb out the study function 
				return;
			}
		}
	}

	// Thu Jul 18 16:49:35 EDT 2024
	// debug 
	if(Input_Debug.GetBoolean() == 1)
	{
		DebugResponse(sc);
	}

	// Check for the Chat ID. 
	if(strlen(Input_ChatID.GetString() ) == 0)
	{
		sc.AddMessageToLog("Error: You must specify a Telegram Chat ID.",1);
		return;
	}

	// variable for telegram bot api token
	SCString token = "";

	// If they want to use a custom bot, make sure they also filled in the token
	if(Input_UseCustomBot.GetBoolean() == 1)
	{
		// debug 
		/* sc.AddMessageToLog("User has requested to use a custom bot", 1); */

		if(strlen(Input_CustomBotToken.GetString() ) == 0) // Do not proceed if there is no Bot Token. 
		{
			msg.Format("Error: User Specified Custom Bot Token but Custom Bot API Token is not set.");
			sc.AddMessageToLog(msg,1);
			return;
		}
		else
		{
			// get the bot token from SC Input. Concatenate a plain string with SC String. 
			token = SCString("bot") + Input_CustomBotToken.GetString(); 	

			// debug 
			/* msg.Format("User Specified Bot Token: %s", token.GetChars()); */
			/* sc.AddMessageToLog(msg,1); */
		}
	}
	else
	{
		// User wants to use the default bot. On the github code there is no default bot so the study will just bomb out.
		/* sc.AddMessageToLog("If you see this message it means you compiled the study on your own. " */
		/* " It is necessary for you to use your own bot using the provided study input field.",1); */
		/* return; */ 

		/* token = SCString("bot") + "your_bot_token_here"; */

	}

	// get the Telegram Chat ID from SC Input. 
	std::string ChatID = Input_ChatID.GetString();

	// declare variables for the host and Telegram method being called
	SCString host = "https://api.telegram.org/";

	// two variables that will be changed later by the parsing function
	SCString sc_alert_chartbook = "";
	int sc_alert_chart_num = 0;

	// Get the current date time (used for formatting file name object)
	SCDateTime CurrentDateTime; 
	CurrentDateTime = sc.CurrentSystemDateTime; 

	// variable used to format image file naming 
	std::string SourceChartImageText = "";

	// Variable for Logs Folder Path
	std::string LogsFolderPath = ""; 
	
	// Confirm if the user specified the path in study input or not
	if(Input_SpecifyPath.GetBoolean() == 1)
	{
		// Catch if they turned it on but did not specify the path.  
		if(strlen(Input_CustomFolderPath.GetString()) == 0)
		{
			sc.AddMessageToLog("Error: User has specified Use Custom Path but no path was provided",1);
		}
		else
		{
			// set the explicitly specified path 
			LogsFolderPath = Input_CustomFolderPath.GetString();
		}
	}
	else
	{
		// Get the Folder Path using the SC Data Files Folder Path
		GetLogsFolderPath(sc, LogsFolderPath);
		if(LogsFolderPath == "Invalid_Path") // if it is invalid bomb out the study function
		{
			return; 
		}
	}

	// Variable to save the file write time in seconds to be compared against
	// the file write time in seconds in memory 
	int64_t Local_LastModTime = 0;

	// variable to save the file name 
	std::string most_recent_filename;

	// count number of files to check against number of files in memory 
	int number_of_files = 0;

	// Persistently REMEMBER the number of files 
	// This is set explicitly in two places:
	// 1. When the study iterates through the directory in the following code block 
	// 2. at the very end of the study function. Which is necessary for when there are no log files to begin
	// And a new log file has been created. 
	int& num_files_memory = sc.GetPersistentInt(10);

	// FIND THE MOST RECENT FILE 
	FindMostRecentFile(sc, LogsFolderPath, number_of_files, Local_LastModTime, most_recent_filename);

	// debug to check if we are getting the right file
	/* msg.Format("There are %d total files | Most recent filename: %s" */
	/* ,number_of_files, most_recent_filename.c_str()); */
	/* sc.AddMessageToLog(msg,1); */

	// Bomb out if there are no alert log files 
	if(number_of_files == 0)
	{
		/* sc.AddMessageToLog("there are no alert files found",1); */
		return; // bomb out until an alert file is found
	}

	// ENSURE on the first call to the function that number of files is initialized to MEMORY
	// NECESSARY for the check below that determines if a new log file was created.
	if(num_files_memory == 0 && number_of_files != 0)
	{
		// we should only get here if there are alert log files and number of files in
		// memory has not been initialized yet. 
	
		// REMEMBER the number of alert log files.
		num_files_memory = number_of_files;
	}	

	// Construct the full path by combining the directory path and filename
	// c++ filesystem variable which is explicitly set to the name of the file
	// with the most recent write time in the specified directory. 
    std::filesystem::path LogFile (LogsFolderPath + "\\" + most_recent_filename);
	if (std::filesystem::exists(LogFile)) 
	{
		// save the filename as a string 
		std::string filename = LogFile.filename().string();
		
		// debug 
		/* msg.Format("Log File Exists...Filename match: %s", filename.c_str()); */
		/* sc.AddMessageToLog(msg,1); */

		// SAVE the last write time of our file	
		std::filesystem::file_time_type currentModTime = std::filesystem::last_write_time(LogFile);

		// CONVERT it to time in seconds from epoch
		auto currentModTimePoint = std::chrono::duration_cast<std::chrono::seconds>
		(currentModTime.time_since_epoch()).count();

		if(num_files_memory != number_of_files)
		{
			// If we get here it means there has been a change
			// to the number of files in the Logs Directory 
			//
			// Case 1: Sierra Chart just created a new log file after being
			// opened, and we need to read the first line out of that new file
			// and send a Telegram.
			//
			// This handles the case for when SC is opened and the study is
			// already on the chart and SC creates a brand new alert log file
			// as soon as an alert is triggered.
			//
			// Case 2: We may also get here if a file is deleted or added to
			// the directory This is being handled on the following lines
			//
			//
			// bug fix 2023-11-20
			// This handles if a file gets removed from the logs folder while
			// the study is active because this is one case where number of
			// files in memory could be larger than number of files. 
			if(num_files_memory > number_of_files)
			{
				num_files_memory = number_of_files;
				return;
			}
			
			// SAVE THE last write time of our file TO MEMORY
			LastModTime = currentModTimePoint;

			// First read how many lines are in the file
			// open the file for reading
			std::ifstream file(LogFile);
			if (file) 
			{
				// Save the line text for formatting 
				std::string line;
				int CountLines = 0; // count the number of lines
				while (std::getline(file, line)) 
				{
					CountLines++;

					// REMEMBER globally the new number of lines 
					NumberOfLines = CountLines;

					// Process the line and send a Telegram
					
					// Handles text formatting for Chart/Study Alerts
					if(line.find("Type: Study") != std::string::npos)
					{
						// Get the user setting for customize message content
						int CustomizeMessageContent = Input_CustomizeMessageContent.GetIndex();

						// Parse the alert log file and prepare the text for the request 
						// Also return the chartbook name that generated the alert 
						ParseChartStudyAlertText(line, SourceChartImageText, CustomizeMessageContent, 
						sc_alert_chartbook, sc_alert_chart_num);

						// Get the name of the chartbook where this study is applied 
						SCString current_chartbook = sc.ChartbookName();

						// The alert came from the same chartbook as where this study exists
						// Each chartbook must handle it's own chart screenshots
						if(sc_alert_chartbook == current_chartbook)
						{
							// Feature: FIND DUPLICATE STUDIES IN THE SAME CHARTBOOK, ENABLED WITH A STUDY INPUT
							if(Input_FindDuplicateStudies.GetYesNo() )
							{
								// Call the function which simply returns a message in the log if a duplicate is found
								FindDuplicateStudiesInSameChartbook(sc, sc.GraphName.GetChars(), msg);
							}
							
							// Confirm which Telegram method the user specified
							if(Input_SendChartImage.GetBoolean() == 0)
							{
								// use sendMessage method instead of sendPhoto
								SCTelegramPostRequest(sc, host, token, ChatID, line, msg);
							}
							else
							{
								// set the method variable which is passed into the URL string
								SCString method = "/sendPhoto?";

								// Set our Telegram URL for the POST request 
								SCString URL = std::move(host) + std::move(token) + std::move(method);

								// get the year month and day from SCDateTimeVariable
								int Year, Month, Day, Hour, Minute, Second;
								CurrentDateTime.GetDateTimeYMDHMS(Year,Month,Day,Hour, Minute,Second);

								// Format the text for the image file name 
								// The source string combined with the current date time 
								msg.Format("\\%s %d-%d-%d %d_%d_%d.png", SourceChartImageText.c_str(), Year, Month, Day, 
								Hour, Minute, Second);

								// Create the file path to our image file 
								std::string FilePath = Input_ImageFolderPath.GetString()
								+ std::string(msg.GetChars());

								// HANDLE if the folder does not exist before accessing the directory 
								if (!std::filesystem::exists(Input_ImageFolderPath.GetString()) && 
								!std::filesystem::is_directory(Input_ImageFolderPath.GetString())) 
								{
									sc.AddMessageToLog("Error: Specified Image Folder Does not Exist! "
									"There may be an error with the request ",1);
								}

								// convert string into SC String to pass into SC screenshot function 
								SCString SC_FilePathName = FilePath.c_str();

								// Take a screen shot 
								sc.SaveChartImageToFileExtended(sc_alert_chart_num, SC_FilePathName, 0,0,0);

								// Call the HTTP POST Request in a separate thread. (not recommended for large scale operations)
								// This function calls sendPhoto Telegram method
								std::thread request_thread(CURLTelegramPostRequest, URL, ChatID, FilePath, line);

								// detach the thread and it will finish on it's own. 
								request_thread.detach();
							}
						}
						else
						{
							// the alert came from a different chartboook
							// Simply do nothing and let the study function finish 
							
							/* sc.AddMessageToLog("Alert was generated from a different chartbook. " */
							/* "That chartbook will handle the alert.",1); */
						}
					}
					else if(line.find("Type: Drawing") != std::string::npos)
					{
						// Get the user setting for customize message content
						int CustomizeMessageContent = Input_CustomizeMessageContent.GetIndex();

						// Parse the alert log file and prepare the text for the request 
						// Also return the chartbook name that generated the alert 
						ParseChartDrawingAlertText(line, SourceChartImageText, CustomizeMessageContent, sc_alert_chartbook, sc_alert_chart_num, sc);

						// Get the name of the chartbook where this study is applied 
						SCString current_chartbook = sc.ChartbookName();

						// the alert came from the same chartbook as where this study exists
						if(sc_alert_chartbook == current_chartbook)
						{
							// FIND DUPLICATE STUDIES IN THE SAME CHARTBOOK, ENABLED WITH A STUDY INPUT
							if(Input_FindDuplicateStudies.GetYesNo() )
							{
								// Call the function which simply returns a message in the log if a duplicate is found
								FindDuplicateStudiesInSameChartbook(sc, sc.GraphName.GetChars(), msg);
							}

							// SEND TELEGRAM message or message with photo 
							// Confirm which Telegram method the user specified
							if(Input_SendChartImage.GetBoolean() == 0)
							{
								// use sendMessage method instead of sendPhoto
								SCTelegramPostRequest(sc, host, token, ChatID, line, msg);
							}
							else
							{
								// set the method variable which is passed into the URL string
								SCString method = "/sendPhoto?";

								// get the year month and day from SCDateTimeVariable
								int Year, Month, Day, Hour, Minute, Second;
								CurrentDateTime.GetDateTimeYMDHMS(Year,Month,Day,Hour, Minute,Second);

								// Format the text for the image file name 
								// The source string combined with the current date time 
								msg.Format("\\%s %d-%d-%d %d_%d_%d.png", SourceChartImageText.c_str(), Year, Month, Day, 
								Hour, Minute, Second);

								// Create the file path to our image file 
								std::string FilePath = Input_ImageFolderPath.GetString()
								+ std::string(msg.GetChars());

								// NOTIFY if the folder does not exist before accessing the directory 
								if (!std::filesystem::exists(Input_ImageFolderPath.GetString()) && 
								!std::filesystem::is_directory(Input_ImageFolderPath.GetString())) 
								{
									sc.AddMessageToLog("Error: Specified Image Folder Does not Exist! "
									"There may be an error with the request ",1);
								}

								// convert/move string into SC String to pass into SC function 
								SCString SC_FilePathName = FilePath.c_str();

								// Take a screen shot and put it in our logs folder directory for safety 
								sc.SaveChartImageToFileExtended(sc_alert_chart_num, SC_FilePathName, 0,0,0);

								// Set our Telegram URL for the POST request 
								SCString URL = std::move(host) + std::move(token) + std::move(method);

								// Call the HTTP POST Request in a separate thread. (not recommended for large scale operations)
								// This function calls sendPhoto Telegram method
								std::thread request_thread(CURLTelegramPostRequest, URL, ChatID, FilePath, line );

								// detach the thread and it will finish on it's own. 
								request_thread.detach();
							}
						}
						else
						{
							// the alert came from a different chartboook
							// Simply do nothing 
							//
							/* sc.AddMessageToLog("Alert was generated from a different chartbook. " */
							/* "That chartbook will handle the alert.",1); */
						}
					}
					else if(line.find("Type: Symbol") != std::string::npos)
					{
						// sc.AddMessageToLog("symbol alert we get here!",1)
						// Symbol alerts are similar to chart drawing alerts
						// with a few minor differences 
						//
						/* msg.Format("line: %s", line.c_str()); */
						/* sc.AddMessageToLog(msg,1); */

						// // NO CHART IMAGE SUPPORT FOR SYMBOL ALERTS 
						// SCTelegramPostRequest(sc, host, token, ChatID, line, msg);

						// Get the user setting for customize message content
						int CustomizeMessageContent = Input_CustomizeMessageContent.GetIndex();

						// to be set in upcoming function
						SCString AlertSymbol = "";

						// Parse the alert log file and prepare the text for the request 
						// Also return the symbol name that generated the alert 
						ParseSymbolAlertText(line, SourceChartImageText, CustomizeMessageContent, sc, AlertSymbol);

						// Get the name of the chartbook where this study is applied 
						SCString current_chartbook = sc.ChartbookName();

						// The alert came from the same chartbook as where this study exists
						// Each chartbook must handle it's own chart screenshots
						// Feature: FIND DUPLICATE STUDIES IN THE SAME CHARTBOOK, ENABLED WITH A STUDY INPUT
						if(Input_FindDuplicateStudies.GetYesNo() )
						{
							// Call the function which simply returns a message in the log if a duplicate is found
							FindDuplicateStudiesInSameChartbook(sc, sc.GraphName.GetChars(), msg);
						}
						
						// Confirm which Telegram method the user specified
						if(Input_SendChartImage.GetBoolean() == 0)
						{
							// use sendMessage method instead of sendPhoto
							SCTelegramPostRequest(sc, host, token, ChatID, line, msg);
						}
						else
						{
							// TODO: Since Symbol alerts are not linked to any chart or chartbook, we need to work around this. 
							// Potentially scan the current chartbook for the symbol 
							// if the symbol is found, obtain the chart number (preferably an intraday chart)
							// and take a photo of that chart. 
							// otherwise resort back to the non photo method 
							//
							int AlertSymbolChartNumber = AlertSymbolExistsInChartBook(sc, AlertSymbol, msg);
							if(AlertSymbolChartNumber != 0)
							{
								// set the method variable which is passed into the URL string
								SCString method = "/sendPhoto?";

								// Set our Telegram URL for the POST request 
								SCString URL = std::move(host) + std::move(token) + std::move(method);

								// get the year month and day from SCDateTimeVariable
								int Year, Month, Day, Hour, Minute, Second;
								CurrentDateTime.GetDateTimeYMDHMS(Year,Month,Day,Hour, Minute,Second);

								// Format the text for the image file name 
								// The source string combined with the current date time 
								msg.Format("\\%s %d-%d-%d %d_%d_%d.png", SourceChartImageText.c_str(), Year, Month, Day, 
								Hour, Minute, Second);

								// Create the file path to our image file 
								std::string FilePath = Input_ImageFolderPath.GetString()
								+ std::string(msg.GetChars());

								// HANDLE if the folder does not exist before accessing the directory 
								if (!std::filesystem::exists(Input_ImageFolderPath.GetString()) && 
								!std::filesystem::is_directory(Input_ImageFolderPath.GetString())) 
								{
									sc.AddMessageToLog("Error: Specified Image Folder Does not Exist! "
									"There may be an error with the request ",1);
								}

								// convert string into SC String to pass into SC screenshot function 
								SCString SC_FilePathName = FilePath.c_str();

								// Take a screen shot 
								sc.SaveChartImageToFileExtended(AlertSymbolChartNumber, SC_FilePathName, 0,0,0);

								// Call the HTTP POST Request in a separate thread. (not recommended for large scale operations)
								// This function calls sendPhoto Telegram method
								std::thread request_thread(CURLTelegramPostRequest, URL, ChatID, FilePath, line);

								// detach the thread and it will finish on it's own. 
								request_thread.detach();
							}
							else
							{
								// otherwise resort back to the non photo method 
								// use sendMessage method instead of sendPhoto
								SCTelegramPostRequest(sc, host, token, ChatID, line, msg);
							}
						}
					}
				}
				// close the alert log file (Outer most file) 
				file.close();
			}
			else
			{
				sc.AddMessageToLog("Error opening file!#@",1);
			}
		}	

		// FIRST TIME READ FILE
		if(FirstTimeStart == 0)
		{
			// We should get here if the study has just been initialized on the chart aka 
			// Sierra Chart has just been opened. 
			//
			// The first time the study reads the directory no http requests are sent. 

			// SAVE THE last write time of our file TO MEMORY
			LastModTime = currentModTimePoint;

			FirstTimeStart = 1; // set this variable to 1 in order to not trigger again
			
			// First read how many lines are in the file
			
			// open the file for reading
			std::ifstream file(LogFile);
			if (file) 
			{
				// count the number of lines in the file 
				std::string line;
				int CountLines = 0; 
				while (std::getline(file, line)) 
				{
					CountLines++;
				}

				// Remember globally the number of lines
				NumberOfLines = CountLines;

				// close the file 
				file.close();
			}
			else
			{
				sc.AddMessageToLog("Could not open file!!!", 1);
			}
			
		} 
		else // EVERY SUBSEQUENT TIME WE READ THE FILE AFTER SC HAS CREATED A NEW ALERT LOG FILE WE GET HERE 
		{
			// Poll the file for a change to the last write time. (time in seconds from epoch)
			if(currentModTimePoint > LastModTime)
			{
				// We get here if write time is different than what was previously saved

				// This means there was an update to the file 
				// and we need to process the update that was made to the file

				// update the last modified time of the file in memory
				LastModTime = currentModTimePoint;
				
				// open the file for reading (precautionary first check)
				std::ifstream file(LogFile);
				if (file) 
				{
					// Variable used to get the line and also format the line to output 
					std::string line;

					// Precaution: if number of lines has not been initialized we need to read it first to figure it out
					if(NumberOfLines == 0)
					{
						int CountLines = 0; // count the number of lines
						while (std::getline(file, line)) 
						{
							CountLines++;
						}
						NumberOfLines = CountLines;
					}

					// Precaution/good practice: close the file after first read because the next command opens it again
					file.close();

					// Open the file for reading 
					std::ifstream file(LogFile);
					if (file) 
					{
						int CountLines = 0; // count the number of lines
						while (std::getline(file, line)) 
						{
							CountLines++;

							// Check if line counter is greater than previously
							// saved number of lines (only process new lines)
							if(CountLines > NumberOfLines)
							{
								// REMEMBER globally the new number of lines 
								NumberOfLines = CountLines;

								// Check for if its a Chart/Study Alert or Chart Drawing Alert 
								if(line.find("Type: Study") != std::string::npos)
								{
									// Get the user setting for customize message content
									int CustomizeMessageContent = Input_CustomizeMessageContent.GetIndex();

									// Parse the alert log file and prepare the text for the request 
									// Also return the chartbook name that generated the alert 
									ParseChartStudyAlertText(line, SourceChartImageText, CustomizeMessageContent, 
									sc_alert_chartbook, sc_alert_chart_num);

									// Get the name of the chartbook where this study is applied 
									SCString current_chartbook = sc.ChartbookName();

									// The alert came from the same chartbook as where this study exists
									// Each chartbook must handle it's own chart screenshots
									if(sc_alert_chartbook == current_chartbook)
									{
										// Feature: FIND DUPLICATE STUDIES IN THE SAME CHARTBOOK, ENABLED WITH A STUDY INPUT
										if(Input_FindDuplicateStudies.GetYesNo() )
										{
											// Call the function which simply returns a message in the log if a duplicate is found
											FindDuplicateStudiesInSameChartbook(sc, sc.GraphName.GetChars(), msg);
										}
										
										// Confirm which Telegram method the user specified
										if(Input_SendChartImage.GetBoolean() == 0)
										{
											// use sendMessage method instead of sendPhoto
											SCTelegramPostRequest(sc, host, token, ChatID, line, msg);
										}
										else
										{
											// set the method variable which is passed into the URL string
											SCString method = "/sendPhoto?";

											// Set our Telegram URL for the POST request 
											SCString URL = std::move(host) + std::move(token) + std::move(method);

											// get the year month and day from SCDateTimeVariable
											int Year, Month, Day, Hour, Minute, Second;
											CurrentDateTime.GetDateTimeYMDHMS(Year,Month,Day,Hour, Minute,Second);

											// Format the text for the image file name 
											// The source string combined with the current date time 
											msg.Format("\\%s %d-%d-%d %d_%d_%d.png", SourceChartImageText.c_str(), Year, Month, Day, 
											Hour, Minute, Second);

											// Create the file path to our image file 
											std::string FilePath = Input_ImageFolderPath.GetString()
											+ std::string(msg.GetChars());

											// HANDLE if the folder does not exist before accessing the directory 
											if (!std::filesystem::exists(Input_ImageFolderPath.GetString()) && 
											!std::filesystem::is_directory(Input_ImageFolderPath.GetString())) 
											{
												sc.AddMessageToLog("Error: Specified Image Folder Does not Exist! "
												"There may be an error with the request ",1);
											}

											// convert string into SC String to pass into SC screenshot function 
											SCString SC_FilePathName = FilePath.c_str();

											// Take a screen shot and put it in our logs folder directory for safety 
											sc.SaveChartImageToFileExtended(sc_alert_chart_num, SC_FilePathName, 0,0,0);

											// Call the HTTP POST Request in a separate thread. 
											// (not recommended for large scale operations)
											// This function calls sendPhoto Telegram method
											std::thread request_thread(CURLTelegramPostRequest, URL, ChatID, FilePath, line );

											// detach the thread and it will finish on it's own. 
											request_thread.detach();
										}
									}
									else
									{
										// the alert came from a different chartboook
										// Simply do nothing and let the study function finish 
										// Important: 1 instance of this study will be necessary per chartbook 
										// in order to handle taking screenshots of those charts in the other chartbooks 
										
										/* sc.AddMessageToLog("Alert was generated from a different chartbook. " */
										/* "That chartbook will handle the alert.",1); */
									}

								}
								else if(line.find("Type: Drawing") != std::string::npos)
								{
									// Get the user setting for customize message content
									int CustomizeMessageContent = Input_CustomizeMessageContent.GetIndex();

									// Parse the alert log file and prepare the text for the request 
									// Also return the chartbook name that generated the alert 
									ParseChartDrawingAlertText(line, SourceChartImageText, CustomizeMessageContent, sc_alert_chartbook, sc_alert_chart_num, sc);

									// Get the name of the chartbook where this study is applied 
									SCString current_chartbook = sc.ChartbookName();

									// the alert came from the same chartbook as where this study exists
									if(sc_alert_chartbook == current_chartbook)
									{
										// FIND DUPLICATE STUDIES IN THE SAME CHARTBOOK, ENABLED WITH A STUDY INPUT
										if(Input_FindDuplicateStudies.GetYesNo() )
										{
											// Call the function which simply returns a message in the log if a duplicate is found
											FindDuplicateStudiesInSameChartbook(sc, sc.GraphName.GetChars(), msg);
										}


										if(Input_SendChartImage.GetBoolean() == 0)
										{
											// use sendMessage method instead of sendPhoto
											SCTelegramPostRequest(sc, host, token, ChatID, line, msg);
										}
										else
										{
											// set the method variable which is passed into the URL string
											SCString method = "/sendPhoto?";

											// Set our Telegram URL for the POST request 
											SCString URL = std::move(host) + std::move(token) + std::move(method);

											// get the year month and day from SCDateTimeVariable
											int Year, Month, Day, Hour, Minute, Second;
											CurrentDateTime.GetDateTimeYMDHMS(Year,Month,Day,Hour, Minute,Second);

											// Format the text for the image file name 
											// The source string combined with the current date time 
											msg.Format("\\%s %d-%d-%d %d_%d_%d.png", SourceChartImageText.c_str(), Year, Month, Day, 
											Hour, Minute, Second);

											// debug image text
											/* sc.AddMessageToLog(msg,1); */

											// Create the file path to our image file 
											std::string FilePath = Input_ImageFolderPath.GetString()
											+ std::string(msg.GetChars());

											// HANDLE if the folder does not exist before accessing the directory 
											if (!std::filesystem::exists(Input_ImageFolderPath.GetString()) && 
											!std::filesystem::is_directory(Input_ImageFolderPath.GetString())) 
											{
												sc.AddMessageToLog("Error: Specified Image Folder Does not Exist! "
												"There may be an error with the request ",1);
											}

											// convert string into SC String to pass into SC screenshot function 
											SCString SC_FilePathName = FilePath.c_str();

											// Take a screen shot and put it in our logs folder directory for safety 
											sc.SaveChartImageToFileExtended(sc_alert_chart_num, SC_FilePathName, 0,0,0);

											// Call the HTTP POST Request in a separate thread. 
											// (not recommended for large scale operations)
											// This function calls sendPhoto Telegram method
											std::thread request_thread(CURLTelegramPostRequest, URL, ChatID, FilePath, line );

											// detach the thread and it will finish on it's own. 
											request_thread.detach();
										}
									}
									else
									{
										// the alert came from a different chartboook
										// Simply do nothing and let the study function finish 
										// Important: 1 instance of this study will be necessary per chartbook 
										// in order to handle taking screenshots of those charts in the other chartbooks 
										
										/* sc.AddMessageToLog("Alert was generated from a different chartbook. " */
										/* "That chartbook will handle the alert.",1); */
									}
								}
								else if(line.find("Type: Symbol") != std::string::npos)
								{
									// sc.AddMessageToLog("symbol alert we get here!",1)
									// Symbol alerts are similar to chart drawing alerts
									// with a few minor differences 
									//
									/* msg.Format("line: %s", line.c_str()); */
									/* sc.AddMessageToLog(msg,1); */

									// // NO CHART IMAGE SUPPORT FOR SYMBOL ALERTS 
									// SCTelegramPostRequest(sc, host, token, ChatID, line, msg);

									// Get the user setting for customize message content
									int CustomizeMessageContent = Input_CustomizeMessageContent.GetIndex();


									// to be set in upcoming function
									SCString AlertSymbol = "";

									// Parse the alert log file and prepare the text for the request 
									// Also return the symbol name that generated the alert 
									ParseSymbolAlertText(line, SourceChartImageText, CustomizeMessageContent, sc, AlertSymbol);

									// Debug to make sure we have correct symbol 
									// msg.Format("AlertSymbol: %s", AlertSymbol.GetChars());
									// sc.AddMessageToLog(msg,1);

									// Get the name of the chartbook where this study is applied 
									SCString current_chartbook = sc.ChartbookName();

									// The alert came from the same chartbook as where this study exists
									// Each chartbook must handle it's own chart screenshots
									// Feature: FIND DUPLICATE STUDIES IN THE SAME CHARTBOOK, ENABLED WITH A STUDY INPUT
									if(Input_FindDuplicateStudies.GetYesNo() )
									{
										// Call the function which simply returns a message in the log if a duplicate is found
										FindDuplicateStudiesInSameChartbook(sc, sc.GraphName.GetChars(), msg);
									}
									
									// Confirm which Telegram method the user specified
									if(Input_SendChartImage.GetBoolean() == 0)
									{
										// use sendMessage method instead of sendPhoto
										SCTelegramPostRequest(sc, host, token, ChatID, line, msg);
									}
									else
									{
										// TODO: Since Symbol alerts are not linked to any chart or chartbook, we need to work around this. 
										// Potentially scan the current chartbook for the symbol 
										// if the symbol is found, obtain the chart number (preferably an intraday chart)
										// and take a photo of that chart. 
										// otherwise resort back to the non photo method 
										//
										int AlertSymbolChartNumber = AlertSymbolExistsInChartBook(sc, AlertSymbol, msg);
										if(AlertSymbolChartNumber != 0)
										{
											// set the method variable which is passed into the URL string
											SCString method = "/sendPhoto?";

											// Set our Telegram URL for the POST request 
											SCString URL = std::move(host) + std::move(token) + std::move(method);

											// get the year month and day from SCDateTimeVariable
											int Year, Month, Day, Hour, Minute, Second;
											CurrentDateTime.GetDateTimeYMDHMS(Year,Month,Day,Hour, Minute,Second);

											// Format the text for the image file name 
											// The source string combined with the current date time 
											msg.Format("\\%s %d-%d-%d %d_%d_%d.png", SourceChartImageText.c_str(), Year, Month, Day, 
											Hour, Minute, Second);

											// Create the file path to our image file 
											std::string FilePath = Input_ImageFolderPath.GetString()
											+ std::string(msg.GetChars());

											// HANDLE if the folder does not exist before accessing the directory 
											if (!std::filesystem::exists(Input_ImageFolderPath.GetString()) && 
											!std::filesystem::is_directory(Input_ImageFolderPath.GetString())) 
											{
												sc.AddMessageToLog("Error: Specified Image Folder Does not Exist! "
												"There may be an error with the request ",1);
											}

											// convert string into SC String to pass into SC screenshot function 
											SCString SC_FilePathName = FilePath.c_str();

											// Take a screen shot 
											sc.SaveChartImageToFileExtended(AlertSymbolChartNumber, SC_FilePathName, 0,0,0);

											// Call the HTTP POST Request in a separate thread. (not recommended for large scale operations)
											// This function calls sendPhoto Telegram method
											std::thread request_thread(CURLTelegramPostRequest, URL, ChatID, FilePath, line);

											// detach the thread and it will finish on it's own. 
											request_thread.detach();
										}
										else
										{
											// otherwise resort back to the non photo method 
											// use sendMessage method instead of sendPhoto
											SCTelegramPostRequest(sc, host, token, ChatID, line, msg);
										}
									}
								}
							}
						}

						// close the alert log file (Outer most file) 
						file.close();
					}
					else
					{
						sc.AddMessageToLog("Error opening file! 2",1);
					}

				} 
				else
				{
					sc.AddMessageToLog("Error opening file! 1",1);
				}
			}
		}
	}

	if(sc.LastCallToFunction)
	{
		/* sc.AddMessageToLog("Last Call To Function!", 1); */
		FirstTimeStart = 0;
		LastModTime = 0; 
		NumberOfLines = 0; 
	}

	// Remember the number of alert log files between calls to the study function
	// This is necessary to handle when Sierra Chart is first opened and a new
	// alert log file gets generated.
	if(num_files_memory != number_of_files)
	{
		num_files_memory = number_of_files;
	}
}

/*==========================================================================*/
// ARG2: 1 signals a target order, 2 signals a stop order 
int GetNearestActiveOrder(SCStudyInterfaceRef& sc, int TargetOrStop, double& LastPrice, s_SCTradeOrder& Order, double& PositionIncludingWorkingOrders, double& ClosestOrderPrice)
{
	// This function is used to find only open attached orders or open OCO
	// orders otherwise the study defaults to another logic if none are found 
	uint32_t OrderIDToModify = 0;
	double LowestDifference = 0;
	int Index = 0;
	while(sc.GetOrderByIndex (Index, Order) != SCTRADING_ORDER_ERROR)
	{
		Index++; // Increment the index for the next call to sc.GetOrderByIndex
	  
		// if the order is NOT OPEN or HELD STATUS, continue to next order
		if (Order.OrderStatusCode != SCT_OSC_OPEN && Order.OrderStatusCode != SCT_OSC_HELD)
		{
			continue;
		}

		// Order is OPEN or HELD status
		else if(Order.OrderStatusCode == SCT_OSC_OPEN
			|| Order.OrderStatusCode == SCT_OSC_HELD) 
		{
			if(TargetOrStop == 1) // is target order code block
			{
				if( (IsLimitOrderV2(Order.OrderTypeAsInt) && Order.IsAttachedOrder() )
					|| (IsLimitOrderV2(Order.OrderTypeAsInt) && Order.OCOSiblingInternalOrderID != 0))
				{
					double difference = std::abs(Order.Price1 - LastPrice);
					
					// first time init
					if(LowestDifference == 0 )
					{
						LowestDifference = difference;
						ClosestOrderPrice = sc.RoundToTickSize(Order.Price1, sc.TickSize);
						OrderIDToModify = Order.InternalOrderID;
					}
					else if(LowestDifference > difference)
					{
						// return the order with the lowest price
						// difference from last price 
						LowestDifference = difference; 
						ClosestOrderPrice = sc.RoundToTickSize(Order.Price1, sc.TickSize);
						OrderIDToModify = Order.InternalOrderID;
					}
				}
				else
				{
					// order is not a limit order or is not an attached order 
					continue;
				}
			}
			else if(TargetOrStop == 2) // is stop order code block
			{
				if( (IsStopOrder(Order.OrderTypeAsInt) && Order.IsAttachedOrder())
					||
					(IsStopOrder(Order.OrderTypeAsInt) && Order.OCOSiblingInternalOrderID != 0))
				{
					double difference = std::abs(Order.Price1 - LastPrice);
					
					// first time init
					if(LowestDifference == 0 )
					{
						LowestDifference = difference;
						ClosestOrderPrice = sc.RoundToTickSize(Order.Price1, sc.TickSize);
						OrderIDToModify = Order.InternalOrderID;
					}
					else if(LowestDifference > difference)
					{
						// return the order with the lowest price
						// difference from last price 
						LowestDifference = difference; 
						ClosestOrderPrice = sc.RoundToTickSize(Order.Price1, sc.TickSize);
						OrderIDToModify = Order.InternalOrderID;
					}
				}
				else
				{
					// order is not a limit order or is not an attached order 
					continue;
				}
			}
		}
	}

	return OrderIDToModify;
}
SCSFExport scsf_ClosestOrderToPrice(SCStudyInterfaceRef sc)
{
	SCInputRef Input_MoveClosestAttachedOrderACSButtonNumber = sc.Input[0];
	SCInputRef Input_PrioritizeActiveOrders = sc.Input[1];
	SCInputRef Input_EnableMoveClosestUnattachedOrder = sc.Input[2];
	SCInputRef Input_MoveClosestUnattachedBuyOrderACSButtonNumber = sc.Input[3];
	SCInputRef Input_MoveClosestUnattachedSellOrderACSButtonNumber = sc.Input[4];
	
	if(sc.SetDefaults)
	{
		sc.StudyDescription = 
		" <br> <br> This is a custom study that performs a feature that we originally discovered in the platform "
		"Jigsaw Daytradr in 2020-2021. It moves the closest OCO order (either the target or stop) to the price you hover the "
		"cursor over on the chart or trading DOM."
		" It is a method of moving attached orders very fast when manually trading. This study is to be used with "
	   	"OCO orders and it will not function with unattached limit orders."
		" There is no need for this because an OCO order should always be used, for the purpose of risk management "
	   	"and protection of your trading account."
		" <br> <br> The instructions for how to use this study are provided in "
	  	"<a href = https://youtu.be/dBVuaFOiNt8 target=_blank rel=noopener noreferrer >This Video</a> by VerrilloTrading. "
		"There is one missing step in that video which is #3 in the following text."
		" <br> <br> Instructions:"
		" <br> 1. Add the study to your chart and choose the desired ACS button to perform the function."
		"<br>  2. Program this ACS button to a keyboard shortcut."
		" <br> 3. On your trading chart or DOM, make sure that Trade > Auto Trading Enabled - Global and"
	    " Auto Trading Enabled - Chart are both turned on." 

		" <br> <br> Here is an <a href = https://youtu.be/Sntmqs92xK4 target=_blank rel=noopener noreferrer >additional video</a> that shows some improvements that were made in April 2024. "

		" <br> <br> For any issues or inquiries, send them to support@verrillotrading.com"

		"<br> <br> Thank you and happy trading."
		"<br> <br> - VerrilloTrading, Content Creator - Programmer";

		std::string StudyGraphName = "Trading - Move Closest Orders To Price | v" + std::to_string(ver) + " |";
		sc.GraphName = StudyGraphName.c_str();
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.GraphRegion = 0;
		sc.AutoLoop		= 0;
		
		sc.ReceivePointerEvents = ACS_RECEIVE_POINTER_EVENTS_WHEN_ACS_BUTTON_ENABLED;
		// sc.UpdateAlways = 1;
		
		Input_MoveClosestAttachedOrderACSButtonNumber.Name = "Move Closest Attached Order ACS Control Bar Button #";
		Input_MoveClosestAttachedOrderACSButtonNumber.SetInt(25);
		Input_MoveClosestAttachedOrderACSButtonNumber.SetIntLimits(1, MAX_ACS_CONTROL_BAR_BUTTONS);
		Input_MoveClosestAttachedOrderACSButtonNumber.SetDescription
		("This is the Advanced Custom Study Button that will be used to move the nearest attached target or stop order.");

		
		Input_PrioritizeActiveOrders.Name = "Always Prioritize Active Attached Orders";
		Input_PrioritizeActiveOrders.SetYesNo(1);
		Input_PrioritizeActiveOrders.SetDescription("This Input allows the user to enable only moving attached orders that have the active status, therefore ignoring orders that are pending.");


		Input_EnableMoveClosestUnattachedOrder.Name = "Enable Move Closest Unattached Limit Orders";
		Input_EnableMoveClosestUnattachedOrder.SetYesNo(0);
		Input_EnableMoveClosestUnattachedOrder.SetDescription("This Input controls if the study will move the closest unattached order using an ACS Button");


		Input_MoveClosestUnattachedBuyOrderACSButtonNumber.Name = "Move Closest Unattached Buy Order ACS Control Bar Button #";
		Input_MoveClosestUnattachedBuyOrderACSButtonNumber.SetInt(26);
		Input_MoveClosestUnattachedBuyOrderACSButtonNumber.SetIntLimits(1, MAX_ACS_CONTROL_BAR_BUTTONS);
		Input_MoveClosestUnattachedBuyOrderACSButtonNumber.SetDescription
		("This is the Advanced Custom Study Button that will be used to move the nearest unattached buy limit order.");

		Input_MoveClosestUnattachedSellOrderACSButtonNumber.Name = "Move Closest Unattached Sell Order ACS Control Bar Button #";
		Input_MoveClosestUnattachedSellOrderACSButtonNumber.SetInt(27);
		Input_MoveClosestUnattachedSellOrderACSButtonNumber.SetIntLimits(1, MAX_ACS_CONTROL_BAR_BUTTONS);
		Input_MoveClosestUnattachedSellOrderACSButtonNumber.SetDescription
		("This is the Advanced Custom Study Button that will be used to move the nearest unattached sell limit order.");

		return;
	}

	SCString msg;
	
	if(sc.IsUserAllowedForSCDLLName == false)
	{
		if(sc.Index == 0)
		{
		  sc.AddMessageToLog("You are not allowed to use this study. Please contact support@verrillotrading.com to obtain free access.",1);
		}
    	return;
	} 
	
	if (sc.MenuEventID != 0)	
	{
		if (!sc.ChartTradeModeEnabled)
		{
			// sc.AddMessageToLog("Chart Trade Mode is not active. No action performed.", 1);
			return;
		}
	}
	
	//For safety we must never do any order management while historical data is being downloaded.
	if (sc.ChartIsDownloadingHistoricalData(sc.ChartNumber))
	{
		//sc.AddMessageToLog("Chart is downloading historical data, no order management allowed.", 1);
		return;
	}
	
	// This line allows the study to be used for live trading
	sc.SendOrdersToTradeService = !sc.GlobalTradeSimulationIsOn;

	// ACS Button Press (Move Closest Attached Order) 
	if (sc.MenuEventID != 0 && sc.MenuEventID == Input_MoveClosestAttachedOrderACSButtonNumber.GetInt() )
	{
		// For this button press we will reset the button back to off state
		// since pressing the button executes a function
		const int ButtonState = (sc.PointerEventType == SC_ACS_BUTTON_ON) ? 1 : 0;
		if (ButtonState == 1)
		{
			sc.SetCustomStudyControlBarButtonEnable(Input_MoveClosestAttachedOrderACSButtonNumber.GetInt(), 0);
		}

		// DECLARATIONS 
		// Get position data
		s_SCPositionData PositionData;
		sc.GetTradePosition(PositionData);

		// extract items from the position data struct
		double& Position = PositionData.PositionQuantity;
		double& PositionIncludingWorkingOrders = PositionData.PositionQuantityWithAllWorkingOrdersExceptAllExits;
		double LastPrice = sc.GetLastPriceForTrading();

		// used to save the price of the order that is obtained. 
		double ClosestOrderPrice = 0;

		
		// Single SC Trade Order Object used for retrieving both Targets and Stops 
		s_SCTradeOrder Order; 	

		// HANDLE EVENTS 
		
		// NOTE: RIght now we have an issue where our func is not triggering so we need to think about how to simplify the code a bit 
		//
		// Button pressed and in a long position and cursor is above the last
		// traded price, Move closest target order
		if (PositionIncludingWorkingOrders >= 1 && sc.ChartTradingOrderPrice >= sc.Bid)
		{
			if (sc.GetNearestTargetOrder(Order))
			{
				// set the order price here in case no other items get triggered 
				ClosestOrderPrice = sc.RoundToTickSize(Order.Price1, sc.TickSize);

				// FUNCTION VERSION 
				if(Input_PrioritizeActiveOrders.GetYesNo())
				{
					// sc.AddMessageToLog("we get here!",1);
					// Check if there is an active order to prioritize 
					int OrderIDToModify = GetNearestActiveOrder(sc, 1, LastPrice, Order, 
						PositionIncludingWorkingOrders, ClosestOrderPrice);
					if(OrderIDToModify != 0)
					{
						// sc.AddMessageToLog("we get here!",1);
						// do not commit the modification if the modification
						// price is same as the order price 
						if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
						{
							// NO NEED FOR TRADINGVIEW PRECAUTION HERE BECAUSE
							// THE ORDER IS ACTIVE AND WE ARE IN A POSITION
							s_SCNewOrder ModifyTarget;
							ModifyTarget.InternalOrderID = OrderIDToModify;
							ModifyTarget.Price1 = sc.ChartTradingOrderPrice;
							sc.ModifyOrder(ModifyTarget);
						}
					}
					else // no active order was found by previous function
					{
						// WE get here if no active target or stop orders were
						// found, now we need to handle their modification here
						// with the same checks we used in the function. 
						//
						uint32_t OrderIDToModify = 0;
						double LowestDifference = 0;
						int Index = 0;
						while(sc.GetOrderByIndex (Index, Order) != SCTRADING_ORDER_ERROR)
						{
							Index++; // Increment the index for the next call to sc.GetOrderByIndex
						  
							if((IsWorkingOrderStatus(Order.OrderStatusCode) 
								&& IsLimitOrderV2(Order.OrderTypeAsInt) 
								&& Order.ParentInternalOrderID != 0 )
								|| 
								(IsWorkingOrderStatus(Order.OrderStatusCode)
								&& IsLimitOrderV2(Order.OrderTypeAsInt)
							   	&& Order.OCOSiblingInternalOrderID != 0	))
							{
								double difference = std::abs(Order.Price1 - LastPrice);
								
								// first time init
								if(LowestDifference == 0 )
								{
									LowestDifference = difference;
									ClosestOrderPrice = sc.RoundToTickSize(Order.Price1, sc.TickSize);
									OrderIDToModify = Order.InternalOrderID;
								}
								else if(LowestDifference > difference)
								{
									// return the order with the lowest price
									// difference from last price 
									LowestDifference = difference; 
									ClosestOrderPrice = sc.RoundToTickSize(Order.Price1, sc.TickSize);
									OrderIDToModify = Order.InternalOrderID;
								}
							}
							else
							{
								// order is not a limit order or is not an attached order 
								continue;
							}

						}
						// precaution 
						
						if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
						{
							// Check if the order has a parent order 
							s_SCTradeOrder Order; 
							sc.GetOrderByOrderID(OrderIDToModify, Order);
							if(Order.ParentInternalOrderID != 0)
							{
								// Get the parent order to check it's price 
								s_SCTradeOrder ParentOrder;
								sc.GetOrderByOrderID(Order.ParentInternalOrderID, ParentOrder);

								// MOVE Sell Limit for a Buy Position 
								// Only do this target modification if the
								// requested price is above the parent order
								// price (Like TradingView)
								if(sc.ChartTradingOrderPrice > ParentOrder.Price1)
								{
									s_SCNewOrder ModifyStop;
									ModifyStop.InternalOrderID = OrderIDToModify;
									ModifyStop.Price1 = sc.ChartTradingOrderPrice;
									sc.ModifyOrder(ModifyStop);
									return;
								}
								else if(sc.ChartTradingOrderPrice < ParentOrder.Price1)
								{
									// Get the other attached order in the pair instead to permit it's movement
									s_SCTradeOrder OCOSibling;
									sc.GetOrderByOrderID(Order.OCOSiblingInternalOrderID, OCOSibling);

									// Modify the sibling instead 
									s_SCNewOrder ModifySibling;
									ModifySibling.InternalOrderID = OCOSibling.InternalOrderID;
									ModifySibling.Price1 = sc.ChartTradingOrderPrice;
									sc.ModifyOrder(ModifySibling);
									return;
								}
							}
							else
							{
								s_SCNewOrder ModifyTarget;
								ModifyTarget.InternalOrderID = OrderIDToModify;
								ModifyTarget.Price1 = sc.ChartTradingOrderPrice;
								sc.ModifyOrder(ModifyTarget);
								return;
							}
						}
					}
				}
				else // user completely disabled the setting 
				{
					// do not commit the modification if the modification
					// price is same as the order price 
					if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
					{
						s_SCNewOrder ModifyTarget;
						ModifyTarget.InternalOrderID = Order.InternalOrderID;
						ModifyTarget.Price1 = sc.ChartTradingOrderPrice;
						sc.ModifyOrder(ModifyTarget);
						return;
					}
				}
			}
		}

		// When in a long position and cursor is below the last
		// traded price, Move closest stop order
		if (PositionIncludingWorkingOrders >= 1 && sc.ChartTradingOrderPrice <= sc.Bid)
		{
			if (sc.GetNearestStopOrder(Order))
			{
				// set the order price here in case no other items get triggered 
				ClosestOrderPrice = sc.RoundToTickSize(Order.Price1, sc.TickSize);
				if(Input_PrioritizeActiveOrders.GetYesNo())
				{
					int OrderIDToModify = GetNearestActiveOrder(sc, 2, LastPrice, Order, PositionIncludingWorkingOrders, 
							ClosestOrderPrice);
					if(OrderIDToModify != 0)
					{
						if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
						{
							// NO NEED FOR TRADINGVIEW PRECAUTION HERE BECAUSE
							// THE ORDER IS ACTIVE AND WE ARE IN A POSITION
							
							s_SCNewOrder ModifyStop;
							ModifyStop.InternalOrderID = OrderIDToModify;
							ModifyStop.Price1 = sc.ChartTradingOrderPrice;
							sc.ModifyOrder(ModifyStop);
							return;
						}
					}
					else // default behaviour (order not active order status)
					{
						// WE get here if no active target or stop orders were
						// found, now we need to handle their modification here
						// with the same checks we used in the function. 
						uint32_t OrderIDToModify = 0;
						double LowestDifference = 0;
						int Index = 0;
						while(sc.GetOrderByIndex (Index, Order) != SCTRADING_ORDER_ERROR)
						{
							Index++; // Increment the index for the next call to sc.GetOrderByIndex
							if((IsWorkingOrderStatus(Order.OrderStatusCode) 
								&& IsStopOrder(Order.OrderTypeAsInt) 
								&& Order.IsAttachedOrder())
								|| 
								(IsWorkingOrderStatus(Order.OrderStatusCode)
								&& IsStopOrder(Order.OrderTypeAsInt)
								&& Order.OCOSiblingInternalOrderID != 0	))
							{
								double difference = std::abs(Order.Price1 - LastPrice);
								
								// first time init
								if(LowestDifference == 0 )
								{
									LowestDifference = difference;
									ClosestOrderPrice = sc.RoundToTickSize(Order.Price1, sc.TickSize);
									OrderIDToModify = Order.InternalOrderID;
								}
								else if(LowestDifference > difference)
								{
									// return the order with the lowest price
									// difference from last price 
									LowestDifference = difference; 
									ClosestOrderPrice = sc.RoundToTickSize(Order.Price1, sc.TickSize);
									OrderIDToModify = Order.InternalOrderID;
								}
							}
							else
							{
								// order is not a stop order or is not an attached order 
								continue;
							}
						}
						if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
						{
							// Check if the order has a parent order 
							s_SCTradeOrder Order; 
							sc.GetOrderByOrderID(OrderIDToModify, Order);
							if(Order.ParentInternalOrderID != 0)
							{
								// Get the parent order to check it's price 
								s_SCTradeOrder ParentOrder;
								sc.GetOrderByOrderID(Order.ParentInternalOrderID, ParentOrder);

								// MOVE Sell Stop for a Buy Position 
								// Only do this stop modification if the
								// requested price is below the parent order
								// price (Like TradingView)
								if(sc.ChartTradingOrderPrice < ParentOrder.Price1 )
								{
									s_SCNewOrder ModifyStop;
									ModifyStop.InternalOrderID = OrderIDToModify;
									ModifyStop.Price1 = sc.ChartTradingOrderPrice;
									sc.ModifyOrder(ModifyStop);
									return;
								}
								else if(sc.ChartTradingOrderPrice > ParentOrder.Price1)
								{
									// Get the other attached order in the pair instead to permit it's movement
									s_SCTradeOrder OCOSibling;
									sc.GetOrderByOrderID(Order.OCOSiblingInternalOrderID, OCOSibling);

									// Modify the sibling instead 
									s_SCNewOrder ModifySibling;
									ModifySibling.InternalOrderID = OCOSibling.InternalOrderID;
									ModifySibling.Price1 = sc.ChartTradingOrderPrice;
									sc.ModifyOrder(ModifySibling);
									return;
								}

							}
							// this order has no parent order therefore the previous check is not necessary 
							else 
							{
								s_SCNewOrder ModifyStop;
								ModifyStop.InternalOrderID = OrderIDToModify;
								ModifyStop.Price1 = sc.ChartTradingOrderPrice;
								sc.ModifyOrder(ModifyStop);
								return;
							}
						}
					}
				}
				else // user completely disabled the setting 
				{
					if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
					{
						s_SCNewOrder ModifyStop;
						ModifyStop.InternalOrderID = Order.InternalOrderID;
						ModifyStop.Price1 = sc.ChartTradingOrderPrice;
						sc.ModifyOrder(ModifyStop);
						return;
					}
				}
			}
		}

		// When in a short position and cursor is below the last
		// traded price, Move closest target order
		if (PositionIncludingWorkingOrders <= -1 && sc.ChartTradingOrderPrice <= sc.Ask)
		{
			if (sc.GetNearestTargetOrder(Order))
			{
				// set the order price here in case no other items get triggered 
				ClosestOrderPrice = sc.RoundToTickSize(Order.Price1, sc.TickSize);

				if(Input_PrioritizeActiveOrders.GetYesNo())
				{
					int OrderIDToModify = GetNearestActiveOrder(sc, 1, LastPrice, Order, PositionIncludingWorkingOrders, 
					ClosestOrderPrice);
					if(OrderIDToModify != 0)
					{
						if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
						{
							// NO NEED FOR TRADINGVIEW PRECAUTION HERE BECAUSE
							// THE ORDER IS ACTIVE AND WE ARE IN A POSITION
							s_SCNewOrder ModifyTarget;
							ModifyTarget.InternalOrderID = OrderIDToModify;
							ModifyTarget.Price1 = sc.ChartTradingOrderPrice;
							sc.ModifyOrder(ModifyTarget);
							return;
						}
					}
					else // default behaviour (order not active order status)
					{
						// WE get here if no active target or stop orders were
						// found, now we need to handle their modification here
						// with the same checks we used in the function. 
						//
						uint32_t OrderIDToModify = 0;
						double LowestDifference = 0;
						int Index = 0;
						while(sc.GetOrderByIndex (Index, Order) != SCTRADING_ORDER_ERROR)
						{
							Index++; // Increment the index for the next call to sc.GetOrderByIndex
						  
							if(IsWorkingOrderStatus(Order.OrderStatusCode) 
								&& IsLimitOrderV2(Order.OrderTypeAsInt) 
								&& Order.IsAttachedOrder() )
							{
								double difference = std::abs(Order.Price1 - LastPrice);
								
								// first time init
								if(LowestDifference == 0 )
								{
									LowestDifference = difference;
									ClosestOrderPrice = sc.RoundToTickSize(Order.Price1, sc.TickSize);
									OrderIDToModify = Order.InternalOrderID;
								}
								else if(LowestDifference > difference)
								{
									// return the order with the lowest price
									// difference from last price 
									LowestDifference = difference; 
									ClosestOrderPrice = sc.RoundToTickSize(Order.Price1, sc.TickSize);
									OrderIDToModify = Order.InternalOrderID;
								}
							}
							else
							{
								// order is not a limit order or is not an attached order 
								continue;
							}

						}
						if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
						{
							// Check if the order has a parent order 
							s_SCTradeOrder Order; 
							sc.GetOrderByOrderID(OrderIDToModify, Order);
							if(Order.ParentInternalOrderID != 0)
							{
								// Get the parent order to check it's price 
								s_SCTradeOrder ParentOrder;
								sc.GetOrderByOrderID(Order.ParentInternalOrderID, ParentOrder);

								// MOVE Buy Limit for a Short Position 
								// Only do this target modification if the
								// requested price is below the parent order
								// price (Like TradingView)
								if(sc.ChartTradingOrderPrice < ParentOrder.Price1)
								{
									s_SCNewOrder ModifyStop;
									ModifyStop.InternalOrderID = OrderIDToModify;
									ModifyStop.Price1 = sc.ChartTradingOrderPrice;
									sc.ModifyOrder(ModifyStop);
									return;
								}
								else if(sc.ChartTradingOrderPrice > ParentOrder.Price1)
								{
									// Get the other attached order in the pair instead to permit it's movement
									s_SCTradeOrder OCOSibling;
									sc.GetOrderByOrderID(Order.OCOSiblingInternalOrderID, OCOSibling);

									// Modify the sibling instead 
									s_SCNewOrder ModifySibling;
									ModifySibling.InternalOrderID = OCOSibling.InternalOrderID;
									ModifySibling.Price1 = sc.ChartTradingOrderPrice;
									sc.ModifyOrder(ModifySibling);
									return;
								}
							}
							else
							{
								s_SCNewOrder ModifyTarget;
								ModifyTarget.InternalOrderID = OrderIDToModify;
								ModifyTarget.Price1 = sc.ChartTradingOrderPrice;
								sc.ModifyOrder(ModifyTarget);
								return;
							}
						}
					}

				}
				else // user completely disabled the setting 
				{
					if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
					{
						s_SCNewOrder ModifyTarget;
						ModifyTarget.InternalOrderID = Order.InternalOrderID;
						ModifyTarget.Price1 = sc.ChartTradingOrderPrice;
						sc.ModifyOrder(ModifyTarget);
						return;
					}
				}
			}
		}

		// When in a short position and cursor is above the last
		// traded price, Move closest stop order
		if (PositionIncludingWorkingOrders <= -1 && sc.ChartTradingOrderPrice >= sc.Ask)
		{
			if (sc.GetNearestStopOrder(Order))
			{
				ClosestOrderPrice = sc.RoundToTickSize(Order.Price1, sc.TickSize);

				if(Input_PrioritizeActiveOrders.GetYesNo())
				{
					int OrderIDToModify = GetNearestActiveOrder(sc, 2, LastPrice, Order, PositionIncludingWorkingOrders, 
					ClosestOrderPrice);
					if(OrderIDToModify != 0)
					{
						if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
						{
							// NO NEED FOR TRADINGVIEW PRECAUTION HERE BECAUSE
							// THE ORDER IS ACTIVE AND WE ARE IN A POSITION
							s_SCNewOrder ModifyStop;
							ModifyStop.InternalOrderID = OrderIDToModify;
							ModifyStop.Price1 = sc.ChartTradingOrderPrice;
							sc.ModifyOrder(ModifyStop);
							return;
						}
					}
					else // default behaviour (order not active order status)
					{
						// WE get here if no active target or stop orders were
						// found, now we need to handle their modification here
						// with the same checks we used in the function. 
						uint32_t OrderIDToModify = 0;
						double LowestDifference = 0;
						int Index = 0;
						while(sc.GetOrderByIndex (Index, Order) != SCTRADING_ORDER_ERROR)
						{
							Index++; // Increment the index for the next call to sc.GetOrderByIndex
							if(IsWorkingOrderStatus(Order.OrderStatusCode) 
								&& IsStopOrder(Order.OrderTypeAsInt) 
								&& Order.IsAttachedOrder())
							{
								double difference = std::abs(Order.Price1 - LastPrice);
								
								// first time init
								if(LowestDifference == 0 )
								{
									LowestDifference = difference;
									ClosestOrderPrice = sc.RoundToTickSize(Order.Price1, sc.TickSize);
									OrderIDToModify = Order.InternalOrderID;
								}
								else if(LowestDifference > difference)
								{
									// return the order with the lowest price
									// difference from last price 
									LowestDifference = difference; 
									ClosestOrderPrice = sc.RoundToTickSize(Order.Price1, sc.TickSize);
									OrderIDToModify = Order.InternalOrderID;
								}
							}
							else
							{
								// order is not a stop order or is not an attached order 
								continue;
							}
						}
						if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
						{
							// Check if the order has a parent order 
							s_SCTradeOrder Order; 
							sc.GetOrderByOrderID(OrderIDToModify, Order);
							if(Order.ParentInternalOrderID != 0)
							{
								// Get the parent order to check it's price 
								s_SCTradeOrder ParentOrder;
								sc.GetOrderByOrderID(Order.ParentInternalOrderID, ParentOrder);

								// MOVE Buy Stop for a Short Position 
								// Only do this stop modification if the
								// requested price is above the parent order
								// price (Like TradingView)
								if(sc.ChartTradingOrderPrice > ParentOrder.Price1)
								{
									s_SCNewOrder ModifyStop;
									ModifyStop.InternalOrderID = OrderIDToModify;
									ModifyStop.Price1 = sc.ChartTradingOrderPrice;
									sc.ModifyOrder(ModifyStop);
									return;
								}
								else if(sc.ChartTradingOrderPrice < ParentOrder.Price1)
								{
									// Get the other attached order in the pair instead to permit it's movement
									s_SCTradeOrder OCOSibling;
									sc.GetOrderByOrderID(Order.OCOSiblingInternalOrderID, OCOSibling);

									// Modify the sibling instead 
									s_SCNewOrder ModifySibling;
									ModifySibling.InternalOrderID = OCOSibling.InternalOrderID;
									ModifySibling.Price1 = sc.ChartTradingOrderPrice;
									sc.ModifyOrder(ModifySibling);
									return;
								}

							}
							// this order has no parent order therefore the previous check is not necessary 
							else 
							{
								s_SCNewOrder ModifyStop;
								ModifyStop.InternalOrderID = OrderIDToModify;
								ModifyStop.Price1 = sc.ChartTradingOrderPrice;
								sc.ModifyOrder(ModifyStop);
								return;
							}
						}
					}
				}
				else // user completely disabled the setting 
				{
					if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
					{
						s_SCNewOrder ModifyStop;
						ModifyStop.InternalOrderID = Order.InternalOrderID;
						ModifyStop.Price1 = sc.ChartTradingOrderPrice;
						sc.ModifyOrder(ModifyStop);
						return;
					}
				}
			}
		}
	}

	// ACS Button Press (move closest unattached Buy order)
	if (sc.MenuEventID != 0 && sc.MenuEventID == Input_MoveClosestUnattachedBuyOrderACSButtonNumber.GetInt() )
	{
		// For this button press we will reset the button back to off state
		// since pressing the button executes a function
		const int ButtonState = (sc.PointerEventType == SC_ACS_BUTTON_ON) ? 1 : 0;
		if (ButtonState == 1)
		{
			sc.SetCustomStudyControlBarButtonEnable(Input_MoveClosestUnattachedBuyOrderACSButtonNumber.GetInt(), 0);
		}

		double LastPrice = sc.GetLastPriceForTrading();
		if(Input_EnableMoveClosestUnattachedOrder.GetBoolean())
		{
			// User Enabled Move Closest unattached order
			// msg.Format("User enabled new feature!",1);
			// sc.AddMessageToLog(msg,1);

			// This is an example of iterating the order list in Sierra Chart for orders
			// matching the Symbol and Trade Account of the chart, and finding the orders
			// that have a Status of Open and are not Attached Orders.

			int ClosestOrderID = 0;
			double ClosestOrderPrice = 0;
			double PriceDifference = 0;

			// Handle the trade and current quote symbol (necessary for traders
			// that trade the micros using the chart for the minis
			SCString ChartSymbol;
			if(sc.TradeAndCurrentQuoteSymbol != "")
			{
				ChartSymbol = sc.TradeAndCurrentQuoteSymbol;
			}
			else
			{
				ChartSymbol = sc.Symbol;
			}

			// msg.Format("1we get here chart symbol: %s" ,ChartSymbol.GetChars() );
			// sc.AddMessageToLog(msg,1);

			int Index = 0;
			s_SCTradeOrder OrderDetails;
			while( sc.GetOrderByIndex (Index, OrderDetails) != SCTRADING_ORDER_ERROR)
			{
				// msg.Format("Order symbol: %s Order ID: %d Order Status Code: %d" , OrderDetails.Symbol.GetChars(), OrderDetails.InternalOrderID,
				// 	OrderDetails.OrderStatusCode);
				// sc.AddMessageToLog(msg,1);
				//
				Index++; // Increment the index for the next call to sc.GetOrderByIndex

				// Only account for BUY open unattached orders, and on the current chart, and must be a limit 
				if (OrderDetails.BuySell == 1 && OrderDetails.Symbol == ChartSymbol // same symbol (trade only)
					&& (OrderDetails.OrderStatusCode ==  SCT_OSC_OPEN  || OrderDetails.OrderStatusCode == SCT_OSC_HELD) // open or held
					&& OrderDetails.ParentInternalOrderID == 0 // not an attached order 
					&& OrderDetails.OCOSiblingInternalOrderID == 0) // not an OCO Order
					// && IsLimitOrderV2(Order.OrderTypeAsInt)) // is a limit order type 
				{

					// debug 
					// msg.Format("2we get here chart symbol: %s" ,ChartSymbol.GetChars() );
					// sc.AddMessageToLog(msg,1);

					// Logic for first time init
					if(ClosestOrderID == 0)
					{
						// save the order id and price 
						ClosestOrderID = OrderDetails.InternalOrderID;
						ClosestOrderPrice = OrderDetails.Price1;

						// Save the price difference between the last price and this order
						PriceDifference = std::abs(LastPrice - OrderDetails.Price1);

						// Continue to the next order for price comparison
						continue;
					}
					// Logic For every other active order (compare against) 
					else
					{
						// Save the price difference between the last price and this order
						double NewOrderPriceDifference = std::abs(LastPrice - OrderDetails.Price1);

						// If this order is closer than the previous order we had in memory
						if(NewOrderPriceDifference < PriceDifference)
						{
							// Save the new closest Order ID and price 
							ClosestOrderID = OrderDetails.InternalOrderID;
							ClosestOrderPrice = OrderDetails.Price1;
						}
					}
				}
				else
				{
					// This is not the order we are looking for therefore continue
					continue;
				}
			}

			// debugging 
			// if(ClosestOrderID == 0)
			// {
			// 	// sc.AddMessageToLog("No Active Unattached Orders!",1);

			// }
			// else
			if(ClosestOrderID != 0)
			{
				// msg.Format("Closest Order is ID: %d", ClosestOrderID);
				// sc.AddMessageToLog(msg,1);

				// Get the pointer price and compare it to the price of our order
				// if they are not equal, carry on with the modification 
				if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
				{
					// Handle how we want to handle buy vs sell orders 
					// If the user cursor is above the last traded price, 
					// It is going to move the closest order including buy liits which
					// would result in an instant fill. 
					//
					// We could have 2 separate buttons: 
					// 1. One for Buy Order types 
					// 2. For Sell Order types 
					//
					// Reason is because the user might have a sell order they want to move closer
					// but their unattached buy order is actually the one that is closer 
					//
					// If we only had one button it would move the buy order up and fill immediately 
					// 	
					// Modify this order to the chart price 
					s_SCNewOrder ClosestOrder;
					ClosestOrder.InternalOrderID = ClosestOrderID;
					ClosestOrder.Price1 = sc.ChartTradingOrderPrice;
					sc.ModifyOrder(ClosestOrder);
					return;
				}
			}
		}
	} // End Move Unattached Buy Limit Order ACS Button Scope
	  
	if (sc.MenuEventID != 0 && sc.MenuEventID == Input_MoveClosestUnattachedSellOrderACSButtonNumber.GetInt() )
	{

		// For this button press we will reset the button back to off state
		// since pressing the button executes a function
		const int ButtonState = (sc.PointerEventType == SC_ACS_BUTTON_ON) ? 1 : 0;
		if (ButtonState == 1)
		{
			sc.SetCustomStudyControlBarButtonEnable(Input_MoveClosestUnattachedSellOrderACSButtonNumber.GetInt(), 0);
		}

		double LastPrice = sc.GetLastPriceForTrading();
		if(Input_EnableMoveClosestUnattachedOrder.GetBoolean())
		{
			// User Enabled Move Closest unattached order
			// msg.Format("User enabled new feature!",1);
			// sc.AddMessageToLog(msg,1);

			// This is an example of iterating the order list in Sierra Chart for orders
			// matching the Symbol and Trade Account of the chart, and finding the orders
			// that have a Status of Open and are not Attached Orders.

			int ClosestOrderID = 0;
			double ClosestOrderPrice = 0;
			double PriceDifference = 0;

			// Handle the trade and current quote symbol (necessary for traders
			// that trade the micros using the chart for the minis
			SCString ChartSymbol;
			if(sc.TradeAndCurrentQuoteSymbol != "")
			{
				ChartSymbol = sc.TradeAndCurrentQuoteSymbol;
			}
			else
			{
				ChartSymbol = sc.Symbol;
			}
			int Index = 0;
			s_SCTradeOrder OrderDetails;
			while( sc.GetOrderByIndex (Index, OrderDetails) != SCTRADING_ORDER_ERROR)
			{
				Index++; // Increment the index for the next call to sc.GetOrderByIndex

				// Only account for BUY open unattached orders, and on the current chart, and must be a limit 
				if (OrderDetails.BuySell == 2 && OrderDetails.Symbol == ChartSymbol // same symbol (trade only)
					&& (OrderDetails.OrderStatusCode ==  SCT_OSC_OPEN  || OrderDetails.OrderStatusCode == SCT_OSC_HELD) // open or held
					&& OrderDetails.ParentInternalOrderID == 0 // not an attached order 
					&& OrderDetails.OCOSiblingInternalOrderID == 0 )// not an OCO Order 
					// && IsLimitOrderV2(Order.OrderTypeAsInt)) // is a limit order type 
				{
					// debug 
					// msg.Format("BuySellEnum %d Order ID %d", OrderDetails.BuySell, OrderDetails.InternalOrderID );
					// sc.AddMessageToLog(msg,1);

					// Logic for first time init
					if(ClosestOrderID == 0)
					{
						// save the order id and price 
						ClosestOrderID = OrderDetails.InternalOrderID;
						ClosestOrderPrice = OrderDetails.Price1;

						// Save the price difference between the last price and this order
						PriceDifference = std::abs(LastPrice - OrderDetails.Price1);

						// Continue to the next order for price comparison
						continue;
					}
					// Logic For every other active order (compare against) 
					else
					{
						// Save the price difference between the last price and this order
						double NewOrderPriceDifference = std::abs(LastPrice - OrderDetails.Price1);

						// If this order is closer than the previous order we had in memory
						if(NewOrderPriceDifference < PriceDifference)
						{
							// Save the new closest Order ID and price 
							ClosestOrderID = OrderDetails.InternalOrderID;
							ClosestOrderPrice = OrderDetails.Price1;
						}
					}
				}
				else
				{
					// This is not the order we are looking for therefore continue
					continue;
				}
			}

			// debugging 
			// if(ClosestOrderID == 0)
			// {
			// 	// sc.AddMessageToLog("No Active Unattached Orders!",1);

			// }
			// else
			if(ClosestOrderID != 0)
			{
				// msg.Format("Closest Order is ID: %d", ClosestOrderID);
				// sc.AddMessageToLog(msg,1);

				// Get the pointer price and compare it to the price of our order
				// if they are not equal, carry on with the modification 
				if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
				{
					// Handle how we want to handle buy vs sell orders 
					// If the user cursor is above the last traded price, 
					// It is going to move the closest order including buy liits which
					// would result in an instant fill. 
					//
					// We could have 2 separate buttons: 
					// 1. One for Buy Order types 
					// 2. For Sell Order types 
					//
					// Reason is because the user might have a sell order they want to move closer
					// but their unattached buy order is actually the one that is closer 
					//
					// If we only had one button it would move the buy order up and fill immediately 
					// 	
					// Modify this order to the chart price 
					s_SCNewOrder ClosestOrder;
					ClosestOrder.InternalOrderID = ClosestOrderID;
					ClosestOrder.Price1 = sc.ChartTradingOrderPrice;
					sc.ModifyOrder(ClosestOrder);
					return;
				}
			}
		}
	} // End Move Unattached Sell Limit Order ACS Button Scope
	 
} 
SCSFExport scsf_CreateOrdersForRiskRewardTool(SCStudyInterfaceRef sc)
{
	// Ensure that a valid risk reward tool is selected before adding to the
	// chart right click menu 
	//
	// We can add 4 items 
	//
	// 1. Create Buy Limit Bracket
	// 2. Create Buy Stop Bracket
	// 3. Create Sell Limit Bracket
	// 4. Create Sell Stop Bracket
	//
	// Orders should always be GTC 
	// for US Equities The Stop Loss should be a stop limit order 
	// Idea: For the stop limit price we can obtain the spread between the bid offer and use a multiple of that amount 
	//
	// This can be for both the entry and exit prices (if entry is a stop limit entry)
	//
	// DONE: DO NOT PERMIT THEM TO SUBMIT A BUY LIMIT ORDER IF THEIR ENTRY PRICE IS ABOVE THE LAST PRICE 
	// 
	// Need to check the last price for this 
	//
	// TODO: Add an input that allows the user to use a client side hidden
	// order for their entry and profit target until the market reaches a
	// certain distance from the order price 
	//
	//
	// TODO: Add an option to submit order using ACS button along with the right click menu. (Faster) 
	//
	// TODO: Add error messages as a text display if the user makes an incorrect function. 
	//
	// TODO: Add a chase function where once the limit if touched price is
	// touched, it auto moves the limit price to the bid or ask to attempt a
	// faster fille
	//
	// The text display drawing will be called and it's line number will be
	// remembered and a timer will be initialized for how long it remains on
	// the screen. 
	//
	// Otherwise check if it is possible to create some kind of dialog prompt with sierra chart, otherwise message log.
	//
	// Study inputs 
	int InputIndex = 0;

	SCInputRef i_BidOfferSpreadMultiplierForStopLimitOffset = sc.Input[InputIndex++];
	SCInputRef i_MaximumSlippageForStopLimitEntry = sc.Input[InputIndex++];
	SCInputRef i_UseLimitIfTouchedOrders = sc.Input[InputIndex++];

	int PersistentVariableIndex = 0;
	int& BuyLimitBracketMenuID = sc.GetPersistentInt(PersistentVariableIndex++);
	int& BuyStopLimitBracketMenuID = sc.GetPersistentInt(PersistentVariableIndex++);
	int& SellLimitBracketMenuID = sc.GetPersistentInt(PersistentVariableIndex++);
	int& SellStopLimitBracketMenuID = sc.GetPersistentInt(PersistentVariableIndex++);

	if(sc.SetDefaults)
	{
		std::string StudyGraphName = "Trading - Create Bracket Orders For Risk Reward Tool | v" + std::to_string(ver) + " |";
		sc.GraphName = StudyGraphName.c_str();
		sc.ValueFormat = sc.BaseGraphValueFormat;
		sc.GraphRegion = 0;
		sc.AutoLoop = 0;
		sc.UpdateAlways = 1; // More efficient if running a fast chart update interval
							 //
		sc.MaximumPositionAllowed = 30;
		sc.AllowOnlyOneTradePerBar = false;
		sc.AllowEntryWithWorkingOrders = true;
		sc.AllowMultipleEntriesInSameDirection = true;
		sc.AllowOppositeEntryWithOpposingPositionOrOrders = true;

		int StudyDisplayOrder = 1;

		i_BidOfferSpreadMultiplierForStopLimitOffset.Name = "Bid/Ask Spread Multiple For Stop Loss Limit Offset";
		i_BidOfferSpreadMultiplierForStopLimitOffset.SetFloat(3.0);
		i_BidOfferSpreadMultiplierForStopLimitOffset.SetFloatLimits(1.0,10.0);
		i_BidOfferSpreadMultiplierForStopLimitOffset.DisplayOrder = StudyDisplayOrder++;

		i_MaximumSlippageForStopLimitEntry.Name = "Stop Entry Limit Offset in Ticks";
		i_MaximumSlippageForStopLimitEntry.SetInt(3);
		i_MaximumSlippageForStopLimitEntry.SetIntLimits(0,1000);
		i_MaximumSlippageForStopLimitEntry.DisplayOrder = StudyDisplayOrder++;

		i_UseLimitIfTouchedOrders.Name = "Use Triggered Client Side Orders For Entry and Target";
		i_UseLimitIfTouchedOrders.SetYesNo(1);
		i_UseLimitIfTouchedOrders.DisplayOrder = StudyDisplayOrder++;

		// Comment out separators during development 
		//
		// sc.AddACSChartShortcutMenuSeparator(sc.ChartNumber);
		BuyLimitBracketMenuID = sc.AddACSChartShortcutMenuItem(sc.ChartNumber, "Risk Reward: Buy Limit Bracket");
		BuyStopLimitBracketMenuID = sc.AddACSChartShortcutMenuItem(sc.ChartNumber, "Risk Reward: Buy Stop Limit Bracket");
		SellLimitBracketMenuID = sc.AddACSChartShortcutMenuItem(sc.ChartNumber, "Risk Reward: Sell Limit Bracket");
		SellStopLimitBracketMenuID = sc.AddACSChartShortcutMenuItem(sc.ChartNumber, "Risk Reward: Sell Stop Limit Bracket");
		// sc.AddACSChartShortcutMenuSeparator(sc.ChartNumber);
		// 
	}

	if(sc.LastCallToFunction)
	{
		// remove custom menu items 
		sc.RemoveACSChartShortcutMenuItem(sc.ChartNumber, BuyLimitBracketMenuID); 
		sc.RemoveACSChartShortcutMenuItem(sc.ChartNumber, BuyStopLimitBracketMenuID); 
		sc.RemoveACSChartShortcutMenuItem(sc.ChartNumber, SellLimitBracketMenuID); 
		sc.RemoveACSChartShortcutMenuItem(sc.ChartNumber, SellStopLimitBracketMenuID) ; 

		// reset persistents
		BuyLimitBracketMenuID = 0;
		BuyStopLimitBracketMenuID = 0;
		SellLimitBracketMenuID = 0;
		SellStopLimitBracketMenuID = 0;

		return;
	}
	// logging object 
	SCString msg;

	if (sc.MenuEventID != 0)	
	{
		if (!sc.ChartTradeModeEnabled)
		{
			// sc.AddMessageToLog("Chart Trade Mode is not active. No action performed.", 1);
			return;
		}
	}
	
	// For safety we must never do any order management while historical data is being downloaded.
	if (sc.ChartIsDownloadingHistoricalData(sc.ChartNumber)
	 || sc.IsChartDataLoadingCompleteForAllCharts() == 0)
	{
		//sc.AddMessageToLog("Chart is downloading historical data, no order management allowed.", 1);
		return;
	}
	
	// This line allows the study to be used for live trading
	sc.SendOrdersToTradeService = !sc.GlobalTradeSimulationIsOn;

	// Get Currently Selected Drawing
	// Fill in with currently selected drawing 
	s_UseTool Tool;
	if(sc.GetSelectedUserDrawnDrawingFromChart(sc.ChartNumber, Tool))
	{
		// Determine what kind of drawing it is 
		// msg.Format("Drawing Type Enum: %d", Tool.DrawingType);
		// sc.AddMessageToLog(msg,1);
		//
		if(Tool.DrawingType == 41)
		{
			// sc.AddMessageToLog("Valid RR Tool is selected!",1);
			//
			// Trigger when our menu items are selected, only while the drawing is selected 
			if(sc.MenuEventID != 0 && sc.MenuEventID == BuyLimitBracketMenuID)
			{
				// sc.AddMessageToLog("Buy Limit Bracket Menu Item!",1);
				// Get the 3 points of the risk reward tool 
				//
				// msg.Format("First Value: %f Second Value: %f Third Value: %f", Tool.BeginValue, Tool.EndValue, Tool.ThirdValue);
				// sc.AddMessageToLog(msg,1);

				double StopPrice = Tool.BeginValue;
				double EntryPrice = Tool.EndValue;
				double TargetPrice = Tool.ThirdValue;
				double Last = sc.GetLastPriceForTrading();

				// Entry Price must be below last for a Buy Limit
				// Stop Price must be below Entry Price for Buy Limit
				if(EntryPrice >= Last
				|| StopPrice >= EntryPrice)
				{
					sc.AddMessageToLog("Not Permitted: Buy Limit Price Must Be Below Last Price and Stop Price Must be Below Entry Price!",1);
					return;
				}
				else
				{
					// Create our orders 
					s_SCNewOrder BuyLimit;

					// Basic Order config 
					BuyLimit.OrderQuantity = sc.TradeWindowOrderQuantity;
					BuyLimit.TimeInForce = SCT_TIF_GTC;

					// Parent Order Price
					BuyLimit.Price1 = EntryPrice;

					if(i_UseLimitIfTouchedOrders.GetYesNo())
					{
						// Use triggered limit order
						BuyLimit.OrderType = SCT_ORDERTYPE_LIMIT_IF_TOUCHED_CLIENT_SIDE;

						// Price 2 is the trigger price 
						// BuyLimit.Price2 = EntryPrice;

						// set the target type
						BuyLimit.AttachedOrderTarget1Type = SCT_ORDERTYPE_LIMIT_IF_TOUCHED_CLIENT_SIDE;

					}
					else
					{
						// Standard limit entry order
						BuyLimit.OrderType = SCT_ORDERTYPE_LIMIT;
					}

					// Target Price 
					BuyLimit.Target1Price = TargetPrice;

					// Stop Type
					BuyLimit.AttachedOrderStop1Type = SCT_ORDERTYPE_STOP_LIMIT;

					// Stop Price 
					BuyLimit.Stop1Price = StopPrice;

					// Stop Limit Offset
					double Spread = sc.Ask - sc.Bid;
					double SpreadWithMultiplier = Spread * i_BidOfferSpreadMultiplierForStopLimitOffset.GetFloat();

					// msg.Format("Spread: %f SpreadWithMult: %f", Spread, SpreadWithMultiplier);
					// sc.AddMessageToLog(msg,1);

					BuyLimit.StopLimitOrderLimitOffset = SpreadWithMultiplier;

					// Send Order
					int Result = static_cast<int>(sc.BuyEntry(BuyLimit));
				}
			}
			else if(sc.MenuEventID != 0 && sc.MenuEventID == BuyStopLimitBracketMenuID)
			{
				// sc.AddMessageToLog("Buy Stop Limit Bracket Menu Item!",1);
				//
				double StopPrice = Tool.BeginValue;
				double EntryPrice = Tool.EndValue;
				double TargetPrice = Tool.ThirdValue;
				double Last = sc.GetLastPriceForTrading();

				// Entry Price must be above last for a Buy Stop Entry
				if(EntryPrice <= Last
				|| StopPrice >= EntryPrice)
				{
					sc.AddMessageToLog("Not Permitted: Buy Stop Price Must Be Above Last and Stop Price must be below Entry Price.",1);
					return;
				}
				else
				{
					// Create our order
					s_SCNewOrder BuyStop;

					// Basic Order config 
					BuyStop.OrderQuantity = sc.TradeWindowOrderQuantity;
					BuyStop.OrderType = SCT_ORDERTYPE_STOP_LIMIT;
					BuyStop.TimeInForce = SCT_TIF_GTC;

					// Order Prices 
					BuyStop.Price1 = EntryPrice;
					BuyStop.Price2 = EntryPrice + (i_MaximumSlippageForStopLimitEntry.GetInt() * sc.TickSize); // Stop Limit Offset Price 
																											   //
					if(i_UseLimitIfTouchedOrders.GetYesNo())
					{
						// Set target order type 
						BuyStop.AttachedOrderTarget1Type = SCT_ORDERTYPE_LIMIT_IF_TOUCHED_CLIENT_SIDE;
					}
					// Target Order Price 
					BuyStop.Target1Price = TargetPrice;

					// Stop Order Config 
					BuyStop.Stop1Price = StopPrice;
					BuyStop.AttachedOrderStop1Type = SCT_ORDERTYPE_STOP_LIMIT;

					double Spread = sc.Ask - sc.Bid;
					double SpreadWithMultiplier = Spread * i_BidOfferSpreadMultiplierForStopLimitOffset.GetFloat();

					// msg.Format("Spread: %f SpreadWithMult: %f", Spread, SpreadWithMultiplier);
					// sc.AddMessageToLog(msg,1);

					BuyStop.StopLimitOrderLimitOffset = SpreadWithMultiplier;

					// Send Order 
					int Result = static_cast<int>(sc.BuyEntry(BuyStop));
				}
			}
			else if(sc.MenuEventID != 0 && sc.MenuEventID == SellLimitBracketMenuID)
			{
				// sc.AddMessageToLog("Sell Limit Bracket Menu Item!",1);
				//
				double StopPrice = Tool.BeginValue;
				double EntryPrice = Tool.EndValue;
				double TargetPrice = Tool.ThirdValue;
				double Last = sc.GetLastPriceForTrading();

				// Entry Price must be above last for a Sell Limit Entry
				if(EntryPrice <= Last
				|| StopPrice <= EntryPrice)
				{
					sc.AddMessageToLog("Not Permitted: Sell Limit Price Must Be Above Last and Stop Price must be above Entry Price.",1);
					return;
				}
				else
				{
					// Create our order
					s_SCNewOrder SellLimit;

					// Basic Order config 
					SellLimit.OrderQuantity = sc.TradeWindowOrderQuantity;
					SellLimit.TimeInForce = SCT_TIF_GTC;

					// Parent Order Price
					SellLimit.Price1 = EntryPrice;

					if(i_UseLimitIfTouchedOrders.GetYesNo())
					{
						// Use Triggered limit order
						SellLimit.OrderType = SCT_ORDERTYPE_LIMIT_IF_TOUCHED_CLIENT_SIDE;

						// Price 2 is the trigger price 
						// SellLimit.Price2 = EntryPrice;

						// Set the Target Type
						SellLimit.AttachedOrderTarget1Type = SCT_ORDERTYPE_LIMIT_IF_TOUCHED_CLIENT_SIDE;
					}
					else
					{
						// Standard limit entry order
						SellLimit.OrderType = SCT_ORDERTYPE_LIMIT;
					}

					// Target price 
					SellLimit.Target1Price = TargetPrice;

					// Stop Order Config 
					SellLimit.Stop1Price = StopPrice;
					SellLimit.AttachedOrderStop1Type = SCT_ORDERTYPE_STOP_LIMIT;

					double Spread = sc.Ask - sc.Bid;
					double SpreadWithMultiplier = Spread * i_BidOfferSpreadMultiplierForStopLimitOffset.GetFloat();

					// msg.Format("Spread: %f SpreadWithMult: %f", Spread, SpreadWithMultiplier);
					// sc.AddMessageToLog(msg,1);

					SellLimit.StopLimitOrderLimitOffset = SpreadWithMultiplier;
					int Result = static_cast<int>(sc.SellEntry(SellLimit));
				}
			}
			else if(sc.MenuEventID != 0 && sc.MenuEventID == SellStopLimitBracketMenuID)
			{
				// sc.AddMessageToLog("Sell Stop Limit Bracket Menu Item!",1);
				//
				double StopPrice = Tool.BeginValue;
				double EntryPrice = Tool.EndValue;
				double TargetPrice = Tool.ThirdValue;
				double Last = sc.GetLastPriceForTrading();

				// Entry Price must be below last for a Sell Limit Entry
				if(EntryPrice >= Last
				|| StopPrice <= EntryPrice)
				{
					sc.AddMessageToLog("Not Permitted: Sell Stop Entry Price Must Be Below Last and Stop Price must be above Entry Price.",1);
					return;
				}
				else
				{
					// Create our order
					s_SCNewOrder SellStop;

					// Basic Order config 
					SellStop.OrderQuantity = sc.TradeWindowOrderQuantity;
					SellStop.OrderType = SCT_ORDERTYPE_STOP_LIMIT;
					SellStop.TimeInForce = SCT_TIF_GTC;

					// Order Prices 
					SellStop.Price1 = EntryPrice;
					SellStop.Price2 = EntryPrice - (i_MaximumSlippageForStopLimitEntry.GetInt() * sc.TickSize); // Stop Limit Offset Price 
																												//
					if(i_UseLimitIfTouchedOrders.GetYesNo())
					{
						// Set target order type 
						SellStop.AttachedOrderTarget1Type = SCT_ORDERTYPE_LIMIT_IF_TOUCHED_CLIENT_SIDE;
					}

					// Target Price 
					SellStop.Target1Price = TargetPrice;


					// Stop Order Config 
					SellStop.Stop1Price = StopPrice;
					SellStop.AttachedOrderStop1Type = SCT_ORDERTYPE_STOP_LIMIT;

					double Spread = sc.Ask - sc.Bid;
					double SpreadWithMultiplier = Spread * i_BidOfferSpreadMultiplierForStopLimitOffset.GetFloat();

					// msg.Format("Spread: %f SpreadWithMult: %f", Spread, SpreadWithMultiplier);
					// sc.AddMessageToLog(msg,1);

					SellStop.StopLimitOrderLimitOffset = SpreadWithMultiplier;

					// Send order 
					int Result = static_cast<int>(sc.SellEntry(SellStop));
				}
			}

		}
		else
		{
			return;


		}
	}
	else
	{
		// sc.AddMessageToLog("no drawing is selected",1);
		return;

	}
}
