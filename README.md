# 2361_UMN_FINAL
Light the Beam Final Project

w code by croix and isaiah and thomas

I am getting a value from RDATAH high register consistently but it is not changing, I am making a new function to exit the wait state to see if that is the reason the register is not updating (Isaiah, Apr 14 3:33 pm)

exitWait function is not fixing the problem so I'm working on a function that reads all 16 bits from the red registers to maybe end the data hold discussed on pg 19 of the sensor Datasheet (Isaiah, Apr 14 4:04 pm)

I added a getRGB() function and improved on digital to hex color conversion. Seems to be accurately measuring color on first run through, but there is large error in proceding samples. I believe this is due to high sample rate but we should take a look as a group soon. Mostly because Croix knows how to change the sample rate on the color sensor better than I do. I also made a copy of our current main function for testing the color sensor in the future when other components/operations may get added(Isaiah, Apr 17 2:37 pm)
