#include "sierrachart.h"

SCDLLName("VerrilloTrading - Move Closest OCO To Price")

/*==========================================================================*/
SCSFExport scsf_ClosestOrderToPrice(SCStudyInterfaceRef sc)
{
	SCInputRef Input_ControlBarButtonNumberForEnableDisable = sc.Input[0];
	
	if(sc.SetDefaults)
	{
		
		sc.StudyDescription = "This DLL was compiled in version 2564 of Sierra Chart on 2023-11-14."
		" <br> <br> This is a custom study that performs a feature that we originally discovered in the platform "
		"Jigsaw Daytradr. It moves the closest OCO order (either the target or stop) to the price you hover the "
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

		" <br> <br> For any issues or inquiries, send them to support@verrillotrading.com"

		"<br> <br> Thank you and happy trading."
		"<br> <br> - VerrilloTrading, Content Creator - Programmer";

		sc.GraphName = "Move Closest Attached Order To Price";
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.GraphRegion = 0;
		sc.AutoLoop		= 0;
		
		sc.ReceivePointerEvents = ACS_RECEIVE_POINTER_EVENTS_WHEN_ACS_BUTTON_ENABLED;
		
		Input_ControlBarButtonNumberForEnableDisable.Name = "ACS Control Bar Button #";
		Input_ControlBarButtonNumberForEnableDisable.SetInt(25);
		Input_ControlBarButtonNumberForEnableDisable.SetIntLimits(1, MAX_ACS_CONTROL_BAR_BUTTONS);
		Input_ControlBarButtonNumberForEnableDisable.SetDescription
		("This is the Advanced Custom Study Button that will be used to move the nearest attached target or stop order.");
		
		return;
	}
	
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
	
	//For safety we must never do any order management while historical data is being downloaded.
	if (sc.ChartIsDownloadingHistoricalData(sc.ChartNumber))
	{
		//sc.AddMessageToLog("Chart is downloading historical data, no order management allowed.", 1);
		return;
	}
	
	// Get position data
	// Right now the study uses open position data but it could be changed to use position quantity with working orders
	// to allow for order modification before the parent order has filled
	s_SCPositionData PositionData;
	sc.GetTradePosition(PositionData);
	int Position = PositionData.PositionQuantity;
	double LastPrice = sc.GetLastPriceForTrading();
	
	// This line allows the study to be used for live trading
	sc.SendOrdersToTradeService = !sc.GlobalTradeSimulationIsOn;
	
	// Button Press when in a long position and cursor is above the last traded price, Move closest target order
	if (sc.MenuEventID == Input_ControlBarButtonNumberForEnableDisable.GetInt() )
	{
		// For this button press we will reset the button back to off state since pressing the button executes a function
		const int ButtonState = (sc.PointerEventType == SC_ACS_BUTTON_ON) ? 1 : 0;
		if (ButtonState == 1)
		{
			sc.SetCustomStudyControlBarButtonEnable(Input_ControlBarButtonNumberForEnableDisable.GetInt(), 0);
		}
		
		// Button press when in a long position and cursor is above the last traded price, Move closest target order
		s_SCTradeOrder LongClosestTarget;
		if (Position >= 1 && sc.ChartTradingOrderPrice >= LastPrice)
		{
			if (sc.GetNearestTargetOrder(LongClosestTarget) )
			{
				s_SCNewOrder ModifyTarget;
				ModifyTarget.InternalOrderID = LongClosestTarget.InternalOrderID;
				ModifyTarget.Price1 = sc.ChartTradingOrderPrice;
				sc.ModifyOrder(ModifyTarget);
			}
		}
		
		// Button Press When in a long position and cursor is below the last traded price, Move closest stop order
		s_SCTradeOrder LongClosestStop;
		if (Position >= 1 && sc.ChartTradingOrderPrice < LastPrice)
		{
			if (sc.GetNearestStopOrder(LongClosestStop))
			{
				s_SCNewOrder ModifyStop;
				ModifyStop.InternalOrderID = LongClosestStop.InternalOrderID;
				ModifyStop.Price1 = sc.ChartTradingOrderPrice;
				sc.ModifyOrder(ModifyStop);
			}
		}
		// Button Press when in a short position and cursor is below the last traded price, Move closest target order
		s_SCTradeOrder ShortClosestTarget;
		if (Position <= -1 && sc.ChartTradingOrderPrice <= LastPrice)
		{
			if (sc.GetNearestTargetOrder(ShortClosestTarget))
			{
				s_SCNewOrder ModifyTarget;
				ModifyTarget.InternalOrderID = ShortClosestTarget.InternalOrderID;
				ModifyTarget.Price1 = sc.ChartTradingOrderPrice;
				sc.ModifyOrder(ModifyTarget);
			}
		}
		
		// Button Press when in a short position and cursor is above the last traded price, Move closest stop order
		s_SCTradeOrder ShortClosestStop;
		if (Position <= -1 && sc.ChartTradingOrderPrice > LastPrice)
		{
			if (sc.GetNearestStopOrder(ShortClosestStop))
			{
				s_SCNewOrder ModifyStop;
				ModifyStop.InternalOrderID = ShortClosestStop.InternalOrderID;
				ModifyStop.Price1 = sc.ChartTradingOrderPrice;
				sc.ModifyOrder(ModifyStop);
			}
		}
	}
} 
