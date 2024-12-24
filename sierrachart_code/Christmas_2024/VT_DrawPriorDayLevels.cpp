#include "sierrachart.h"

SCDLLName("VerrilloTrading - Draw Prior Day Levels")
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

	// Enable Alert 
	SCInputRef i_EnableNewHighLowAlert = sc.Input[InputIndex++];
	SCInputRef i_AlertNumberForHighLowAlert = sc.Input[InputIndex++];

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

	// Persistent Variables 
	double& LowOfDayMemory = sc.GetPersistentDoubleFast(0);
	double& HighOfDayMemory = sc.GetPersistentDoubleFast(1);
	int& LowOfDayLineNumberMemory = sc.GetPersistentIntFast(2);
	int& HighOfDayLineNumberMemory = sc.GetPersistentIntFast(3);

	//Used for remembering if the drawings are hidden 
	int& HideLevels = sc.GetPersistentIntFast(4);

	// logging object 
	SCString msg;

	// Persistent Vector used to save what lines we have drawn, in order
	// to ensure correct cleanup behaviour
    std::vector<int>* p_LineNumbers = reinterpret_cast<std::vector<int>*>
		(sc.GetPersistentPointer(5));

	// First time initialization
    if (p_LineNumbers == NULL) 
	{
        // array of structs to hold our CSV labels for each price
        p_LineNumbers = new std::vector<int>;
        sc.SetPersistentPointer(5, p_LineNumbers);
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
			sc.SetPersistentPointer(5, NULL);
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

		return;
	}

	// Pressing this ACS Button will Draw or Delete and ReDraw all lines that
	// this study drew
	//
	// Also trigger a redraw if the input number of days was changed 
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
			int NumDrawings = 0;
			for(int i = 0; i < p_LineNumbers->size(); i++)
			{
				sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, (*p_LineNumbers)[i]);
				NumDrawings++;
			}

			// debug 
			// msg.Format("Num Drawings Deleted: %d",NumDrawings);
			// sc.AddMessageToLog(msg,1);

			// clear the vector 
			p_LineNumbers->clear();
		}

		//-----------------------------------------------------------
		// Set the Alert Only Once per Bar Option
		sc.AlertOnlyOncePerBar = i_AlertOnlyOncePerBar.GetBoolean();
		//-----------------------------------------------------------
		//
		// TODO: Handle the session times
		int IntradayStorageTimeUnit = sc.IntradayDataStorageTimeUnit;

		// msg.Format("Storage Time Unit: %d", IntradayStorageTimeUnit);
		// sc.AddMessageToLog(msg,1);

		// LOGIC FOR THE STUDY HERE 
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

			EveningStartTime = r_ChartSessionTimes.EveningStartTime;
			EveningEndTime = r_ChartSessionTimes.EveningEndTime;

			// ints
			UseEveningSessionTimes = r_ChartSessionTimes.UseEveningSessionTimes;
			NewBarAtSessionStart = r_ChartSessionTimes.NewBarAtSessionStart;
			LoadWeekendDataSetting = r_ChartSessionTimes.LoadWeekendDataSetting;
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
		// MidnightPriceTime.SetTimeHMS(4,59,59); // 5:00 AM UTC Time (4:59:59)
											   //
		// MidnightPriceTime.SetTimeHMS(00,00,00); // 5:00 AM UTC Time (4:59:59)
		MidnightPriceTime.SetTimeHMS(5,0,0); // 5:00 AM UTC Time (05:00:00)
												//
		CMECloseTime.SetTimeHMS(22,0,0); // 22:00 PM UTC Time 
		EUOpenTime.SetTimeHMS(8,0,0); // 8:00 AM UTC Time 
									  //
		// EUCloseTime.SetTimeHMS(15,59,59); // 16:30 PM UTC Time  (15:59:59
		EUCloseTime.SetTimeHMS(16,30,0); // 16:30 PM UTC Time  (15:59:59
		
		// UTC back to chart timezone 
		SCDateTimeMS ConvertedMidnightPrice = 
			sc.ConvertDateTimeToChartTimeZone(MidnightPriceTime, TIMEZONE_UTC);

		SCDateTimeMS ConvertedCMECloseTime = 
			sc.ConvertDateTimeToChartTimeZone(CMECloseTime, TIMEZONE_UTC);

		SCDateTimeMS ConvertedEUOpenTime = 
			sc.ConvertDateTimeToChartTimeZone(EUOpenTime, TIMEZONE_UTC);

		SCDateTimeMS ConvertedEUCloseTime = 
			sc.ConvertDateTimeToChartTimeZone(EUCloseTime, TIMEZONE_UTC);

		// Example Only: convert our date time to UTC if necessary
		// ConvertedCheckDateTime = sc.ConvertDateTimeFromChartTimeZone(
		// 	sc.BaseDateTimeIn[BarIndex], TIMEZONE_UTC);

		// adjusted for real-time or if using chart replay 
		// get the current day
		// if(sc.isreplayrunning() == 1)
		// {
		// 	scdatetime currentdatetime;
		// 	if (sc.isreplayrunning())
		// 		currentdatetime = sc.currentdatetimeforreplay;
		// 	else
		// 		currentdatetime = sc.currentsystemdatetime;

		// }
		//
		// This function is supposed to return the time of the chart so it
		// adjusts to chart replay too.
		SCDateTime CurrentTime = sc.GetCurrentDateTime();

		int CurrentDay = CurrentTime.GetDate();

		if(CurrentDay == 0)
		{
			// sc.AddMessageToLog("to solve a glitch with market replay",1);
			return;

			// When doing a chart replay and the replay is paused and the button is
			// pressed to go back a bar. the CurrentDay variable returns a 0 which
			// represents 0 days from epoch. When this vlue is returned we just
			// quit the study function until it returns a valid day number again. 
			//
			// Or else it results in many drawings being drawn incorrectly due to the Day being wrong.

		}

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
		int LastDrawnSettlementDate = 0; 
		bool SettlementPriceDrawn = 0;

		// Experimental Feature only to see if we can handle Intraday Storage Time units greater than 1 second. 
		// Still being developed
		// Mon Dec 23 09:56:31 PM UTC 2024
		if(IntradayStorageTimeUnit > 1) 
		{
			// Change to the session end time based on what the intraday storae time unit is set to. 
			// This is because people typically use 59:59 as session end times as recommended by Sierra Chart.
			// sc.AddMessageToLog("Storage Time unit greater than 1 second!",1);
			if(IntradayStorageTimeUnit == 2)
			{
				SessionEndTimeInSeconds -= 1;
			}
			else if(IntradayStorageTimeUnit == 4)
			{
				SessionEndTimeInSeconds -= 3;
			}
			else if(IntradayStorageTimeUnit == 5)
			{
				SessionEndTimeInSeconds -= 4;
			}
			else if(IntradayStorageTimeUnit == 10)
			{
				SessionEndTimeInSeconds -= 9;
			}
			else if(IntradayStorageTimeUnit == 30)
			{
				SessionEndTimeInSeconds -= 29;
			}
			else if(IntradayStorageTimeUnit == 60)
			{
				SessionEndTimeInSeconds -= 59;
			}
		}
		// -----------------------------

		// Most people use session end times that end with the number 9.
		// 15:59:59
		// Thing is if the istuis is set higher than 1 second. that time no longer
		// exists on the chart. 
		//
		// Midnight Price Setup
		// 
		// int MidnightPriceInSeconds = MidnightPriceTime.GetTimeInSeconds();
		int MidnightPriceInSeconds = ConvertedMidnightPrice.GetTimeInSeconds();
		int LastDrawnMidnightPriceDate = 0; 
		bool MidnightPriceDrawn = 0;

		//------------------------------------
		//  Same variables for CME Close Time 
		// 
		int CMECloseTimeInSeconds = ConvertedCMECloseTime.GetTimeInSeconds();
		int LastDrawnCMECloseDate = 0; 
		bool CMEClosePriceDrawn = 0;
		// ------------------------------------
		//
		//  Same Variables for EU Open time 
		//
		int EUOpenTimeInSeconds = ConvertedEUOpenTime.GetTimeInSeconds();
		int LastDrawnEUOpenDate = 0; 
		bool EUOpenPriceDrawn = 0;
		// ------------------------------------

		// Same Variables for EU Close time 
		//
		int EUCloseTimeInSeconds = ConvertedEUCloseTime.GetTimeInSeconds();
		int LastDrawnEUCloseDate = 0; 
		bool EUClosePriceDrawn = 0;
		// ------------------------------------
		
		// msg.Format("EU Close: %d, EU Open: %d, CMEClose: %d, Midnight: %d",
		// 	EUCloseTimeInSeconds, EUOpenTimeInSeconds, CMECloseTimeInSeconds, MidnightPriceInSeconds);
		// sc.AddMessageToLog(msg,1);

		// Variable used for passing in line names for line labels 
		SCString LineName = "";

		// Iterate from the starting point until end of the chart 
		for(int Index = HighLowStartDateTimeIndex; Index < sc.ArraySize; Index++)
		{
			// Get the date
			CurrentBarDate = sc.BaseDateTimeIn.DateAt(Index);

			// draw only one drawing per day 
			if(CurrentBarDate > HighLowStartDate)
			{
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


			// important debug 
			// msg.Format("SessionEndTimeInSeconds: %d CurrentBarStartTime: %d CurrentBarEndTime: %d", 
			// 	SessionEndTimeInSeconds, CurrentBarStartTime,CurrentBarEndTime);
			// sc.AddMessageToLog(msg,1);

			// msg.Format("CurrentBarStartTime: %d, CurrentBarEndTime: %d",
			// 	CurrentBarStartTime, CurrentBarEndTime);
			// sc.AddMessageToLog(msg,1);

			// ----------------------------------------------------------------------
			// LOGIC FOR SETTLEMENT PRICE 
			if(i_DrawSessionClose.GetInt() == 1)
			{
				// if we are on a new day and settlement price has 
				// been drawn, we need to reset this variable,
				// so we can again be looking for the settlement price on the new day 
				if(CurrentBarDate > LastDrawnSettlementDate && SettlementPriceDrawn == 1)
				{
					SettlementPriceDrawn = 0;
					// sc.AddMessageToLog("we get here reset SettlementPriceDrawn",1);
				}

				// only looks for the settlemnt price if the settlement price 
				// on the day has not already been drawn 
				if(SettlementPriceDrawn == 0)
				{
					// Check if Session End Time exists within the boundaries of our bar 
					bool IsWithinBarTime = 
					(SessionEndTimeInSeconds >= CurrentBarStartTime && 
					 SessionEndTimeInSeconds <= CurrentBarEndTime);

					if(IsWithinBarTime)
					{
						// we're on current settlement bar

						// Draw it using our regular drawing tool 
						s_UseTool SettlementLine;

						// SettlementLine.AddAsUserDrawnDrawing = 1;
						SettlementLine.AllowCopyToOtherCharts = 1;
						SettlementLine.AllowSaveToChartbook = 1;
						SettlementLine.ChartNumber = sc.ChartNumber;
						SettlementLine.LineNumber = -1;  

						SettlementLine.DrawingType = DRAWING_HORIZONTAL_LINE_NON_EXTENDED;
						SettlementLine.LineWidth = 4;
						SettlementLine.LineStyle = LINESTYLE_DASHDOTDOT;
						SettlementLine.DisplayHorizontalLineValue = 1;

						// Price Value 
						SettlementLine.BeginValue = sc.Close[Index];
						SettlementLine.EndValue = SettlementLine.BeginValue;

						// GET BAR INDEX 
						SettlementLine.BeginIndex = Index;
						SettlementLine.EndIndex = sc.ArraySize-1 + sc.NumberOfForwardColumns;

						SettlementLine.AddMethod = UTAM_ADD_OR_ADJUST;
						SettlementLine.Region = sc.GraphRegion;

						SettlementLine.Color = i_SettlementPriceColor.GetColor();

						// Code used for Fixing Line Labels 
						
						// Convert bar index into a date time variable, number of days since 1899
						SCDateTime DrawingStartDate = sc.BaseDateTimeIn[Index].GetDate();
						
						// get the year month and day from SCDateTimeVariable
						int Year, Month, Day;
						DrawingStartDate.GetDateYMD(Year,Month,Day);

						// Assign the line name for this drawing and pass to function
						LineName = "";

						// Call fix line labels 
						VT_FixLineLabels(Month, Day, SettlementLine, msg, Last, LineName);

						sc.UseTool(SettlementLine);
						p_LineNumbers->push_back(SettlementLine.LineNumber);

						//-----------------------------------
						// ESSENTIAL RESET VARIABLES 
						// Set variables to ensure we only get here again on the next day 
						LastDrawnSettlementDate = sc.BaseDateTimeIn[Index].GetDate();
						SettlementPriceDrawn = 1;
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
						// msg.Format("MidnightPriceInSeconds: %d, CurrentBarStartTime: %d, CurrentBarEndTime: %d",
						// 	MidnightPriceInSeconds, CurrentBarStartTime, CurrentBarEndTime);
						// sc.AddMessageToLog(msg,1);

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
						SessionOpen.BeginValue = sc.Close[Index];
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
				if(CMEClosePriceDrawn == 0)
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

						sc.UseTool(CMESessionClose);
						p_LineNumbers->push_back(CMESessionClose.LineNumber);

						//-----------------------------------
						// ESSENTIAL RESET VARIABLES 
						// Set variables to ensure we only get here again on the next day 
						LastDrawnCMECloseDate = sc.BaseDateTimeIn[Index].GetDate();
						CMEClosePriceDrawn = 1;
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
						EUOpen.BeginValue = sc.Open[Index];
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

						sc.UseTool(EUOpen);
						p_LineNumbers->push_back(EUOpen.LineNumber);

						//-----------------------------------
						// ESSENTIAL RESET VARIABLES 
						// Set variables to ensure we only get here again on the next day 
						LastDrawnEUOpenDate = sc.BaseDateTimeIn[Index].GetDate();
						EUOpenPriceDrawn = 1;
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
						EUClose.LineWidth = i_EuropeOpenLineWidth.GetInt();
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

						sc.UseTool(EUClose);
						p_LineNumbers->push_back(EUClose.LineNumber);

						//-----------------------------------
						// ESSENTIAL RESET VARIABLES 
						// Set variables to ensure we only get here again on the next day 
						// LastDrawnEUCloseDate = sc.BaseDateTimeIn[Index].GetDate();
						LastDrawnCMECloseDate = sc.BaseDateTimeIn[Index].GetDate();
						EUClosePriceDrawn = 1;
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
		LineName = "Low";

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
		LineName = "High";

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
