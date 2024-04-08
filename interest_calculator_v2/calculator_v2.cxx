// Last Updated: Sun Apr  7 19:22:45 CST 2024

#include "calculator_v2.h"
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <boost/format.hpp>
#include <array>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/fl_ask.H>

// This version is refactored in a C++ style using classes for each window. This application is the first program I've written using fltk 1.4 and fluid.

// The goal of the application is allow the user to get a yearly average rate
// of return that takes multiple currency positions as well as the proceeds of 
// short currency positions into account.

// More documentation will follow.

static int version_num = 22;
// sqlite objects
static char* err; sqlite3* db; const char* dbPath = "config.db"; sqlite3_stmt* stmt;

Fl_Menu_Item results_window::menu_C1_currencychoice[] = {
 {"Currency 1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"Currency 2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"Currency 3", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Menu_Item results_window::menu_C2_currencychoice[] = {
 {"Currency 1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"Currency 2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"Currency 3", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Menu_Item results_window::menu_C3_currencychoice[] = {
 {"Currency 1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"Currency 2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"Currency 3", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Menu_Item results_window::menu_C4_currencychoice[] = {
 {"Currency 1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"Currency 2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"Currency 3", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

results_window::results_window(int X, int Y, int W, int H, const char *L) :
  Fl_Window(X, Y, W, H, L)
{
  _results_window();
}

results_window::results_window(int W, int H, const char *L) :
  Fl_Window(0, 0, W, H, L)
{
  clear_flag(16);
  _results_window();
}

results_window::results_window() :
  Fl_Window(0, 0, 471, 727, 0)
{
  clear_flag(16);
  _results_window();
}

void results_window::_results_window() {
  this->box(FL_FLAT_BOX);
  this->color(FL_BACKGROUND_COLOR);
  this->selection_color(FL_BACKGROUND_COLOR);
  this->labeltype(FL_NO_LABEL);
  this->labelfont(0);
  this->labelsize(14);
  this->labelcolor(FL_FOREGROUND_COLOR);
  this->callback((Fl_Callback*)WinQuit_CB, (void*)(this));
  this->align(Fl_Align(FL_ALIGN_TOP));
  this->when(FL_WHEN_RELEASE);
  { Fl_Flex* o = new Fl_Flex(14, 31, 430, 107);
    o->type(1);
    { c1_output = new Fl_Text_Display(14, 31, 215, 107, "Currency 1");
      c1_output->box(FL_OXY_UP_FRAME);
      c1_output->labelcolor((Fl_Color)96);
      c1_output->textcolor(FL_GRAY0);
      //c1_output->wrap_mode(4,0);
    } // Fl_Text_Display* c1_output
    { c2_output = new Fl_Text_Display(229, 31, 215, 107, "Currency 2");
      c2_output->box(FL_OXY_UP_FRAME);
      c2_output->textcolor(FL_GRAY0);
      // c2_output->wrap_mode(4,0);
    } // Fl_Text_Display* c2_output
    o->fixed(o->child(0), 215);
    o->fixed(o->child(1), 215);
    o->end();
  } // Fl_Flex* o
  { Fl_Flex* o = new Fl_Flex(16, 181, 430, 107);
    o->type(1);
    { c3_output = new Fl_Text_Display(16, 181, 215, 107, "Currency 3");
      c3_output->box(FL_OXY_UP_FRAME);
      c3_output->textcolor(FL_GRAY0);
      // c3_output->wrap_mode(4,0);
    } // Fl_Text_Display* c3_output
    { tbill_output = new Fl_Text_Display(231, 181, 215, 107, "Fixed Income");
      tbill_output->box(FL_OXY_UP_FRAME);
      tbill_output->textcolor(FL_GRAY0);
      // tbill_output->wrap_mode(4,0);
    } // Fl_Text_Display* tbill_output
    o->fixed(o->child(0), 215);
    o->fixed(o->child(1), 215);
    o->end();
  } // Fl_Flex* o
  { blended_output = new Fl_Text_Display(19, 561, 363, 151, "Total Return / Cost to Cover");
    blended_output->box(FL_SHADOW_BOX);
    blended_output->color((Fl_Color)34);
    blended_output->selection_color(FL_BACKGROUND2_COLOR);
    blended_output->labeltype(FL_NO_LABEL);
    blended_output->textsize(15);
    blended_output->textcolor((Fl_Color)55);
    blended_output->wrap_mode(4,0);
  } // Fl_Text_Display* blended_output
  { Fl_Box* o = new Fl_Box(16, 142, 427, 22, "label");
    o->box(FL_THIN_UP_FRAME);
    o->color((Fl_Color)40);
    o->selection_color((Fl_Color)78);
    o->labeltype(FL_NO_LABEL);
  } // Fl_Box* o
  { Fl_Box* o = new Fl_Box(16, 291, 427, 22, "label");
    o->box(FL_THIN_UP_FRAME);
    o->color((Fl_Color)40);
    o->selection_color((Fl_Color)78);
    o->labeltype(FL_NO_LABEL);
  } // Fl_Box* o
  { C1_currencychoice = new Fl_Choice(117, 142, 97, 21, "Cover using:");
    C1_currencychoice->box(FL_GTK_DOWN_BOX);
    C1_currencychoice->down_box(FL_BORDER_BOX);
    C1_currencychoice->labelsize(13);
    C1_currencychoice->callback((Fl_Callback*)Recalculate_CB);
    C1_currencychoice->when(FL_WHEN_CHANGED);
    C1_currencychoice->menu(menu_C1_currencychoice);
  } // Fl_Choice* C1_currencychoice
  { C2_currencychoice = new Fl_Choice(324, 141, 97, 21, "Cover using:");
    C2_currencychoice->box(FL_GTK_DOWN_BOX);
    C2_currencychoice->down_box(FL_BORDER_BOX);
    C2_currencychoice->labelsize(13);
    C2_currencychoice->callback((Fl_Callback*)Recalculate_CB);
    C2_currencychoice->when(FL_WHEN_CHANGED);
    C2_currencychoice->menu(menu_C2_currencychoice);
  } // Fl_Choice* C2_currencychoice
  { C3_currencychoice = new Fl_Choice(107, 291, 97, 21, "Cover using:");
    C3_currencychoice->box(FL_GTK_DOWN_BOX);
    C3_currencychoice->down_box(FL_BORDER_BOX);
    C3_currencychoice->labelsize(13);
    C3_currencychoice->callback((Fl_Callback*)Recalculate_CB);
    C3_currencychoice->when(FL_WHEN_CHANGED);
    C3_currencychoice->menu(menu_C3_currencychoice);
  } // Fl_Choice* C3_currencychoice
  { C4_currencychoice = new Fl_Choice(324, 291, 97, 21, "Cover using:");
    C4_currencychoice->box(FL_GTK_DOWN_BOX);
    C4_currencychoice->down_box(FL_BORDER_BOX);
    C4_currencychoice->labelsize(13);
    C4_currencychoice->callback((Fl_Callback*)Recalculate_CB);
    C4_currencychoice->when(FL_WHEN_CHANGED);
    C4_currencychoice->menu(menu_C4_currencychoice);
  } // Fl_Choice* C4_currencychoice
  { Fl_Flex* o = new Fl_Flex(16, 330, 427, 105);
    o->type(1);
    { c1_reinvest_output = new Fl_Text_Display(16, 330, 214, 105, "Currency 1 Re-invest");
      c1_reinvest_output->box(FL_OXY_UP_FRAME);
      c1_reinvest_output->labelcolor((Fl_Color)96);
      c1_reinvest_output->textcolor(FL_GRAY0);
      c1_reinvest_output->align(Fl_Align(161));
      // c1_reinvest_output->wrap_mode(4,0);
    } // Fl_Text_Display* c1_reinvest_output
    { c2_reinvest_output = new Fl_Text_Display(230, 330, 213, 105, "Currency 2 Re-invest");
      c2_reinvest_output->box(FL_OXY_UP_FRAME);
      c2_reinvest_output->textcolor(FL_GRAY0);
      // c2_reinvest_output->wrap_mode(4,0);
    } // Fl_Text_Display* c2_reinvest_output
    o->fixed(o->child(0), 214);
    o->fixed(o->child(1), 213);
    o->end();
  } // Fl_Flex* o
  { Fl_Flex* o = new Fl_Flex(16, 452, 427, 105);
    o->type(1);
    { c3_reinvest_output = new Fl_Text_Display(16, 452, 214, 105, "Currency 3 Re-invest");
      c3_reinvest_output->box(FL_OXY_UP_FRAME);
      c3_reinvest_output->labelcolor((Fl_Color)96);
      c3_reinvest_output->textcolor(FL_GRAY0);
      c3_reinvest_output->align(Fl_Align(161));
      // c3_reinvest_output->wrap_mode(4,0);
    } // Fl_Text_Display* c3_reinvest_output
    { tbill_reinvest_output = new Fl_Text_Display(230, 452, 213, 105, "Short Bonds Re-invest");
      tbill_reinvest_output->box(FL_OXY_UP_FRAME);
      tbill_reinvest_output->textcolor(FL_GRAY0);
      // tbill_reinvest_output->wrap_mode(4,0);
    } // Fl_Text_Display* tbill_reinvest_output
    o->fixed(o->child(0), 214);
    o->fixed(o->child(1), 213);
    o->end();
  } // Fl_Flex* o
  { recalc_button = new Fl_Button(387, 563, 67, 53, "Calculate");
    recalc_button->box(FL_GTK_THIN_UP_BOX);
    recalc_button->down_box(FL_GTK_THIN_DOWN_BOX);
    recalc_button->color((Fl_Color)215);
    recalc_button->selection_color((Fl_Color)175);
    recalc_button->callback((Fl_Callback*)Recalculate_CB, (void*)(this));
  } // Fl_Button* recalc_button
  { save_button = new Fl_Button(386, 622, 67, 40, "Save");
    save_button->box(FL_GTK_THIN_UP_BOX);
    save_button->down_box(FL_GTK_THIN_DOWN_BOX);
    save_button->color((Fl_Color)166);
    save_button->selection_color((Fl_Color)166);
    save_button->user_data((void*)(this));
  } // Fl_Button* save_button
  { close_button = new Fl_Button(387, 671, 67, 40, "Close");
    close_button->box(FL_GTK_THIN_UP_BOX);
    close_button->down_box(FL_GTK_THIN_DOWN_BOX);
    close_button->color((Fl_Color)214);
    close_button->selection_color((Fl_Color)214);
    close_button->user_data((void*)(this));
  } // Fl_Button* close_button
  show();
  end();
}

void results_window::setWindowPointers(main_window* mainwindow) {
  p_main_window = mainwindow;
}

void results_window::Recalculate_CB(Fl_Widget*w, void* userdata) {
  // Code that will be called when we press the calculate button 
  //
  // get the pointer to the object that generated the callback 
  results_window* instance = static_cast<results_window*>(userdata);

  //
  // Some of the calculations are borrowed from int calculator v1
  // The first thing we should do is safely obtain all of the user inputs 
  // all amounts are obtained as doubles
  //
  // const char* myvalue = input_currency1size->value(); Convert the const char*
  // to double (this function automatically removes invalid characters from the
  // end of the double

  // obtain the 4 primary position sizes 
  // OBTAINING FROM A POINTER TO MAIN WINDOW 
  double currency1size = std::strtod(instance->p_main_window->input_currency1size->value(), nullptr);
  double currency2size = std::strtod(instance->p_main_window->input_currency2size->value(), nullptr);
  double currency3size = std::strtod(instance->p_main_window->input_currency3size->value(), nullptr);
  double tbillsize = std::strtod(instance->p_main_window->input_tbillsize->value(), nullptr);

  // obtain the 4 primary position interest rates 
  double currency1rate = std::strtod(instance->p_main_window->input_currency1rate->value(), nullptr) / 100;
  double currency2rate = std::strtod(instance->p_main_window->input_currency2rate->value(), nullptr) / 100;
  double currency3rate = std::strtod(instance->p_main_window->input_currency3rate->value(), nullptr) / 100;
  double tbillrate = std::strtod(instance->p_main_window->input_tbillrate->value(), nullptr) / 100;

  // obtain the 4 proceeds position sizes 
  double currency1proceeds = std::strtod(instance->p_main_window->input_currency1proceeds->value(), nullptr);
  double currency2proceeds = std::strtod(instance->p_main_window->input_currency2proceeds->value(), nullptr);
  double currency3proceeds = std::strtod(instance->p_main_window->input_currency3proceeds->value(), nullptr);
  double tbillproceeds = std::strtod(instance->p_main_window->input_tbillproceeds->value(), nullptr);

  // obtain the 4 proceeds position interest rates 
  double currency1proceeds_rate = std::strtod(instance->p_main_window->input_currency1proceedsrate->value(), nullptr) / 100;
  double currency2proceeds_rate = std::strtod(instance->p_main_window->input_currency2proceedsrate->value(), nullptr) / 100;
  double currency3proceeds_rate = std::strtod(instance->p_main_window->input_currency3proceedsrate->value(), nullptr) / 100;
  double tbillproceeds_rate = std::strtod(instance->p_main_window->input_tbillproceedsrate->value(), nullptr) / 100;

  // obtain fx rate change values 
  double fxrate1 = std::strtod(instance->p_main_window->input_fxrate1->value(), nullptr);
  double fxrate2 = std::strtod(instance->p_main_window->input_fxrate2->value(), nullptr);
  double fxrate3 = std::strtod(instance->p_main_window->input_fxrate3->value(), nullptr);

  // debug 
  /* std::cout << "tbillsize: " << tbillsize << " tbillrate: " << tbillrate << */
  /* " 1size: " << currency1size << " 1rate: " << currency1rate << " 2size: " << */ 
  /* currency2size << " 2rate: " << currency2rate << " 3size: " */ 
  /* << currency3size << " 3rate: " << currency3rate << std::endl; */

  // debug 
  /* std::cout << "new fx rate: " << fxrate1 << std::endl; */

  // variables to use 
  double blended_one_year_profit = 0;
  double total_currency = 0;
  double blended_percent_return = 0;

  // items for cost to cover calculation
  double c1_cost_to_cover = 0;
  double c2_cost_to_cover = 0;
  double c3_cost_to_cover = 0;

  // used to sum negative balances 
  double total_debt = 0;

  // fx rate values will be 0 if they have not been set by the user.

  // so basically we want to calculate the rate of return for each position 
  // and then get the sum of those rate of returns to get the total rate 
  if(currency1size != 0)
  {
  	if(currency1size > 0)
  	{
  		total_currency += currency1size;
  	}

  	// solve for the daily int rate 
  	double daily_int_rate = currency1rate / 365;

  	// one year PL 
  	double one_year_profit = (currency1size * currency1rate) ;

  	// daily profit 
  	double daily_profit = one_year_profit / 365;

  	// solve for one year percent rate of return 
  	double final_percent_return = (one_year_profit / currency1size) * 100;

  	// if profit is negative, calculate cost to cover using cover currency input 
  	if(one_year_profit < 0)
  	{
  		// since our profit is negative we have a negative balance so append it to the total debt
  		total_debt += currency1size;

  		// get selected cover currency from FL_Input
  		int cover_currency = instance->C1_currencychoice->value();
  		switch (cover_currency) 
  		{
  		case 0:
  			{
  			// CASE 0 IS THE CURRENT CURRENCY.
  			// No exchange rate possible.

  			c1_cost_to_cover = std::abs(currency1size);

  			// Formatting will be same as below 
  			boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  			"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency1size 
  			%final_percent_return %one_year_profit %daily_profit %c1_cost_to_cover;

  			std::string c1_result = fmt.str();

  			// create buffer and assign it to the appropriate text display widget 
  			Fl_Text_Buffer *c1buff = new Fl_Text_Buffer();
  			instance->c1_output->buffer(c1buff);

  			// add the text to our buffer (to the box)
  			c1buff->text(c1_result.c_str());

  			// append the values to our total
  			blended_one_year_profit += one_year_profit;
  			}
  			break;
  		case 1:
  			if(fxrate1 != 0) // they needed to input an exchange rate  
  			{
  				// cover with currency2 (fxrate1 is between C1 and C2 so we reverse the rate here)
  				c1_cost_to_cover = std::abs(currency1size / (1 / fxrate1) ); 

  				// format text and set it  
  				boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  				"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency1size 
  				%final_percent_return %one_year_profit %daily_profit %c1_cost_to_cover;

  				// write it to string
  				std::string c1_result = fmt.str();

  				// create buffer and assign it to the appropriate text display widget 
  				Fl_Text_Buffer *c1buff = new Fl_Text_Buffer();
  				instance->c1_output->buffer(c1buff);

  				// add the text to our buffer (to the box)
  				c1buff->text(c1_result.c_str());

  				// append the values to our total
  				blended_one_year_profit += one_year_profit;

  			}
  			else
  			{
  				std::cout << "Error: Currency 1 Cover currency is Currency 2." 
  				" User did not enter FX rate between these currencies." << std::endl;
  			}
  			break;
  		case 2:
  			if(fxrate3 != 0) // they needed to input an exchange rate  
  			{
  				// cover with currency3 (fxrate3 is between C1 and C3)
  				c1_cost_to_cover = std::abs(currency1size / (1 / fxrate3) ) ;	

  				// format text and set it  
  				boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  				"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency1size 
  				%final_percent_return %one_year_profit %daily_profit %c1_cost_to_cover;

  				// write it to string
  				std::string c1_result = fmt.str();

  				// create buffer and assign it to the appropriate text display widget 
  				Fl_Text_Buffer *c1buff = new Fl_Text_Buffer();
  				instance->c1_output->buffer(c1buff);

  				// add the text to our buffer (to the box)
  				c1buff->text(c1_result.c_str());

  				// append the values to our total
  				blended_one_year_profit += one_year_profit;
  			}
  			else
  			{
  				std::cout << "Error: Currency 1 Cover currency is Currency 3." 
  				" User did not enter FX rate between these currencies." << std::endl;
  			}
  			break;
  		}

  	}		
  	else // Profit is positive or flat therefore no cost to cover calculation
  	{
  		boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  		"One Year P/L: $%3%\nDaily P/L: $%4%") %currency1size %final_percent_return 
  		%one_year_profit %daily_profit;

  		std::string c1_result = fmt.str();

  		/* std::cout << c1_result << std::endl; */

  		// This should maybe be global but not sure. It works. 
  		// create buffer and assign it to the appropriate text display widget 
  		Fl_Text_Buffer *c1buff = new Fl_Text_Buffer();
  		instance->c1_output->buffer(c1buff);

  		// add the text to our buffer (to the box)
  		c1buff->text(c1_result.c_str());

  		// append the values to our total
  		blended_one_year_profit += one_year_profit;
  	}
  }

  if(currency2size != 0)
  {
  	if(currency2size > 0)
  	{
  		total_currency += currency2size;
  	}

  	/* std::cout << "total currency: " << total_currency << std::endl; */

  	// solve for the daily int rate 
  	double daily_int_rate = currency2rate / 365;

  	// calculate one year rate of return 
  	double one_year_profit = (currency2size * currency2rate) ;

  	// daily profit 
  	double daily_profit = one_year_profit / 365;

  	// solve for one year percent rate of return 
  	double final_percent_return = (one_year_profit / currency2size) * 100;

  	// if profit is negative, calculate cost to cover using cover currency input 
  	if(one_year_profit < 0)
  	{
  		// since our profit is negative we have a negative balance so append it to the total debt
  		total_debt += currency2size;

  		// get selected cover currency from FL_Input
  		int cover_currency = instance->C2_currencychoice->value();
  		switch (cover_currency) 
  		{
  		case 0: // Currency1
  			if(fxrate1 != 0) // they needed to input an exchange rate  
  			{
  				// cover with currency1 (fxrate1 is between C1 and C2)
  				c2_cost_to_cover = std::abs(currency2size / fxrate1);	

  				// format text and set it  
  				boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  				"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency2size 
  				%final_percent_return %one_year_profit %daily_profit %c2_cost_to_cover;

  				// write it to string
  				std::string c2_result = fmt.str();

  				// create buffer and assign it to the appropriate text display widget 
  				Fl_Text_Buffer *c2buff = new Fl_Text_Buffer();
  				instance->c2_output->buffer(c2buff);

  				// add the text to our buffer (to the box)
  				c2buff->text(c2_result.c_str());

  				// append the values to our total
  				blended_one_year_profit += one_year_profit;

  			}
  			else
  			{
  				std::cout << "Error: Currency 2 Cover currency is Currency 1." 
  				" User did not enter FX rate between these currencies." << std::endl;
  			}
  			break;
  		case 1: //Currency2
  			{
  			// CASE NOT APPLICABLE BECAUSE IT IS THE CURRENT CURRENCY.
  			// No exchange rate possible.
  			
  			c2_cost_to_cover = std::abs(currency2size);

  			// Formatting will be same as below 
  			boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  			"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency2size %final_percent_return 
  			%one_year_profit %daily_profit %c2_cost_to_cover;

  			std::string c2_result = fmt.str();

  			// create buffer and assign it to the appropriate text display widget 
  			Fl_Text_Buffer *c2buff = new Fl_Text_Buffer();
  			instance->c2_output->buffer(c2buff);

  			// add the text to our buffer (to the box)
  			c2buff->text(c2_result.c_str());

  			// append the values to our total
  			blended_one_year_profit += one_year_profit;
  			}
  			break;
  		case 2:
  			if(fxrate2 != 0) // they needed to input an exchange rate  
  			{
  				// cover with currency3 (fxrate3 is between C2 and C3)
  				c2_cost_to_cover = std::abs(currency2size / (1 / fxrate2) );	

  				// format text and set it  
  				boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  				"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency2size 
  				%final_percent_return %one_year_profit %daily_profit %c2_cost_to_cover;

  				// write it to string
  				std::string c2_result = fmt.str();

  				// create buffer and assign it to the appropriate text display widget 
  				Fl_Text_Buffer *c2buff = new Fl_Text_Buffer();
  				instance->c2_output->buffer(c2buff);

  				// add the text to our buffer (to the box)
  				c2buff->text(c2_result.c_str());

  				// append the values to our total
  				blended_one_year_profit += one_year_profit;
  			}
  			else
  			{
  				std::cout << "Error: Currency 2 Cover currency is Currency 3." 
  				" User did not enter FX rate between these currencies." << std::endl;
  			}
  			break;
  		}

  	}		
  	else // Profit is positive or flat therefore no cost to cover calculation
  	{
  		boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  		"One Year P/L: $%3%\nDaily P/L: $%4%") %currency2size %final_percent_return 
  		%one_year_profit %daily_profit;

  		std::string c2_result = fmt.str();

  		// create buffer and assign it to the appropriate text display widget 
  		Fl_Text_Buffer *c2buff = new Fl_Text_Buffer();

  		instance->c2_output->buffer(c2buff);

  		// add the text to our buffer (to the box)
  		c2buff->text(c2_result.c_str());

  		// append the values to our total
  		blended_one_year_profit += one_year_profit;
  	}

  }

  if(currency3size != 0)
  {
  	// only count positive currencies in our total capital 
  	if(currency3size > 0)
  	{
  		total_currency += currency3size;
  	}

  	// solve for the daily int rate 
  	double daily_int_rate = currency3rate / 365;

  	// calculate one year rate of return 
  	double one_year_profit = (currency3size * currency3rate) ;

  	// daily profit 
  	double daily_profit = one_year_profit / 365;

  	// solve for one year percent rate of return 
  	double final_percent_return = (one_year_profit / currency3size) * 100;

  	// if profit is negative, calculate cost to cover using cover currency input 
  	if(one_year_profit < 0)
  	{
  		// since our profit is negative we have a negative balance so append it to the total debt
  		total_debt += currency3size;

  		// get selected cover currency from FL_Input
  		int cover_currency = instance->C3_currencychoice->value();
  		switch (cover_currency) 
  		{
  		case 0: // Currency1
  			if(fxrate3 != 0) // they needed to input an exchange rate  
  			{
  				// cover with currency1 (fxrate3 is between C1 and C3)
  			 	c3_cost_to_cover = std::abs(currency3size / fxrate3);	

  				// format text and set it  
  				boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  				"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency3size 
  				%final_percent_return %one_year_profit %daily_profit %c3_cost_to_cover;

  				// write it to string
  				std::string c3_result = fmt.str();

  				// create buffer and assign it to the appropriate text display widget 
  				Fl_Text_Buffer *c3buff = new Fl_Text_Buffer();
  				instance->c3_output->buffer(c3buff);

  				// add the text to our buffer (to the box)
  				c3buff->text(c3_result.c_str());

  				// append the values to our total
  				blended_one_year_profit += one_year_profit;

  			}
  			else
  			{
  				std::cout << "Error: Currency 3 Cover currency is Currency 1." 
  				" User did not enter FX rate between these currencies." << std::endl;
  			}
  			break;
  		case 1: // cover with Currency 2
  			if(fxrate2 != 0) // they needed to input an exchange rate  
  			{
  				// cover with currency2 (fxrate2 is between C2 and C3)
  				c3_cost_to_cover = std::abs(currency3size / fxrate2);	

  				// format text and set it  
  				boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  				"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency3size 
  				%final_percent_return %one_year_profit %daily_profit %c3_cost_to_cover;

  				// write it to string
  				std::string c3_result = fmt.str();

  				// create buffer and assign it to the appropriate text display widget 
  				Fl_Text_Buffer *c3buff = new Fl_Text_Buffer();
  				instance->c3_output->buffer(c3buff);

  				// add the text to our buffer (to the box)
  				c3buff->text(c3_result.c_str());

  				// append the values to our total
  				blended_one_year_profit += one_year_profit;
  			}
  			else
  			{
  				std::cout << "Error: Currency 3 Cover currency is Currency 2." 
  				" User did not enter FX rate between these currencies." << std::endl;
  			}
  			break;
  		case 2:
  			{
  			// CASE NOT APPLICABLE BECAUSE IT IS THE CURRENT CURRENCY.
  			// No exchange rate possible.
  			
  			c3_cost_to_cover = std::abs(currency3size);

  			boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  			"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency3size %final_percent_return 
  			%one_year_profit %daily_profit %c3_cost_to_cover;

  			std::string c3_result = fmt.str();

  			// create buffer and assign it to the appropriate text display widget 
  			Fl_Text_Buffer *c3buff = new Fl_Text_Buffer();
  			instance->c3_output->buffer(c3buff);

  			// add the text to our buffer (to the box)
  			c3buff->text(c3_result.c_str());

  			// append the values to our total
  			blended_one_year_profit += one_year_profit;
  			}
  			break;
  		}

  	}		
  	else // Profit is positive or flat therefore no cost to cover calculation
  	{
  		boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  		"One Year P/L: $%3%\nDaily P/L: $%4%") %currency3size %final_percent_return 
  		%one_year_profit %daily_profit;

  		std::string c3_result = fmt.str();

  		// create buffer and assign it to the appropriate text display widget 
  		Fl_Text_Buffer *c3buff = new Fl_Text_Buffer();

  		instance->c3_output->buffer(c3buff);

  		// add the text to our buffer (to the box)
  		c3buff->text(c3_result.c_str());

  		// append the values to our total
  		blended_one_year_profit += one_year_profit;
  	}
  }

  if(tbillsize != 0)
  {
  	// only count positive currencies 
  	if(tbillsize > 0)
  	{
  		total_currency += tbillsize;
  	}

  	// solve for the daily int rate 
  	double daily_int_rate = tbillrate / 365;

  	// set a trade fee to subtract 
  	double trade_fee = 7.5;

  	// calculate one year rate of return 
  	double one_year_profit = (tbillsize * tbillrate) - trade_fee;

  	// daily profit 
  	double daily_profit = one_year_profit / 365;

  	// solve for one year percent rate of return 
  	double final_percent_return = (one_year_profit / tbillsize) * 100;

  	boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  	"One Year P/L: $%3%\nDaily P/L: $%4%") %tbillsize %final_percent_return %one_year_profit %daily_profit;

  	std::string tbill_result = fmt.str();

  	// create buffer and assign it to the appropriate text display widget 
  	Fl_Text_Buffer *tbill_buffer = new Fl_Text_Buffer();
  	instance->tbill_output->buffer(tbill_buffer);

  	// add the text to our buffer (to the box)
  	tbill_buffer->text(tbill_result.c_str());

  	// append the values to our total
  	blended_one_year_profit += one_year_profit;
  }

  // PROCEEDS TEXT OUTPUT
  if(currency1proceeds != 0)
  {
  	// append amount to total currency since we are in possession of the short
  	// position proceeds
  	total_currency += currency1proceeds;
  	
  	// solve for the daily int rate 
  	double daily_int_rate = currency1proceeds_rate / 365;

  	// calculate one year rate of return 
  	double one_year_profit = (currency1proceeds * currency1proceeds_rate) ;

  	// daily profit 
  	double daily_profit = one_year_profit / 365;

  	// solve for one year percent rate of return 
  	double final_percent_return = (one_year_profit / currency1proceeds) * 100;

  	boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  	"One Year P/L: $%3%\nDaily P/L: $%4%") %currency1proceeds %final_percent_return %one_year_profit %daily_profit;

  	std::string result = fmt.str();

  	// create buffer and assign it to the appropriate text display widget 
  	Fl_Text_Buffer *c1_buffer = new Fl_Text_Buffer();
  	instance->c1_reinvest_output->buffer(c1_buffer);

  	// add the text to our buffer (to the box)
  	c1_buffer->text(result.c_str());

  	// append the values to our total
  	blended_one_year_profit += one_year_profit;
  	blended_one_year_profit += one_year_profit;
  	std::cout << "1 one year profit: " << one_year_profit << std::endl;
  	std::cout << "1 blended one year profit: " << blended_one_year_profit << std::endl;
  }

  if(currency2proceeds != 0)
  {
  	// append amount to total currency since we are in possession of the short
  	// position proceeds
  	total_currency += currency2proceeds;

  	// solve for the daily int rate 
  	double daily_int_rate = currency2proceeds_rate / 365;

  	// calculate one year rate of return 
  	double one_year_profit = (currency2proceeds * currency2proceeds_rate) ;

  	// daily profit 
  	double daily_profit = one_year_profit / 365;

  	// solve for one year percent rate of return 
  	double final_percent_return = (one_year_profit / currency2proceeds) * 100;

  	boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  	"One Year P/L: $%3%\nDaily P/L: $%4%") %currency2proceeds %final_percent_return %one_year_profit %daily_profit;

  	std::string result = fmt.str();

  	// create buffer and assign it to the appropriate text display widget 
  	Fl_Text_Buffer *c2_buffer = new Fl_Text_Buffer();
  	instance->c2_reinvest_output->buffer(c2_buffer);

  	// add the text to our buffer (to the box)
  	c2_buffer->text(result.c_str());

  	// append the values to our total
  	blended_one_year_profit += one_year_profit;
  	std::cout << "2 one year profit: " << one_year_profit << std::endl;
  	std::cout << "2 blended one year profit: " << blended_one_year_profit << std::endl;
  }

  if(currency3proceeds != 0)
  {
  	// append amount to total currency since we are in possession of the short
  	// position proceeds
  	total_currency += currency3proceeds;

  	// solve for the daily int rate 
  	double daily_int_rate = currency3proceeds_rate / 365;

  	// calculate one year rate of return 
  	double one_year_profit = (currency3proceeds * currency3proceeds_rate) ;

  	// daily profit 
  	double daily_profit = one_year_profit / 365;

  	// solve for one year percent rate of return 
  	double final_percent_return = (one_year_profit / currency3proceeds) * 100;

  	boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  	"One Year P/L: $%3%\nDaily P/L: $%4%") %currency3proceeds %final_percent_return %one_year_profit %daily_profit;

  	std::string result = fmt.str();

  	// create buffer and assign it to the appropriate text display widget 
  	Fl_Text_Buffer *c3_buffer = new Fl_Text_Buffer();
  	instance->c3_reinvest_output->buffer(c3_buffer);

  	// add the text to our buffer (to the box)
  	c3_buffer->text(result.c_str());

  	// append the values to our total
  	blended_one_year_profit += one_year_profit;
  	std::cout << "3 one year profit: " << one_year_profit << std::endl;
  	std::cout << "3 blended one year profit: " << blended_one_year_profit << std::endl;
  }
  if(tbillproceeds != 0)
  {
  	// Proceeds will always be positive
  	
  	// append amount to total currency since we are in possession of the short
  	// position proceeds
  	total_currency += tbillproceeds;

  	// solve for the daily int rate 
  	double daily_int_rate = tbillproceeds_rate / 365;

  	// calculate one year rate of return 
  	double one_year_profit = (tbillproceeds * tbillproceeds_rate) ;

  	// daily profit 
  	double daily_profit = one_year_profit / 365;

  	// solve for one year percent rate of return 
  	double final_percent_return = (one_year_profit / tbillproceeds) * 100;

  	boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  	"One Year P/L: $%3%\nDaily P/L: $%4%") %tbillproceeds %final_percent_return %one_year_profit %daily_profit;

  	std::string result = fmt.str();

  	// create buffer and assign it to the appropriate text display widget 
  	Fl_Text_Buffer *tbill_buffer = new Fl_Text_Buffer();
  	instance->tbill_reinvest_output->buffer(tbill_buffer);

  	// add the text to our buffer (to the box)
  	tbill_buffer->text(result.c_str());

  	// append the values to our total
  	blended_one_year_profit += one_year_profit;
  	std::cout << "4 one year profit: " << one_year_profit << std::endl;
  	std::cout << "4 blended one year profit: " << blended_one_year_profit << std::endl;
  }

  // LOGIC FOR TOTALS CALCULATIONS 

  // solve for blended percent return 
  blended_percent_return = (blended_one_year_profit / total_currency) * 100;

  // used for solving cost to cover if applicable 
  double cost_to_cover = 0;

  // used for formatting text to pass into buffer 
  std::string result; 

  // if any of these currencies are negative we need to format text with cost to cover
  if(currency1size < 0 || currency2size < 0 || currency3size < 0)
  {
  	// simply add up all cost to covers and we will get a total
  	double total_cost_to_cover = c1_cost_to_cover + c2_cost_to_cover + c3_cost_to_cover;

  	boost::format fmt = boost::format("Total Capital: $%1%\nTotal Debt: $%2%\n"
  	"Blended Interest Rate: %3%%%\nOne Year P/L: $%4%\n"
  	"Cost to Cover with FX rate: $%5%") %total_currency %total_debt %blended_percent_return 
  	%blended_one_year_profit % total_cost_to_cover ;

  	// write to the string
  	result = fmt.str();
  }
  else
  {
  	// no currencies are negative therefore format without cost to cover 

  	boost::format fmt = boost::format("Total Capital: $%1%\n"
  	"Blended Interest Rate: %2%%%\nOne Year P/L: $%3%\n") %total_currency 
  	%blended_percent_return %blended_one_year_profit ;

  	// write to the string
  	result = fmt.str();
  }

  // debug 
  /* std::cout << "Total One Year Profit/Loss was: " << blended_one_year_profit */ 
  /* << std::endl; */
  /* std::cout << "Total return: " << blended_percent_return */ 
  /* << "%" << std::endl; */

  // create a new buffer 
  Fl_Text_Buffer *total_buffer = new Fl_Text_Buffer();

  // assign the buffer to a text display widget
  instance->blended_output->buffer(total_buffer);

  // add the text to our buffer 
  total_buffer->text(result.c_str());
}

void results_window::WinQuit_CB(Fl_Widget*w, void* userdata) {
  // WINQUIT RESULT WINDOW / SAVE VALUES 
  //
  // This callback handles the window close button for response window
  //
  // get the pointer to the object that generated the callback 
  results_window* instance = static_cast<results_window*>(userdata);

  // SAVE the values for which currencies for cost to cover to the db

  int C1_covercurrency = instance->C1_currencychoice->value();
  int C2_covercurrency = instance->C2_currencychoice->value();
  int C3_covercurrency = instance->C3_currencychoice->value();
  int C4_covercurrency = instance->C4_currencychoice->value();

  // debug 
  /* std::cout << C1_covercurrency << " " << C2_covercurrency << " " */ 
  /* << C3_covercurrency << " " << C4_covercurrency << std::endl; */

  // open the database
  sqlite3_open(dbPath, &db);

  // ADD OR REPLACE THE ROW IN OUR TABLE 
  const std::string update_sql = "UPDATE MAINWINDOW_INPUTS SET "
      "C1_COVERCURRENCY = '" + std::to_string(C1_covercurrency) + "', "
      "C2_COVERCURRENCY = '" + std::to_string(C2_covercurrency) + "', "
      "C3_COVERCURRENCY = '" + std::to_string(C3_covercurrency) + "', "
      "C4_COVERCURRENCY = '" + std::to_string(C4_covercurrency) + "' "
      "WHERE ID = 1;";

  int result = sqlite3_exec(db, update_sql.c_str(), NULL, 0, &err); 
  if(result != SQLITE_OK) 
  { 
  	std::cout << "error with insert: " << err << std::endl; 
  	sqlite3_free(err); // need to look into that function 
  } 
  else
  {
  	/* std::cout << "inserted: " << result << std::endl; */
  }
  	
  sqlite3_close(db); // close the database
  				   
  /* w->~Fl_Widget(); */
  // hide the widget 
  instance->hide();
}

main_window::main_window(int X, int Y, int W, int H, const char *L) :
  Fl_Window(X, Y, W, H, L)
{
  _main_window();
}

main_window::main_window(int W, int H, const char *L) :
  Fl_Window(0, 0, W, H, L)
{
  clear_flag(16);
  _main_window();
}

main_window::main_window() :
  Fl_Window(0, 0, 611, 506, 0)
{
  clear_flag(16);
  _main_window();
}

void main_window::_main_window() {
  this->box(FL_FLAT_BOX);
  this->color(FL_BACKGROUND_COLOR);
  this->selection_color(FL_BACKGROUND_COLOR);
  this->labeltype(FL_NO_LABEL);
  this->labelfont(0);
  this->labelsize(14);
  this->labelcolor(FL_FOREGROUND_COLOR);
  this->user_data((void*)(this));
  this->align(Fl_Align(FL_ALIGN_TOP));
  this->when(FL_WHEN_RELEASE);
  { Fl_Group* o = new Fl_Group(14, 16, 578, 482);
    o->box(FL_ENGRAVED_BOX);
    { save_button = new Fl_Button(313, 449, 120, 37, "Save");
      save_button->box(FL_OXY_UP_BOX);
      save_button->callback((Fl_Callback*)save_CB, (void*)(this));
    } // Fl_Button* save_button
    { load_button = new Fl_Button(184, 449, 120, 37, "Load");
      load_button->box(FL_OXY_UP_BOX);
      load_button->callback((Fl_Callback*)load_CB, (void*)(this));
    } // Fl_Button* load_button
    { clear_button = new Fl_Button(64, 453, 110, 29, "Clear All");
      clear_button->box(FL_OXY_UP_BOX);
      clear_button->color((Fl_Color)31);
      clear_button->labelcolor((Fl_Color)55);
      clear_button->callback((Fl_Callback*)clear_CB, (void*)(this));
    } // Fl_Button* clear_button
    { submit_button = new Fl_Return_Button(443, 449, 133, 37, "Calculate");
      submit_button->tooltip("Press this once values are entered");
      submit_button->box(FL_OXY_UP_BOX);
      submit_button->color((Fl_Color)16);
      submit_button->callback((Fl_Callback*)Calculate_CB, (void*)(this));
      submit_button->when(FL_WHEN_RELEASE | FL_WHEN_ENTER_KEY);
    } // Fl_Return_Button* submit_button
    { Fl_Box* o = new Fl_Box(14, 44, 573, 61, "label");
      o->box(FL_OXY_BUTTON_DOWN_BOX);
      o->labeltype(FL_NO_LABEL);
    } // Fl_Box* o
    { Fl_Box* o = new Fl_Box(14, 104, 573, 58, "label");
      o->box(FL_OXY_BUTTON_DOWN_BOX);
      o->color((Fl_Color)28);
      o->labeltype(FL_NO_LABEL);
    } // Fl_Box* o
    { Fl_Box* o = new Fl_Box(14, 188, 576, 61, "label");
      o->box(FL_OXY_BUTTON_DOWN_BOX);
      o->labeltype(FL_NO_LABEL);
    } // Fl_Box* o
    { Fl_Box* o = new Fl_Box(14, 248, 576, 58, "label");
      o->box(FL_OXY_BUTTON_DOWN_BOX);
      o->color((Fl_Color)28);
      o->labeltype(FL_NO_LABEL);
    } // Fl_Box* o
    { left_flex = new Fl_Flex(185, 45, 101, 117);
      { input_currency1size = new Fl_Input(185, 45, 101, 30, "Currency 1 position size:");
        input_currency1size->callback((Fl_Callback*)input_CB, (void*)(this));
        input_currency1size->when(FL_WHEN_CHANGED);
      } // Fl_Input* input_currency1size
      { input_currency1rate = new Fl_Input(185, 75, 101, 29, "Currency 1 interest rate:");
        input_currency1rate->callback((Fl_Callback*)input_CB, (void*)(this));
        input_currency1rate->when(FL_WHEN_CHANGED);
      } // Fl_Input* input_currency1rate
      { input_currency3size = new Fl_Input(185, 104, 101, 29, "Currency 3 position size:");
        input_currency3size->callback((Fl_Callback*)input_CB, (void*)(this));
        input_currency3size->when(FL_WHEN_CHANGED);
      } // Fl_Input* input_currency3size
      { input_currency3rate = new Fl_Input(185, 133, 101, 29, "Currency 3 interest rate:");
        input_currency3rate->callback((Fl_Callback*)input_CB, (void*)(this));
        input_currency3rate->when(FL_WHEN_CHANGED);
      } // Fl_Input* input_currency3rate
      left_flex->end();
    } // Fl_Flex* left_flex
    { right_flex = new Fl_Flex(460, 45, 131, 118);
      { input_currency2size = new Fl_Input(460, 45, 131, 30, "Currency 2 position size:");
        input_currency2size->callback((Fl_Callback*)input_CB, (void*)(this));
        input_currency2size->when(FL_WHEN_CHANGED);
      } // Fl_Input* input_currency2size
      { input_currency2rate = new Fl_Input(460, 75, 131, 30, "Currency 2 interest rate:");
        input_currency2rate->callback((Fl_Callback*)input_CB, (void*)(this));
        input_currency2rate->when(FL_WHEN_CHANGED);
      } // Fl_Input* input_currency2rate
      { input_tbillsize = new Fl_Input(460, 105, 131, 29, "Long T-Bill position size:");
        input_tbillsize->callback((Fl_Callback*)input_CB, (void*)(this));
        input_tbillsize->when(FL_WHEN_CHANGED);
      } // Fl_Input* input_tbillsize
      { input_tbillrate = new Fl_Input(460, 134, 131, 29, "T-Bill interest rate:");
        input_tbillrate->callback((Fl_Callback*)input_CB, (void*)(this));
        input_tbillrate->when(FL_WHEN_CHANGED);
      } // Fl_Input* input_tbillrate
      right_flex->end();
    } // Fl_Flex* right_flex
    { middle_left_flex = new Fl_Flex(186, 189, 101, 117);
      { input_currency1proceeds = new Fl_Input(186, 189, 101, 30, "Currency 1 proceeds:");
        input_currency1proceeds->callback((Fl_Callback*)input_CB, (void*)(this));
        input_currency1proceeds->when(FL_WHEN_CHANGED);
      } // Fl_Input* input_currency1proceeds
      { input_currency1proceedsrate = new Fl_Input(186, 219, 101, 29, "Proceeds Rate of Return:");
        input_currency1proceedsrate->callback((Fl_Callback*)input_CB, (void*)(this));
        input_currency1proceedsrate->when(FL_WHEN_CHANGED);
      } // Fl_Input* input_currency1proceedsrate
      { input_currency3proceeds = new Fl_Input(186, 248, 101, 29, "Currency 3 proceeds:");
        input_currency3proceeds->callback((Fl_Callback*)input_CB, (void*)(this));
        input_currency3proceeds->when(FL_WHEN_CHANGED);
      } // Fl_Input* input_currency3proceeds
      { input_currency3proceedsrate = new Fl_Input(186, 277, 101, 29, "Proceeds Rate of Return:");
        input_currency3proceedsrate->callback((Fl_Callback*)input_CB, (void*)(this));
        input_currency3proceedsrate->when(FL_WHEN_CHANGED);
      } // Fl_Input* input_currency3proceedsrate
      middle_left_flex->end();
    } // Fl_Flex* middle_left_flex
    { middle_right_flex = new Fl_Flex(460, 189, 131, 118);
      { input_currency2proceeds = new Fl_Input(460, 189, 131, 30, "Currency 2 proceeds:");
        input_currency2proceeds->callback((Fl_Callback*)input_CB, (void*)(this));
        input_currency2proceeds->when(FL_WHEN_CHANGED);
      } // Fl_Input* input_currency2proceeds
      { input_currency2proceedsrate = new Fl_Input(460, 219, 131, 30, "Proceeds Rate of Return:");
        input_currency2proceedsrate->callback((Fl_Callback*)input_CB, (void*)(this));
        input_currency2proceedsrate->when(FL_WHEN_CHANGED);
      } // Fl_Input* input_currency2proceedsrate
      { input_tbillproceeds = new Fl_Input(460, 249, 131, 29, "Short T-Bill Proceeds:");
        input_tbillproceeds->callback((Fl_Callback*)input_CB, (void*)(this));
        input_tbillproceeds->when(FL_WHEN_CHANGED);
      } // Fl_Input* input_tbillproceeds
      { input_tbillproceedsrate = new Fl_Input(460, 278, 131, 29, "Proceeds Rate of Return:");
        input_tbillproceedsrate->callback((Fl_Callback*)input_CB, (void*)(this));
        input_tbillproceedsrate->when(FL_WHEN_CHANGED);
      } // Fl_Input* input_tbillproceedsrate
      middle_right_flex->end();
    } // Fl_Flex* middle_right_flex
    { Fl_Box* o = new Fl_Box(35, 333, 485, 32, "label");
      o->box(FL_THIN_UP_FRAME);
      o->color((Fl_Color)40);
      o->selection_color((Fl_Color)78);
      o->labeltype(FL_NO_LABEL);
      o->labelsize(15);
    } // Fl_Box* o
    { input_fxrate1 = new Fl_Input(346, 338, 99, 23, "Projected FX Rate Between Currency 1 && 2:");
      input_fxrate1->box(FL_GTK_THIN_DOWN_BOX);
      input_fxrate1->color((Fl_Color)96);
      input_fxrate1->selection_color(FL_LIGHT1);
      input_fxrate1->labelsize(15);
      input_fxrate1->labelcolor(FL_GRAY0);
      input_fxrate1->textcolor((Fl_Color)55);
      input_fxrate1->callback((Fl_Callback*)input_CB, (void*)(this));
      input_fxrate1->when(FL_WHEN_CHANGED);
    } // Fl_Input* input_fxrate1
    { invert_rate1 = new Fl_Button(450, 338, 64, 22, "Invert");
      invert_rate1->callback((Fl_Callback*)invert_CB, (void*)(this));
    } // Fl_Button* invert_rate1
    { Fl_Box* o = new Fl_Box(35, 367, 485, 32, "label");
      o->box(FL_THIN_UP_FRAME);
      o->color((Fl_Color)40);
      o->selection_color((Fl_Color)78);
      o->labeltype(FL_NO_LABEL);
      o->labelsize(15);
    } // Fl_Box* o
    { input_fxrate2 = new Fl_Input(346, 372, 99, 23, "Projected FX Rate Between Currency 2 && 3:");
      input_fxrate2->box(FL_GTK_THIN_DOWN_BOX);
      input_fxrate2->color((Fl_Color)96);
      input_fxrate2->selection_color(FL_LIGHT1);
      input_fxrate2->labelsize(15);
      input_fxrate2->labelcolor(FL_GRAY0);
      input_fxrate2->textcolor((Fl_Color)55);
      input_fxrate2->callback((Fl_Callback*)input_CB, (void*)(this));
      input_fxrate2->when(FL_WHEN_CHANGED);
    } // Fl_Input* input_fxrate2
    { invert_rate2 = new Fl_Button(450, 372, 64, 22, "Invert");
      invert_rate2->callback((Fl_Callback*)invert_CB, (void*)(this));
    } // Fl_Button* invert_rate2
    { Fl_Box* o = new Fl_Box(35, 401, 485, 32, "label");
      o->box(FL_THIN_UP_FRAME);
      o->color((Fl_Color)40);
      o->selection_color((Fl_Color)78);
      o->labeltype(FL_NO_LABEL);
      o->labelsize(15);
    } // Fl_Box* o
    { input_fxrate3 = new Fl_Input(346, 406, 99, 23, "Projected FX Rate Between Currency 1 && 3:");
      input_fxrate3->box(FL_GTK_THIN_DOWN_BOX);
      input_fxrate3->color((Fl_Color)96);
      input_fxrate3->selection_color(FL_LIGHT1);
      input_fxrate3->labelsize(15);
      input_fxrate3->labelcolor(FL_GRAY0);
      input_fxrate3->textcolor((Fl_Color)55);
      input_fxrate3->callback((Fl_Callback*)input_CB, (void*)(this));
      input_fxrate3->when(FL_WHEN_CHANGED);
    } // Fl_Input* input_fxrate3
    { invert_rate3 = new Fl_Button(450, 406, 64, 22, "Invert");
      invert_rate3->callback((Fl_Callback*)invert_CB, (void*)(this));
    } // Fl_Button* invert_rate3
    { header_grid1 = new Fl_Grid(15, 17, 572, 27);
      header_grid1->box(FL_NO_BOX);
      header_grid1->align(Fl_Align(FL_ALIGN_TOP|FL_ALIGN_INSIDE));
      header_grid1->deactivate();
      header_grid1->layout(1, 3);
      { account_positions_header = new Fl_Output(198, 17, 205, 27);
        account_positions_header->box(FL_NO_BOX);
        account_positions_header->color(FL_BACKGROUND_COLOR);
        account_positions_header->selection_color(FL_BACKGROUND_COLOR);
        account_positions_header->labeltype(FL_NO_LABEL);
        account_positions_header->labelsize(15);
        account_positions_header->align(Fl_Align(FL_ALIGN_RIGHT));
        account_positions_header->value("Account Positions (Long/Short)");
      } // Fl_Output* account_positions_header
      Fl_Grid::Cell *cell = NULL;
      cell = header_grid1->widget(header_grid1->child(0), 0, 1, 1, 1, 48);
      if (cell) cell->minimum_size(21, 20);
      header_grid1->end();
    } // Fl_Grid* header_grid1
    { header_grid2 = new Fl_Grid(15, 162, 572, 27);
      header_grid2->box(FL_NO_BOX);
      header_grid2->align(Fl_Align(FL_ALIGN_TOP|FL_ALIGN_INSIDE));
      header_grid2->deactivate();
      header_grid2->layout(1, 3);
      static const int colwidths[] = { 12, 0, 0 };
      header_grid2->col_width(colwidths, 3);
      { proceeds_positions_header = new Fl_Output(207, 162, 200, 27);
        proceeds_positions_header->box(FL_NO_BOX);
        proceeds_positions_header->color(FL_BACKGROUND_COLOR);
        proceeds_positions_header->selection_color(FL_BACKGROUND_COLOR);
        proceeds_positions_header->labeltype(FL_NO_LABEL);
        proceeds_positions_header->labelsize(15);
        proceeds_positions_header->align(Fl_Align(FL_ALIGN_RIGHT));
        proceeds_positions_header->value("Proceeds from Short Positions (Long Only)");
      } // Fl_Output* proceeds_positions_header
      Fl_Grid::Cell *cell = NULL;
      cell = header_grid2->widget(header_grid2->child(0), 0, 1, 1, 1, 48);
      if (cell) cell->minimum_size(20, 20);
      header_grid2->end();
    } // Fl_Grid* header_grid2
    { header_grid4 = new Fl_Grid(17, 306, 572, 27);
      header_grid4->box(FL_NO_BOX);
      header_grid4->layout(1, 3);
      static const int colwidths[] = { 65, 0, 0 };
      header_grid4->col_width(colwidths, 3);
      { settings_header = new Fl_Output(245, 306, 182, 27);
        settings_header->box(FL_NO_BOX);
        settings_header->color(FL_GRAY0);
        settings_header->selection_color(FL_GRAY0);
        settings_header->labeltype(FL_NO_LABEL);
        settings_header->labelsize(15);
        settings_header->labelcolor(FL_GRAY0);
        settings_header->textcolor(FL_GRAY0);
        settings_header->align(Fl_Align(FL_ALIGN_RIGHT));
        settings_header->value("General Settings");
      } // Fl_Output* settings_header
      Fl_Grid::Cell *cell = NULL;
      cell = header_grid4->widget(header_grid4->child(0), 0, 1, 1, 1, 48);
      if (cell) cell->minimum_size(20, 20);
      header_grid4->end();
    } // Fl_Grid* header_grid4
    o->end();
  } // Fl_Group* o
  show();
  end();
}

void main_window::input_CB(Fl_Widget*w, void* userdata) {
  // USER TEXT INPUT CALLBACK
  // Get the user data field from the widget to determine which input was pressed
  /* std::string_view which_input = (const char*)w->user_data(); */

  // get the pointer to the object that generated the callback 
  main_window* instance = static_cast<main_window*>(userdata);
  //
  // we know it was a Fl Input so static cast the widget pointer to that 
  Fl_Input *which_input = static_cast<Fl_Input*>(w);
  //
  // debug
  /* std::cout << which_input << std::endl; */

  // different arrays with valid characters the user can enter 
  std::array<char, 12> valid_characters = {'0','1','2','3','4','5'
  ,'6','7','8','9','-','.'};

  std::array<char, 13> rate_valid_characters = {'0','1','2','3','4','5'
  ,'6','7','8','9','%','-','.'};

  std::array<char, 11> proceeds_valid_characters = {'0','1','2','3','4','5'
  ,'6','7','8','9','.'};

  // set to false until proven otherwise
  bool invalid_character_found = false;

  if(which_input == instance->input_currency1size )
  {
  	// input_currency1size
  	std::string_view value = instance->input_currency1size->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < valid_characters.size(); i++)
  		{	
  			if(value[index] == valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// catch if they inputted a - sign at any point other than the start of the number  
  		if (index > 0 && value[index] == '-')
  		{
  			is_valid_character = false; 
  		}
  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			// create a new string that removes the invalid character from the string
  			std::string_view prefix = value.substr(0, index);
  			std::string_view suffix = value.substr(index + 1);

  			// Combine the substrings 
  			std::string new_string = std::string(prefix) + std::string(suffix);

  			/* std::cout << "Original String: " << value << std::endl; */
  			/* std::cout << "String Without Character at Index " << index << ": " */
  			/* << new_string << std::endl; */

  			instance->input_currency1size->value(new_string.c_str());
  		}
  	}
  }
  else if(which_input == instance->input_currency1rate )
  {
  	// input_currency1rate
  	std::string_view value = instance->input_currency1rate->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < rate_valid_characters.size(); i++)
  		{	
  			if(value[index] == rate_valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// catch if they inputted a - sign at any point other than the start of the input  
  		if (index > 0 && value[index] == '-' )
  		{
  			is_valid_character = false; 
  		}

  		// catch if they inputted a % sign at any point other than the end of the input
  		if(index < value.size() - 1 && value[index] == '%')
  		{
  			is_valid_character = false; 
  		}

  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			// create a new string that removes the invalid character from the string
  			std::string_view prefix = value.substr(0, index);
  			std::string_view suffix = value.substr(index + 1);

  			// Combine the substrings 
  			std::string new_string = std::string(prefix) + std::string(suffix);

  			/* std::cout << "Original String: " << value << std::endl; */
  			/* std::cout << "String Without Character at Index " << index << ": " */
  			/* << new_string << std::endl; */

  			instance->input_currency1rate->value(new_string.c_str());
  		}
  	}
  }
  else if(which_input ==  instance->input_currency2size)
  {
  	// input_currency2size
  	std::string_view value = instance->input_currency2size->value();

  	/* std::cout << "debug input: " << value << std::endl; */

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < valid_characters.size(); i++)
  		{	
  			if(value[index] == valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// catch if they inputted a - sign at any point other than the start of the number  
  		if (index > 0 && value[index] == '-')
  		{
  			is_valid_character = false; 
  		}
  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			// create a new string that removes the invalid character from the string
  			std::string_view prefix = value.substr(0, index);
  			std::string_view suffix = value.substr(index + 1);

  			// Combine the substrings 
  			std::string new_string = std::string(prefix) + std::string(suffix);

  			/* std::cout << "Original String: " << value << std::endl; */
  			/* std::cout << "String Without Character at Index " << index << ": " */
  			/* << new_string << std::endl; */

  			instance->input_currency2size->value(new_string.c_str());
  		}
  	}
  }
  else if(which_input == instance->input_currency2rate )
  {
  	// input_currency2rate
  	std::string_view value = instance->input_currency2rate->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < rate_valid_characters.size(); i++)
  		{	
  			if(value[index] == rate_valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// catch if they inputted a - sign at any point other than the start of the number  
  		if (index > 0 && value[index] == '-' )
  		{
  			is_valid_character = false; 
  		}
  		
  		// catch if they inputted a % sign at any point other than the end of the input
  		if(index < value.size() - 1 && value[index] == '%')
  		{
  			is_valid_character = false; 
  		}

  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			// create a new string that removes the invalid character from the string
  			std::string_view prefix = value.substr(0, index);
  			std::string_view suffix = value.substr(index + 1);

  			// Combine the substrings 
  			std::string new_string = std::string(prefix) + std::string(suffix);

  			/* std::cout << "Original String: " << value << std::endl; */
  			/* std::cout << "String Without Character at Index " << index << ": " */
  			/* << new_string << std::endl; */

  			instance->input_currency2rate->value(new_string.c_str());
  		}
  	}
  }
  else if(which_input ==  instance->input_currency3size)
  {
  	// input_currency2size
  	std::string_view value = instance->input_currency3size->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < valid_characters.size(); i++)
  		{	
  			if(value[index] == valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// catch if they inputted a - sign at any point other than the start of the number  
  		if (index > 0 && value[index] == '-')
  		{
  			is_valid_character = false; 
  		}
  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			// create a new string that removes the invalid character from the string
  			std::string_view prefix = value.substr(0, index);
  			std::string_view suffix = value.substr(index + 1);

  			// Combine the substrings 
  			std::string new_string = std::string(prefix) + std::string(suffix);

  			/* std::cout << "Original String: " << value << std::endl; */
  			/* std::cout << "String Without Character at Index " << index << ": " */
  			/* << new_string << std::endl; */

  			instance->input_currency3size->value(new_string.c_str());
  		}
  	}
  }
  else if(which_input == instance->input_currency3rate)
  {
  	// input_currency2rate
  	std::string_view value = instance->input_currency3rate->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < rate_valid_characters.size(); i++)
  		{	
  			if(value[index] == rate_valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// catch if they inputted a - sign at any point other than the start of the number  
  		if (index > 0 && value[index] == '-' )
  		{
  			is_valid_character = false; 
  		}

  		// catch if they inputted a % sign at any point other than the end of the input
  		if(index < value.size() - 1 && value[index] == '%')
  		{
  			is_valid_character = false; 
  		}

  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			// create a new string that removes the invalid character from the string
  			std::string_view prefix = value.substr(0, index);
  			std::string_view suffix = value.substr(index + 1);

  			// Combine the substrings 
  			std::string new_string = std::string(prefix) + std::string(suffix);

  			/* std::cout << "Original String: " << value << std::endl; */
  			/* std::cout << "String Without Character at Index " << index << ": " */
  			/* << new_string << std::endl; */

  			instance->input_currency3rate->value(new_string.c_str());
  		}
  	}
  }

  else if(which_input ==  instance->input_tbillsize)
  {
  	// input_currency2size
  	std::string_view value = instance->input_tbillsize->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < valid_characters.size(); i++)
  		{	
  			if(value[index] == valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// catch if they inputted a - sign at any point other than the start of the number  
  		if (index > 0 && value[index] == '-')
  		{
  			is_valid_character = false; 
  		}
  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			// create a new string that removes the invalid character from the string
  			std::string_view prefix = value.substr(0, index);
  			std::string_view suffix = value.substr(index + 1);

  			// Combine the substrings 
  			std::string new_string = std::string(prefix) + std::string(suffix);

  			/* std::cout << "Original String: " << value << std::endl; */
  			/* std::cout << "String Without Character at Index " << index << ": " */
  			/* << new_string << std::endl; */

  			instance->input_tbillsize->value(new_string.c_str());
  		}
  	}
  }
  else if(which_input == instance->input_tbillrate)
  {
  	// input_currency2rate
  	std::string_view value = instance->input_tbillrate->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < rate_valid_characters.size(); i++)
  		{	
  			if(value[index] == rate_valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// catch if they inputted a - sign at any point other than the start of the number  
  		if (index > 0 && value[index] == '-' )
  		{
  			is_valid_character = false; 
  		}

  		// catch if they inputted a % sign at any point other than the end of the input
  		if(index < value.size() - 1 && value[index] == '%')
  		{
  			is_valid_character = false; 
  		}

  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			// create a new string that removes the invalid character from the string
  			std::string_view prefix = value.substr(0, index);
  			std::string_view suffix = value.substr(index + 1);

  			// Combine the substrings 
  			std::string new_string = std::string(prefix) + std::string(suffix);

  			/* std::cout << "Original String: " << value << std::endl; */
  			/* std::cout << "String Without Character at Index " << index << ": " */
  			/* << new_string << std::endl; */

  			instance->input_tbillrate->value(new_string.c_str());
  		}
  	}
  }

  else if(which_input ==  instance->input_currency1proceeds)
  {
  	// input_currency2size
  	std::string_view value = instance->input_currency1proceeds->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < proceeds_valid_characters.size(); i++)
  		{	
  			if(value[index] == proceeds_valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			// create a new string that removes the invalid character from the string
  			std::string_view prefix = value.substr(0, index);
  			std::string_view suffix = value.substr(index + 1);

  			// Combine the substrings 
  			std::string new_string = std::string(prefix) + std::string(suffix);

  			/* std::cout << "Original String: " << value << std::endl; */
  			/* std::cout << "String Without Character at Index " << index << ": " */
  			/* << new_string << std::endl; */

  			instance->input_currency1proceeds->value(new_string.c_str());
  		}
  	}

  	// PROCEEDS CANNOT EXCEED POSITION THAT GENERATED THE PROCEEDS 
  	// obtain input values as doubles 
  	double currency1size = std::strtod(instance->input_currency1size->value(), nullptr);
  	double currency1proceeds = std::strtod(instance->input_currency1proceeds->value(), nullptr);

  	// if user inputted a higher number than what would be allowed 
  	if(currency1proceeds > std::abs(currency1size))
  	{
  		// Only allow two decimal places in the output 
  		std::ostringstream ss;
  		ss << std::fixed << std::setprecision(2) << std::abs(currency1size);

  		// set it to cap at the short currency position size 
  		instance->input_currency1proceeds->value(ss.str().c_str());
  	}
  }
  else if(which_input == instance->input_currency1proceedsrate)
  {
  	// input_currency2rate
  	std::string_view value = instance->input_currency1proceedsrate->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < rate_valid_characters.size(); i++)
  		{	
  			if(value[index] == rate_valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// catch if they inputted a - sign at any point other than the start of the number  
  		if (index > 0 && value[index] == '-' )
  		{
  			is_valid_character = false; 
  		}

  		// catch if they inputted a % sign at any point other than the end of the input
  		if(index < value.size() - 1 && value[index] == '%')
  		{
  			is_valid_character = false; 
  		}

  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			// create a new string that removes the invalid character from the string
  			std::string_view prefix = value.substr(0, index);
  			std::string_view suffix = value.substr(index + 1);

  			// Combine the substrings 
  			std::string new_string = std::string(prefix) + std::string(suffix);

  			/* std::cout << "Original String: " << value << std::endl; */
  			/* std::cout << "String Without Character at Index " << index << ": " */
  			/* << new_string << std::endl; */

  			instance->input_currency1proceedsrate->value(new_string.c_str());
  		}
  	}
  }
  else if(which_input ==  instance->input_currency2proceeds)
  {
  	// input_currency2size
  	std::string_view value = instance->input_currency2proceeds->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < proceeds_valid_characters.size(); i++)
  		{	
  			if(value[index] == proceeds_valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			// create a new string that removes the invalid character from the string
  			std::string_view prefix = value.substr(0, index);
  			std::string_view suffix = value.substr(index + 1);

  			// Combine the substrings 
  			std::string new_string = std::string(prefix) + std::string(suffix);

  			/* std::cout << "Original String: " << value << std::endl; */
  			/* std::cout << "String Without Character at Index " << index << ": " */
  			/* << new_string << std::endl; */

  			instance->input_currency2proceeds->value(new_string.c_str());
  		}
  	}

  	// PROCEEDS CANNOT EXCEED POSITION THAT GENERATED THE PROCEEDS 
  	// obtain input values as doubles 
  	double currency2size = std::strtod(instance->input_currency2size->value(), nullptr);
  	double currency2proceeds = std::strtod(instance->input_currency2proceeds->value(), nullptr);


  	// if user inputted a higher number than what would be allowed 
  	if(currency2proceeds > std::abs(currency2size))
  	{
  		// Only allow two decimal places in the output 
  		std::ostringstream ss;
  		ss << std::fixed << std::setprecision(2) << std::abs(currency2size);

  		// set it to cap at the short currency position size 
  		instance->input_currency2proceeds->value(ss.str().c_str());

  	}
  }
  else if(which_input == instance->input_currency2proceedsrate)
  {
  	// input_currency2rate
  	std::string_view value = instance->input_currency2proceedsrate->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < rate_valid_characters.size(); i++)
  		{	
  			if(value[index] == rate_valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// catch if they inputted a - sign at any point other than the start of the number  
  		if (index > 0 && value[index] == '-' )
  		{
  			is_valid_character = false; 
  		}

  		// catch if they inputted a % sign at any point other than the end of the input
  		if(index < value.size() - 1 && value[index] == '%')
  		{
  			is_valid_character = false; 
  		}

  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			// create a new string that removes the invalid character from the string
  			std::string_view prefix = value.substr(0, index);
  			std::string_view suffix = value.substr(index + 1);

  			// Combine the substrings 
  			std::string new_string = std::string(prefix) + std::string(suffix);

  			/* std::cout << "Original String: " << value << std::endl; */
  			/* std::cout << "String Without Character at Index " << index << ": " */
  			/* << new_string << std::endl; */

  			/* std::cout << "debug new string: " << new_value << std::endl; */
  			instance->input_currency2proceedsrate->value(new_string.c_str());
  		}
  	}
  }
  else if(which_input ==  instance->input_currency3proceeds)
  {
  	// input_currency2size
  	std::string_view value = instance->input_currency3proceeds->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < proceeds_valid_characters.size(); i++)
  		{	
  			if(value[index] == proceeds_valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			// create a new string that removes the invalid character from the string
  			std::string_view prefix = value.substr(0, index);
  			std::string_view suffix = value.substr(index + 1);

  			// Combine the substrings 
  			std::string new_string = std::string(prefix) + std::string(suffix);

  			/* std::cout << "Original String: " << value << std::endl; */
  			/* std::cout << "String Without Character at Index " << index << ": " */
  			/* << new_string << std::endl; */

  			instance->input_currency3proceeds->value(new_string.c_str());
  		}
  	}

  	// PROCEEDS CANNOT EXCEED POSITION THAT GENERATED THE PROCEEDS 
  	// obtain input values as doubles 
  	double currency3size = std::strtod(instance->input_currency3size->value(), nullptr);
  	double currency3proceeds = std::strtod(instance->input_currency3proceeds->value(), nullptr);

  	// if user inputted a higher number than what would be allowed 
  	if(currency3proceeds > std::abs(currency3size))
  	{
  		// Only allow two decimal places in the output 
  		std::ostringstream ss;
  		ss << std::fixed << std::setprecision(2) << std::abs(currency3size);

  		// set it to cap at the short currency position size 
  		instance->input_currency3proceeds->value(ss.str().c_str());
  	}
  }
  else if(which_input == instance->input_currency3proceedsrate)
  {
  	// input_currency2rate
  	std::string_view value = instance->input_currency3proceedsrate->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < rate_valid_characters.size(); i++)
  		{	
  			if(value[index] == rate_valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// catch if they inputted a - sign at any point other than the start of the number  
  		if (index > 0 && value[index] == '-' )
  		{
  			is_valid_character = false; 
  		}

  		// catch if they inputted a % sign at any point other than the end of the input
  		if(index < value.size() - 1 && value[index] == '%')
  		{
  			is_valid_character = false; 
  		}

  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			// create a new string that removes the invalid character from the string
  			std::string_view prefix = value.substr(0, index);
  			std::string_view suffix = value.substr(index + 1);

  			// Combine the substrings 
  			std::string new_string = std::string(prefix) + std::string(suffix);

  			/* std::cout << "Original String: " << value << std::endl; */
  			/* std::cout << "String Without Character at Index " << index << ": " */
  			/* << new_string << std::endl; */

  			instance->input_currency3proceedsrate->value(new_string.c_str());
  		}
  	}
  }
  else if(which_input ==  instance->input_tbillproceeds)
  {
  	// input_currency2size
  	std::string_view value = instance->input_tbillproceeds->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < proceeds_valid_characters.size(); i++)
  		{	
  			if(value[index] == proceeds_valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			// create a new string that removes the invalid character from the string
  			std::string_view prefix = value.substr(0, index);
  			std::string_view suffix = value.substr(index + 1);

  			// Combine the substrings 
  			std::string new_string = std::string(prefix) + std::string(suffix);

  			/* std::cout << "Original String: " << value << std::endl; */
  			/* std::cout << "String Without Character at Index " << index << ": " */
  			/* << new_string << std::endl; */

  			instance->input_tbillproceeds->value(new_string.c_str());
  		}
  	}

  	// PROCEEDS CANNOT EXCEED POSITION THAT GENERATED THE PROCEEDS 
  	// obtain input values as doubles 
  	double tbillsize = std::strtod(instance->input_tbillsize->value(), nullptr);
  	double tbillproceeds = std::strtod(instance->input_tbillproceeds->value(), nullptr);

  	// if user inputted a higher number than what would be allowed 
  	if(tbillproceeds > std::abs(tbillsize))
  	{
  		// Only allow two decimal places in the output 
  		std::ostringstream ss;
  		ss << std::fixed << std::setprecision(2) << std::abs(tbillsize);

  		// set it to cap at the short currency position size 
  		instance->input_tbillproceeds->value(ss.str().c_str());
  	}
  }
  else if(which_input == instance->input_tbillproceedsrate)
  {
  	// input_currency2rate
  	std::string_view value = instance->input_tbillproceedsrate->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < rate_valid_characters.size(); i++)
  		{	
  			if(value[index] == rate_valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// catch if they inputted a - sign at any point other than the start of the number  
  		if (index > 0 && value[index] == '-' )
  		{
  			is_valid_character = false; 
  		}

  		// catch if they inputted a % sign at any point other than the end of the input
  		if(index < value.size() - 1 && value[index] == '%')
  		{
  			is_valid_character = false; 
  		}

  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			// create a new string that removes the invalid character from the string
  			std::string_view prefix = value.substr(0, index);
  			std::string_view suffix = value.substr(index + 1);

  			// Combine the substrings 
  			std::string new_string = std::string(prefix) + std::string(suffix);

  			/* std::cout << "Original String: " << value << std::endl; */
  			/* std::cout << "String Without Character at Index " << index << ": " */
  			/* << new_string << std::endl; */

  			instance->input_tbillproceedsrate->value(new_string.c_str());
  		}
  	}
  }
  else if(which_input ==  instance->input_fxrate1)
  {
  	// input_currency2size
  	std::string_view value = instance->input_fxrate1->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < proceeds_valid_characters.size(); i++)
  		{	
  			if(value[index] == proceeds_valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			std::string_view new_value = value.substr(0, value.size() -1);

  			/* std::cout << "debug new string: " << new_value << std::endl; */
  			instance->input_fxrate1->value(std::string(new_value).c_str());
  		}
  	}
  }
  else if(which_input ==  instance->input_fxrate2)
  {
  	// input_currency2size
  	std::string_view value = instance->input_fxrate2->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < proceeds_valid_characters.size(); i++)
  		{	
  			if(value[index] == proceeds_valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			std::string_view new_value = value.substr(0, value.size() -1);

  			/* std::cout << "debug new string: " << new_value << std::endl; */
  			instance->input_fxrate2->value(std::string(new_value).c_str());
  		}
  	}
  }
  else if(which_input ==  instance->input_fxrate3)
  {
  	// input_currency2size
  	std::string_view value = instance->input_fxrate3->value();

  	// for each character in the string
  	for(int index = 0; index < value.size(); index++)
  	{
  		// compare current character of the input string against list of valid
  		// characters to see if the character is valid.
  		bool is_valid_character = false;

  		// iterate over valid characters array and remember if we got a match
  		for(int i = 0; i < proceeds_valid_characters.size(); i++)
  		{	
  			if(value[index] == proceeds_valid_characters[i])
  			{
  				// character is valid
  				is_valid_character = true;
  				break;
  			}
  		}
  		// if we went through the nested for loop and no valid match was found,
  		// it means we have found an invalid character at the current index. 
  		if(is_valid_character == false)
  		{
  			std::string_view new_value = value.substr(0, value.size() -1);

  			/* std::cout << "debug new string: " << new_value << std::endl; */
  			instance->input_fxrate3->value(std::string(new_value).c_str());
  		}
  	}
  }
}

void main_window::save_CB(Fl_Widget*w, void* userdata) {
  // Set up to read existing sqlite3 database (or create if not exists)

  main_window* instance = static_cast<main_window*>(userdata);
  // open the database
  sqlite3_open(dbPath, &db);

  // Create the table 
  const char* sql = "CREATE TABLE IF NOT EXISTS MAINWINDOW_INPUTS("
  	"ID INTEGER PRIMARY KEY, "
  	"CURRENCY1SIZE TEXT, "
  	"CURRENCY1RATE TEXT, "
  	"CURRENCY2SIZE TEXT, "
  	"CURRENCY2RATE TEXT, "
  	"CURRENCY3SIZE TEXT, "
  	"CURRENCY3RATE TEXT, "
  	"TBILLSIZE TEXT, "
  	"TBILLRATE TEXT, "
  	"CURRENCY1PROCEEDS_SIZE TEXT, "
  	"CURRENCY1PROCEEDS_RATE TEXT, "
  	"CURRENCY2PROCEEDS_SIZE TEXT, "
  	"CURRENCY2PROCEEDS_RATE TEXT, "
  	"CURRENCY3PROCEEDS_SIZE TEXT, "
  	"CURRENCY3PROCEEDS_RATE TEXT, "
  	"TBILLPROCEEDS_SIZE TEXT, "
  	"TBILLPROCEEDS_RATE TEXT, "
  	"FXRATE1 TEXT, "
  	"FXRATE2 TEXT, "
  	"FXRATE3 TEXT, "
  	"C1_COVERCURRENCY INT, "
  	"C2_COVERCURRENCY INT, "
  	"C3_COVERCURRENCY INT, "
  	"C4_COVERCURRENCY INT);";

  int result = sqlite3_exec(db, sql, NULL, 0, &err);
  if(result != SQLITE_OK)
  {
  	/* std::cout << "error creating table: " << err << std::endl; */
  	sqlite3_free(err); // need to look into that function
  }

  // ADD OR REPLACE THE ROW IN OUR TABLE 
  const std::string insert_sql = "INSERT OR REPLACE INTO MAINWINDOW_INPUTS ("
  	"ID, "
  	"CURRENCY1SIZE, "
  	"CURRENCY1RATE, "
      "CURRENCY2SIZE, "
      "CURRENCY2RATE, "
      "CURRENCY3SIZE, "
  	"CURRENCY3RATE, "
  	"TBILLSIZE, "
  	"TBILLRATE, "
  	"CURRENCY1PROCEEDS_SIZE, "
      "CURRENCY1PROCEEDS_RATE, "
      "CURRENCY2PROCEEDS_SIZE, "
      "CURRENCY2PROCEEDS_RATE, "
      "CURRENCY3PROCEEDS_SIZE, "
      "CURRENCY3PROCEEDS_RATE, "
      "TBILLPROCEEDS_SIZE, "
      "TBILLPROCEEDS_RATE, "
      "FXRATE1, "
      "FXRATE2, "
      "FXRATE3) "
   	" VALUES((SELECT ID FROM MAINWINDOW_INPUTS LIMIT 1),"
  	"'"+ std::string(instance->input_currency1size->value()) +"', "
  	"'"+ std::string(instance->input_currency1rate->value()) +"', "
  	"'"+ std::string(instance->input_currency2size->value()) +"', "
  	"'"+ std::string(instance->input_currency2rate->value()) +"', "
  	"'"+ std::string(instance->input_currency3size->value()) +"', "
  	"'"+ std::string(instance->input_currency3rate->value()) +"', "
  	"'"+ std::string(instance->input_tbillsize->value()) +"', "
  	"'"+ std::string(instance->input_tbillrate->value()) +"', "
  	"'"+ std::string(instance->input_currency1proceeds->value()) +"', "
  	"'"+ std::string(instance->input_currency1proceedsrate->value()) +"', "
  	"'"+ std::string(instance->input_currency2proceeds->value()) +"', "
  	"'"+ std::string(instance->input_currency2proceedsrate->value()) +"', "
  	"'"+ std::string(instance->input_currency3proceeds->value()) +"', "
  	"'"+ std::string(instance->input_currency3proceedsrate->value()) +"', "
  	"'"+ std::string(instance->input_tbillproceeds->value()) +"', "
  	"'"+ std::string(instance->input_tbillproceedsrate->value()) +"', "
  	"'"+ std::string(instance->input_fxrate1->value()) +"', "
  	"'"+ std::string(instance->input_fxrate2->value()) +"', "
  	"'"+ std::string(instance->input_fxrate3->value()) +"');"; 

  /* std::cout << "sql debug: " << insert_sql << std::endl; */

  result = sqlite3_exec(db, insert_sql.c_str(), NULL, 0, &err); 
  if(result != SQLITE_OK) 
  { 
  	std::cout << "error with insert: " << err << std::endl; 
  	sqlite3_free(err); // need to look into that function 
  } 
  else
  {
  	/* std::cout << "inserted: " << result << std::endl; */
  }
  	
  sqlite3_close(db); // close the database
}

void main_window::load_CB(Fl_Widget*w, void* userdata) {
  // POPULATE SAVED INPUT VALUES FROM DATABASE 

  // get pointer to our object in question 
  main_window* instance = static_cast<main_window*>(userdata);

  // open db
  sqlite3_open(dbPath, &db);

  const char* checkTableExistsQuery = "SELECT name FROM sqlite_master WHERE type='table' AND name='MAINWINDOW_INPUTS';";

  if (sqlite3_prepare_v2(db, checkTableExistsQuery, -1, &stmt, nullptr) == SQLITE_OK) 
  {
  	if (sqlite3_step(stmt) == SQLITE_ROW) 
  	{
  		/* std::cout << "Table exists!" << std::endl; */

  		// FINALIZE before going to next prepared statement 
  		int finalize = sqlite3_finalize(stmt);
  		if(finalize != SQLITE_OK) 
  		{
  			std::cout<< finalize << std::endl; 
  		}
  		/* else */
  		/* { */
  			/* std::cout << " SQL Statement Finalized..." << std::endl; */
  		/* } */

  		// prepare statement to obtain values from the DB 
  		sqlite3_prepare_v2(db, "SELECT * from MAINWINDOW_INPUTS LIMIT 1", -1, &stmt, 0); 

  		while(sqlite3_step(stmt) != SQLITE_DONE) 
  		{ 
  			const unsigned char* currency1size = sqlite3_column_text(stmt, 1);	
  			instance->input_currency1size->value((const char*)currency1size);

  			const unsigned char* currency1rate = sqlite3_column_text(stmt, 2);	
  			instance->input_currency1rate->value((const char*)currency1rate);

  			const unsigned char* currency2size = sqlite3_column_text(stmt, 3);	
  			instance->input_currency2size->value((const char*)currency2size);

  			const unsigned char* currency2rate = sqlite3_column_text(stmt, 4);	
  			instance->input_currency2rate->value((const char*)currency2rate);

  			const unsigned char* currency3size = sqlite3_column_text(stmt, 5);	
  			instance->input_currency3size->value((const char*)currency3size);

  			const unsigned char* currency3rate = sqlite3_column_text(stmt, 6);	
  			instance->input_currency3rate->value((const char*)currency3rate);

  			const unsigned char* tbillsize = sqlite3_column_text(stmt, 7);	
  			instance->input_tbillsize->value((const char*)tbillsize);

  			const unsigned char* tbillrate = sqlite3_column_text(stmt, 8);	
  			instance->input_tbillrate->value((const char*)tbillrate);

  			const unsigned char* currency1proceeds_size = sqlite3_column_text(stmt, 9);	
  			instance->input_currency1proceeds->value((const char*)currency1proceeds_size);

  			const unsigned char* currency1proceeds_rate = sqlite3_column_text(stmt, 10);	
  			instance->input_currency1proceedsrate->value((const char*)currency1proceeds_rate);

  			const unsigned char* currency2proceeds_size = sqlite3_column_text(stmt, 11);	
  			instance->input_currency2proceeds->value((const char*)currency2proceeds_size);

  			const unsigned char* currency2proceeds_rate = sqlite3_column_text(stmt, 12);	
  			instance->input_currency2proceedsrate->value((const char*)currency2proceeds_rate);

  			const unsigned char* currency3proceeds_size = sqlite3_column_text(stmt, 13);	
  			instance->input_currency3proceeds->value((const char*)currency3proceeds_size);

  			const unsigned char* currency3proceeds_rate = sqlite3_column_text(stmt, 14);	
  			instance->input_currency3proceedsrate->value((const char*)currency3proceeds_rate);

  			const unsigned char* tbillproceeds = sqlite3_column_text(stmt, 15);	
  			instance->input_tbillproceeds->value((const char*)tbillproceeds);

  			const unsigned char* tbillproceeds_rate = sqlite3_column_text(stmt, 16);	
  			instance->input_tbillproceedsrate->value((const char*)tbillproceeds_rate);

  			const unsigned char* fxrate1 = sqlite3_column_text(stmt, 17);	
  			instance->input_fxrate1->value((const char*)fxrate1);

  			const unsigned char* fxrate2 = sqlite3_column_text(stmt, 18);	
  			instance->input_fxrate2->value((const char*)fxrate2);

  			const unsigned char* fxrate3 = sqlite3_column_text(stmt, 19);	
  			instance->input_fxrate3->value((const char*)fxrate3);
  		}

  		// The application must finalize every prepared statement in order to avoid resource leaks.
  		// // finalize second statement 
  		finalize = sqlite3_finalize(stmt);
  		if(finalize != SQLITE_OK) 
  		{
  			std::cout<< finalize << std::endl; 
  		}
  		/* else */
  		/* { */
  			/* std::cout << " SQL Statement Finalized..." << std::endl; */
  		/* } */
  	} 
  	/* else */ 
  	/* { */
  	/* 	std::cout << "Table does not exist. Gracefully doing nothing..." << std::endl; */
  	/* } */

  } 
  else 
  {
  	std::cerr << "Error preparing SQL statement" << std::endl;
  }

  sqlite3_close(db); // close the database
}

void main_window::clear_CB(Fl_Widget*w, void* userdata) {
  //// POPULATE SAVED INPUT VALUES FROM DATABASE 
  // this callback will reset the items in the input fields and clear what is in the SQLite3 database.

  // get pointer to our object in question 
  main_window* instance = static_cast<main_window*>(userdata);

  // open db
  sqlite3_open(dbPath, &db);

  const char* checkTableExistsQuery = "SELECT name FROM sqlite_master WHERE type='table' AND name='MAINWINDOW_INPUTS';";

  if (sqlite3_prepare_v2(db, checkTableExistsQuery, -1, &stmt, nullptr) == SQLITE_OK) 
  {
  	if (sqlite3_step(stmt) == SQLITE_ROW) 
  	{
  		/* std::cout << "Table exists!" << std::endl; */

  		// FINALIZE before going to next prepared statement 
  		int finalize = sqlite3_finalize(stmt);
  		if(finalize != SQLITE_OK) 
  		{
  			std::cout<< finalize << std::endl; 
  		}
  		/* else */
  		/* { */
  			/* std::cout << " SQL Statement Finalized..." << std::endl; */
  		/* } */

  		// prepare statement to obtain values from the DB 
  		sqlite3_prepare_v2(db, "SELECT * from MAINWINDOW_INPUTS LIMIT 1", -1, &stmt, 0); 

  		while(sqlite3_step(stmt) != SQLITE_DONE) 
  		{ 
  			const unsigned char* currency1size = sqlite3_column_text(stmt, 1);	
  			instance->input_currency1size->value("");

  			const unsigned char* currency1rate = sqlite3_column_text(stmt, 2);	
  			instance->input_currency1rate->value("");

  			const unsigned char* currency2size = sqlite3_column_text(stmt, 3);	
  			instance->input_currency2size->value("");

  			const unsigned char* currency2rate = sqlite3_column_text(stmt, 4);	
  			instance->input_currency2rate->value("");

  			const unsigned char* currency3size = sqlite3_column_text(stmt, 5);	
  			instance->input_currency3size->value("");

  			const unsigned char* currency3rate = sqlite3_column_text(stmt, 6);	
  			instance->input_currency3rate->value("");

  			const unsigned char* tbillsize = sqlite3_column_text(stmt, 7);	
  			instance->input_tbillsize->value("");

  			const unsigned char* tbillrate = sqlite3_column_text(stmt, 8);	
  			instance->input_tbillrate->value("");

  			const unsigned char* currency1proceeds_size = sqlite3_column_text(stmt, 9);	
  			instance->input_currency1proceeds->value("");

  			const unsigned char* currency1proceeds_rate = sqlite3_column_text(stmt, 10);	
  			instance->input_currency1proceedsrate->value("");

  			const unsigned char* currency2proceeds_size = sqlite3_column_text(stmt, 11);	
  			instance->input_currency2proceeds->value("");

  			const unsigned char* currency2proceeds_rate = sqlite3_column_text(stmt, 12);	
  			instance->input_currency2proceedsrate->value("");

  			const unsigned char* currency3proceeds_size = sqlite3_column_text(stmt, 13);	
  			instance->input_currency3proceeds->value("");

  			const unsigned char* currency3proceeds_rate = sqlite3_column_text(stmt, 14);	
  			instance->input_currency3proceedsrate->value("");

  			const unsigned char* tbillproceeds = sqlite3_column_text(stmt, 15);	
  			instance->input_tbillproceeds->value("");

  			const unsigned char* tbillproceeds_rate = sqlite3_column_text(stmt, 16);	
  			instance->input_tbillproceedsrate->value("");

  			const unsigned char* fxrate1 = sqlite3_column_text(stmt, 17);	
  			instance->input_fxrate1->value("");

  			const unsigned char* fxrate2 = sqlite3_column_text(stmt, 18);	
  			instance->input_fxrate2->value("");

  			const unsigned char* fxrate3 = sqlite3_column_text(stmt, 19);	
  			instance->input_fxrate3->value("");
  		}

  		// The application must finalize every prepared statement in order to avoid resource leaks.
  		// // finalize second statement 
  		finalize = sqlite3_finalize(stmt);
  		if(finalize != SQLITE_OK) 
  		{
  			std::cout<< finalize << std::endl; 
  		}
  		/* else */
  		/* { */
  			/* std::cout << " SQL Statement Finalized..." << std::endl; */
  		/* } */

  	} 
  	/* else */ 
  	/* { */
  	/* 	std::cout << "Table does not exist. Gracefully doing nothing..." << std::endl; */
  	/* } */
  } 
  else 
  {
  	std::cerr << "Error preparing SQL statement" << std::endl;
  }

  sqlite3_close(db); // close the database
}

void main_window::invert_CB(Fl_Widget*w, void* userdata) {
  // Handle the inversion of the FX rate for quick swaps. 

  // get pointer to our object in question 
  main_window* instance = static_cast<main_window*>(userdata);

  // check the widget pointer against various widgets to determine which one 
  // triggered this callback 
  Fl_Button* which_button = static_cast<Fl_Button*>(w);

  /* std::cout << "button: " << which_button << std::endl; */
  if(which_button == instance->invert_rate1)
  {
  	double previous_value = std::strtod(instance->input_fxrate1->value(), nullptr);
  	double reverse_rate = 1 / previous_value;

  	// Allow five decimal places in the output 
  	std::ostringstream ss;
  	ss << std::fixed << std::setprecision(5) << std::abs(reverse_rate);

  	instance->input_fxrate1->value(ss.str().c_str());

  }
  else if(which_button == instance->invert_rate2)
  {
  	double previous_value = std::strtod(instance->input_fxrate2->value(), nullptr);
  	double reverse_rate = 1 / previous_value;

  	// Allow five decimal places in the output 
  	std::ostringstream ss;
  	ss << std::fixed << std::setprecision(5) << std::abs(reverse_rate);

  	instance->input_fxrate2->value(ss.str().c_str());

  }
  else if(which_button == instance->invert_rate3)
  {
  	double previous_value = std::strtod(instance->input_fxrate3->value(), nullptr);
  	double reverse_rate = 1 / previous_value;

  	// Allow five decimal places in the output 
  	std::ostringstream ss;
  	ss << std::fixed << std::setprecision(5) << std::abs(reverse_rate);

  	instance->input_fxrate3->value(ss.str().c_str());
  }
}

void main_window::Calculate_CB(Fl_Widget*w, void* userdata) {
  // COMPUTE RESULTS CALLBACK 
  //
  // grab pointer to main window object 
  main_window* instance = static_cast<main_window*>(userdata);

  // Create an instance of new Result Window 
  if(instance->p_result_window == nullptr)
  {
  	instance->p_result_window = new results_window(444, 150, 471, 727, "Interest Calculator Results");

  	// pass in the pointer of main window into results window 
  	instance->p_result_window->setWindowPointers(instance);
  }
  else
  {
  	instance->p_result_window->show();
  }


  // Load the values from the DB for currencies to cover 
  // open (ALREADY IN ABOVE CODE)
  sqlite3_open(dbPath, &db);

  const char* checkTableExistsQuery = "SELECT name FROM sqlite_master WHERE type='table' AND name='MAINWINDOW_INPUTS';";

  if (sqlite3_prepare_v2(db, checkTableExistsQuery, -1, &stmt, nullptr) == SQLITE_OK) 
  {
  	if (sqlite3_step(stmt) == SQLITE_ROW) 
  	{
  		/* std::cout << "Table exists!" << std::endl; */

  		// FINALIZE before going to next prepared statement 
  		int finalize = sqlite3_finalize(stmt);
  		if(finalize != SQLITE_OK) 
  		{
  			std::cout<< finalize << std::endl; 
  		}
  		/* else */
  		/* { */
  			/* std::cout << " SQL Statement Finalized..." << std::endl; */
  		/* } */

  		// prepare statement to obtain values from the DB 
  		sqlite3_prepare_v2(db, "SELECT * from MAINWINDOW_INPUTS LIMIT 1", -1, &stmt, 0); 

  		while(sqlite3_step(stmt) != SQLITE_DONE) 
  		{ 
  			int C1_covercurrency = sqlite3_column_int(stmt, 20);	
  			instance->p_result_window->C1_currencychoice->value(C1_covercurrency);

  			int C2_covercurrency = sqlite3_column_int(stmt, 21);	
  			instance->p_result_window->C2_currencychoice->value(C2_covercurrency);

  			int C3_covercurrency = sqlite3_column_int(stmt, 22);	
  			instance->p_result_window->C3_currencychoice->value(C3_covercurrency);

  			int C4_covercurrency = sqlite3_column_int(stmt, 23);	
  			instance->p_result_window->C4_currencychoice->value(C4_covercurrency);
  		}

  		// The application must finalize every prepared statement in order to avoid resource leaks.
  		// // finalize second statement 
  		finalize = sqlite3_finalize(stmt);
  		if(finalize != SQLITE_OK) 
  		{
  			std::cout<< finalize << std::endl; 
  		}
  		/* else */
  		/* { */
  			/* std::cout << " SQL Statement Finalized..." << std::endl; */
  		/* } */
  	} 
  	/* else */ 
  	/* { */
  	/* 	std::cout << "Table does not exist. Gracefully doing nothing..." << std::endl; */
  	/* } */
  } 
  else 
  {
  	std::cerr << "Error preparing SQL statement" << std::endl;
  }

  sqlite3_close(db); // close the database
  //
  // Some of the calculations are borrowed from int calculator v1
  // The first thing we should do is safely obtain all of the user inputs 
  // all amounts are obtained as doubles
  //
  // const char* myvalue = input_currency1size->value(); Convert the const char*
  // to double (this function automatically removes invalid characters from the
  // end of the double

  // obtain the 4 primary position sizes 
  double currency1size = std::strtod(instance->input_currency1size->value(), nullptr);
  double currency2size = std::strtod(instance->input_currency2size->value(), nullptr);
  double currency3size = std::strtod(instance->input_currency3size->value(), nullptr);
  double tbillsize = std::strtod(instance->input_tbillsize->value(), nullptr);

  // obtain the 4 primary position interest rates 
  double currency1rate = std::strtod(instance->input_currency1rate->value(), nullptr) / 100;
  double currency2rate = std::strtod(instance->input_currency2rate->value(), nullptr) / 100;
  double currency3rate = std::strtod(instance->input_currency3rate->value(), nullptr) / 100;
  double tbillrate = std::strtod(instance->input_tbillrate->value(), nullptr) / 100;

  // obtain the 4 proceeds position sizes 
  double currency1proceeds = std::strtod(instance->input_currency1proceeds->value(), nullptr);
  double currency2proceeds = std::strtod(instance->input_currency2proceeds->value(), nullptr);
  double currency3proceeds = std::strtod(instance->input_currency3proceeds->value(), nullptr);
  double tbillproceeds = std::strtod(instance->input_tbillproceeds->value(), nullptr);

  // obtain the 4 proceeds position interest rates 
  double currency1proceeds_rate = std::strtod(instance->input_currency1proceedsrate->value(), nullptr) / 100;
  double currency2proceeds_rate = std::strtod(instance->input_currency2proceedsrate->value(), nullptr) / 100;
  double currency3proceeds_rate = std::strtod(instance->input_currency3proceedsrate->value(), nullptr) / 100;
  double tbillproceeds_rate = std::strtod(instance->input_tbillproceedsrate->value(), nullptr) / 100;

  // obtain fx rate change values 
  double fxrate1 = std::strtod(instance->input_fxrate1->value(), nullptr);
  double fxrate2 = std::strtod(instance->input_fxrate2->value(), nullptr);
  double fxrate3 = std::strtod(instance->input_fxrate3->value(), nullptr);

  // debug 
  /* std::cout << "tbillsize: " << tbillsize << " tbillrate: " << tbillrate << */
  /* " 1size: " << currency1size << " 1rate: " << currency1rate << " 2size: " << */ 
  /* currency2size << " 2rate: " << currency2rate << " 3size: " */ 
  /* << currency3size << " 3rate: " << currency3rate << std::endl; */

  // debug 
  /* std::cout << "new fx rate: " << fxrate1 << std::endl; */

  // variables to use 
  double blended_one_year_profit = 0;
  double total_currency = 0;
  double blended_percent_return = 0;

  // items for cost to cover calculation
  double c1_cost_to_cover = 0;
  double c2_cost_to_cover = 0;
  double c3_cost_to_cover = 0;

  // used to sum negative balances 
  double total_debt = 0;

  // fx rate values will be 0 if they have not been set by the user.

  // so basically we want to calculate the rate of return for each position 
  // and then get the sum of those rate of returns to get the total rate 
  if(currency1size != 0)
  {
  	if(currency1size > 0)
  	{
  		total_currency += currency1size;
  	}

  	// solve for the daily int rate 
  	double daily_int_rate = currency1rate / 365;

  	// one year PL 
  	double one_year_profit = (currency1size * currency1rate) ;

  	// daily profit 
  	double daily_profit = one_year_profit / 365;

  	// solve for one year percent rate of return 
  	double final_percent_return = (one_year_profit / currency1size) * 100;

  	// if profit is negative, calculate cost to cover using cover currency input 
  	if(one_year_profit < 0)
  	{
  		// since our profit is negative we have a negative balance so append it to the total debt
  		total_debt += currency1size;

  		// get selected cover currency from FL_Input
  		int cover_currency = instance->p_result_window->C1_currencychoice->value();
  		switch (cover_currency) 
  		{
  		case 0:
  			{
  			// CASE 0 IS THE CURRENT CURRENCY.
  			// No exchange rate possible.

  			c1_cost_to_cover = std::abs(currency1size);

  			// Formatting will be same as below 
  			boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  			"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency1size 
  			%final_percent_return %one_year_profit %daily_profit %c1_cost_to_cover;

  			std::string c1_result = fmt.str();

  			// create buffer and assign it to the appropriate text display widget 
  			Fl_Text_Buffer *c1buff = new Fl_Text_Buffer();
  			instance->p_result_window->c1_output->buffer(c1buff);

  			// add the text to our buffer (to the box)
  			c1buff->text(c1_result.c_str());

  			// append the values to our total
  			blended_one_year_profit += one_year_profit;
  			}
  			break;
  		case 1:
  			if(fxrate1 != 0) // they needed to input an exchange rate  
  			{
  				// cover with currency2 (fxrate1 is between C1 and C2 so we reverse the rate here)
  				c1_cost_to_cover = std::abs(currency1size / (1 / fxrate1) ); 

  				// format text and set it  
  				boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  				"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency1size 
  				%final_percent_return %one_year_profit %daily_profit %c1_cost_to_cover;

  				// write it to string
  				std::string c1_result = fmt.str();

  				// create buffer and assign it to the appropriate text display widget 
  				Fl_Text_Buffer *c1buff = new Fl_Text_Buffer();
  				instance->p_result_window->c1_output->buffer(c1buff);

  				// add the text to our buffer (to the box)
  				c1buff->text(c1_result.c_str());

  				// append the values to our total
  				blended_one_year_profit += one_year_profit;

  			}
  			else
  			{
  				std::cout << "Error: Currency 1 Cover currency is Currency 2." 
  				" User did not enter FX rate between these currencies." << std::endl;
  			}
  			break;
  		case 2:
  			if(fxrate3 != 0) // they needed to input an exchange rate  
  			{
  				// cover with currency3 (fxrate3 is between C1 and C3)
  				c1_cost_to_cover = std::abs(currency1size / (1 / fxrate3) ) ;	

  				// format text and set it  
  				boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  				"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency1size 
  				%final_percent_return %one_year_profit %daily_profit %c1_cost_to_cover;

  				// write it to string
  				std::string c1_result = fmt.str();

  				// create buffer and assign it to the appropriate text display widget 
  				Fl_Text_Buffer *c1buff = new Fl_Text_Buffer();
  				instance->p_result_window->c1_output->buffer(c1buff);

  				// add the text to our buffer (to the box)
  				c1buff->text(c1_result.c_str());

  				// append the values to our total
  				blended_one_year_profit += one_year_profit;
  			}
  			else
  			{
  				std::cout << "Error: Currency 1 Cover currency is Currency 3." 
  				" User did not enter FX rate between these currencies." << std::endl;
  			}
  			break;
  		}

  	}		
  	else // Profit is positive or flat therefore no cost to cover calculation
  	{
  		boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  		"One Year P/L: $%3%\nDaily P/L: $%4%") %currency1size %final_percent_return 
  		%one_year_profit %daily_profit;

  		std::string c1_result = fmt.str();

  		/* std::cout << c1_result << std::endl; */

  		// This should maybe be global but not sure. It works. 
  		// create buffer and assign it to the appropriate text display widget 
  		Fl_Text_Buffer *c1buff = new Fl_Text_Buffer();
  		instance->p_result_window->c1_output->buffer(c1buff);

  		// add the text to our buffer (to the box)
  		c1buff->text(c1_result.c_str());

  		// append the values to our total
  		blended_one_year_profit += one_year_profit;
  	}
  }

  if(currency2size != 0)
  {
  	if(currency2size > 0)
  	{
  		total_currency += currency2size;
  	}

  	/* std::cout << "total currency: " << total_currency << std::endl; */

  	// solve for the daily int rate 
  	double daily_int_rate = currency2rate / 365;

  	// calculate one year rate of return 
  	double one_year_profit = (currency2size * currency2rate) ;

  	// daily profit 
  	double daily_profit = one_year_profit / 365;

  	// solve for one year percent rate of return 
  	double final_percent_return = (one_year_profit / currency2size) * 100;

  	// if profit is negative, calculate cost to cover using cover currency input 
  	if(one_year_profit < 0)
  	{
  		// since our profit is negative we have a negative balance so append it to the total debt
  		total_debt += currency2size;

  		// get selected cover currency from FL_Input
  		int cover_currency = instance->p_result_window->C2_currencychoice->value();
  		switch (cover_currency) 
  		{
  		case 0: // Currency1
  			if(fxrate1 != 0) // they needed to input an exchange rate  
  			{
  				// cover with currency1 (fxrate1 is between C1 and C2)
  				c2_cost_to_cover = std::abs(currency2size / fxrate1);	

  				// format text and set it  
  				boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  				"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency2size 
  				%final_percent_return %one_year_profit %daily_profit %c2_cost_to_cover;

  				// write it to string
  				std::string c2_result = fmt.str();

  				// create buffer and assign it to the appropriate text display widget 
  				Fl_Text_Buffer *c2buff = new Fl_Text_Buffer();
  				instance->p_result_window->c2_output->buffer(c2buff);

  				// add the text to our buffer (to the box)
  				c2buff->text(c2_result.c_str());

  				// append the values to our total
  				blended_one_year_profit += one_year_profit;

  			}
  			else
  			{
  				std::cout << "Error: Currency 2 Cover currency is Currency 1." 
  				" User did not enter FX rate between these currencies." << std::endl;
  			}
  			break;
  		case 1: //Currency2
  			{
  			// CASE NOT APPLICABLE BECAUSE IT IS THE CURRENT CURRENCY.
  			// No exchange rate possible.
  			
  			c2_cost_to_cover = std::abs(currency2size);

  			// Formatting will be same as below 
  			boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  			"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency2size %final_percent_return 
  			%one_year_profit %daily_profit %c2_cost_to_cover;

  			std::string c2_result = fmt.str();

  			// create buffer and assign it to the appropriate text display widget 
  			Fl_Text_Buffer *c2buff = new Fl_Text_Buffer();
  			instance->p_result_window->c2_output->buffer(c2buff);

  			// add the text to our buffer (to the box)
  			c2buff->text(c2_result.c_str());

  			// append the values to our total
  			blended_one_year_profit += one_year_profit;
  			}
  			break;
  		case 2:
  			if(fxrate2 != 0) // they needed to input an exchange rate  
  			{
  				// cover with currency3 (fxrate3 is between C2 and C3)
  				c2_cost_to_cover = std::abs(currency2size / (1 / fxrate2) );	

  				// format text and set it  
  				boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  				"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency2size 
  				%final_percent_return %one_year_profit %daily_profit %c2_cost_to_cover;

  				// write it to string
  				std::string c2_result = fmt.str();

  				// create buffer and assign it to the appropriate text display widget 
  				Fl_Text_Buffer *c2buff = new Fl_Text_Buffer();
  				instance->p_result_window->c2_output->buffer(c2buff);

  				// add the text to our buffer (to the box)
  				c2buff->text(c2_result.c_str());

  				// append the values to our total
  				blended_one_year_profit += one_year_profit;
  			}
  			else
  			{
  				std::cout << "Error: Currency 2 Cover currency is Currency 3." 
  				" User did not enter FX rate between these currencies." << std::endl;
  			}
  			break;
  		}

  	}		
  	else // Profit is positive or flat therefore no cost to cover calculation
  	{
  		boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  		"One Year P/L: $%3%\nDaily P/L: $%4%") %currency2size %final_percent_return 
  		%one_year_profit %daily_profit;

  		std::string c2_result = fmt.str();

  		// create buffer and assign it to the appropriate text display widget 
  		Fl_Text_Buffer *c2buff = new Fl_Text_Buffer();

  		instance->p_result_window->c2_output->buffer(c2buff);

  		// add the text to our buffer (to the box)
  		c2buff->text(c2_result.c_str());

  		// append the values to our total
  		blended_one_year_profit += one_year_profit;
  	}

  }

  if(currency3size != 0)
  {
  	// only count positive currencies in our total capital 
  	if(currency3size > 0)
  	{
  		total_currency += currency3size;
  	}

  	// solve for the daily int rate 
  	double daily_int_rate = currency3rate / 365;

  	// calculate one year rate of return 
  	double one_year_profit = (currency3size * currency3rate) ;

  	// daily profit 
  	double daily_profit = one_year_profit / 365;

  	// solve for one year percent rate of return 
  	double final_percent_return = (one_year_profit / currency3size) * 100;

  	// if profit is negative, calculate cost to cover using cover currency input 
  	if(one_year_profit < 0)
  	{
  		// since our profit is negative we have a negative balance so append it to the total debt
  		total_debt += currency3size;

  		// get selected cover currency from FL_Input
  		int cover_currency = instance->p_result_window->C3_currencychoice->value();
  		switch (cover_currency) 
  		{
  		case 0: // Currency1
  			if(fxrate3 != 0) // they needed to input an exchange rate  
  			{
  				// cover with currency1 (fxrate3 is between C1 and C3)
  			 	c3_cost_to_cover = std::abs(currency3size / fxrate3);	

  				// format text and set it  
  				boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  				"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency3size 
  				%final_percent_return %one_year_profit %daily_profit %c3_cost_to_cover;

  				// write it to string
  				std::string c3_result = fmt.str();

  				// create buffer and assign it to the appropriate text display widget 
  				Fl_Text_Buffer *c3buff = new Fl_Text_Buffer();
  				instance->p_result_window->c3_output->buffer(c3buff);

  				// add the text to our buffer (to the box)
  				c3buff->text(c3_result.c_str());

  				// append the values to our total
  				blended_one_year_profit += one_year_profit;

  			}
  			else
  			{
  				std::cout << "Error: Currency 3 Cover currency is Currency 1." 
  				" User did not enter FX rate between these currencies." << std::endl;
  			}
  			break;
  		case 1: // cover with Currency 2
  			if(fxrate2 != 0) // they needed to input an exchange rate  
  			{
  				// cover with currency2 (fxrate2 is between C2 and C3)
  				c3_cost_to_cover = std::abs(currency3size / fxrate2);	

  				// format text and set it  
  				boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  				"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency3size 
  				%final_percent_return %one_year_profit %daily_profit %c3_cost_to_cover;

  				// write it to string
  				std::string c3_result = fmt.str();

  				// create buffer and assign it to the appropriate text display widget 
  				Fl_Text_Buffer *c3buff = new Fl_Text_Buffer();
  				instance->p_result_window->c3_output->buffer(c3buff);

  				// add the text to our buffer (to the box)
  				c3buff->text(c3_result.c_str());

  				// append the values to our total
  				blended_one_year_profit += one_year_profit;
  			}
  			else
  			{
  				std::cout << "Error: Currency 3 Cover currency is Currency 2." 
  				" User did not enter FX rate between these currencies." << std::endl;
  			}
  			break;
  		case 2:
  			{
  			// CASE NOT APPLICABLE BECAUSE IT IS THE CURRENT CURRENCY.
  			// No exchange rate possible.
  			
  			c3_cost_to_cover = std::abs(currency3size);

  			boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  			"One Year P/L: $%3%\nDaily P/L: $%4%\nCost to Cover: $%5%") %currency3size %final_percent_return 
  			%one_year_profit %daily_profit %c3_cost_to_cover;

  			std::string c3_result = fmt.str();

  			// create buffer and assign it to the appropriate text display widget 
  			Fl_Text_Buffer *c3buff = new Fl_Text_Buffer();
  			instance->p_result_window->c3_output->buffer(c3buff);

  			// add the text to our buffer (to the box)
  			c3buff->text(c3_result.c_str());

  			// append the values to our total
  			blended_one_year_profit += one_year_profit;
  			}
  			break;
  		}

  	}		
  	else // Profit is positive or flat therefore no cost to cover calculation
  	{
  		boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  		"One Year P/L: $%3%\nDaily P/L: $%4%") %currency3size %final_percent_return 
  		%one_year_profit %daily_profit;

  		std::string c3_result = fmt.str();

  		// create buffer and assign it to the appropriate text display widget 
  		Fl_Text_Buffer *c3buff = new Fl_Text_Buffer();

  		instance->p_result_window->c3_output->buffer(c3buff);

  		// add the text to our buffer (to the box)
  		c3buff->text(c3_result.c_str());

  		// append the values to our total
  		blended_one_year_profit += one_year_profit;
  	}
  }

  if(tbillsize != 0)
  {
  	// only count positive currencies 
  	if(tbillsize > 0)
  	{
  		total_currency += tbillsize;
  	}

  	// solve for the daily int rate 
  	double daily_int_rate = tbillrate / 365;

  	// set a trade fee to subtract 
  	double trade_fee = 7.5;

  	// calculate one year rate of return 
  	double one_year_profit = (tbillsize * tbillrate) - trade_fee;

  	// daily profit 
  	double daily_profit = one_year_profit / 365;

  	// solve for one year percent rate of return 
  	double final_percent_return = (one_year_profit / tbillsize) * 100;

  	boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  	"One Year P/L: $%3%\nDaily P/L: $%4%") %tbillsize %final_percent_return %one_year_profit %daily_profit;

  	std::string tbill_result = fmt.str();

  	// create buffer and assign it to the appropriate text display widget 
  	Fl_Text_Buffer *tbill_buffer = new Fl_Text_Buffer();

  	instance->p_result_window->tbill_output->buffer(tbill_buffer);

  	// add the text to our buffer (to the box)
  	tbill_buffer->text(tbill_result.c_str());

  	// append the values to our total
  	blended_one_year_profit += one_year_profit;
  }

  // PROCEEDS TEXT OUTPUT
  if(currency1proceeds != 0)
  {
  	// append amount to total currency since we are in possession of the short
  	// position proceeds
  	total_currency += currency1proceeds;
  	
  	// solve for the daily int rate 
  	double daily_int_rate = currency1proceeds_rate / 365;

  	// calculate one year rate of return 
  	double one_year_profit = (currency1proceeds * currency1proceeds_rate) ;

  	// daily profit 
  	double daily_profit = one_year_profit / 365;

  	// solve for one year percent rate of return 
  	double final_percent_return = (one_year_profit / currency1proceeds) * 100;

  	boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  	"One Year P/L: $%3%\nDaily P/L: $%4%") %currency1proceeds %final_percent_return %one_year_profit %daily_profit;

  	std::string result = fmt.str();

  	// create buffer and assign it to the appropriate text display widget 
  	Fl_Text_Buffer *c1_buffer = new Fl_Text_Buffer();
  	instance->p_result_window->c1_reinvest_output->buffer(c1_buffer);

  	// add the text to our buffer (to the box)
  	c1_buffer->text(result.c_str());

  	// append the values to our total
  	blended_one_year_profit += one_year_profit;
  	blended_one_year_profit += one_year_profit;
  	std::cout << "1 one year profit: " << one_year_profit << std::endl;
  	std::cout << "1 blended one year profit: " << blended_one_year_profit << std::endl;
  }

  if(currency2proceeds != 0)
  {
  	// append amount to total currency since we are in possession of the short
  	// position proceeds
  	total_currency += currency2proceeds;

  	// solve for the daily int rate 
  	double daily_int_rate = currency2proceeds_rate / 365;

  	// calculate one year rate of return 
  	double one_year_profit = (currency2proceeds * currency2proceeds_rate) ;

  	// daily profit 
  	double daily_profit = one_year_profit / 365;

  	// solve for one year percent rate of return 
  	double final_percent_return = (one_year_profit / currency2proceeds) * 100;

  	boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  	"One Year P/L: $%3%\nDaily P/L: $%4%") %currency2proceeds %final_percent_return %one_year_profit %daily_profit;

  	std::string result = fmt.str();

  	// create buffer and assign it to the appropriate text display widget 
  	Fl_Text_Buffer *c2_buffer = new Fl_Text_Buffer();
  	instance->p_result_window->c2_reinvest_output->buffer(c2_buffer);

  	// add the text to our buffer (to the box)
  	c2_buffer->text(result.c_str());

  	// append the values to our total
  	blended_one_year_profit += one_year_profit;
  	std::cout << "2 one year profit: " << one_year_profit << std::endl;
  	std::cout << "2 blended one year profit: " << blended_one_year_profit << std::endl;
  }

  if(currency3proceeds != 0)
  {
  	// append amount to total currency since we are in possession of the short
  	// position proceeds
  	total_currency += currency3proceeds;

  	// solve for the daily int rate 
  	double daily_int_rate = currency3proceeds_rate / 365;

  	// calculate one year rate of return 
  	double one_year_profit = (currency3proceeds * currency3proceeds_rate) ;

  	// daily profit 
  	double daily_profit = one_year_profit / 365;

  	// solve for one year percent rate of return 
  	double final_percent_return = (one_year_profit / currency3proceeds) * 100;

  	boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  	"One Year P/L: $%3%\nDaily P/L: $%4%") %currency3proceeds %final_percent_return %one_year_profit %daily_profit;

  	std::string result = fmt.str();

  	// create buffer and assign it to the appropriate text display widget 
  	Fl_Text_Buffer *c3_buffer = new Fl_Text_Buffer();
  	instance->p_result_window->c3_reinvest_output->buffer(c3_buffer);

  	// add the text to our buffer (to the box)
  	c3_buffer->text(result.c_str());

  	// append the values to our total
  	blended_one_year_profit += one_year_profit;
  	std::cout << "3 one year profit: " << one_year_profit << std::endl;
  	std::cout << "3 blended one year profit: " << blended_one_year_profit << std::endl;
  }
  if(tbillproceeds != 0)
  {
  	// Proceeds will always be positive
  	
  	// append amount to total currency since we are in possession of the short
  	// position proceeds
  	total_currency += tbillproceeds;

  	// solve for the daily int rate 
  	double daily_int_rate = tbillproceeds_rate / 365;

  	// calculate one year rate of return 
  	double one_year_profit = (tbillproceeds * tbillproceeds_rate) ;

  	// daily profit 
  	double daily_profit = one_year_profit / 365;

  	// solve for one year percent rate of return 
  	double final_percent_return = (one_year_profit / tbillproceeds) * 100;

  	boost::format fmt = boost::format("Position Size: $%1%\nInterest Rate: %2%%%\n"
  	"One Year P/L: $%3%\nDaily P/L: $%4%") %tbillproceeds %final_percent_return %one_year_profit %daily_profit;

  	std::string result = fmt.str();

  	// create buffer and assign it to the appropriate text display widget 
  	Fl_Text_Buffer *tbill_buffer = new Fl_Text_Buffer();
  	instance->p_result_window->tbill_reinvest_output->buffer(tbill_buffer);

  	// add the text to our buffer (to the box)
  	tbill_buffer->text(result.c_str());

  	// append the values to our total
  	blended_one_year_profit += one_year_profit;
  	std::cout << "4 one year profit: " << one_year_profit << std::endl;
  	std::cout << "4 blended one year profit: " << blended_one_year_profit << std::endl;
  }

  // LOGIC FOR TOTALS CALCULATIONS 

  // solve for blended percent return 
  blended_percent_return = (blended_one_year_profit / total_currency) * 100;

  // used for solving cost to cover if applicable 
  double cost_to_cover = 0;

  // used for formatting text to pass into buffer 
  std::string result; 

  // if any of these currencies are negative we need to format text with cost to cover
  if(currency1size < 0 || currency2size < 0 || currency3size < 0)
  {
  	// simply add up all cost to covers and we will get a total
  	double total_cost_to_cover = c1_cost_to_cover + c2_cost_to_cover + c3_cost_to_cover;

  	boost::format fmt = boost::format("Total Capital: $%1%\nTotal Debt: $%2%\n"
  	"Blended Interest Rate: %3%%%\nOne Year P/L: $%4%\n"
  	"Cost to Cover with FX rate: $%5%") %total_currency %total_debt %blended_percent_return 
  	%blended_one_year_profit % total_cost_to_cover ;

  	// write to the string
  	result = fmt.str();
  }
  else
  {
  	// no currencies are negative therefore format without cost to cover 

  	boost::format fmt = boost::format("Total Capital: $%1%\n"
  	"Blended Interest Rate: %2%%%\nOne Year P/L: $%3%\n") %total_currency 
  	%blended_percent_return %blended_one_year_profit ;

  	// write to the string
  	result = fmt.str();
  }

  // debug 
  /* std::cout << "Total One Year Profit/Loss was: " << blended_one_year_profit */ 
  /* << std::endl; */
  /* std::cout << "Total return: " << blended_percent_return */ 
  /* << "%" << std::endl; */

  // create a new buffer 
  Fl_Text_Buffer *total_buffer = new Fl_Text_Buffer();

  // assign the buffer to a text display widget
  instance->p_result_window->blended_output->buffer(total_buffer);

  // add the text to our buffer 
  total_buffer->text(result.c_str());
}

int main(int argc, char **argv) {
  // OPEN WINDOW & POPULATE SAVED INPUT VALUES FROM DATABASE 
  main_window* mainwindow = new main_window(475,268,611,506,"Interest Calculator V2");

  // open (ALREADY IN ABOVE CODE)
  sqlite3_open(dbPath, &db);

  const char* checkTableExistsQuery = "SELECT name FROM sqlite_master WHERE type='table' AND name='MAINWINDOW_INPUTS';";

  if (sqlite3_prepare_v2(db, checkTableExistsQuery, -1, &stmt, nullptr) == SQLITE_OK) 
  {
  	if (sqlite3_step(stmt) == SQLITE_ROW) 
  	{
  		/* std::cout << "Table exists!" << std::endl; */

  		// FINALIZE before going to next prepared statement 
  		int finalize = sqlite3_finalize(stmt);
  		if(finalize != SQLITE_OK) 
  		{
  			std::cout<< finalize << std::endl; 
  		}
  		/* else */
  		/* { */
  			/* std::cout << " SQL Statement Finalized..." << std::endl; */
  		/* } */

  		// prepare statement to obtain values from the DB 
  		sqlite3_prepare_v2(db, "SELECT * from MAINWINDOW_INPUTS LIMIT 1", -1, &stmt, 0); 

  		while(sqlite3_step(stmt) != SQLITE_DONE) 
  		{ 
  			const unsigned char* currency1size = sqlite3_column_text(stmt, 1);	
  			mainwindow->input_currency1size->value((const char*)currency1size);

  			const unsigned char* currency1rate = sqlite3_column_text(stmt, 2);	
  			mainwindow->input_currency1rate->value((const char*)currency1rate);

  			const unsigned char* currency2size = sqlite3_column_text(stmt, 3);	
  			mainwindow->input_currency2size->value((const char*)currency2size);

  			const unsigned char* currency2rate = sqlite3_column_text(stmt, 4);	
  			mainwindow->input_currency2rate->value((const char*)currency2rate);

  			const unsigned char* currency3size = sqlite3_column_text(stmt, 5);	
  			mainwindow->input_currency3size->value((const char*)currency3size);

  			const unsigned char* currency3rate = sqlite3_column_text(stmt, 6);	
  			mainwindow->input_currency3rate->value((const char*)currency3rate);

  			const unsigned char* tbillsize = sqlite3_column_text(stmt, 7);	
  			mainwindow->input_tbillsize->value((const char*)tbillsize);

  			const unsigned char* tbillrate = sqlite3_column_text(stmt, 8);	
  			mainwindow->input_tbillrate->value((const char*)tbillrate);

  			const unsigned char* currency1proceeds_size = sqlite3_column_text(stmt, 9);	
  			mainwindow->input_currency1proceeds->value((const char*)currency1proceeds_size);

  			const unsigned char* currency1proceeds_rate = sqlite3_column_text(stmt, 10);	
  			mainwindow->input_currency1proceedsrate->value((const char*)currency1proceeds_rate);

  			const unsigned char* currency2proceeds_size = sqlite3_column_text(stmt, 11);	
  			mainwindow->input_currency2proceeds->value((const char*)currency2proceeds_size);

  			const unsigned char* currency2proceeds_rate = sqlite3_column_text(stmt, 12);	
  			mainwindow->input_currency2proceedsrate->value((const char*)currency2proceeds_rate);

  			const unsigned char* currency3proceeds_size = sqlite3_column_text(stmt, 13);	
  			mainwindow->input_currency3proceeds->value((const char*)currency3proceeds_size);

  			const unsigned char* currency3proceeds_rate = sqlite3_column_text(stmt, 14);	
  			mainwindow->input_currency3proceedsrate->value((const char*)currency3proceeds_rate);

  			const unsigned char* tbillproceeds = sqlite3_column_text(stmt, 15);	
  			mainwindow->input_tbillproceeds->value((const char*)tbillproceeds);

  			const unsigned char* tbillproceeds_rate = sqlite3_column_text(stmt, 16);	
  			mainwindow->input_tbillproceedsrate->value((const char*)tbillproceeds_rate);

  			const unsigned char* fxrate1 = sqlite3_column_text(stmt, 17);	
  			mainwindow->input_fxrate1->value((const char*)fxrate1);

  			const unsigned char* fxrate2 = sqlite3_column_text(stmt, 18);	
  			mainwindow->input_fxrate2->value((const char*)fxrate2);

  			const unsigned char* fxrate3 = sqlite3_column_text(stmt, 19);	
  			mainwindow->input_fxrate3->value((const char*)fxrate3);
  		}

  		// The application must finalize every prepared statement in order to avoid resource leaks.
  		// // finalize second statement 
  		finalize = sqlite3_finalize(stmt);
  		if(finalize != SQLITE_OK) 
  		{
  			std::cout<< finalize << std::endl; 
  		}
  		/* else */
  		/* { */
  			/* std::cout << " SQL Statement Finalized..." << std::endl; */
  		/* } */
  	} 
  	/* else */ 
  	/* { */
  	/* 	std::cout << "Table does not exist. Gracefully doing nothing..." << std::endl; */
  	/* } */
  } 
  else 
  {
  	std::cerr << "Error preparing SQL statement" << std::endl;
  }

  sqlite3_close(db); // close the database
  // ICON CODE 
  const char *name = (argc > 1) ? argv[1] : "logo.png";
  if (argc>1) argc = 1; // BAD HACK !

  Fl_PNG_Image *img = new Fl_PNG_Image(name); // load image
  if (img->w() == 0 || img->h() == 0) {
  fl_message("Can't open icon file '%s'",name);
  exit(1);
  }

  // set icon and xclass

  mainwindow->icon(img); // set window icon

  const char *xc = strrchr(name,'/'); // find file name part
  if (!xc) xc = name; // no directory part
  mainwindow->xclass(xc); // set xclass for grouping
  return Fl::run();
}
