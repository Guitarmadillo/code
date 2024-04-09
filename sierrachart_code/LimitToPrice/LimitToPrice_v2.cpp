#include "sierrachart.h"

SCDLLName("VerrilloTrading - Move Closest OCO To Price")
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
int GetNearestActiveOrder(SCStudyInterfaceRef& sc, int TargetOrStop, double& LastPrice, s_SCTradeOrder& Order, double& Position, double& ClosestOrderPrice)
{
	if(Position != 0 && Order.OrderStatusCode != SCT_OSC_OPEN)
	{
		// FIND AN OPEN ORDER INSTEAD 
		
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
			else // order is open status
			{
				if(TargetOrStop == 1) // is target order code block
				{
					if(Order.IsLimitOrder() 
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
						// order is not a limit order or is not an attached order 
						continue;
					}

				}
				else if(TargetOrStop == 2) // is stop order code block
				{
					if(IsStopOrder(Order.OrderTypeAsInt) 
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
						// order is not a limit order or is not an attached order 
						continue;
					}
				}
			}
		}

		return OrderIDToModify;
	}
	else
	{
		return 0;
	}

	return 0;
}
SCSFExport scsf_ClosestOrderToPrice(SCStudyInterfaceRef sc)
{
	SCInputRef Input_ControlBarButtonNumberForEnableDisable = sc.Input[0];
	SCInputRef Input_PrioritizeActiveOrders = sc.Input[1];
	
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
		
		Input_ControlBarButtonNumberForEnableDisable.Name = "ACS Control Bar Button #";
		Input_ControlBarButtonNumberForEnableDisable.SetInt(25);
		Input_ControlBarButtonNumberForEnableDisable.SetIntLimits(1, MAX_ACS_CONTROL_BAR_BUTTONS);
		Input_ControlBarButtonNumberForEnableDisable.SetDescription
		("This is the Advanced Custom Study Button that will be used to move the nearest attached target or stop order.");
		
		Input_PrioritizeActiveOrders.Name = "Always Prioritize Active Attached Orders";
		Input_PrioritizeActiveOrders.SetYesNo(1);
		Input_PrioritizeActiveOrders.SetDescription("This Input allows the user to enable only moving attached orders that have the active status, therefore ignoring orders that are pending.");
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
	

	// ACS Button Press 
	if (sc.MenuEventID == Input_ControlBarButtonNumberForEnableDisable.GetInt() )
	{
		// For this button press we will reset the button back to off state
		// since pressing the button executes a function
		const int ButtonState = (sc.PointerEventType == SC_ACS_BUTTON_ON) ? 1 : 0;
		if (ButtonState == 1)
		{
			sc.SetCustomStudyControlBarButtonEnable(Input_ControlBarButtonNumberForEnableDisable.GetInt(), 0);
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

		// This line allows the study to be used for live trading
		sc.SendOrdersToTradeService = !sc.GlobalTradeSimulationIsOn;
		
		// Single SC Trade Order Object used for retrieving both Targets and Stops 
		s_SCTradeOrder Order; 	

		// HANDLE EVENTS 
		
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
					int OrderIDToModify = GetNearestActiveOrder(sc, 1, LastPrice, Order, 
						Position, ClosestOrderPrice);
					if(OrderIDToModify != 0)
					{
						// do not commit the modification if the modification
						// price is same as the order price 
						if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
						{
							s_SCNewOrder ModifyTarget;
							ModifyTarget.InternalOrderID = OrderIDToModify;
							ModifyTarget.Price1 = sc.ChartTradingOrderPrice;
							sc.ModifyOrder(ModifyTarget);
							return;
						}
					}
					else // default behaviour (order not active order status)
					{
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
					int OrderIDToModify = GetNearestActiveOrder(sc, 2, LastPrice, Order, Position, 
							ClosestOrderPrice);
					if(OrderIDToModify != 0)
					{
						if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
						{
							s_SCNewOrder ModifyStop;
							ModifyStop.InternalOrderID = OrderIDToModify;
							ModifyStop.Price1 = sc.ChartTradingOrderPrice;
							sc.ModifyOrder(ModifyStop);
							return;
						}
					}
					else // default behaviour (order not active order status)
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
				else // user comppletely disabled the setting 
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
					int OrderIDToModify = GetNearestActiveOrder(sc, 1, LastPrice, Order, Position, 
					ClosestOrderPrice);
					if(OrderIDToModify != 0)
					{
						if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
						{
							s_SCNewOrder ModifyTarget;
							ModifyTarget.InternalOrderID = OrderIDToModify;
							ModifyTarget.Price1 = sc.ChartTradingOrderPrice;
							sc.ModifyOrder(ModifyTarget);
							return;
						}
					}
					else // default behaviour (order not active order status)
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
					int OrderIDToModify = GetNearestActiveOrder(sc, 2, LastPrice, Order, Position, 
					ClosestOrderPrice);
					if(OrderIDToModify != 0)
					{
						if(ClosestOrderPrice != sc.ChartTradingOrderPrice)
						{
							s_SCNewOrder ModifyStop;
							ModifyStop.InternalOrderID = OrderIDToModify;
							ModifyStop.Price1 = sc.ChartTradingOrderPrice;
							sc.ModifyOrder(ModifyStop);
							return;
						}
					}
					else // default behaviour (order not active order status)
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
} 
