# OverwatchSMSNotifier

This is a simple program that sends you a text when you get queued into an Overwatch game. Role Queue has made queuing times quite long for DPS players in particular, so this will allow you to do productive things in the meantime without worrying about being kicked for inactivity.

As far as I know, there's no noticeable performance impact, but there's also an option to stop screen capture completely after a game has started.

You will need  to have a [Twilio](https://www.twilio.com/try-twilio) account to enter your account info into `config.yaml`. You can set everything up without paying (I haven't paid a cent), it just adds a "sent from Twilio" watermark. You will need to set up a Twilio phone number (the FROM number) as well as register your phone number (the TO number) on Twilio's website to be able to receive SMS messages.

If it isn't working, open the console to check for error messages. It can be annoying to get the credentials correct.

## How do I use it?
Download the [binary zip from the releases page](https://github.com/sealj553/OverwatchSMSNotifier/releases/tag/0.1),
add your Twilio details and message to `config.yaml`, and run the .exe.

## How does it work?
* The screen is captured
* The area where the "Searching" dialog appears is cropped and filtered for its (somewhat) unique background color
* The ratio of the background color to other colors is monitored until it's at ~0.8. That most likely means it's searching for a game.
* An event is triggered when the ratio drops. That probably means a game has been found.

So far during my testing, I haven't had a single false positive in-game.

## Limitations (easy fix)
* Color range hardcoded, might depend on brightness and gamma
* Queue dialog hardcoded for 1920x1080

## Libraries Used
* OpenCV for image processing
* Twilio for sending SMS messages
* libcurl to communicate with the Twilio REST API
* yaml-cpp for the config file
* WinAPI for simple GUI

Programming on Windows sucks.
