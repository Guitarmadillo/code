This advanced custom study was written by Guitarmadillo at VerrilloTrading in Q3-Q4 of 2023. This study will be open source and it can serve as an example for how to use a Telegram Bot with Sierra Chart. To compile this study users will need to include the necessary dependencies like boost::format and CURL. I tried as much as possible to stick to the Sierra Chart libraries but some things are much more practical with other c++ libraries. For example, there are very few if any existing examples of HTTP POST functionality using ACSIL. An example of this is provided in combination with nlohmann json to format the json object. CURL was necessary to perform the sendPhoto Telegram method for sending multipart/form data. Anyone is welcome to repurpose the code, improve it or add functionality.

This study contains two examples of sending a http post request from Sierra Chart.
1. using CURL to call the Telegram sendPhoto method where the chart image is passed as multipart/form data.
2. using sc.MakeHTTPPOSTRequest() to call Telegram sendMessage method which is a native Sierra Chart ACSIL function.

The second example is in the study in the case that the user does not want to recieve the chart screenshot. Otherwise it is meant to serve as an example.

Reasons for sharing the source code:
1. Greater Transparency
2. Helps the community
3. Show off skills and features in Sierra Chart

If you want to use this study and do not want to bother building it yourself you can send an email to support@verrillotrading.com with your Sierra Chart username and I will add your username to the list of allowed users for this .DLL. The .DLL has been compiled using this exact source code file **(except one change) and a build script.

** For convenience purposes The .DLL provided by VerrilloTrading provides a default bot that users can rely on instead of having to create their own bot. The bot token for that bot was removed from the repository for security reasons.

HOW TO SETUP AND USE THIS STUDY:

When this study is enabled and a chart drawing alert takes place, a Telegram message will be sent to the specified Chat Id. This message will contain information about the alert that took place, and a real-time screenshot of the chart where the alert came from. It is possible for the user to disable the screenshot feature and send a simple message.

Use Telegram Web to get the Telegram Chat ID. It is necessary to be on Telegram Web version A to get the Chat ID from the address bar. This can be selected from the settings window in the top left when using Telegram Web. The Chat ID appears in the address bar. Example: https://web.telegram.org/a/#-12345678 where '-12345567' is the Chat ID. Groups and sub topics in groups typically have negative numbers as the Chat ID. To get your personal chat ID, go to the Saved Messages chat. On Telegram Web this can be accessed by clicking at the top right and it should appear in the drop down as 'Saved Messages'.

Telegram Groups and Private chats are supported. Channels are a bit more complex so they have been left out for the time being. You will need to add the bot to the group first. For private chats, you need to send a /start message to the bot first, Telegram bots cannot message you first. The default bot that the study uses on Telegram is https://t.me/vt_sierrachart_sender_bot. You need to message this bot with /start on Telegram, unless you are using your own bot.

Users can override the default bot with their their own bot if they prefer.

The chart drawing will handle the alert and it's condition, but when the alert occurs, this study will be able to see that a new alert occurred by reading the log file.

It is necessary to enable this setting in Sierra Chart:
Global Settings >> Log >> Save Alerts Log to File

Important: It is necessary to make sure Input #4 is set to the path that is set in Global Settings > Paths > Chart Image Folder. When this Setting is changed in Sierra Chart, the folder will be automatically automatically created. It is absolutely critical that you set the study input #4 text to this path. You will get an Error showing up in the message log if the path you specified in the Study input does not exist. Example: The default path for this on Windows is: C:\SierraChart\Images If you are running on Linux thrugh Wine or if you changed the drive letter of your C drive it might look like this: Z:\mnt\SierraChart\Images

When a chart drawing alert is triggered, a new line is written to a file in the following directory: C:\SierraChart\Logs

This study handles the various cases for when Sierra Chart generates new alert files.

Important: For the screenshot function to work when charts are hidden behind other charts in your chartbook it is necessary to disable Destroy Chart Windows When Hidden.
More info about this setting and how it is handled for the screenshot function:
sc.SaveChartImageToFileExtended

Important: The screenshot function from Sierra Chart is limited to taking screenshots of charts in the chartbook where the function call originates from. For this reason it is necessary to have 1 instance of this study open per chartbook. So if you have 5 open chartbooks with different symbols waiting for alerts, you will need to open this study on a chart in each one of those chartbooks. When the study is active in any particular chartbook it will handle all the alerts that take place within that chartbook and provide an image of the chart as it is displayed in the chartbook. The chart can be hidden behind other charts and the screenshot will work as long as Destroy Chart Windows When Hidden is disabled.

INFO ABOUT THE LOG FILE PATH:

The study should function correctly AS LONG AS:
1. You have not changed the default location of the Data files folder.

It is not recommended to move the location of the Data Files Folder from the Sierra Chart installation directory!

If you absolutely need to move this folder you will need to perform this next step:

IMPORTANT: If you have changed the location of the Data Files Folder, You will need to explicitly specify the directory of the Sierra Chart Logs folder. This folder will be located in the same directory where Sierra Chart is installed. The reason why this is necessary is because the study uses the Data Files Folder Path to get the Path to the Logs Folder. Example: if you moved your Data folder path to ANY PATH that is not the Sierra Chart Installation directory, you will now need to specify the Path of the Logs Folder which should be in the directory where Sierra Chart is installed. You need to explicitly specify the Logs Folder path in the last study input field.

The default location of this folder is: C:\SierraChart\Logs

For any issues or inquiries, send them to support@verrillotrading.com

Thank you and I hope you enjoy the study!

-VerrilloTrading, Content Creator - Programmer
