#include <boost/format.hpp>
#include <curl/curl.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <thread>
#include "json.hpp" // convenience 
					//
#include "sierrachart.h"

SCDLLName("VerrilloTrading - Open Source Studies")
SCSFExport scsf_LinkChartSymbol(SCStudyInterfaceRef sc)
{
	//
	// -------------------------------------------------------------------------
	int InputIndex = 0;
	SCInputRef i_ChartNumberToMonitor = sc.Input[InputIndex++];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Link Chart Symbol To Target Chart Symbol";
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
void VT_FixLineLabels(int& Month, int& Day, s_UseTool& Tool, SCString& msg, double& Last, SCString& LevelName)
{
	// This logic is from Clean Line Labels study to format the text string with the date
	// time from when the line originates from.
	if(Month < 10 && Day < 10 && Tool.Text.GetSubString(3,0) != msg.Format("%d-%d",Month,Day) )
	{
		// set the text formatting and transparent label background
		Tool.Text.Format("%d-%d %s",Month,Day, LevelName.GetChars());
		Tool.TransparentLabelBackground = 0;

		// Logic for Text Alignment
		// line is less than price value
		if(Tool.BeginValue < Last && Tool.TextAlignment != 8 )
		{
			Tool.TextAlignment = DT_BOTTOM | DT_LEFT;
		}
		// line is above price
		else if(Tool.BeginValue > Last && Tool.TextAlignment != 0 )
		{
			Tool.TextAlignment = DT_TOP | DT_LEFT;
		} 	
	}
	else if(Month < 10 && Day >= 10 && Tool.Text.GetSubString(4,0) != msg.Format("%d-%d",Month,Day) )
	{
		Tool.Text.Format("%d-%d %s",Month,Day, LevelName.GetChars());
		Tool.TransparentLabelBackground = 0;
		// line is less than price value
		if(Tool.BeginValue < Last && Tool.TextAlignment != 8 )
		{
			Tool.TextAlignment = DT_BOTTOM | DT_LEFT;
		}
		// line is above price
		else if(Tool.BeginValue > Last && Tool.TextAlignment != 0 )
		{
			Tool.TextAlignment = DT_TOP | DT_LEFT;
		} 	
	}
	else if(Month >= 10 && Day < 10 && Tool.Text.GetSubString(4,0) != msg.Format("%d-%d",Month,Day) )
	{
		Tool.Text.Format("%d-%d %s",Month,Day, LevelName.GetChars());
		Tool.TransparentLabelBackground = 0;

		// line is less than price value
		if(Tool.BeginValue < Last && Tool.TextAlignment != 8 )
		{
			Tool.TextAlignment = DT_BOTTOM | DT_LEFT;
		}
		// line is above price
		else if(Tool.BeginValue > Last && Tool.TextAlignment != 0 )
		{
			Tool.TextAlignment = DT_TOP | DT_LEFT;
		} 	
	}
	else if(Month >= 10 && Day >= 10 && Tool.Text.GetSubString(5,0) != msg.Format("%d-%d",Month,Day) )
	{
		Tool.Text.Format("%d-%d %s",Month,Day, LevelName.GetChars());
		Tool.TransparentLabelBackground = 0;

		// line is less than price value
		if(Tool.BeginValue < Last && Tool.TextAlignment != 8 )
		{
			Tool.TextAlignment = DT_BOTTOM | DT_LEFT;
		}
		// line is above price
		else if(Tool.BeginValue > Last && Tool.TextAlignment != 0 )
		{
			Tool.TextAlignment = DT_TOP | DT_LEFT;
		} 	
	}
}
SCSFExport scsf_DrawPriorDaysLevels(SCStudyInterfaceRef sc)
{
	int InputIndex = 1;

	// num days 
	SCInputRef i_NumDaysToCalculateHighLow = sc.Input[InputIndex++];
	SCInputRef i_DrawLevelsOnWeekend = sc.Input[InputIndex++];
	SCInputRef i_IsDaylightSavings = sc.Input[InputIndex++];

	// on off inputs 
	SCInputRef i_DrawSessionOpen = sc.Input[InputIndex++];
	SCInputRef i_DrawSessionClose = sc.Input[InputIndex++];
	SCInputRef i_DrawGlobexClose = sc.Input[InputIndex++];
	SCInputRef i_DrawEuropeOpen = sc.Input[InputIndex++];
	SCInputRef i_DrawEuropeClose = sc.Input[InputIndex++];
	SCInputRef i_DrawMidnightPrice = sc.Input[InputIndex++];

	// colors 
	SCInputRef i_IntradayHighLowColor = sc.Input[InputIndex++];
	SCInputRef i_IntradayHighLowLineStyle = sc.Input[InputIndex++];
	SCInputRef i_IntradayHighLowLineWidth = sc.Input[InputIndex++];

	SCInputRef i_SessionOpenColor = sc.Input[InputIndex++];
	SCInputRef i_SessionOpenLineStyle = sc.Input[InputIndex++];
	SCInputRef i_SessionOpenLineWidth = sc.Input[InputIndex++];

	SCInputRef i_SettlementPriceColor = sc.Input[InputIndex++];
	SCInputRef i_SettlementPriceLineStyle = sc.Input[InputIndex++];
	SCInputRef i_SettlementPriceLineWidth = sc.Input[InputIndex++];

	SCInputRef i_MidnightPriceColor = sc.Input[InputIndex++];
	SCInputRef i_MidnightPriceLineStyle = sc.Input[InputIndex++];
	SCInputRef i_MidnightPriceLineWidth = sc.Input[InputIndex++];

	SCInputRef i_GlobexClosePriceColor = sc.Input[InputIndex++];
	SCInputRef i_GlobexClosePriceLineStyle = sc.Input[InputIndex++];
	SCInputRef i_GlobexClosePriceLineWidth = sc.Input[InputIndex++];

	SCInputRef i_EuropeOpenColor = sc.Input[InputIndex++];
	SCInputRef i_EuropeOpenLineStyle = sc.Input[InputIndex++];
	SCInputRef i_EuropeOpenLineWidth = sc.Input[InputIndex++];

	SCInputRef i_EuropeCloseColor = sc.Input[InputIndex++];
	SCInputRef i_EuropeCloseLineStyle = sc.Input[InputIndex++];
	SCInputRef i_EuropeCloseLineWidth = sc.Input[InputIndex++];

	// ACS Buttons 
	SCInputRef i_ACSButtonToDrawLevels = sc.Input[InputIndex++];
	SCInputRef i_ACSButtonToHideLevels = sc.Input[InputIndex++];

	// Enable Alerts for various levels  
	SCInputRef i_EnableNewHighLowAlert = sc.Input[InputIndex++];

	SCInputRef i_EnablePreviousOpenAlert = sc.Input[InputIndex++];
	SCInputRef i_EnablePreviousCloseAlert = sc.Input[InputIndex++];
	SCInputRef i_EnablePreviousMidnightAlert = sc.Input[InputIndex++];
	SCInputRef i_EnablePreviousCMECloseAlert = sc.Input[InputIndex++];

	// Alert Number 
	SCInputRef i_AlertNumberForHighLowAlert = sc.Input[InputIndex++];

	// Alert once per bar for all alerts 
	SCInputRef i_AlertOnlyOncePerBar = sc.Input[InputIndex++];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Draw Prior Days Session High Low Close";
		sc.ValueFormat = sc.BaseGraphValueFormat;
		sc.GraphRegion = 0;
		sc.AutoLoop		= 0; // manual loop 
		
		// Update Interval is Controlled in Chart Settings -> Performance ->
		// ACSIL Performance -> Minimum Chart Update Interval in Milliseconds
		// For ACSIL UpdateAlways. 
		sc.UpdateAlways = 1; 
							 
		// Necessary for obtaining the Bar End Time 
		sc.MaintainAdditionalChartDataArrays = 1;

		i_NumDaysToCalculateHighLow.Name = "Number of Days Back to Draw Levels";
		i_NumDaysToCalculateHighLow.SetInt(4);
		i_NumDaysToCalculateHighLow.SetIntLimits(0,30);
		i_NumDaysToCalculateHighLow.SetDescription("Controls the Number of Days Back the Study draws the Levels. A setting of 0 draws only for the current day.");

		i_DrawLevelsOnWeekend.Name = "Keep Drawing Levels on Weekend (Crypto)";
		i_DrawLevelsOnWeekend.SetYesNo(0);

		i_IsDaylightSavings.Name = "Adjust Study Times for Daylight Savings Time";
		i_IsDaylightSavings.SetYesNo(1);
		i_IsDaylightSavings.SetDescription("Enable this if you set your Chart Timezone and Session Times based on New York time and we are between the dates of March and November when Daylight savings time is observed");
 

		// Session Open Time (Set in Chart Settings -> Session Times)
		i_DrawSessionOpen.Name = "Draw Session Open Price";
		i_DrawSessionOpen.SetYesNo(1);
		i_DrawSessionOpen.SetDescription("Enable Drawing the Session Open Price.");

		// Session Close Time (Set in Chart Settings -> Session Times)
		i_DrawSessionClose.Name = "Draw Session Close Price";
		i_DrawSessionClose.SetYesNo(1);
		i_DrawSessionClose.SetDescription("Enable Drawing the Session Close Price.");

		// Futures 5PM ET Close Time 
		i_DrawGlobexClose.Name = "Draw CME Futures Close Price";
		i_DrawGlobexClose.SetYesNo(1);
		i_DrawGlobexClose.SetDescription("Enable Drawing the CME Globex Close Price.");

		// Europe Open (3AM ET)
		i_DrawEuropeOpen.Name = "Draw Europe Open Price";
		i_DrawEuropeOpen.SetYesNo(1);
		i_DrawEuropeOpen.SetDescription("Enable Drawing the Europe Session Open Price. 3AM New York Time");

		// Europe Close (11:30AM ET)
		i_DrawEuropeClose.Name = "Draw Europe Close Price";
		i_DrawEuropeClose.SetYesNo(1);
		i_DrawEuropeClose.SetDescription("Enable Drawing the Europe Session Close Price. 11:30 AM New York Time");

		// Extra Levels / Open Close Times 
		// Midnight Price 
		i_DrawMidnightPrice.Name = "Draw New York Midnight Price";
		i_DrawMidnightPrice.SetYesNo(1);
		i_DrawMidnightPrice.SetDescription("Enable Drawing the New York Midnight Price.");

		// Line Color & Style inputs 
		i_IntradayHighLowColor.Name = "Intraday High/Low Line Color";
		i_IntradayHighLowColor.SetColor(128,255,255); // baby blue 
		i_IntradayHighLowColor.SetDescription("Line Color for High/Low Line Drawings");
													  
		i_IntradayHighLowLineStyle.Name = "Intraday High/Low Line Style";
		i_IntradayHighLowLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_IntradayHighLowLineStyle.SetCustomInputIndex(0);
		i_IntradayHighLowLineStyle.SetDescription("Line Style for High/Low Line Drawings");

		i_IntradayHighLowLineWidth.Name = "Intraday High/Low Line Width";
		i_IntradayHighLowLineWidth.SetInt(2);
		i_IntradayHighLowLineWidth.SetIntLimits(1,20);
		i_IntradayHighLowLineWidth.SetDescription("Line Width for High/Low Line Drawings");

		// Session Open
		i_SessionOpenColor.Name = "Session Open Price Color";
		i_SessionOpenColor.SetColor(128,255,128); // light green
		i_SessionOpenColor.SetDescription("Line Color for Session Open Drawings");
												  
		i_SessionOpenLineStyle.Name = "Session Open Price Line Style";
		i_SessionOpenLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_SessionOpenLineStyle.SetCustomInputIndex(4);
		i_SessionOpenLineStyle.SetDescription("Line Style for Session Open Drawings");

		i_SessionOpenLineWidth.Name = "Session Open Price Line Width";
		i_SessionOpenLineWidth.SetInt(4);
		i_SessionOpenLineWidth.SetIntLimits(1,20);
		i_SessionOpenLineWidth.SetDescription("Line Width for Session Open Drawings");

		// Session Close 
		i_SettlementPriceColor.Name = "Session Close Price Color";
		i_SettlementPriceColor.SetColor(255,20, 128); // cherry ish (dash dot)
		i_SettlementPriceColor.SetDescription("Line Color for Session Close Drawings");

		i_SettlementPriceLineStyle.Name = "Session Close Price Line Style";
		i_SettlementPriceLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_SettlementPriceLineStyle.SetCustomInputIndex(4);
		i_SettlementPriceLineStyle.SetDescription("Line Style for Session Close Drawings");

		i_SettlementPriceLineWidth.Name = "Session Close Price Line Width";
		i_SettlementPriceLineWidth.SetInt(4);
		i_SettlementPriceLineWidth.SetIntLimits(1,20);
		i_SettlementPriceLineWidth.SetDescription("Line Width for Session Close Drawings");

		// Midnight Price 
		i_MidnightPriceColor.Name = "Midnight Price Color";
		i_MidnightPriceColor.SetColor(255,0,255); // magenta ish (dash dot) 
		i_MidnightPriceColor.SetDescription("Line Color for Midnight Price Drawings");
												  
		i_MidnightPriceLineStyle.Name = "Midnight Price Line Style";
		i_MidnightPriceLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_MidnightPriceLineStyle.SetCustomInputIndex(4);
		i_MidnightPriceLineStyle.SetDescription("Line Style for Midnight Price Drawings");

		i_MidnightPriceLineWidth.Name = "Midnight Price Line Width";
		i_MidnightPriceLineWidth.SetInt(4);
		i_MidnightPriceLineWidth.SetIntLimits(1,20);
		i_MidnightPriceLineWidth.SetDescription("Line Width for Midnight Price Drawings");

		// CME Close price 
		i_GlobexClosePriceColor.Name = "CME Futures Close Price Color";
		i_GlobexClosePriceColor.SetColor(0,255,255); // darker cyan (dash dot) 
		i_GlobexClosePriceColor.SetDescription("Line Color for CME Close Price Drawings");
													 
		i_GlobexClosePriceLineStyle.Name = "CME Futures Close Price Line Style";
		i_GlobexClosePriceLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_GlobexClosePriceLineStyle.SetCustomInputIndex(4);
		i_GlobexClosePriceLineStyle.SetDescription("Line Style for CME Close Price Drawings");

		i_GlobexClosePriceLineWidth.Name = "CME Futures Close Price Line Width";
		i_GlobexClosePriceLineWidth.SetInt(4);
		i_GlobexClosePriceLineWidth.SetIntLimits(1,20);
		i_GlobexClosePriceLineWidth.SetDescription("Line Width for CME Close Price Drawings");

		// EU Open Price 
		i_EuropeOpenColor.Name = "Europe Open Price Color";
		i_EuropeOpenColor.SetColor(0,0,255); // navy blue dash dot) 
		i_EuropeOpenColor.SetDescription("Line Color for EU Open Price Drawings");
		
		i_EuropeOpenLineStyle.Name = "Europe Open Price Line Style";
		i_EuropeOpenLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_EuropeOpenLineStyle.SetCustomInputIndex(4);
		i_EuropeOpenLineStyle.SetDescription("Line Style for EU Open Price Drawings");

		i_EuropeOpenLineWidth.Name = "Europe Open Price Line Width";
		i_EuropeOpenLineWidth.SetInt(4);
		i_EuropeOpenLineWidth.SetIntLimits(1,20);
		i_EuropeOpenLineWidth.SetDescription("Line Width for EU Open Price Drawings");

		// EU Close Price 
		i_EuropeCloseColor.Name = "Europe Close Price Color";
		i_EuropeCloseColor.SetColor(128,0,128); // navy blue 
		i_EuropeCloseColor.SetDescription("Line Color for EU Close Price Drawings");
												
		i_EuropeCloseLineStyle.Name = "Europe Close Price Line Style";
		i_EuropeCloseLineStyle.SetCustomInputStrings(
			"Solid;Dash;Dot;DashDot;DashDotDot;Alternate");
		i_EuropeCloseLineStyle.SetCustomInputIndex(4);
		i_EuropeCloseLineStyle.SetDescription("Line Style for EU Close Price Drawings");

		i_EuropeCloseLineWidth.Name = "Europe Close Price Line Width";
		i_EuropeCloseLineWidth.SetInt(4);
		i_EuropeCloseLineWidth.SetIntLimits(1,20);
		i_EuropeCloseLineWidth.SetDescription("Line Width for EU Close Price Drawings");

		// ACS Button inputs 
		i_ACSButtonToDrawLevels.Name = "(Debugging only) ACS Button To ReDraw Prior Day Levels";
		i_ACSButtonToDrawLevels.SetInt(110);
		i_ACSButtonToDrawLevels.SetIntLimits(0, 150);
		i_ACSButtonToDrawLevels.SetDescription("ACS Button Number that was used to redraw the drawings during debugging");

		i_ACSButtonToHideLevels.Name = "ACS Button To Hide Prior Day Levels";
		i_ACSButtonToHideLevels.SetInt(111);
		i_ACSButtonToHideLevels.SetIntLimits(0,150);
		i_ACSButtonToHideLevels.SetDescription("ACS Button Number to be used to temporarily Hide/Show the drawings created by this study.");

		// Alert Inputs 
		i_EnableNewHighLowAlert.Name = "Enable Study Alert for New Low/High of Day";
		i_EnableNewHighLowAlert.SetYesNo(1);
		i_EnableNewHighLowAlert.SetDescription("Enable Study Alert when there is a new High/Low of current Day");

		i_EnablePreviousOpenAlert.Name = "Enable Alert When Test Previous Open";
		i_EnablePreviousOpenAlert.SetYesNo(1);
		i_EnablePreviousOpenAlert.SetDescription("Enable Study Alert When Test Previous Open Price");

		i_EnablePreviousCloseAlert.Name = "Enable Alert When Test Previous Close";
		i_EnablePreviousCloseAlert.SetYesNo(1);
		i_EnablePreviousCloseAlert.SetDescription("Enable Study Alert When Test Previous Close Price");

		i_EnablePreviousMidnightAlert.Name = "Enable Alert When Test Previous Midnight Price";
		i_EnablePreviousMidnightAlert.SetYesNo(1);
		i_EnablePreviousMidnightAlert.SetDescription("Enable Study Alert When Test Previous Midnight Price");

		i_EnablePreviousCMECloseAlert.Name = "Enable Alert When Test Previous CME Close Price";
		i_EnablePreviousCMECloseAlert.SetYesNo(1);
		i_EnablePreviousCMECloseAlert.SetDescription("Enable Study Alert When Test Previous CME Close Price");
		
		// Alert Number for the alert
		// Alert Numbers and sounds are configured in 
		// Global Settings > General Settings > Alerts 
		i_AlertNumberForHighLowAlert.Name = "Alert Number for New Low/High Alert";
		i_AlertNumberForHighLowAlert.SetInt(4);
		i_AlertNumberForHighLowAlert.SetIntLimits(1,200);
		i_AlertNumberForHighLowAlert.SetDescription("Alert Number to use for New High/Low of Day Alert");

		// Alert Only Once per bar or everytime there is a new high or low of day. 
		//
		// The user would set this to 0 if they wanted to recieve many alerts when
		// there is a new high or low. In the case that they want to be notified 
		// if the market is moving. 
		i_AlertOnlyOncePerBar.Name = "Send New High/Low Alert Only Once Per Bar";
		i_AlertOnlyOncePerBar.SetYesNo(1);
		i_AlertOnlyOncePerBar.SetDescription("It is recommended to leave this input enabled and only disable it if you want to recieve many alerts when a new high/low is made.");

		// Study Description
		sc.StudyDescription = "This study was written by Christian at VerrilloTrading in December of 2024. "
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
			"It is recommended to set the Time Zone of your chart explicitly using Chart Settings > Session Times > Time Zone. As opposed to setting it to <u>Use Global Time Zone</u>. One reason for this is because when this setting is changed it automatically changes the session start and end times for the chart. Making it easier if you move to a different Time Zone and need to change the Session Times for the Chart. In addition to this I recommend linking the Session Time and Time Zone on any charts in your chartbook that use the same Session Times. This is controlled using Chart Settings > Linking > Chart Linking."

			"<br><br>"
			"<strong>Hide/Show Prior Day Levels:</strong>"
			"<br><br>"
			"There is support for hiding the drawings that this study draws. This is done using the Input Setting named <strong>ACS Button To Hide Prior Day Levels</strong> to select which Advanced Custom Study button will be used to hide or show the drawings. By default they are always shown."
			"<br><br>"
			"<strong>Using Different Intraday Storage Time Units:</strong>"
			"<br><br>"
			"Currently the study only supports using an intraday storage time unit of one tick or one second."
			"<br><br>"
			"The fix for this is in the process of being added to the study. Because we also use intraday storage time unit 2 seconds and above on a Sierra Chart instance running remotely only for the purpose of sending alerts, obtaining quotes or position updates."
			"<br><br>"
			"<strong>Steps to Ensure the CME Globex Close Price is Drawn:</strong>"
			"<br><br>"
			"It is necessary to enable this setting: Chart Settings > Bar Period > Include Columns With No Data."
			"<br><br>"
			"It is also important that this setting remains in an off state: Chart Settings > Session Times > Apply Intra Day Session Times To Intraday Chart.  "
			"<br><br>"
			"Because or else every time the Intraday chart symbol is changed, the Session Times also get modified."
			"<br><br>"
			"<strong>New High/Low of Day Telegram Alerts:</strong>"
			"<br><br>"
			"This study support sending a study alert when a new high or low price of day is made. We might also add the option of having a study alert trigger when the additional levels in the study are crossed."
			"<br>"
			"It is possible to forward these alerts directly to a Telegram chat using either the VerrilloTrading <a href = https://youtu.be/EQZI9pBtDrE target=_blank rel=noopener noreferrer >Telegram Chart Drawing Alerts</a> study or using the built-in Sierra Chart method."
			"<br><br>"
			"<strong>General Notes:</strong>"
			"<br><br>"
			"For the extra overnight levels to be drawn the user must enable the Evening Session using Chart Settings > Session Times > Use Evening Session and have their Evening Session times set correctly relative to their Day Session times."
			"<br><br>"
			"This study has only been tested with Denali Exchange Data Feed on Sierra Chart and not other Data feeds like IQFeed."
			"<br><br>"
			"If you encounter issues with the overnight prices not displaying, you can report this to support@verrillotrading.com. This study is open source and there will not be any free ongoing development of it, with the exception of a few items we still want to add. There is no guarantee we will be able to get to any support requests, or if we will consider additional feature requests. "
			"<br><br>"
			"If you absolutely need an issue resolved or want to show gratitude for this work you can pay using this donation page:"
			"<br><br>"
			"<a href = https://verrillotrading.com/tips target=_blank rel=noopener noreferrer >https://verrillotrading.com/tips</a>"
			"<br><br>"
			"I genuinely hope you enjoy the study,"
			"<br><br>"
			"Good Trading, "
			"<br><br>"
			"-Christian"
			"<br><br>"
			"VerrilloTrading, Content Creator, Programmer"
			"";

		return;
	}

	// do not calculate if chart is still downloading data 
	if (sc.ChartIsDownloadingHistoricalData(sc.ChartNumber))
	{
		// sc.AddMessageToLog(
		// 	"Chart is downloading historical data. Wait for it to finish.", 1);
		return;
	}

	// Used for remembering if the drawings are hidden 
	int& HideLevels = sc.GetPersistentIntFast(1);

	// Other Persistent Variables 
	// 
	// used to remember price levels for triggering realtime alerts 
	double& LowOfDayMemory = sc.GetPersistentDoubleFast(2);
	double& HighOfDayMemory = sc.GetPersistentDoubleFast(3);

	// These are used to remember the levels drawn in real-time 
	// In order to not draw them again and also modify if necessary
	int& LowOfDayLineNumberMemory = sc.GetPersistentIntFast(4);
	int& HighOfDayLineNumberMemory = sc.GetPersistentIntFast(5);

	int& SessionOpenLineNumberMemory = sc.GetPersistentIntFast(6);
	int& SessionCloseLineNumberMemory = sc.GetPersistentIntFast(7);
	int& CMECloseLineNumberMemory = sc.GetPersistentIntFast(8);
	int& MidnightPriceLineNumberMemory = sc.GetPersistentIntFast(9);
	int& EUOpenPriceLineNumberMemory = sc.GetPersistentIntFast(10);
	int& EUClosePriceLineNumberMemory = sc.GetPersistentIntFast(11);

	// logging object 
	SCString msg;

	// Persistent Vector used to save what lines we have drawn, in order
	// to ensure correct cleanup behaviour
    std::vector<int>* p_LineNumbers = reinterpret_cast<std::vector<int>*>
		(sc.GetPersistentPointer(0));

	// First time initialization
    if (p_LineNumbers == NULL) 
	{
		// initialize 
        p_LineNumbers = new std::vector<int>;
        sc.SetPersistentPointer(0, p_LineNumbers);
    }
    else 
	{
		// If not null, just do nothing because we will recalc only on a press
		// of acs button after deleting the drawings 
    }

	// handle last call to function
	if(sc.LastCallToFunction)
	{
		if(p_LineNumbers != 0)
		{
			delete p_LineNumbers;
			sc.SetPersistentPointer(0, NULL);
		}
	}

	// handle full chart recalculation
	if(sc.IsFullRecalculation)
	{
		// Could empty the vector here and call sc.MenuEventID to redraw all levels 
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

		// Reset Persistent Variables 
		LowOfDayMemory = 0;
		HighOfDayMemory = 0;
		LowOfDayLineNumberMemory = 0;
		HighOfDayLineNumberMemory = 0;

		SessionOpenLineNumberMemory = 0;
		SessionCloseLineNumberMemory = 0;
		CMECloseLineNumberMemory = 0;
		MidnightPriceLineNumberMemory = 0;
		EUOpenPriceLineNumberMemory = 0;
		EUClosePriceLineNumberMemory = 0;
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
	
	SCDateTime MidnightPriceTime, CMECloseTime, EUOpenTime, EUCloseTime;
										   
	// Maybe instead of hardcoding these we can set them relative to the session open or close times. 
	// 
	// For now I will adjust all of these times down by one hour and use the input setting to account for this 
	MidnightPriceTime.SetDate(CurrentDay);
	CMECloseTime.SetDate(CurrentDay);
	EUOpenTime.SetDate(CurrentDay);
	EUCloseTime.SetDate(CurrentDay);
	if(i_IsDaylightSavings.GetBoolean())
	{
		// Adjusted times for when New York observes daylight savings time 
		MidnightPriceTime.SetTimeHMS(4,0,0); // 4:00 AM UTC Time (04:00:00)
												//
		CMECloseTime.SetTimeHMS(21,0,0); // 21:00 PM UTC Time 
										 //
		EUOpenTime.SetTimeHMS(7,0,0); // 7:00 AM UTC Time 
									  //
		EUCloseTime.SetTimeHMS(15,30,0); // 15:30 PM UTC Time  (15:59:59
	}
	else
	{

		MidnightPriceTime.SetTimeHMS(5,0,0); // 5:00 AM UTC Time (05:00:00)
												//
		CMECloseTime.SetTimeHMS(22,0,0); // 22:00 PM UTC Time 
										 //
		EUOpenTime.SetTimeHMS(8,0,0); // 8:00 AM UTC Time 
									  //
		EUCloseTime.SetTimeHMS(16,30,0); // 16:30 PM UTC Time  (15:59:59

	}
	
	// UTC back to chart timezone 
	SCDateTimeMS ConvertedMidnightPrice = 
		sc.ConvertDateTimeToChartTimeZone(MidnightPriceTime, TIMEZONE_UTC);

	SCDateTimeMS ConvertedCMECloseTime = 
		sc.ConvertDateTimeToChartTimeZone(CMECloseTime, TIMEZONE_UTC);

	SCDateTimeMS ConvertedEUOpenTime = 
		sc.ConvertDateTimeToChartTimeZone(EUOpenTime, TIMEZONE_UTC);

	SCDateTimeMS ConvertedEUCloseTime = 
		sc.ConvertDateTimeToChartTimeZone(EUCloseTime, TIMEZONE_UTC);

	// Calculate the starting day for high/lows
	int NumDaysToCalculateHighLow = i_NumDaysToCalculateHighLow.GetInt();

	// get the start dates for calculation
	int HighLowStartDate = CurrentDay - NumDaysToCalculateHighLow;
	
	// debug 
	// msg.Format("Current Day: %d HighLow Start Date: %d NumDaysCalculateHighLow: %d", 
	// CurrentDay, HighLowStartDate, NumDaysToCalculateHighLow);
	// sc.AddMessageToLog(msg,1);

	// Assign that day to SCDateTime variables so we can call the next function 
	SCDateTime HighLowStartDateTime(HighLowStartDate,0);

	// I will use this function start iterating from the correct bar index for each day 
	int HighLowStartDateTimeIndex = 
		sc.GetContainingIndexForSCDateTime(sc.ChartNumber, HighLowStartDateTime);

	// DONE: Keep track of the day, when the day moves to the net day, stop
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
	// Midnight Price Setup
	int MidnightPriceInSeconds = ConvertedMidnightPrice.GetTimeInSeconds();
	int LastDrawnMidnightPriceDate = 0; 
	bool MidnightPriceDrawn = 0;

	//-------------------------------------
	//  Same variables for CME Close Time 
	int CMECloseTimeInSeconds = ConvertedCMECloseTime.GetTimeInSeconds();
	int LastDrawnCMECloseDate = 0; 
	bool CMEClosePriceDrawn = 0;
	// ------------------------------------
	//
	//  Same Variables for EU Open time 
	int EUOpenTimeInSeconds = ConvertedEUOpenTime.GetTimeInSeconds();
	int LastDrawnEUOpenDate = 0; 
	bool EUOpenPriceDrawn = 0;
	// ------------------------------------

	// Same Variables for EU Close time 
	int EUCloseTimeInSeconds = ConvertedEUCloseTime.GetTimeInSeconds();
	int LastDrawnEUCloseDate = 0; 
	bool EUClosePriceDrawn = 0;
	// ------------------------------------
	
	// Variable used for passing in line names for line labels 
	SCString LineName = "";

	
	// ACS REDRAW BUTTON SCOPE
	//
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


		// Iterate from the starting point until end of the chart 
		for(int Index = HighLowStartDateTimeIndex; Index < sc.ArraySize; Index++)
		{
			// Get the date
			CurrentBarDate = sc.BaseDateTimeIn.DateAt(Index);

			// draw only one drawing per day 
			if(CurrentBarDate > HighLowStartDate)
			{
				// Check input setting for draw on weekend
				if(i_DrawLevelsOnWeekend.GetBoolean() == false)
				{
					// check if it is sunday 
					if(sc.BaseDateTimeIn[Index].IsSunday() == 1
						|| sc.BaseDateTimeIn[Index].IsSaturday() == 1)
					{
						// msg.Format("Is Sunday CurrentBarDate: %d", CurrentBarDate);
						// sc.AddMessageToLog(msg,1);	

						HighLowStartDate++;
						continue;
					}
				}

				// we are now on a new day 
				//
				// draw stuff and increment variables 
				s_UseTool LowOfDay;

				// LowOfDay.AddAsUserDrawnDrawing = 1;
				LowOfDay.AllowCopyToOtherCharts = 1;
				LowOfDay.AllowSaveToChartbook = 1;
				LowOfDay.ChartNumber = sc.ChartNumber;

				LowOfDay.LineNumber = -1;  

				LowOfDay.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
				LowOfDay.LineWidth = i_IntradayHighLowLineWidth.GetInt();
				LowOfDay.LineStyle = static_cast<SubgraphLineStyles>(i_IntradayHighLowLineStyle.GetIndex());
				LowOfDay.DisplayHorizontalLineValue = 1;

				// Price Value
				LowOfDay.BeginValue = sc.Low[LowIndex];
				LowOfDay.EndValue = LowOfDay.BeginValue;

				// GET BAR INDEX 
				LowOfDay.BeginIndex = LowIndex;
				LowOfDay.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

				LowOfDay.AddMethod = UTAM_ADD_OR_ADJUST;
				LowOfDay.Region = sc.GraphRegion;

				LowOfDay.Color = i_IntradayHighLowColor.GetColor();

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

				// ---------------------------------------------------------------------
				// Now do High of day 
				s_UseTool HighOfDay;

				// HighOfDay.AddAsUserDrawnDrawing = 1;
				HighOfDay.AllowCopyToOtherCharts = 1;
				HighOfDay.AllowSaveToChartbook = 1;
				HighOfDay.ChartNumber = sc.ChartNumber;
				//
				HighOfDay.LineNumber = -1;  

				HighOfDay.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
				HighOfDay.LineWidth = i_IntradayHighLowLineWidth.GetInt();
				HighOfDay.LineStyle = static_cast<SubgraphLineStyles>(i_IntradayHighLowLineStyle.GetIndex());
				HighOfDay.DisplayHorizontalLineValue = 1;

				// Price Value 
				HighOfDay.BeginValue = sc.High[HighIndex];
				HighOfDay.EndValue = HighOfDay.BeginValue;

				// GET BAR INDEX 
				HighOfDay.BeginIndex = HighIndex;
				HighOfDay.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

				HighOfDay.AddMethod = UTAM_ADD_OR_ADJUST;
				HighOfDay.Region = sc.GraphRegion;

				HighOfDay.Color = i_IntradayHighLowColor.GetColor();

				// Code used for Fixing Line Labels 
				
				// Convert bar index into a date time variable, number of days since 1899
				DrawingStartDate = sc.BaseDateTimeIn[HighIndex].GetDate();
				
				// get the year month and day from SCDateTimeVariable
				DrawingStartDate.GetDateYMD(Year,Month,Day);

				// Assign the line name for this drawing and pass to function
				LineName = "H";

				// Call fix line labels 
				VT_FixLineLabels(Month, Day, HighOfDay, msg, Last, LineName);

				sc.UseTool(HighOfDay);
				p_LineNumbers->push_back(HighOfDay.LineNumber);

				// --------------------------------------------------
				// reset high and low 
				High = DefaultHighValue;
				Low = DefaultLowValue;

				HighLowStartDate++; // increment starting date 
			}

			// get the high of the day 
			if(High < sc.High[Index])
			{
				High = sc.High[Index];
				HighIndex = Index;
			}

			// get the low of the day 
			if(Low > sc.Low[Index])
			{
				Low = sc.Low[Index];
				LowIndex = Index;
			}

			// Get Start and End Time of the Current Bar 
			// CurrentBarStartTime = sc.BaseDateTimeIn[Index].GetTime();
			// CurrentBarEndTime = sc.BaseDataEndDateTime[Index].GetTime();

			// other syntax 
			CurrentBarStartTime = sc.BaseDateTimeIn.TimeAt(Index);
			CurrentBarEndTime = sc.BaseDataEndDateTime.TimeAt(Index);

			// useful debug 
			// msg.Format("SessionEndTimeInSeconds: %d CurrentBarStartTime: %d CurrentBarEndTime: %d", 
			// 	SessionEndTimeInSeconds, CurrentBarStartTime,CurrentBarEndTime);
			// sc.AddMessageToLog(msg,1);

			// ----------------------------------------------------------------------
			// LOGIC FOR SETTLEMENT PRICE 
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

						// SessionClose.AddAsUserDrawnDrawing = 1;
						SessionClose.AllowCopyToOtherCharts = 1;
						SessionClose.AllowSaveToChartbook = 1;
						SessionClose.ChartNumber = sc.ChartNumber;
						SessionClose.LineNumber = -1;  

						SessionClose.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
						SessionClose.LineWidth = i_SettlementPriceLineWidth.GetInt();
						SessionClose.LineStyle = LINESTYLE_DASHDOTDOT;
						SessionClose.DisplayHorizontalLineValue = 1;

						// Price Value 
						SessionClose.BeginValue = sc.Open[Index];
						SessionClose.EndValue = SessionClose.BeginValue;

						// GET BAR INDEX 
						SessionClose.BeginIndex = Index -1;
						SessionClose.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

						SessionClose.AddMethod = UTAM_ADD_OR_ADJUST;
						SessionClose.Region = sc.GraphRegion;

						SessionClose.Color = i_SettlementPriceColor.GetColor();

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
					// End Settlement Price Logic 
				}
			}

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

						// MidnightPrice.AddAsUserDrawnDrawing = 1;
						MidnightPrice.AllowCopyToOtherCharts = 1;
						MidnightPrice.AllowSaveToChartbook = 1;
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
			// START SESSION OPEN TIME LOGIC 
			if(i_DrawSessionOpen.GetInt() == 1)
			{
				// logic to reset looking for new session open price 
				if(CurrentBarDate > LastDrawnOpenDate && SessionOpenPriceDrawn == 1)
				{
					SessionOpenPriceDrawn = 0;
				}

				// only looks for the openprice if the open price 
				// on the day has not already been drawn 
				if(SessionOpenPriceDrawn == 0)
				{
					// Check if Session End Time exists within the boundaries of our bar 
					bool IsWithinBarTime = 
					(SessionStartTimeInSeconds >= CurrentBarStartTime && 
					 SessionStartTimeInSeconds <= CurrentBarEndTime);

					if(IsWithinBarTime)
					{
						// we're on current settlement bar

						// Draw it using our regular drawing tool 
						s_UseTool SessionOpen;

						// SessionOpen.AddAsUserDrawnDrawing = 1;
						SessionOpen.AllowCopyToOtherCharts = 1;
						SessionOpen.AllowSaveToChartbook = 1;
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
			}
			// END SESSION OPEN LOGIC 
			//
			if(i_DrawGlobexClose.GetInt() == 1)
			{
				// START CME CLOSE LOGIC 
				if(CurrentBarDate > LastDrawnCMECloseDate && CMEClosePriceDrawn == 1)
				{
					CMEClosePriceDrawn = 0;
				}

				// only looks for the openprice if the open price 
				// on the day has not already been drawn 
				if(CMEClosePriceDrawn == 0 && sc.BaseDateTimeIn[Index].IsSunday() != 1
					&& sc.BaseDateTimeIn[Index].IsSaturday() != 1)
				{
					// Check if Session End Time exists within the boundaries of our bar 
					bool IsWithinBarTime = 
					(CMECloseTimeInSeconds >= CurrentBarStartTime && 
					 CMECloseTimeInSeconds <= CurrentBarEndTime);

					bool IsWithinBarTimeCME = 
					(CMECloseTimeInSeconds == CurrentBarStartTime && 
					 CurrentBarEndTime == 0 
					 || CMECloseTimeInSeconds -1 == CurrentBarStartTime && CurrentBarEndTime == 0);

					if(IsWithinBarTime || IsWithinBarTimeCME)
					{
						// we're on current settlement bar

						// Draw it using our regular drawing tool 
						s_UseTool CMESessionClose;

						// CMESessionClose.AddAsUserDrawnDrawing = 1;
						CMESessionClose.AllowCopyToOtherCharts = 1;
						CMESessionClose.AllowSaveToChartbook = 1;
						CMESessionClose.ChartNumber = sc.ChartNumber;
						CMESessionClose.LineNumber = -1;  

						CMESessionClose.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
						CMESessionClose.LineWidth = i_GlobexClosePriceLineWidth.GetInt();
						CMESessionClose.LineStyle = static_cast<SubgraphLineStyles>(i_GlobexClosePriceLineStyle.GetIndex());
						CMESessionClose.DisplayHorizontalLineValue = 1;

						// Price Value 
						CMESessionClose.BeginValue = sc.Close[Index];
						CMESessionClose.EndValue = CMESessionClose.BeginValue;

						// GET BAR INDEX 
						CMESessionClose.BeginIndex = Index;
						CMESessionClose.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

						CMESessionClose.AddMethod = UTAM_ADD_OR_ADJUST;
						CMESessionClose.Region = sc.GraphRegion;

						CMESessionClose.Color = i_GlobexClosePriceColor.GetColor();

						// Code used for Fixing Line Labels 
						
						// Convert bar index into a date time variable, number of days since 1899
						SCDateTime DrawingStartDate = sc.BaseDateTimeIn[Index].GetDate();
						
						// get the year month and day from SCDateTimeVariable
						int Year, Month, Day;
						DrawingStartDate.GetDateYMD(Year,Month,Day);

						// Assign the line name for this drawing and pass to function
						LineName = "";

						// Call fix line labels 
						VT_FixLineLabels(Month, Day, CMESessionClose, msg, Last, LineName);

						// draw it 
						sc.UseTool(CMESessionClose);

						// remember line number 
						p_LineNumbers->push_back(CMESessionClose.LineNumber);

						// ESSENTIAL RESET VARIABLES 
						// Set variables to ensure we only get here again on the next day 
						LastDrawnCMECloseDate = sc.BaseDateTimeIn[Index].GetDate();
						CMEClosePriceDrawn = 1;

						if(CurrentDay == DrawingStartDate && CMECloseTimeInSeconds != 0)
						{
							// This drawing takes place on current day therefore we should remember its line number 
							// This is to inform the real-time code that the drawing has already been drawn. 

							CMECloseLineNumberMemory = CMESessionClose.LineNumber;
						}
					}
				}
			}
			// -------------------------
			// END CME CLOSE PRICE LOGIC

			// START EU OPEN PRICE LOGIC 
			if(i_DrawEuropeOpen.GetInt() == 1)
			{
				if(CurrentBarDate > LastDrawnEUOpenDate && EUOpenPriceDrawn == 1)
				{
					EUOpenPriceDrawn = 0;
				}

				// only looks for the openprice if the open price 
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

						// EUOpen.AddAsUserDrawnDrawing = 1;
						EUOpen.AllowCopyToOtherCharts = 1;
						EUOpen.AllowSaveToChartbook = 1;
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
			//
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

						// EUClose.AddAsUserDrawnDrawing = 1;
						EUClose.AllowCopyToOtherCharts = 1;
						EUClose.AllowSaveToChartbook = 1;
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
		}

		// debug midnight price 
		// msg.Format("NumResets: %d CurrentBarDate: %d LastDrawnMidnightPriceDate: %d",
		// 	NumResets, CurrentBarDate, LastDrawnMidnightPriceDate);
		// sc.AddMessageToLog(msg,1);
		//
		// HANDLE MOST RECENT HIGH/LOW 
		// ---------------------------------------
		// Once we exit this for loop, the high and low should be set to the
		// high/low of the current day. 
		//
		// Therefore we need to handle the drawing for
		// the current day's high low here and separetely 
		//
		s_UseTool LowOfDay;

		// LowOfDay.AddAsUserDrawnDrawing = 1;
		LowOfDay.AllowCopyToOtherCharts = 1;
		LowOfDay.AllowSaveToChartbook = 1;
		LowOfDay.ChartNumber = sc.ChartNumber;
		LowOfDay.LineNumber = -1;  
		LowOfDay.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
		LowOfDay.LineWidth = i_IntradayHighLowLineWidth.GetInt();
		LowOfDay.LineStyle = static_cast<SubgraphLineStyles>(i_IntradayHighLowLineStyle.GetIndex());
		LowOfDay.DisplayHorizontalLineValue = 1;

		// Price Value 
		LowOfDay.BeginValue = sc.Low[LowIndex];
		LowOfDay.EndValue = LowOfDay.BeginValue;

		// GET BAR INDEX 
		LowOfDay.BeginIndex = LowIndex;
		LowOfDay.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

		LowOfDay.AddMethod = UTAM_ADD_OR_ADJUST;
		LowOfDay.Region = sc.GraphRegion;

		LowOfDay.Color = i_IntradayHighLowColor.GetColor();

		// Date Time Object 
		SCDateTime DrawingStartDate;

		// Convert bar index into a date time variable, number of days since 1899
		DrawingStartDate = sc.BaseDateTimeIn[LowIndex].GetDate();
		
		// get the year month and day from SCDateTimeVariable
		int Year, Month, Day;
		DrawingStartDate.GetDateYMD(Year,Month,Day);

		// Assign the line name for this drawing and pass to function
		LineName = "L";

		// Call fix line labels 
		VT_FixLineLabels(Month, Day, LowOfDay, msg, Last,  LineName);

		sc.UseTool(LowOfDay);
		p_LineNumbers->push_back(LowOfDay.LineNumber);

		// save low of day into memory to be alerted on new low of day 
		LowOfDayLineNumberMemory = LowOfDay.LineNumber;
		LowOfDayMemory = LowOfDay.BeginValue;

		// No need to increment line number because this is last drawing
		
		// now do High of day 
		s_UseTool HighOfDay;

		// HighOfDay.AddAsUserDrawnDrawing = 1;
		HighOfDay.AllowCopyToOtherCharts = 1;
		HighOfDay.AllowSaveToChartbook = 1;
		HighOfDay.ChartNumber = sc.ChartNumber;
		HighOfDay.LineNumber = -1;  
		HighOfDay.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
		HighOfDay.LineWidth = i_IntradayHighLowLineWidth.GetInt();
		HighOfDay.LineStyle = static_cast<SubgraphLineStyles>(i_IntradayHighLowLineStyle.GetIndex());
		HighOfDay.DisplayHorizontalLineValue = 1;

		// Price Value 
		HighOfDay.BeginValue = sc.High[HighIndex];
		HighOfDay.EndValue = HighOfDay.BeginValue;

		// GET BAR INDEX 
		HighOfDay.BeginIndex = HighIndex;
		HighOfDay.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

		HighOfDay.AddMethod = UTAM_ADD_OR_ADJUST;
		HighOfDay.Region = sc.GraphRegion;

		HighOfDay.Color = i_IntradayHighLowColor.GetColor();

		// Code used for Fixing Line Labels 
		
		// Convert bar index into a date time variable, number of days since 1899
		DrawingStartDate = sc.BaseDateTimeIn[HighIndex].GetDate();
		
		// get the year month and day from SCDateTimeVariable
		DrawingStartDate.GetDateYMD(Year,Month,Day);

		// Assign the line name for this drawing and pass to function
		LineName = "H";

		// Call fix line labels 
		VT_FixLineLabels(Month, Day, HighOfDay, msg, Last, LineName);

		sc.UseTool(HighOfDay);
		p_LineNumbers->push_back(HighOfDay.LineNumber);

		// save high of day into memory to be alerted on new low of day 
		HighOfDayLineNumberMemory = HighOfDay.LineNumber;
		HighOfDayMemory = HighOfDay.BeginValue;

		// No need to increment the line number anymore because this is our last
		// drawing. 
	}
	// End ACS Button press 

	// START REALTIME LOGIC 
	//
	// TODO: Go over array of levels saved from this study 
	// TODO: Write alert functionality for the extra levels
	// If we reclaim prior day high or low, midnight price etc. 
	//
	// We can iterate through all saved levels in vector and compare which are
	// closest to last price and monitor those.
	

	s_UseTool Line;
	// Check levels in array for realtime tests 
	for(int i = 0; i < p_LineNumbers->size();i++)
	{
		if(sc.GetACSDrawingByLineNumber(sc.ChartNumber, (*p_LineNumbers)[i], Line))
		{
			// Check for levels based on their color and draw style 
			if(Line.Color == i_SessionOpenColor.GetColor()
				&& Line.LineStyle == static_cast<SubgraphLineStyles>(i_SessionOpenLineStyle.GetIndex())
				&& Line.LineWidth == i_SessionOpenLineWidth.GetInt())
			{

				// This line is a prior day open 
				// Check if the user enabled alerts for those levels 
				if(i_EnablePreviousOpenAlert.GetBoolean())
				{

					// check if the last price or current bar price has breached this level 

				}
				else
				{

				}


			}
			//

		}

	}

	// check for new low of day 
	if(sc.Low[sc.ArraySize-1] < LowOfDayMemory)
	{
		// Remember the new low 
		LowOfDayMemory = sc.Low[sc.ArraySize-1];
		
		// redraw the drawing 
		s_UseTool LowOfDay;

		LowOfDay.LineNumber = LowOfDayLineNumberMemory;  

		// Update Price
		LowOfDay.BeginValue = sc.Low[sc.ArraySize-1];
		LowOfDay.EndValue = LowOfDay.BeginValue;

		// Update BAR INDEX 
		LowOfDay.BeginIndex = sc.ArraySize-1;
		LowOfDay.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

		LowOfDay.AddMethod = UTAM_ADD_OR_ADJUST;

		// draw 
		sc.UseTool(LowOfDay);

		// if alerts are enabled 
		if(i_EnableNewHighLowAlert.GetBoolean())
		{
			// Get the current chart symbol for alert text 
			SCString CurrentSymbol = sc.Symbol;

			// Create alert text 
			SCString AlertText = SCString("New Low of Day on ") 
				+ std::move(CurrentSymbol) + SCString("!");

			// Create New Low of day alert 
			sc.SetAlert(i_AlertNumberForHighLowAlert.GetInt(), 
				sc.ArraySize-1, std::move(AlertText)); 
		}
	}
	// check for new high of day 
	if(sc.High[sc.ArraySize-1] > HighOfDayMemory)
	{
		// remember the new high 
		HighOfDayMemory = sc.High[sc.ArraySize-1];

		// redraw the drawing 
		s_UseTool HighOfDay;
		HighOfDay.LineNumber = HighOfDayLineNumberMemory;  

		// Update Price   
		HighOfDay.BeginValue = sc.High[sc.ArraySize-1];
		HighOfDay.EndValue = HighOfDay.BeginValue;

		// Update Bar Index 
		HighOfDay.BeginIndex = sc.ArraySize-1;
		HighOfDay.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

		// draw
		sc.UseTool(HighOfDay);

		// if alerts are enabled 
		if(i_EnableNewHighLowAlert.GetBoolean())
		{
			// Get the current chart symbol for alert text 
			SCString CurrentSymbol = sc.Symbol;

			// Create alert text 
			SCString AlertText = SCString("New High of Day on ") 
				+ std::move(CurrentSymbol) + SCString("!");

			// Create New High of day alert 
			sc.SetAlert(i_AlertNumberForHighLowAlert.GetInt(), 
				sc.ArraySize-1, std::move(AlertText)); 
		}
	}

	//
	// --------------------------------------------------------------------------------
	// Draw real-time session open close levels as they happen 
	
	// Get Most recent bar start and end time 
	CurrentBarStartTime = sc.BaseDateTimeIn.TimeAt(sc.ArraySize-1);
	CurrentBarEndTime = sc.BaseDataEndDateTime.TimeAt(sc.ArraySize-1);

	// Input enabled and logic to determine if we are on the right bar 
	if(i_DrawSessionOpen.GetBoolean() && 
		CurrentBarStartTime == SessionStartTimeInSeconds)
	{
		// Line Number will be saved if it was already drawn 
		if(SessionOpenLineNumberMemory == 0)
		{
			// Draw the session open price 
			s_UseTool SessionOpen;

			// SessionOpen.AddAsUserDrawnDrawing = 1;
			SessionOpen.AllowCopyToOtherCharts = 1;
			SessionOpen.AllowSaveToChartbook = 1;
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

	// Input enabled and logic to determine if we are on the right bar 
	if(i_DrawSessionClose.GetBoolean() && 
		CurrentBarStartTime == SessionEndTimeInSeconds)
	{
		// Line Number will be saved if it was already drawn 
		if(SessionCloseLineNumberMemory == 0)
		{
			// Draw it using our regular drawing tool 
			s_UseTool SessionClose;

			// SessionClose.AddAsUserDrawnDrawing = 1;
			SessionClose.AllowCopyToOtherCharts = 1;
			SessionClose.AllowSaveToChartbook = 1;
			SessionClose.ChartNumber = sc.ChartNumber;
			SessionClose.LineNumber = -1;  

			SessionClose.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
			SessionClose.LineWidth = 4;
			SessionClose.LineStyle = LINESTYLE_DASHDOTDOT;
			SessionClose.LineStyle = static_cast<SubgraphLineStyles>(i_SettlementPriceLineStyle.GetIndex());
			SessionClose.DisplayHorizontalLineValue = 1;

			// Price Value 
			SessionClose.BeginValue = sc.Open[sc.ArraySize-1];
			SessionClose.EndValue = SessionClose.BeginValue;

			// GET BAR INDEX 
			SessionClose.BeginIndex = sc.ArraySize-2;
			SessionClose.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

			SessionClose.AddMethod = UTAM_ADD_OR_ADJUST;
			SessionClose.Region = sc.GraphRegion;

			SessionClose.Color = i_SettlementPriceColor.GetColor();

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

	if(i_DrawMidnightPrice.GetBoolean() && 
		CurrentBarStartTime == MidnightPriceInSeconds)
	{
		// Line Number will be saved if it was already drawn 
		if(MidnightPriceLineNumberMemory == 0)
		{
			// Draw it using our regular drawing tool 
			s_UseTool MidnightPrice;

			// MidnightPrice.AddAsUserDrawnDrawing = 1;
			MidnightPrice.AllowCopyToOtherCharts = 1;
			MidnightPrice.AllowSaveToChartbook = 1;
			MidnightPrice.ChartNumber = sc.ChartNumber;
			MidnightPrice.LineNumber = -1;  

			MidnightPrice.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
			MidnightPrice.LineWidth = 4;
			MidnightPrice.LineStyle = LINESTYLE_DASHDOTDOT;
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
	
	// May need different logic for this time 
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
			s_UseTool CMESessionClose;

			// CMESessionClose.AddAsUserDrawnDrawing = 1;
			CMESessionClose.AllowCopyToOtherCharts = 1;
			CMESessionClose.AllowSaveToChartbook = 1;
			CMESessionClose.ChartNumber = sc.ChartNumber;
			CMESessionClose.LineNumber = -1;  

			CMESessionClose.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
			CMESessionClose.LineWidth = i_GlobexClosePriceLineWidth.GetInt();
			CMESessionClose.LineStyle = static_cast<SubgraphLineStyles>(i_GlobexClosePriceLineStyle.GetIndex());
			CMESessionClose.DisplayHorizontalLineValue = 1;

			// Price Value 
			CMESessionClose.BeginValue = sc.Close[sc.ArraySize-2];
			CMESessionClose.EndValue = CMESessionClose.BeginValue;

			// GET BAR INDEX 
			CMESessionClose.BeginIndex = sc.ArraySize-2;
			CMESessionClose.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

			CMESessionClose.AddMethod = UTAM_ADD_OR_ADJUST;
			CMESessionClose.Region = sc.GraphRegion;

			CMESessionClose.Color = i_GlobexClosePriceColor.GetColor();

			// Code used for Fixing Line Labels 
			
			// Convert bar index into a date time variable, number of days since 1899
			SCDateTime DrawingStartDate = sc.BaseDateTimeIn[sc.ArraySize-1].GetDate();
			
			// get the year month and day from SCDateTimeVariable
			int Year, Month, Day;
			DrawingStartDate.GetDateYMD(Year,Month,Day);

			// Assign the line name for this drawing and pass to function
			LineName = "";

			// Call fix line labels 
			VT_FixLineLabels(Month, Day, CMESessionClose, msg, Last, LineName);

			// draw it 
			sc.UseTool(CMESessionClose);

			// remember line number 
			p_LineNumbers->push_back(CMESessionClose.LineNumber);

			// Remember the Line Number so our historical drawings do not redraw it 
			CMECloseLineNumberMemory = CMESessionClose.LineNumber;
		}
	}

	if(i_DrawEuropeOpen.GetBoolean() && 
		CurrentBarStartTime == EUOpenTimeInSeconds)
	{
		// Line Number will be saved if it was already drawn 
		if(EUOpenPriceLineNumberMemory == 0)
		{
			s_UseTool EUOpen;

			// EUOpen.AddAsUserDrawnDrawing = 1;
			EUOpen.AllowCopyToOtherCharts = 1;
			EUOpen.AllowSaveToChartbook = 1;
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

	if(i_DrawEuropeClose.GetBoolean() && 
		CurrentBarStartTime == EUCloseTimeInSeconds)
	{
		// Line Number will be saved if it was already drawn 
		if(EUClosePriceLineNumberMemory == 0)
		{
			s_UseTool EUClose;

			// EUClose.AddAsUserDrawnDrawing = 1;
			EUClose.AllowCopyToOtherCharts = 1;
			EUClose.AllowSaveToChartbook = 1;
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
				// if(sc.GetUserDrawnDrawingByLineNumber(sc.ChartNumber, (*p_LineNumbers)[i], Line))
				if(sc.GetACSDrawingByLineNumber(sc.ChartNumber, (*p_LineNumbers)[i], Line))
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
				if(sc.GetACSDrawingByLineNumber(sc.ChartNumber, (*p_LineNumbers)[i], Line))
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
		"<br> <br> It is necessary to be running Sierra Chart Version <strong>2644</strong> or higher to use this study. "
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
		sc.GraphName = "Telegram Chart & Drawing Alerts";
		sc.AutoLoop = 0;//Manual looping
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
		  sc.AddMessageToLog("User is not authorized to use this study",1);
		}
    	return;
	} 


	// Add support for Enabling and Disabling the study via the input
	if(Input_Enabled.GetBoolean() == 0)
	{
		// debug 
		// sc.AddMessageToLog("Error: Study is disabled.",1);
		
		return; // the study is turned off
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
// Last Updated: Tue Apr  9 16:52:10 CST 2024
// const int version = 20;
/*==========================================================================*/
bool IsStopOrder(int& OrderTypeAsInt )
{
	return(OrderTypeAsInt == SCT_ORDERTYPE_STOP
		|| OrderTypeAsInt == SCT_ORDERTYPE_STOP_LIMIT
		|| OrderTypeAsInt == SCT_ORDERTYPE_TRAILING_STOP
		|| OrderTypeAsInt == SCT_ORDERTYPE_TRAILING_STOP_LIMIT
		|| OrderTypeAsInt == SCT_ORDERTYPE_TRIGGERED_TRAILING_STOP_3_OFFSETS 
		|| OrderTypeAsInt == SCT_ORDERTYPE_TRIGGERED_TRAILING_STOP_LIMIT_3_OFFSETS
		|| OrderTypeAsInt == SCT_ORDERTYPE_STEP_TRAILING_STOP
		|| OrderTypeAsInt == SCT_ORDERTYPE_STEP_TRAILING_STOP_LIMIT
		|| OrderTypeAsInt == SCT_ORDERTYPE_TRIGGERED_STEP_TRAILING_STOP
		|| OrderTypeAsInt == SCT_ORDERTYPE_TRIGGERED_STEP_TRAILING_STOP_LIMIT
		|| OrderTypeAsInt == SCT_ORDERTYPE_BID_ASK_QUANTITY_TRIGGERED_STOP
		|| OrderTypeAsInt == SCT_ORDERTYPE_TRADE_VOLUME_TRIGGERED_STOP
		|| OrderTypeAsInt == SCT_ORDERTYPE_STOP_WITH_BID_ASK_TRIGGERING
		|| OrderTypeAsInt == SCT_ORDERTYPE_STOP_WITH_LAST_TRIGGERING
		|| OrderTypeAsInt == SCT_ORDERTYPE_STOP_LIMIT_CLIENT_SIDE
		|| OrderTypeAsInt == SCT_ORDERTYPE_TRIGGERED_STOP);
}

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
	  
		// if the order is NOT OPEN STATUS, continue to next order
		if (Order.OrderStatusCode !=  SCT_OSC_OPEN)
		{
			continue;
		}
		else if(Order.OrderStatusCode == SCT_OSC_OPEN) // order is open status
		{
			if(TargetOrStop == 1) // is target order code block
			{
				if( (Order.IsLimitOrder() && Order.IsAttachedOrder() )
					|| (Order.IsLimitOrder() && Order.OCOSiblingInternalOrderID != 0))
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

		sc.GraphName = "Move Closest Attached Order To Price";
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
		  sc.AddMessageToLog("You are not allowed to use this study",1);
		}
    	return;
	} 
	
	if (sc.MenuEventID != 0)	
	{
		if (!sc.ChartTradeModeEnabled)
		{
			sc.AddMessageToLog("Chart Trade Mode is not active. No action performed.", 1);
			return;
		}
	}
	
	// For safety we must never do any order management while historical data is being downloaded.
	if (sc.ChartIsDownloadingHistoricalData(sc.ChartNumber))
	{
		//sc.AddMessageToLog("Chart is downloading historical data, no order management allowed.", 1);
		return;
	}
	
	// This line allows the study to be used for live trading
	sc.SendOrdersToTradeService = !sc.GlobalTradeSimulationIsOn;

	// ACS Button Press (Move Closest Attached Order) 
	if (sc.MenuEventID == Input_MoveClosestAttachedOrderACSButtonNumber.GetInt() )
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
					// Check if there is an active order to prioritize 
					int OrderIDToModify = GetNearestActiveOrder(sc, 1, LastPrice, Order, 
						PositionIncludingWorkingOrders, ClosestOrderPrice);
					if(OrderIDToModify != 0)
					{
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
								&& Order.IsLimitOrder() 
								&& Order.ParentInternalOrderID != 0 )
								|| 
								(IsWorkingOrderStatus(Order.OrderStatusCode)
								&& Order.IsLimitOrder()
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
								&& Order.IsLimitOrder() 
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
	if (sc.MenuEventID == Input_MoveClosestUnattachedBuyOrderACSButtonNumber.GetInt() )
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
				// msg.Format("order symbol: %s ChartSymol: %s" , OrderDetails.Symbol.GetChars(), ChartSymbol.GetChars());
				// sc.AddMessageToLog(msg,1);
				Index++; // Increment the index for the next call to sc.GetOrderByIndex

				// Only account for BUY open unattached orders, and on the current chart, and must be a limit 
				if (OrderDetails.BuySell == 1 && OrderDetails.Symbol == ChartSymbol // same symbol (trade only)
					&& OrderDetails.OrderStatusCode ==  SCT_OSC_OPEN  // open
					&& OrderDetails.ParentInternalOrderID == 0 // not an attached order 
					&& OrderDetails.OCOSiblingInternalOrderID == 0) // not an OCO Order
					// && OrderDetails.IsLimitOrder()) // is a limit order type 
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
	  
	if (sc.MenuEventID == Input_MoveClosestUnattachedSellOrderACSButtonNumber.GetInt() )
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
					&& OrderDetails.OrderStatusCode ==  SCT_OSC_OPEN  // open
					&& OrderDetails.ParentInternalOrderID == 0 // not an attached order 
					&& OrderDetails.OCOSiblingInternalOrderID == 0 )// not an OCO Order 
					// && OrderDetails.IsLimitOrder()) // is a limit order type 
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


// Tue May  7 00:55:43 EDT 2024
// Proposed Change
// 1. Add the ability to move unnattached parent orders that are in an active state 
// This should be a separate ACS button than what is used to move target/stops 
//
// The IsAttachedOrder parameter should be false for a non attached order 
// The function will still work the same for stop and limit order depending on if above or below the current bid/ask
//
//-------------------------------------------------------------------------------------------------------
// Last Updated: Apr 9 12:03:40 CST 2024
//
// Proposed Changes 
//
// 1. Modify Pending Attached Orders when there is no position 
// 2. Add support for Only Modify Active attached Orders when in a position
//
// 3. Allow the user to Modify the Limit Order to be at a price where it is marketable. 
// If the trader is long they are permitted to move their limit target order to the current 
// bid price which will hit the bid with the limit order. 
// Previously you could put the limit target on the ask (for a sell profit
// taker) and on the bid (for a buy cover)
//
// 4. Do not submit order modification unless the modification price is
// difference than where the order already is
//
// DONE: DO NOT ALLOW ORDER TO BE MODIFIED IF ORDER PRICE IS THE SAME AS THE MODIFICATION PRICE 
// Notes: 


// Mon Apr  8 11:30:33 CST 2024
// 1. Create another ACS button which can handle moving orders that are pending 
// 2. Use the same button for all orders 
//
//-----------------------------------------------------
// 3. Allow the study to prioritize active orders 
// determine if any other orders exist at this price of the order obtained using sc.GetNearestTargetOrder
// This way we can select to modify the active order
//
//
// The While Loop gets each order 
// DONE: Checks if it is the correct order type in question
//
// DONE: If Study prioritizes Active Orders, it should first determine if the order obtained by the function is 
// NOT an Active Order. 
//
// Issue: The order obtained from GetClosestTargetOrder may be either
// Active
// Pending 
//
//--------------------
// If the order is PENDING or other status, it should then proceed with an
// operation to determine the closest ACTIVE attached order 
//------------------------
//
//

				/* 	//Get the internal order ID */
				/* 	int InternalOrderID = Order.InternalOrderID; */

				/* } */

// Func: (should know 1, if it's a target or a stop, the sc trade order object )
// The stop orders check for IsStopOrder() 
// Target Orders check for IsLimitOrder()
//
// Func returns an order ID
//
