#include <cstdlib>
#include <iostream>
#include <cmath>

// This program can calculate the difference of how much interest is earned
// using different maturity durations with different fixed income investment products. 
//
// This code is considered quick and dirty and should not considered
// high performance code by any means. There are things that may be improved upon,
// the main purpose of providing this is to accompany the video that it was used in. 
//
// This code does not support user input. The values that this program
// needs are hardcoded, and therefore to adjust them, you will need to change their
// values explicitly. We did consider creating a simple web app to perform what
// this code does, this may come in time. 
//
// 1. A GIC (Guaranteed Investment Certificate) often offered by Canadian Banks
// have payouts monthly as well as yearly, depending upon the term of the GIC.  
//
// 2. US T-BILLS can be purchased for durations up to 52 weeks with a trade fee
// at Interactive Brokers 
//
// 3. An additional example (not related to IBKR) where the investor recieves
// 85% of their profit which is paid out every day. The interest rate is higher
// and they can reinvest as often as they want without any added penalty. 
//
// 4. Calulate the blended interest paid on negative cash withdrawn from an IBKR trading
// account where there is are open T-Bill positions.

double US_TBills(float daily_int_rate, float daily_profit, float principal, int num_reinvests, float trade_fee)
{
	std::cout << "Example 2: The investor earns the same rate as Example 1 but "
	"is able to reinvest their earnings on a more frequent basis." <<
	std::endl << std::endl;

	std::cout << "This example subtracts a USD $" << trade_fee << 
	" trade fee everytime a reinvestment occurs." << std::endl;

	// SUPPORT FOR USING A DIFFERENT INTEREST RATE HERE (line 36 and 39 are also necessary) 
	double interest_rate = 0.0530;

	std::cout << "The Principal capital is: USD $" << principal << std::endl;
	std::cout << "The Interest rate is: " << interest_rate * 100 << "%" << std::endl;
	std::cout << "There will be " << num_reinvests << " reinvests in the 1 year period." << std::endl;
	
	// Get the daily interest rate 
	daily_int_rate = interest_rate / 365; 
	
	// How much profit is accrued per day 
	daily_profit = daily_int_rate * principal;

	// How many days the interest is accrued before being paid
	int num_days = round( 365.0 / static_cast<double>(num_reinvests) );

	// Remember the amount the investor started with before the compounding
	float starting_principal = principal; 

	// Variable for total profit 
	double total_profit = 0;

	// How many times we reinvest our principal
	for(int i = 0; i < num_reinvests; i++)
	{
		// Save accrued profit over a period of num_days 
		double accrued_profit = 0;

		for(int i = 0; i < num_days; i++)
		{
			// append the daily profit after each day passes 
			accrued_profit += daily_profit ; 

			// Total profit should only be used at the end 
			total_profit += daily_profit ;
		}

		// APPEND the accrued profit to our principal
		// subtract trade fee from the principal

		// Account for Trade Fee
		if(accrued_profit > 1007.50)
		{
			principal += 1000;
		}

		else
		{
			std::cout << std::endl << "Error: Investment amount too small or too many reinvests."
			" Profit must be greater than USD $1000 before reinvesting. "
			"Try increasing the principal investment or decreasing the number of reinvests." 
			<< std::endl;

			return 0; 
		}

		// Calculate the total profit earned after trade fees
		total_profit = total_profit - trade_fee;

		// SET the new daily profit amount of our new principal investment amount.
		daily_profit = daily_int_rate * principal;

		// only print the last iteration 
		/* if(i == num_reinvests -1) */
		/* { */
			std::cout << "After " << num_days * (i+1) << " days total profit is: " << total_profit << std::endl;
			std::cout << "New Principal is: " << principal << std::endl;
			std::cout << "Investor is now earning: $" << daily_profit << " per day." << std::endl;
		/* } */

	}

	std::cout << std::endl << "Example 2: Total Profit was $" << total_profit << std::endl;

	// Solving for the final rate of return 
	float final_percent_return = (total_profit / starting_principal) * 100;
	std::cout << "Return on investment was: " << final_percent_return << "%" << std::endl;
	
	std::cout << "/////////////////////////////////////////////////////////////////////////////" 
	<< std::endl << std::endl;

	return total_profit;
}

// This function emulates the rate of return accounting for the structure on
// some crypto exchanges where the lender of the asset keeps 85% of their profit. 
double USDT_Lending(float daily_int_rate, float daily_profit, float principal, int num_reinvests)
{
	std::cout << "Example 3: The investor earns a higher rate of return but the "
	"investor only recieves 85% of that rate of return. (15% fee)" <<
	std::endl << std::endl;

	// SUPPORT FOR USING A DIFFERENT INTEREST RATE HERE 
	double interest_rate = 0.15;
	
	std::cout << "The Interest rate is: " << interest_rate * 100 << "%" << std::endl;

	// Get the daily interest rate 
	daily_int_rate = interest_rate / 365; 
	
	// How much profit is accrued per day 
	daily_profit = daily_int_rate * principal;

	// Remember the amount the investor started with before the compounding
	float starting_principal = principal; 

	// Set Explicitly how many times we reinvest our principal
	num_reinvests = 365;
	std::cout << "There will be " << num_reinvests << " reinvests in the 1 year period." 
	<< std::endl << std::endl;

	// How many days the interest is accrued before we reinvest our new higher
	// principal 
	int num_days = round( 365.0 / static_cast<double>(num_reinvests) );

	// Variable for saving total profit 
	double total_profit = 0;

	// test calculate after fee daily_int rate
	double AfterFeeDailyIntRate = daily_int_rate - (daily_int_rate * 0.15);
	std::cout << "Daily Int rate : " << daily_int_rate << " After fee daily int rate : " 
	<< AfterFeeDailyIntRate << std::endl;

	// Accumulate profit and handle fees between each reinvest
	for(int i = 0; i < num_reinvests; i++)
	{
		// Initialize Accrued profit over a period of num days 
		double accrued_profit = 0;

		for(int i = 0; i < num_days; i++)
		{
			// APPEND the daily profit after each day passes 
			// Total profit should only be used at the end 
			
			// THIS IS for debug without the trade fee
			/* accrued_profit += daily_profit ; */ 
			/* total_profit += daily_profit ; */

			// INCLUDING SUBTRACTING THE 15% fee on Kucoin 
			double after_fee_profit = daily_profit - (daily_profit * 0.15);

			accrued_profit += after_fee_profit; 
			total_profit += after_fee_profit;
		}

		// APPEND the accrued profit to our principal
		principal += accrued_profit;

		// SET the new daily profit amount of our NEW principal investment amount.
		daily_profit = daily_int_rate * principal;


		// Used to explicitly print out amount of profit upon each reinvest
		/* if(i == num_reinvests -1) */ 
		/* { */
			/* std::cout << "After " << num_days * (i+1) << " days total profit is: " << total_profit << std::endl; */
			/* std::cout << "New Principal is: " << principal << std::endl; */
			/* std::cout << "Investor is now earning: $" << daily_profit << " per day." << std::endl; */
		/* } */

	}

	// print
	std::cout << std::endl << "Example 3: Total Profit was $" << total_profit << std::endl;

	// Solve for the rate of return 
	float final_percent_return = (total_profit / starting_principal) * 100;
	std::cout << "Return on investment was: " << final_percent_return << "%" << std::endl;
	std::cout << "/////////////////////////////////////////////////////////////////////////////" 
	<< std::endl << std::endl;

	return total_profit;
}

// This calculation is used to determine what is the optimal T-Bill maturity based upon the current
// interest rate and amount of principal capital 
void example_31(float daily_int_rate, float daily_profit, float principal, int num_reinvests)
{
	// For T-Bills on IBKR It is necessary to earn 1000 plus the trade fee
	// before reinvesting again or else the reinvestment will only hurt the
	// longer term rate of return. 
	
	// amount we need to earn
	float to_earn = 1007.50;

	// Solve for the number of days to earn this amount
	int days_to_earn_1k = round( to_earn / daily_profit);

	std::cout << "Minimum holding period to earn USD $" << to_earn << " is: " 
	<< days_to_earn_1k << std::endl;

}

// This function is used to calculate the blended interest rate that will be
// paid on negative cash assuming the investor also has a position in T-Bills. 
void example_4()
{
	std::cout << "Example 4: The investor buys bonds and takes a securities "
	"backed withdrawal out of their margin account. The investor pays the "
	"difference between how much the bonds are earning and how much the "
	"broker charges on their negative cash balance." << std::endl << std::endl;
	  
	double bond_purchase_size = 50000;
	std::cout << "The investor purchases $" << bond_purchase_size << 
	" worth of T-Bills or T-Bill ETFs." << std::endl;

	double example4_initial_rate = 0.0530;
	std::cout << "The investor is earning a blended rate of " << 
	example4_initial_rate * 100 << "% on this investment per year." << std::endl;

	double cash_withdrawal = 49000;
	std::cout << std::endl << "Investor now decides to withdraw currency from "
	"this account as a securities backend margin loan." << std::endl 
	<< "The investor withdraws $" << cash_withdrawal << " in cash." << std::endl;

	double example4_paid_rate = 0.0683;
	std::cout << "Investor will pay " << example4_paid_rate * 100 
	<< "% per year on this negative balance to their broker." << std::endl;

	// Solve for the blended rate 
	double account1_blended_rate = example4_initial_rate - example4_paid_rate; 
	std::cout << "Blended Rate of return in the Investors account is: " 
	<< account1_blended_rate * 100 << "%." << std::endl;

	return;
}

int main()
{
	// What is the yearly interest rate? 
	double interest_rate = 0.053;
	
	// Solve for the daily interest rate 
	double daily_int_rate = interest_rate / 365; 

	// What is the principal investment amount? 
	double principal = 100000;

	// Number of treasury purchases per year 
	int num_reinvests = 5;

	std::cout << "Example 1: The investor is only paid when their 1 year GIC "
	"reaches maturity, which is pretty common with Canadian banks." << std::endl << std::endl;

	std::cout << "Investor earns " << interest_rate * 100 << "% per year on $" 
	<< principal << std::endl;

	// Create a trade fee and subtract it from the total profit (used in example 2)
	double trade_fee = 7.5;

	// for reference and comparison
	double one_year_profit = (principal * interest_rate) ;

	// How much profit is accrued per day 
	double daily_profit = daily_int_rate * principal;
	
	std::cout << "With this principal and interest rate, we would earn: $" << daily_profit 
	<< " per day." << std::endl << std::endl; 

	// Print out: Example 1 has no trade fee
	std::cout << "Assuming a trade fee of: $0.00" << std::endl
	<< std::endl << "Example 1: Total profit would be $" << one_year_profit << std::endl;

	// Solve for the rate of return
	double final_percent_return = (one_year_profit / principal) * 100;
	std::cout << "Return on investment was: " << final_percent_return << "%" << std::endl;

	std::cout << "/////////////////////////////////////////////////////////////////////////////" << std::endl << std::endl;

	// Example 2: US T bills. This example will assume a higher payout
	// frequency.  Each time they recieve the payment, the investor starts a
	// new position but with the new principal.

	// EXAMPLE 2 (see US_TBills())
	double example2_profit = US_TBills(daily_int_rate, daily_profit, principal, num_reinvests, trade_fee);

	// Solve for the difference in percentage return between the two totals (Example1 vs Example2)
	double difference = example2_profit - one_year_profit;
	double avg = (example2_profit + one_year_profit) / 2;
	double percent_difference = (difference / avg) * 100;

	// Only print if example 2 result was not 0
	if(example2_profit != 0)
	{
		std::cout << "Example 2 earns: " << percent_difference << 
		"% more over a 1 year period" << std::endl << std::endl;
	}

	// Figure out how many days is necessary to make 1000
	example_31(daily_int_rate, daily_profit, principal, num_reinvests);

	// EXAMPLE 3 (see USDT_Lending())
	/* double example3_profit = USDT_Lending(daily_int_rate, daily_profit, principal, num_reinvests); */

	/* difference = example3_profit - one_year_profit; */
	/* avg = (example3_profit + one_year_profit) / 2; */
	/* percent_difference = (difference / avg) * 100; */

	/* std::cout << "Example 3 earns: " << percent_difference << */ 
	/* "% more over a 1 year period" << std::endl << std::endl; */

	// See example4()
	example_4();

	return 0;
}
