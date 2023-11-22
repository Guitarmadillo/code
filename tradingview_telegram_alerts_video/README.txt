How to Set up a Telegram Alert from TradingView (No Code)

This README file contains all of the contents that appear on this web page: 
https://www.verrillotrading.com/tradingview-telegram-chart-alerts/

It is recommended to view the web page instead because it contains images with annotations. 

The video on YouTube that shows this demonstration: 
https://www.youtube.com/watch?v=P4vRNH7GJuc

This guide shows how to use TradingView to send a Chart Alert to a Private Telegram Chat or Group Chat using a Telegram Bot. No code is necessary and it is straight forward to set up if you follow this guide. TradingView already has the necessary functionality built in for sending a HTTP POST request. You just need to specify where this request goes, here we will use the Telegram Bot API. The only limitation TradingView has at the time of this video being published is that TradingView is not capable of dynamically taking a real-time screenshot of the chart when a chart alert is triggered. This functionality is fully working in another study that exists in Sierra Chart.

Why would someone do this since TradingView already is capable of email and push notifications?

These are features that Telegram provides that makes this method worth using:

- Customize Notifications for each Telegram Chat independently.
- Use Custom Audio Files (up to 300kb size) as notifications for specific Telegram Chats.
- Use Telegram Groups to notify multiple users of a specific signal or price alert.
- Telegram and it’s APIs are impressively fast and reliable and it’s back-end servers are built in c++.
- Getting trade alerts by email isn’t great, this method provides a way to stay organized with alert

Emails are slow! The existing notification method in TradingView is fine, except if you need to be notified when your mobile device is not near you. With Telegram you can configure on a per chat basis how notifications work. You can mute specific chats, you can set custom audio tracks as the notification sound. This means you could set a particularly loud alert sound and only enable it for one specific chat on Telegram, while the others are muted.

You could set a very long notification sound in the case that you want it to wake you up from sleep. You would record yourself saying a specific voice message saying “wake up, etc”.

How it Works

TradingView has a WebHooks feature which allows the user to tell TradingView to send a HTTP Post request to the desired URL. It allows the user the provide a json serialized object as the body of the request which is exactly what the Telegram Bot API requires. The TradingView user needs to set up the Webhook URL one time and provide an Alert Message Body. Once configured it should be plug and play unless you need to change a chat ID or the general formatting of the alert message.

    This requires a paid TradingView Plan
    It might require Real-Time Market Data on TradingView to ensure Price and DateTime values are correct.

WebHooks only work when the user is logged in with Two Factor Authentication. It is necessary to pay for TradingView in order to enable Two Factor Authentication on your TradingView Account.

Once you enable Two Factor Authentication, refresh the TradingView chart page.

Now it is time to create a new Telegram Bot using Bot Father on Telegram

Obtain Your Telegram Bot Token

In this context, a token is a string that authenticates your bot (not your account) on the bot API. Each bot has a unique token which can also be revoked at any time via @BotFather.

Obtaining a token is as simple as contacting @BotFather, issuing the /newbot command and following the steps until you’re given a new token. You can find a step-by-step guide here.

Your token will look something like this:

4839574812:AAFD39kkdpWt3ywyRZergyOLMaJhac60qc

Make sure to save your token in a secure place, treat it like a password and don’t share it with anyone.

Start a Private Chat with the Bot / Add it to a Group

Allow your bot to send you messages by sending it the /start command.

Add your bot to your Group using Telegram Mobile or Telegram Desktop.

Get the Telegram Chat ID using Telegram Web A

Ensure that Version A is enabled on Telegram Web.

Save the Chat ID from the address bar. Chat IDs for Groups and Group sub-topics are normally negative numbers.

The Saved Messages chat in Telegram Web A provides your Private Chat ID. Use this ID if you want the Bot to send you a private message after sending it the /start command. 


Create TradingView Chart Alert and Setup the WebHook

Let’s proceed back to TradingView to create our alert. We can reuse the same configuration later so these steps only need to be taken once.

Create a new TradingView Chart Alert using your method of choice. In the Alerts Menu you can re-use alerts that have already been triggered.

Input Webhook URL with Bot Token and API Method

1. Assuming the bot token is:

4839574812:AAFD39kkdpWt3ywyRZergyOLMaJhac60qc

2. Assuming we want to use sendMessage method of the Telegram Bot API.

The URL to input in the TradingView Webhook URL field should have this format:

https://api.telegram.org/bot4839574812:AAFD39kkdpWt3ywyRZergyOLMaJhac60qc/sendMessage

It is also possible to call other Telegram methods like sendPhoto however TradingView does not have the capability to provide an updated image of the chart when an alert goes off. You can pass in the link to your chart in the message body, this is a temporary workaround. More examples will follow. 

Create the Alert Message Body using Json and TradingView Objects

IMPORTANT: For all of the following examples you need to input your own Chat ID and Chart URL, or Chart Image URL. The Chat ID Provided in the Json examples is not valid.

Example 1:

This alert will send a simple message using TradingView objects to pass in the Exchange, Ticker and Last Price. Markdown formatting is used to achieve Bold and Monospace text.

{
  "chat_id": "1234567890",
  "text": "*T\\/ Tradingview Alert T\\/*\n{{exchange}}:{{ticker}}, price = `{{close}}`\nDateTime = {{time}}",
  "parse_mode": "Markdown"
}

Copy the Alert Message Body exactly as provided, change the Chat ID and create the alert. TradingView will let you know if the json format is correct. 

Wait for Alert to Trigger and Magic!

TradingView has some funky formatting for their {{time}} variable, and they do mention it is limited to UTC time. (annoying in my opinion)


NOT BAD for a first alert! Can we get a chart screenshot in there, or something else? After all there is a free Sierra Chart Study called Telegram Chart Drawing Alerts that does exactly this: 

Solutions for Sending the Chart Image on TradingView

In this next example I am going to show you a temporarily workaround for the real-time screenshot not being available in TradingView.

This next example requires the user to enable Read-Only Chart Sharing for their chart on TradingView:

The link to your TradingView Chart can be passed into the message text, however there will be no link preview for this. The user can click on the link provided in the Telegram message to view the TradingView Chart.


Enable Read-Only Chart Sharing and Pass in the Chart Link

Enable Read-Only Chart Sharing for this specific TradingView Chart.

Copy the TradingView Chart URL to pass into the alert message body.

Example 2:

The following example passes in the link to a TradingView Chart. If Read-Only Chart Sharing is enabled, the reader of the Telegram Alert can view the chart in a browser.

{
"chat_id": "123456790",
"text": " *T\\/ Tradingview Alert T\\/*\n{{exchange}}:{{ticker}}, price = `{{close}}`\nCheck out the chart [here:](https://www.tradingview.com/chart/wwACrVji/)", "parse_mode": "Markdown",
"disable_web_page_preview": 1
 }

Example 3:

Sending a photo along with the message is supported with the Telegram Bot API as you have seen in the Sierra Chart example. Assuming we had an up to date URL to a screenshot of our chart when the alert was triggered, how would we pass this in from TradingView? For now we can demonstrate this by generating a Chart Snapshot on TradingView and passing this image URL into our message body.


Once you have the Image URL you can pass it as the “photo” item in the following Json object.

For this specific object to work you need to change the end of your Webhook URL to sendPhoto instead of sendMessage.

Assuming the bot token is:

4839574812:AAFD39kkdpWt3ywyRZergyOLMaJhac60qc

The Webhook URL needs to be changed to:

https://api.telegram.org/bot4839574812:AAFD39kkdpWt3ywyRZergyOLMaJhac60qc/sendPhoto

Then use a Json object like this for the TradingView Alert Message Body. This message body is meant to use with the sendPhoto method of the Telegram Bot API.


{
  "chat_id": "-0123456789",
  "caption": "this is a TradingView text to accompany the image!",
  "photo": "https://www.tradingview.com/x/hv8mksgi/"
}

If you wanted to add some formatting to the text like we did in the previous message, try this:

{
  "chat_id": "-0123456789",
  "caption": " *T\\/ Tradingview Alert T\\/*\n{{exchange}}:{{ticker}}, price = `{{close}}`\nCheck out the chart [here:](https://www.tradingview.com/chart/wwACrVji/)",
  "photo": "https://www.tradingview.com/x/hv8mksgi/",
  "parse_mode": "Markdown",
  "disable_web_page_preview": 1
}

The issue with this is the image has been taken in the past and there is no way for TradingView to take a screenshot of the chart in real-time.

TradingView should add support for generating a screenshot image of the chart at a URL that is previously known when an Alert is triggered.

Someone requested the feature on reddit:
https://www.reddit.com/r/TradingView/comments/170fki3/feature_request_automate_taking_a_real_time/


Small Disadvantage for Streamers:

If you are a streamer and frequently share screens with the public, when you hover the cursor over existing alerts, they show the alert message body which can contain the Telegram Chat ID. You might not want to display your Private Chat ID. In this case it is recommended to send your alerts to a Private Group, because even with the Chat ID, new users need to be admitted to join the group.
To Conclude:

Overall I think this method is very easy to use and it might help people come up with new ways to trade and manage their groups.

I hope you enjoyed this short detailed guide for sending a TradingView Alert to Telegram.

VerrilloTrading, Content Creator – Programmer

*Telegram Bot API Documentation was used from: https://core.telegram.org/bots/tutorial
