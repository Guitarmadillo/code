#include "sierrachart.h"

#include <boost/format.hpp>
#include <curl/curl.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <thread>

#include "json.hpp" // convenience 

// Sleep is only necessary for a specific case to wait the http request thread for 1 second 
#ifdef WIN32
	#include <windows.h>		// Sleep(), in miliseconds
#else
	#include <unistd.h>			// usleep(), in microseconds
	#define Sleep( m ) usleep( m*1000 )
#endif

SCDLLName("VerrilloTrading - Telegram Chart Drawing Alerts")
////////////////////////////////////////////////////////////////////////////////////////////////////////

// This function performs a curl synchronous request to the inputted URL
int send_photo(const SCString& URL, const std::string& ChatID, const std::string& FilePath, const std::string& caption)
{
	// Pause thread for 4 second to allow the screenshot file to be properly
	// saved. Some image files take longer to be saved which can result in the
	// previously saved screenshot being passed into the request instead of the
	// new one. This is why this pause is necessary to ensure the new one is sent. 
	Sleep(4000);
	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL* curl = curl_easy_init();

	if (curl) 
	{
		// Create a new form
		struct curl_httppost* formpost = nullptr;
		struct curl_httppost* lastptr = nullptr;

		// Add form fields (specific to telegram sendPhoto method)
		curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, 
		"chat_id", CURLFORM_COPYCONTENTS, ChatID.c_str(), CURLFORM_END);
		curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, 
		"photo", CURLFORM_FILE, FilePath.c_str(), CURLFORM_END);
		curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, 
		"caption", CURLFORM_COPYCONTENTS, caption.c_str(), CURLFORM_END);
		curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, 
		"parse_mode", CURLFORM_COPYCONTENTS, "HTML", CURLFORM_END);

		// Set the URL
		curl_easy_setopt(curl, CURLOPT_URL, URL.GetChars());

		// Set the form data
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

		// Perform the HTTP POST request
		// This causes SC to freeze
		CURLcode res = curl_easy_perform(curl);

		// Check for errors
		if (res != CURLE_OK) 
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}

		// Clean up
		curl_formfree(formpost);
		curl_easy_cleanup(curl);
	}

	return 1;
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

	SCString msg; // logging object

	if (sc.SetDefaults)
	{
		// This is what appears when you select Description from the study settings.
		//
		// The text written here might be slightly out of date, for the up to date version view the README or
		// this web page by VerrilloTrading. https://www.verrillotrading.com/sc-telegram-chart-drawing-alerts/
		sc.StudyDescription = "This advanced custom study was written by Guitarmadillo at VerrilloTrading in Q3-Q4 of 2023. This study will be open source and it can serve as an example for how to use a Telegram Bot with Sierra Chart. To compile this study users will need to include the necessary dependencies like boost::format and CURL. I tried as much as possible to stick to the Sierra Chart libraries but some things are much more practical with other c++ libraries. For example, there are very few if any existing examples of HTTP POST functionality using ACSIL. An example of this is provided in combination with nlohmann json to format the json object. CURL was necessary to perform the <a href = https://core.telegram.org/bots/api#sendphoto target=_blank ref=noopener noreferrer >sendPhoto</a> Telegram method for sending multipart/form data. Anyone is welcome to repurpose the code, improve it or add functionality."

		" <br> <br> This study contains two examples of sending a http post request from Sierra Chart."
		"<br> 1. using CURL to call the Telegram <a href = https://core.telegram.org/bots/api#sendphoto target=_blank ref=noopener noreferrer >sendPhoto</a> method where the chart image is passed as multipart/form data."
		"<br> 2. using <a href = https://www.sierrachart.com/index.php?page=doc/ACSIL_Members_Functions.html#scMakeHTTPPOSTRequest target=_blank ref=noopener noreferrer >sc.MakeHTTPPOSTRequest()</a> to call Telegram <a href = https://core.telegram.org/bots/api#sendmessage target=_blank ref=noopener noreferrer >sendMessage</a> method which is a native Sierra Chart ACSIL function."

		"<br> <br> The second example is in the study in the case that the user does not want to recieve the chart screenshot. Otherwise it is meant to serve as an example."

		"<br> <br> Reasons for sharing the source code:"
		"<br> 1. Greater Transparency " 
		"<br> 2. Helps the community"
		"<br> 3. Show off skills and features in Sierra Chart"
		 
		" <br> <br> If you want to use this study and do not want to bother building it yourself you can send an email to support@verrillotrading.com with your Sierra Chart username and I will add your username to the list of allowed users for this .DLL."
		" The .DLL has been compiled using this exact source code file **(except one change) and a build script."

		"<br> <br> ** For convenience purposes The .DLL provided by VerrilloTrading provides a default bot that users can rely on instead of having to create their own bot. The bot token for that bot was removed from the repository for security reasons."

		"<br> <br> <strong><u>HOW TO SETUP AND USE THIS STUDY:</u></strong> "

		"<br> <br> When this study is enabled and a chart drawing alert takes place, a Telegram message will be sent to the specified Chat Id. This message will contain information about the alert that took place, and a real-time screenshot of the chart where the alert came from. It is possible for the user to disable the screenshot feature and send a simple message." 

		"<br> <br> Use Telegram Web to get the Telegram Chat ID. <span style=\"background-color: yellow; text-decoration: underline;\">It is necessary to be on Telegram Web version A to get the Chat ID from the address bar.</span> This can be selected from the settings window in the top left when using Telegram Web. The Chat ID appears in the address bar. Example: https://web.telegram.org/a/#-12345678 where '-12345567' is the Chat ID. Groups and sub topics in groups typically have negative numbers as the Chat ID. To get your personal chat ID, go to the Saved Messages chat. On Telegram Web this can be accessed by clicking at the top right and it should appear in the drop down as 'Saved Messages'. "

		" <br> <br> <u>Telegram Groups and Private chats are supported.</u> Channels are a bit more complex so they have been left out for the time being. You will need to add the bot to the group first. For private chats, you need to send a /start message to the bot first, Telegram bots cannot message you first. The default bot that the study uses on Telegram is https://t.me/vt_sierrachart_sender_bot. <span style=\"background-color: yellow; text-decoration: underline;\">You need to message this bot with /start on Telegram, unless you are using your own bot.</span> "

		"<br> <br> Users can override the default bot with their their own bot if they prefer. "

		"<br> <br> The chart drawing will handle the alert and its condition, but when the alert occurs, this study will be able to see that a new alert occurred by reading the log file."

		"<br> <br> It is necessary to enable this setting in Sierra Chart:"
		"<br> <span style=\"background-color: yellow; font-weight: bold;\">Global Settings >> Log >> Save Alerts Log to File</span>"

		"<br> <br> <strong>Important:</strong> It is necessary to make sure Input #4 is set to the path that is set in <strong>Global Settings > Paths > Chart Image Folder.</strong> When this Setting is changed in Sierra Chart, the folder will be automatically created. It is absolutely critical that you set the study input #4 text to this path. You will get an Error showing up in the message log if the path you specified in the study input does not exist. Example: The default path for this on Windows is: <strong>C:\\SierraChart\\Images</strong> If you are running on Linux thrugh Wine or if you changed the drive letter of your C drive it might look like this: <strong>Z:\\mnt\\SierraChart\\Images</strong>"


		"<br> <br> When a chart drawing alert is triggered, a new line is written to a file in the following directory: <strong>C:\\SierraChart\\Logs</strong>"

		"<br> <br> This study handles the various cases for when Sierra Chart generates new alert files. "

		"<br> <br> <strong>Important:</strong> For the screenshot function to work when charts are hidden behind other charts in your chartbook it is necessary to <span style=\"background-color: yellow; font-weight: bold;\">disable</span> <a href = https://www.sierrachart.com/index.php?page=doc/GeneralSettings.html#DestroyChartWindowsWhenHidden target=_blank rel=noopener noreferrer >Destroy Chart Windows When Hidden</a>."

	   	"<br> More info about this setting and how it is handled for the screenshot function:"
		"<br> <a href = https://www.sierrachart.com/index.php?page=doc/ACSIL_Members_Functions.html#scSaveChartImageToFileExtended target=_blank rel=noopener noreferrer >sc.SaveChartImageToFileExtended</a>"

		"<br> <br> <strong>Important:</strong> The screenshot function from Sierra Chart is limited to taking screenshots of charts in the chartbook where the function call originates from. For this reason it is necessary to have 1 instance of this study open per chartbook. So if you have 5 open chartbooks with different symbols waiting for alerts, you will need to open this study on a chart in each one of those chartbooks. When the study is active in any particular chartbook it will handle all the alerts that take place within that chartbook and provide an image of the chart as it is displayed in the chartbook. The chart can be hidden behind other charts and the screenshot will work as long as <a href = https://www.sierrachart.com/index.php?page=doc/GeneralSettings.html#DestroyChartWindowsWhenHidden target=_blank rel=noopener noreferrer >Destroy Chart Windows When Hidden</a> is <span style=\"background-color: yellow; font-weight: bold;\">disabled</span>. This setting is found in: Global Settings > General Settings > GUI > Destroy Chart Windows When Hidden."

		"<br> <br> <strong><u>INFO ABOUT THE LOG FILE PATH:</u></strong> "

		"<br> <br> The study should function correctly AS LONG AS:"
		"<br>1. You have not changed the default location of the Data files folder." 

		"<br> <br> <strong>It is not recommended to move the location of the <a href = https://www.sierrachart.com/index.php?page=doc/GeneralSettings.html#DataFilesFolder target=_blank rel=noopener referrer >Data Files Folder</a> from the Sierra Chart installation directory!</strong> "
		"<br> <br> If you absolutely need to move this folder you will need to perform this next step: "
		"<br> <br> <strong>IMPORTANT:</strong> If you have changed the location of the Data Files Folder, You will need to explicitly specify the directory of the Sierra Chart Logs folder. This folder will be located in the same directory where Sierra Chart is installed. The reason why this is necessary is because the study uses the Data Files Folder Path to get the Path to the Logs Folder. Example: if you moved your Data folder path to ANY PATH that is not the Sierra Chart Installation directory, you will now need to specify the Path of the Logs Folder which is in the directory where Sierra Chart is installed. <span style=\"background-color: yellow; text-decoration: underline;\">You need to explicitly specify the Logs Folder path in the last study input field.</span> "

		"<br> <br> The default location of this folder is: <strong>C:\\SierraChart\\Logs</strong>"
		
		"<br> <br> For any issues or inquiries, send them to support@verrillotrading.com"

		"<br> <br> Thank you and I hope you enjoy the study!" 
		"<br> <br> -VerrilloTrading, Content Creator - Programmer";

		// Study Defaults
		sc.GraphName = "Telegram Chart Drawing Alerts";
		sc.AutoLoop = 0;//Manual looping
		sc.GraphRegion = 0;
		sc.ScaleRangeType= SCALE_SAMEASREGION;

		sc.UpdateAlways = 1; // not strictly necessary but can make it faster 

		Input_Enabled.Name = "Send Telegram on Chart Drawing Alert";
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

		// Persistent variable precaution 
		if(FirstTimeStart != 0)
		{
			FirstTimeStart = 0;
		}	

		return;
		// End Study Defaults 
	}

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
		sc.AddMessageToLog("If you see this message it means you compiled the study on your own. "
		"1. You need to remove or comment out lines 271-274."
		" 2. It is necessary for you to use your own bot using the provided input settings.",1);
		return; 

		/* token = SCString("bot") + "your_bot_token"; */

	}

	// Wrap the entire study function in a 1-5 second timer	in order to not poll the file too often
	
	// Get the current date time
	SCDateTime CurrentDateTime; 
	CurrentDateTime = sc.CurrentSystemDateTime; 

	// The following code adds support for chart replay. 
	// IF the user is doing a chart replay, get the time of the replaying
	// chart, instead of the current time. 
	// It may not be strictly necessary for all use cases.
	int ReplayRunning = sc.GetReplayStatusFromChart(sc.ChartNumber);

	if (ReplayRunning >= 1)
    {
		CurrentDateTime = sc.CurrentDateTimeForReplay;
	}
	else
	{
    	CurrentDateTime = sc.CurrentSystemDateTime; 
	} 

	// declare necessary persistent variables 
	std::int64_t& LastUpdated = sc.GetPersistentInt64(1); 
	std::int64_t& TimeInSeconds = sc.GetPersistentInt64(2);

	// TIMER: Interval is set to the length of our timer in seconds
    int Interval = 2;

    // Get the time in seconds and store it to persistent memory
    TimeInSeconds = CurrentDateTime.GetTimeInSeconds();
	
	// This is because when replaying a chart, LastUpdated is still at the
	// current time and needs to be reset to the time of the replaying chart. 
	if(LastUpdated > TimeInSeconds)
	{
		LastUpdated = TimeInSeconds;
	}
	
    // Have X seconds elapsed since last updated?
    if (LastUpdated + Interval > TimeInSeconds) 
	{
        // X seconds have NOT elapsed, bomb out
        return;
    }
	
	// When the Timer has passed
	// LastUpdated gets updated to the current time, it resets the timer.
	LastUpdated = TimeInSeconds;

	// CONTINUE WITH THE STUDY

	// get the chat id from SC Input. 
	std::string ChatID = Input_ChatID.GetString();

	// declare variables for the host and Telegram method being called
	SCString host = "https://api.telegram.org/";
	SCString method = "";

	// Get the directory of sierra data folder from a function directly into std::string
	std::string DataFolderPath = sc.DataFilesFolder().GetChars();

	// print out path debug
	/* msg.Format("1: Data Folder Path: %s", DataFolderPath.c_str()); */
	/* sc.AddMessageToLog(msg,1); */

	// NOTE: 
	// In C++, the backslash character ("\") is used as an escape character in
	// string literals. This means that if you want to use a backslash as part
	// of a string, you need to escape it by typing it twice ("\").
	//
	// Get the Starting position of 'SierraChart' text from Data Folder Path 
	std::size_t start_pos = DataFolderPath.find("SierraChart");

	// find the first backslash after SierraChart (support for Sierrachart2,3,4... etc)
	std::size_t end_pos = DataFolderPath.find('\\', start_pos); 																
	if(start_pos != std::string::npos)
	{
		//debug
		/* sc.AddMessageToLog("SierraChart was found in the provided String", 1); */

		// Create a string from the existing string that omits the "Data" characters
		// and adds the "Logs" characters to the end of it. 
		 
		// re assign our directory string to the new value
		DataFolderPath = DataFolderPath.substr(0, end_pos + 1) + "Logs";

		// print out the new directory
		/* msg.Format("Logs Directory Path: %s",DataFolderPath.c_str()); */
		/* sc.AddMessageToLog(msg,1); */

	}
	else 
	{	
		// If we get here it means the Data Files Folder path does not have
		// SierraChart in it.  The user moved it to some other directory. This
		// means the log file path needs to be specified through the study input.
		//
		/* sc.AddMessageToLog("SierraChart not found in path, */ 
		/* " logs directory should be specified manually", 1); */

		// Catch if they turned it on but did not specify the path.  
		if(strlen(Input_CustomFolderPath.GetString()) == 0)
		{
			sc.AddMessageToLog("Error: text 'SierraChart' not found in Data Folder Path"
			" but no path to Logs Folder was provided",1);
			return; 
		}
		else
		{
			// set the explicitly specified path 
			DataFolderPath = Input_CustomFolderPath.GetString();
		}
	}

	// Confirm if the user specified the path even if 'SierraChart' text was found in the Data Folder Path. 
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
			DataFolderPath = Input_CustomFolderPath.GetString();
		}
	}

	// debug
	/* msg.Format("Study assumes Log Folder Path is: %s", DataFolderPath.c_str()); */
	/* sc.AddMessageToLog(msg,1); */
	/* sc.AddMessageToLog("If this is not the path of your Alert Logs directory there will be a problem", 1); */
	
	
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

	// HANDLE if the folder does not exist before accessing the directory 
	if (!std::filesystem::exists(DataFolderPath) && !std::filesystem::is_directory(DataFolderPath)) 
	{
		sc.AddMessageToLog("Error: Logs Folder does not exist! Check if the folder exists in SC install directory."
		" Enable Save Alerts Log to File under General Settings > Log. Then trigger a chart drawing alert and "
		"the directory should be created.",1);
		return;
	}

	// Iterate through the directory to find the number of files that match
	// find the file with the most recent modification time
	for (const auto& entry : std::filesystem::directory_iterator(DataFolderPath)) 
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

	// debug to check if we are getting the right file
	/* msg.Format("There are %d total files | Most recent filename: %s" */
	/* ,number_of_files, most_recent_filename.c_str()); */
	/* sc.AddMessageToLog(msg,1); */

	// Bomb out if there are no alert log files 
	if(number_of_files == 0)
	{
		// debug 
		/* sc.AddMessageToLog("there are no alert files found",1); */

		// bomb out until an alert file is found
		return;
	}

	// ENSURE on the first call to the function that number of files is initialized to MEMORY
	// NECESSARY for the check below that determines if a new log file was created.
	if(num_files_memory == 0 && number_of_files != 0)
	{
		// we should only get here if there are alert log files and number of files in
		// memory has not been initialized yet. 
		//
		// REMEMBER the number of alert log files.
		num_files_memory = number_of_files;
	}	

	// Construct the full path by combining the directory path and filename
	// c++ filesystem variable which is explicitly set to the name of the file
	// with the most recent write time in the specified directory. 
    std::filesystem::path LogFile (DataFolderPath + "\\" + most_recent_filename);
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
			// If we get here it means Sierra Chart just created a new log file
			// after being opened, and we need to read the first line out of
			// that new file and send a Telegram.
			//
			// The reason why it is necessary to put this code here is because
			// in the later code, the Telegram is only sent if a new line is
			// picked up in the most recent alert log file. Since it is saving
			// the number of lines in a file and a new file was just created. the
			// number of lines saved will be greater than the number of lines
			// in the current file which will not allow the check below to
			// return true and send the telegram. 
			// 
			// This handles the case for when SC is opened and the study is already
			// on the chart and SC creates a brand new log file as soon as an alert
			// is triggered.
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

			// debug this should only print the very first time opening SC and a chart drawing alert is triggered 
			/* sc.AddMessageToLog("we get here: Num files in memory not equal to number of files",1); */ 

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

					// STRING PARSING LOGIC 
					// find various text inside the string
					std::size_t source_start_pos = line.find("Source");
					std::size_t source_end_pos = line.find('|', source_start_pos);

					std::size_t text_start_pos = line.find("Chart Drawing");
					std::size_t text_end_pos = line.find('|', text_start_pos);

					// ADD MONOSPACE FORMATTING TO THE PRICE in HTML FORMAT
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

					// continue with the other substrings
					std::size_t bar_datetime_start_pos = line.find("Bar date-time");
					std::size_t bar_datetime_end_pos = line.find('|', bar_datetime_start_pos);

					// Only need the start position since it reads until end of the string
					std::size_t chartbook_start_pos = line.find("Chartbook");

					// SAVE the alert log chartbook name independantly 
					std::string_view alert_chartbook (line.c_str() + chartbook_start_pos + 11);

					// debug chartbook name 
					/* msg.Format("Chartbook Name: %s",std::string(alert_chartbook).c_str()); */
					/* sc.AddMessageToLog(msg,1); */

					// Save the chart number independantly 
					// search backward from source_end_pos until the first # character 
					std::size_t cht_number_start_pos = line.rfind('#', source_end_pos);

					// save only the number itself as a single character 
					std::string_view alert_cht_number (line.c_str() + cht_number_start_pos + 1, 
					(source_end_pos -1) - cht_number_start_pos);

					// debug chart number 
					/* msg.Format("Chart Number: %s",std::string(alert_cht_number).c_str()); */
					sc.AddMessageToLog(msg,1);
					
					if(source_start_pos != std::string::npos) // safety check/good measure 
					{
						// Make reference to different sequences of characters from the underlying string
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

						// debug the text string we formatted 
						/* msg.Format("telegram text: %s", line.c_str()); */
						/* sc.AddMessageToLog(msg,1); */
					}

					// Cast/Convert chartbook name stringview into SCString (1 allocation)
					SCString sc_alert_chartbook = std::string(alert_chartbook).c_str();

					// Get the current chartbook name 
					SCString current_chartbook = sc.ChartbookName();

					// debug the chartbook names 
					/* msg.Format("current chartbook: %s alert chartbook: %s", current_chartbook.GetChars(), */ 
					/* sc_alert_chartbook.GetChars() ); */
					/* sc.AddMessageToLog(msg,1); */

					if(sc_alert_chartbook == current_chartbook)
					{
						// the alert came from the same chartbook as where this study exists
						// SEND TELEGRAM message or message with photo 
						
						// Cast/Convert chart number stringview into int 
						int sc_alert_chart_num = std::stoi(std::string(alert_cht_number));

						// Confirm which Telegram method the user specified
						if(Input_SendChartImage.GetBoolean() == 0)
						{
							// use sendMessage method instead of sendPhoto
							method = "/sendMessage?";
							
							// Set our Telegram URL for the POST request 
							SCString URL = std::move(host) + std::move(token) + std::move(method);

							// SIERRA CHART POST REQUEST WITH JSON BODY EXAMPLE 
							
							// Create the json object for Telegram
							// For simplicity using nlohmann json
							nlohmann::json object = {
							{"chat_id", std::move(ChatID)},
							{"text", std::move(line)},
							/* {"parse_mode", "Markdown"}, */	
							}; 

							// TODO Low priority: Write logic to handle the necessary escape
							// sequences in the Alert Text as well as Source
							// text strings that are extracted from the alert
							// log line text. In order to regain compatibility
							// with MarkDown text formatting.
							
							// Convert Json into SCString in one line. 
							SCString query = object.dump().c_str(); 						
							
							/* msg.Format("Query debug: %s", query.GetChars() ); */
							/* sc.AddMessageToLog(msg,1); */

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
						else
						{
							// set the method variable which is passed into the URL string
							method = "/sendPhoto?";

							// Create the file path to our image file 
							std::string FilePath = Input_ImageFolderPath.GetString()
							+ std::string("\\chart_image.png");

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

							// Precaution: Open the image file for reading to ensure it exists
							std::ifstream file(FilePath, std::ios::binary);
							if (!file.is_open()) 
							{
								sc.AddMessageToLog("Error opening image file!"
								" This might also be the first time the image was saved.",1);
							}

							// Close the image file
							file.close();

							// Set our Telegram URL for the POST request 
							SCString URL = std::move(host) + std::move(token) + std::move(method);

							// Call the HTTP POST Request in a separate thread. (not recommended for large scale operations)
							// This function calls sendPhoto Telegram method
							std::thread request_thread(send_photo, URL, ChatID, FilePath, line);

							// detach the thread and it will finish on it's own. 
							request_thread.detach();
						}
					}
					else
					{
						// the alert came from a different chartboook
						// Simply do nothing 
						// IMPORTANT: 1 Instance of this study should exist per open chartbook
						//
						// Debug 
						/* sc.AddMessageToLog("Alert was generated from a different chartbook. " */
						/* "That chartbook will handle the alert.",1); */
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
			// The first time the study reads the file it should not be necessary to send any http requests
			
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
				
				// debug 
				/* msg.Format("line counter: %d num lines: %d", CountLines, NumberOfLines ); */
				/* sc.AddMessageToLog(msg, 1) ; */

			}
			else
			{
				sc.AddMessageToLog("Could not open file!!!", 1);
			}
			
		} 
		else // EVERY OTHER TIME WE READ THE FILE AFTER SC HAS STARTED WE GET HERE 
		{
			// Poll the file for a change to the last write time. (time in seconds from epoch)
			if(currentModTimePoint > LastModTime)
			{
				// We get here if write time is different than what was previously saved

				// This means there was an update to the file 
				// and we need to process the update that was made to the file

				// update the last modified time of the file in memory
				LastModTime = currentModTimePoint;
				
				// open the file for reading
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

					// open the file for reading
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

								// Process the new line and send a Telegram

								// find various text inside the string
								std::size_t source_start_pos = line.find("Source");
								std::size_t source_end_pos = line.find('|', source_start_pos);

								std::size_t text_start_pos = line.find("Chart Drawing");
								std::size_t text_end_pos = line.find('|', text_start_pos);

								// ADD MONOSPACE FORMATTING TO THE PRICE in HTML FORMAT
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

								// continue with the other substrings
								std::size_t bar_datetime_start_pos = line.find("Bar date-time");
								std::size_t bar_datetime_end_pos = line.find('|', bar_datetime_start_pos);

								// only need the start position since it reads until end of the string
								std::size_t chartbook_start_pos = line.find("Chartbook");
								
								// SAVE the alert log chartbook name independantly 
								std::string_view alert_chartbook (line.c_str() + chartbook_start_pos + 11);

								// debug chartbook name 
								/* msg.Format("Chartbook Name: %s",std::string(alert_chartbook).c_str()); */
								/* sc.AddMessageToLog(msg,1); */

								// Save the chartbook number independantly 
								// search backward from source_end_pos until the first # character 
								std::size_t cht_number_start_pos = line.rfind('#', source_end_pos);

								// save only the number itself  as a single character 
								std::string_view alert_cht_number (line.c_str() + cht_number_start_pos + 1, (source_end_pos -1) - cht_number_start_pos);

								// debug chart number 
								/* msg.Format("Chart Number: %s",std::string(alert_cht_number).c_str()); */
								/* sc.AddMessageToLog(msg,1); */


								if(source_start_pos != std::string::npos) // safety check
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

									// debug the text string we formatted 
									/* msg.Format("telegram text: %s", line.c_str()); */
									/* sc.AddMessageToLog(msg,1); */
								}

								// Cast/Convert chartbook name stringview into SCString (1 allocation)
								SCString sc_alert_chartbook = std::string(alert_chartbook).c_str();


								// Get the name of the chartbook where this study is applied 
								SCString current_chartbook = sc.ChartbookName();

								// debug the chartbook names 
								/* msg.Format("current chartbook: %s alert chartbook: %s", current_chartbook.GetChars(), */ 
								/* sc_alert_chartbook.GetChars() ); */
								/* sc.AddMessageToLog(msg,1); */

								if(sc_alert_chartbook == current_chartbook)
								{
									// the alert came from the same chartbook as where this study exists
									
									// Cast/Convert chart number stringview into int 
									int sc_alert_chart_num = std::stoi(std::string(alert_cht_number));

									// Confirm which Telegram method the user specified
									if(Input_SendChartImage.GetBoolean() == 0)
									{
										// use sendMessage method instead of sendPhoto
										method = "/sendMessage?";

										// Set our Telegram URL for the POST request 
										SCString URL = std::move(host) + std::move(token) + std::move(method);

										// SIERRA CHART POST REQUEST WITH JSON BODY EXAMPLE 
										//
										// Create the json object for Telegram
										// For simplicity using nlohmann json
										nlohmann::json object = {
										{"chat_id", std::move(ChatID)},
										{"text", std::move(line)},
										/* {"parse_mode", "Markdown"}, */	
										}; 

										// Convert nlohmann json into SCString in one line. 
										SCString query = object.dump().c_str(); 						
										
										/* msg.Format("Query debug: %s", query.GetChars() ); */
										/* sc.AddMessageToLog(msg,1); */

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
									else
									{
										method = "/sendPhoto?";
										// Set our Telegram URL for the POST request 
										SCString URL = std::move(host) + std::move(token) + std::move(method);

										// Create the file path to our image file 
										std::string FilePath = Input_ImageFolderPath.GetString()
										+ std::string("\\chart_image.png");

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

										// Close the image file
										file.close();

										// Call the HTTP POST Request in a separate thread. 
										// (not recommended for large scale operations)
										// This function calls sendPhoto Telegram method
										std::thread request_thread(send_photo, URL, ChatID, FilePath, line);

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
									
									// Debug 
									/* sc.AddMessageToLog("Alert was generated from a different chartbook. " */
									/* "That chartbook will handle the alert.",1); */
								}
							}
						}

						// close the alert log file (Outer most file) 
						file.close();
					}
					else
					{
						sc.AddMessageToLog("Error opening file! 1",1);
					}

				} 
				else
				{
					sc.AddMessageToLog("Error opening file! 2",1);
				}
			}
		}
	}

	if(sc.LastCallToFunction)
	{
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
