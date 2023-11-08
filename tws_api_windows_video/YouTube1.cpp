/*========================================================================
 * TwsApiC++ - Demonstration program
 *========================================================================
 */

// This code was put together in February 2023 and it has not been revised since. 
// This source code is meant to serve as a general example for how to use
// various EWrapper methods in the TWS API. This file combines some source code
// from examples on the JanBoonen repository which is why there is stylistic
// differences in some of the code. This file has only been formatted for
// readability using vim but nothing in the code has been changed so that it
// matches the code that was used in the demonstration video. 
//
// This source file shows a few different EWrapper methods which are not meant to be 
// used all at the same time. You will need to comment and uncomment the appropriate 
// things in the main function. 
//
// Not strictly necessary but helpful: 
// I recommend vim as the text editor and
// installing vim-commentary plugin which is super useful:
// https://github.com/tpope/vim-commentary
//
// Video by VerrilloTrading (VT) that uses this source file for the demonstation: 
// https://youtu.be/5moyX0qwkCA
//
// Include the TWS C++ API Header file
#include "TwsApiL0.h"

///Faster: Check spelling of parameter at compile time instead of runtime.
#include "TwsApiDefs.h"

// Used for debugging and formatting text strings
#include <boost/format.hpp>
#include <algorithm>

using namespace TwsApi; // for TwsApiDefs.h

// The following macro is from JanBoonen source code to use a Sleep() function
// for debugging.  It makes sure that the Sleep() function works on Windows and
// Linux. I am not sure if it accounts for Mac. 

#ifdef WIN32
	#include <windows.h>		// Sleep(), in miliseconds
	#include <iostream>
	#include <process.h>
	#define CurrentThreadId GetCurrentThreadId
#else
	#include <unistd.h>			// usleep(), in microseconds
	#define Sleep( m ) usleep( m*1000 )
	#include <pthread.h>
	#define CurrentThreadId pthread_self
#endif

#define PrintProcessId printf("%ld  ", CurrentThreadId() )

///Advantages of deriving from EWrapperL0
/// Faster: implement only the methods you need.
/// Safe: receive notification of methods called you didn't implement
/// Robust: exceptions thrown by your code are catched safely and notified to you
class YourEWrapper: public EWrapperL0 {

  public:
  bool m_Done, m_ErrorForRequest;
  bool notDone( void ) { return !(m_Done || m_ErrorForRequest); }
 
  ///Easier: The EReader calls all methods automatically(optional)
  YourEWrapper( bool runEReader = true ): EWrapperL0( runEReader ) {
    m_Done            = false;
    m_ErrorForRequest = false;
  }

  ///Methods winError & error print the errors reported by IB TWS
  virtual void winError( const IBString& str, int lastError ) {
    fprintf( stderr, "WinError: %d = %s\n", lastError, (const char*)str );
    m_ErrorForRequest = true;
  }

  virtual void error( const int id, const int errorCode, const IBString errorString ) {
    fprintf( stderr, "Error for id=%d: %d = %s\n"
	   , id, errorCode, (const char*)errorString );
    m_ErrorForRequest = (id > 0);    // id == -1 are 'system' messages, not for user requests
  }
  
  virtual void connectionClosed()
	{
		PrintProcessId,printf( "Connection Closed\n");
	}

  ///Safer: uncatched exceptions are catched before they reach the IB library code.
  ///       The Id is tickerId, orderId, or reqId, or -1 when no id known
  virtual void OnCatch( const char* MethodName, const long Id ) {
    fprintf( stderr, "*** Catch in EWrapper::%s( Id=%ld, ...) \n", MethodName, Id );
  }

 	// class member variable to save the position quantity 
    int MyPosition = 0; 						

	// maybe use a vector or array to push back symbols along with position sizes to keep track of multiple positions. 
	IBString PositionSymbol = ""; 								  

	// Position updates
	virtual void position ( const IBString& account, const Contract& contract, int position, double avgCost) 	
	{
		// Do not include Forex Positions 
	 	if(contract.symbol != "USD" && contract.symbol != "EUR" && contract.symbol != "CAD") 		
		{
			MyPosition = position; // remember the position quantity. Append it to array 
			if(MyPosition != 0) // If there is a position, remember the symbol, append it to array. 
			{
				PositionSymbol = "AAPL"; // testing only one symbol for now. 
			}
			else
			{
			 	// if position closed, forget the symbol. 
				PositionSymbol = "";
			}

			std::cout <<boost::format("Position Update. Acct: %1% Symbol: %2% Position is: %3%\n")
			%account%PositionSymbol%MyPosition;
		} 
	}
	virtual void positionEnd()  
	{
		// did not finish testing this method 
		
	}

  	// used to save contract details end response. 
  	int Req4 = 0; 	
	
	virtual void contractDetailsEnd(int reqId)
	{
		if(reqId == 4) // debug to confirm our request Id is valid. 
		{
			Req4 = 1; 
			printf("contract details end\n");
		}
	}
	
	IBString Expiry;
	std::vector<double>Strikes; 
  	virtual void contractDetails ( int reqId, const ContractDetails& contractDetails )
	{
		Contract C = contractDetails.summary;

		// Do not push back after we recieve the contract details end response. 
		if(C.strike != 0 && Req4 == 0) 		
		{
			Strikes.push_back(C.strike);
		}
	}

	// Used to get Options Live streaming data. 
    virtual void tickOptionComputation ( TickerId tickerId, TickType tickType, double impliedVol, 
	double delta, double optPrice, double pvDividend ,double gamma, double vega, double theta, double undPrice)
	{	
		std::cout << boost::format("TickerId: %1% TickType: %2% UndPrice: %3% ImpliedVol: %4% OptPrice: "
		"%5% Delta: %6% Gamma: %7% Vega: %8% Theta: %9% \n")
		%tickerId%*(TickTypes::ENUMS)tickType%undPrice%impliedVol%optPrice%delta%gamma%vega%theta; 
		
	}
	
	virtual void connectionOpened( void )
	{
		PrintProcessId,printf( "Connection Opened\n");
	}

	virtual void checkMessagesStarted( void )
	{
		PrintProcessId,printf( ">>> checkMessagesStarted\n");
	}

	virtual void checkMessagesStopped( void )
	{
		PrintProcessId,printf( "<<< checkMessagesStopped\n");
	}
};


//========================================================================
// main entry
//========================================================================
int main(void) 
{
    ///Easier: just allocate your wrapper and instantiate the EClientL0 with it.
    YourEWrapper  YW( false );                      // false: not using the EReader
    EClientL0*    EC = EClientL0::New( &YW );

    ///Easier: All structures defined by IB are immediately available.
    ///Faster: Use of TwsApiDefs.h codes check typos at compile time
 /*    Contract          C;
    C.symbol          = "MSFT";
    C.secType         = *SecType::STK;              // instead of "STK"
    C.currency        = "USD";
    C.exchange        = *Exchange::IB_SMART;        // instead of "SMART"
    C.primaryExchange = *Exchange::AMEX; */            // instead of "AMEX"

    ///Easier: Use of TwsApiDefs.h makes code self explanatory,
    ///        i.e. UseRTH::OnlyRegularTradingData instead of true or 1.

    if( EC->eConnect( "", 7496, 0 ) )
	{
		
		/* EC->reqPositions (); */
 		
      /* EC->reqHistoricalData */
      /*   ( 20 */
      /*   , C */
      /*   , EndDateTime(2013,02,20)                   // 20130220 00:00:00 */
      /*   , DurationStr(1, *DurationHorizon::Months)  // instead of "1 M" */
      /*   , *BarSizeSetting::_1_day                   // instead of "1 day" */
      /*   , *WhatToShow::TRADES                       // instead of "TRADES" */
      /*   , UseRTH::OnlyRegularTradingData            // instead of 1 */
      /*   , FormatDate::AsDate                        // instead of 1 */
      /*   ); */  
		
	
 		//int tickerId = 0;
 	 	/* Contract c; // struct that contains contract parameters that will be set. */ 
		/* c.symbol = "AAPL"; */
		/* c.secType = *SecType::OPT; */
		/* c.right = *ContractRight::CALL; */
		/* c.expiry = "20230224"; // t's year month day format. */
		/* c.currency = "USD"; */
		/* c.exchange = *Exchange::IB_SMART; */
		/* c.multiplier = "100"; */
		/* c.includeExpired = false; */
		/* c.strike = 155; //double type. */
		
		/* EC->reqMktData ( 5 , c , ""  , false ); //requesting market data for each option. */ 
		
		// request contract details test 
		
 		/* Contract es; */
		/* es.localSymbol = "MESH3"; */
		/* //es.symbol = "MES"; */
		/* es.secType = *SecType::FUT; */
		/* //es.expiry = "20230317"; */
		/* es.currency = "USD"; */
		/* es.exchange = *Exchange::CME; */
	
	 	Contract C;
		C.symbol	= "AAPL";
		C.secType	= *SecType::OPT;		//"STK"
		C.currency	= "USD";
		C.exchange	= *Exchange::IB_SMART;	//"SMART";
		//C.primaryExchange = *Exchange::ISLAND;             // instead of "AMEX" */
 	 	C.right = *ContractRight::CALL;
		C.multiplier = "100";
		//C.includeExpired = false;	
		//C.expiry = "20230224";
		EC->reqContractDetails( 4, C );    

		// This is an example that will not work when there are odd or inbetween strike prices. 
	 	/* for(int i = 0; i < NumberOfStrikes; i++) */ 		
		/* { */
			/* c.strike = StartingStrikePrice; //double type. */ 
			/* EC->reqMktData ( tickerid , c , ""  , false ); //requesting market data for each option. */ 
			/* StartingStrikePrice++; //increment the strike price. */ 
			/* tickerid++; */
		/* } */  
		
		// This int is being used as a boolean to remember the state of something
		int ReqOptions = 0;

		// create a vector of pairs of doubles
		std::vector<std::pair<double, double>> differences; // pair of (difference, strikeprice)
															
		// Explicitly set the price just for EXAMPLE, in practice you would get
		// the price using an EWrapper call like ReqMktData().
		float UnderlyingPrice = 155; 

      	//	Easier: Call checkMessages() in a loop. No need to wait between two calls.
		while( YW.notDone() ) 
		{
			EC->checkMessages();
		
			// if we are done getting the option contracts 
			if(YW.Req4 == 1) 
			{
				if(ReqOptions == 0) // Get number of strikes and call mkt data for those strikes. 
				{
					// sort vector from lowest to highest. 
					std::sort(YW.Strikes.begin(), YW.Strikes.end()); 					

					// Step 1: Calculate the absolute difference between the value and each element in the vector.
					for (const auto& strike : YW.Strikes)  					
					{
						double diff = std::abs(strike - UnderlyingPrice);
						differences.push_back(std::make_pair(diff, strike));
					}
					
					// sort by the difference value
					std::sort(differences.begin(), differences.end());

					// get the closest 20 values into a vector 
					std::vector<double> closest_numbers;
					for (int i = 0; i < 20; i++) 
					{
						closest_numbers.push_back(differences[i].second);
					} 
				
					// Printing and Debugging  
				  	/* std::cout<<boost::format("The 20 closest numbers to %f are: ")%UnderlyingPrice; */
					/* for (const auto& number : closest_numbers) */ 
					/* { */
						/* std::cout << number << " "; // all on the same line. */ 
					/* } */

					/* std::cout << std::endl; // put a newline after it's done. */ 
					
				 	// Print out the entire vector. 
				 	/* for(int Index = 0; Index < YW.Strikes.size(); Index++) */ 
					/* { */
						/* std::cout << boost::format("Strike: %f \n")%YW.Strikes[Index]; */
					/* } */   

					// increment a ticker ID to request market data for each contract. 
					// Ticker IDs need to be handled properly in TWS API Programs. 
					// You should make sure every request for market data in
					// your application has a unique ticker ID.
					
 			  	  	int tickerID = 30; 					
					for(int Index = 0; Index < closest_numbers.size(); Index++)
					{
						// Getting strike prices from the vector. 
						C.strike = closest_numbers[Index]; 

						// Requesting market data for each contract. 
						EC->reqMktData ( tickerID , C , ""  , false ); 

						// increment the ticker ID
						tickerID++;
					} 

					// Inform the program that we have made the request and it should not go through again
					// int being used as a bool should just be a bool 
					ReqOptions = 1; 				
				} 

				// Debug: Prints out how many contracts we retrieved. 
				std::cout << boost::format("vector size: %1% \n")%YW.Strikes.size();
				printf("all strikes retrieved, going to sleep\n"); 
				Sleep(10000); // update interval for debugging
			}
		}
    }
    EC->eDisconnect();
    delete EC;
}
