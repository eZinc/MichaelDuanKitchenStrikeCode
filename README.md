# MichaelDuanKitchenStrikeCode
A copy of the source code for Kitchen Strike (http://blog.kitchenstrike.com/) by Michael Duan.

I intended to go back and clean up all the code and replace interfaces with a component based system and add more classes server-side as opposed to a glob of code in the GameMode class. In particular, the code for player to object interaction is kind of nasty. Also, this is in Unreal Engine 4 so there are a decent number of calls to Blueprint scripts for UI/animations. Still, I decided to showcase this project because I was the sole contributor to the project and it had a good amount of typical AAA game features. It is an online multiplayer game, but it uses the default UE4 networking model instead of custom networking with C++ sockets.
