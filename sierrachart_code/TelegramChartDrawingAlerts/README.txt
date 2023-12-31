Sierra Chart Custom Study: Telegram Chart & Drawing Alerts
This video demonstrates the usage and set up of this Advanced Custom Study: https://youtu.be/EQZI9pBtDrE

This source code for this study can be viewed and downloaded here: https://github.com/Guitarmadillo/code/tree/main/sierrachart_code/TelegramChartDrawingAlerts

Info About the Code:

This advanced custom study was written by VerrilloTrading in Q3-Q4 of 2023. This study is open source and it can serve as an example for how to use a Telegram Bot with Sierra Chart. To compile this study users will need to include the necessary dependencies like boost::format and CURL. I tried as much as possible to stick to the Sierra Chart libraries but some things are much more practical with other c++ libraries. For example, there are very few existing examples of HTTP POST functionality using ACSIL. The study provides an example of this in combination with nlohmann json to prepare the request body. CURL was necessary to perform the sendPhoto Telegram method for sending multipart/form data. Anyone is welcome to repurpose the code, improve it or add functionality.

This study contains two examples of sending a http post request from Sierra Chart.
1. using CURL to call the Telegram sendPhoto method where the chart image is passed as multipart/form data.
2. using sc.MakeHTTPPOSTRequest() to call Telegram sendMessage method which is a native Sierra Chart ACSIL function.

The second example is in the study in the case that the user does not want to recieve the chart screenshot. Otherwise it is meant to serve as an example.

Reasons for sharing the source code:
1. Greater Transparency
2. Helps the community
3. Show off skills and features in Sierra Chart

Info for Users:

If you want to use this study and do not want to bother building it yourself you can send an email to support@verrillotrading.com with your Sierra Chart username and I will add your username to the list of authorized users for this DLL. When your username is added to this list the file will appear in your Sierra Chart Data Folder the next time you start Sierra Chart. The DLL has been compiled using this exact source code file **(except one change) and a build script.

** For convenience purposes The DLL provided by VerrilloTrading provides a default bot that users can rely on instead of having to create their own bot. The bot token for that bot was removed from the repository for security reasons.

Description of Study Functionality:

When this study is enabled and a Sierra Chart Drawing Alert or Chart/Study Alert takes place, a Telegram message including a chart screenshot will be sent to the specified Telegram Chat ID. This message will contain information about the alert that took place. For a full demonstration please watch this accompanying video on YouTube, and continue to follow along with this document. This study handles different cases and exceptions for when Sierra Chart generates new Alert Log Files. If you find any undefined behaviour or a replicable bug, please inform the study developer at the above email.

HOW TO SETUP AND USE THIS STUDY:

Step 1:

It is necessary to be running Sierra Chart Version 2566 or higher to use this study.

For this study to work it is necessary to Enable this setting in Sierra Chart:
Global Settings >> Log >> Save Alerts Log to File

When a Chart Drawing Alert is triggered, a new line is written to a file in the C:\SierraChart\Logs directory. The path to this Folder is relative to where you have Sierra Chart installed. It is also possible that this Logs Folder does not exist yet. Follow the next steps to ensure this folder exists.

When putting the study on the chart for the first time it will provide instructions in the message log. The first message will inform the user to enter the Telegram Chat ID.

Step 2:

Get the Telegram Chat ID of where we want to send the alert. The easiest way is to use Telegram Web to get the Telegram Chat ID. Telegram Groups and Private Chats are supported. Channels are a bit more complex so they have been left out for the time being. It is necessary to be on Telegram Web version A to get the Chat ID from the browser address bar. The version can be selected from the settings window in the top left corner when using Telegram Web. The Chat ID for the currently open chat appears in the address bar. Try changing to different chats to see the end of the address bar change. Example: https://web.telegram.org/a/#-12345678 where ‘-12345567’ is the Chat ID. Groups and Group Sub-Topics typically have negative numbers as the Chat ID. To get your personal chat ID, go to the Saved Messages chat. On Telegram Web this can be accessed by clicking at the top left and selecting ‘Saved Messages’. The address bar should display your Private Chat ID.

The default bot that the study uses on Telegram is https://t.me/vt_sierrachart_sender_bot.
You need to message this bot with /start on Telegram or add it to your group, unless you are using your own bot. Telegram bots cannot message you first and they cannot read messages sent by other bots.

Users can override the default bot with their their own bot if they prefer.

Step 3:

Perform the following steps after you have inputted the Telegram Chat ID in Step 2. If you still see an Error in the Message Log about not being able to find the Logs folder, perform the following steps:

1. Make sure you enabled the setting mentioned above named Save Alerts Log to File.
2. Check if the Folder ‘Logs’ exists in your Sierra Chart installation directory.
3. If the Logs folder does not exist, create a new Chart Drawing Alert and make sure it is triggered. Once this first alert is triggered, the folder should be created.
4. Check inside the Logs folder for a file that starts with ‘Alert Log’ followed by the current DateTime. If you do not see this file, repeat the instructions in Step 1.

Step 4:

Important: Make sure Study Input #4 is set to the Image Folder Path that is set in Global Settings > Paths > Chart Image Folder. When this setting is changed in Sierra Chart, a new folder will be automatically created. It is absolutely critical that you set Study Input #4 input text to this path, or the screenshot will not work. You will get an Error showing up in the message log if the path you specified in this Study Input does not exist. The default path for this on Windows is: C:\SierraChart\Images If you are running on Linux through Wine or if you changed the drive letter of your C drive it might look like this: Z:\mnt\SierraChart\Images. This study is confirmed to work when Sierra Chart is being run on Mac using Parallels.

For the screenshot function to work when charts are hidden behind other charts in your chartbook it is necessary to disable Destroy Chart Windows When Hidden.

STUDY AND ALERT CONFIGURATION:

Alert Settings:

Users have multiple options for how they can configure Alerts in Sierra Chart. A Chart Alert is set up by going to Chart Settings > Alert. Identical settings exist for Study Alerts which can be set up in the Study Settings for any particular study and going to the Alerts tab. To enable a Chart/Study Alert, an alert formula is necessary. Refer to this documentation for info about Chart/Study Alerts and Formula examples. If the user wants to be repeatedly notified of an alert being triggered, they should disable Disable Simple Alert After Trigger. This will result in repeated alerts being sent to Telegram, about one second apart. If the user wants to send one alert and disable the alert when it is triggered, it is necessary to enable this setting.

Chart Drawing Alerts are configured by right clicking on an existing Chart Drawing and selecting Alerts. Similar settings exist for Chart Drawing Alerts where the user can choose to enable Only Trigger Once or Alert Once Per Bar.

Usage Across Multiple Chartbooks:

The screenshot function from Sierra Chart is limited to taking screenshots of charts in the chartbook where the function call originates from. For this reason it is necessary to have 1 instance of this study open per chartbook. If you have 5 open chartbooks with different symbols waiting for alerts, you will need to open this study on a chart in each one of those chartbooks. When the study is active in any particular chartbook it will handle all the alerts that take place within that chartbook and provide an image of the chart as it is displayed in the chartbook. The chart can be hidden behind other charts and the screenshot will work as long as Destroy Chart Windows When Hidden is disabled.

Detecting Duplicates of This Study Within the Same Chartbook:

It is necessary to keep no more than 1 instance of this study per open chartbook. If the user added the study more than once by accident, this will result in the user recieving duplicate alerts on Telegram. This study contains a feature which can detect if another instance of this study exists within the same chartbook.

1. It is necessary to be on the newest version of the study.
2. Enable Study Input #9 which is named Find Duplicate Studies in Current Chartbook.
3. Open the Sierra Chart Message Log by going to Window > Message Log.
4. Create and trigger a Chart Drawing or Chart Alert and monitor the Message Log.
5. Look out for a message that looks like this:

“A duplicate of Telegram Chart Drawing Alerts study is detected in the same chartbook. This duplicate was found on chart number (here it tells you the chart number). Reduce the number of instances of this study to one per chartbook, or you will recieve double alerts.”

6. Then you can proceed to Window > Windows and Chartbooks, select the chart that has this chart number and remove the study from this chart.

INFO ABOUT THE LOG FILE PATH:

The following information applies to you if you have moved the Sierra Chart Data Files Folder out of the SC Installation directory.

It is not recommended to move the location of the Data Files Folder from the Sierra Chart installation directory.

If you absolutely need to move this folder or have already moved it, you will need to perform this next step:

IMPORTANT: If you have changed the location of the Data Files Folder, You will need to explicitly specify the directory of the Sierra Chart Logs folder. This folder will be located in the same directory where Sierra Chart is installed. The reason why this is necessary is because the study uses the Data Files Folder Path to get the Path to the Logs Folder. Example: if you moved the Data Folder to ANY PATH that is not the Sierra Chart Installation directory, you will now need to specify the Path of the Logs Folder which is in the directory where Sierra Chart is installed. You need to explicitly specify the Logs Folder path in the last study input field.

The default location of this folder is: C:\SierraChart\Logs If you are running on Linux through Wine or if you changed the drive letter of your C drive it might look like this: Z:\mnt\SierraChart\Logs

For any issues or inquiries, send them to support@verrillotrading.com

Thank you and enjoy the study!

-VerrilloTrading, Content Creator – Programmer

© 2023 VerrilloTrading Inc. All Rights Reserved.
