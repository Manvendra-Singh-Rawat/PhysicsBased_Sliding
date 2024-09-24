# PhysicsBased_Sliding
 
Implemented Crouch, Running and slide mechanics.
<br>Crouch and running were easy.
<br>Sliding required some basic math and vector multiplication skills and tweaking the deceleration friction for the player.
<br>Slideing according the incline surface and animation montage is not done as of now, will complete the project fully in a couple of days.
<p>
<br>Mostly used C++ with the use of Timelines from UE blueprint (2 times only)
<br>Timelines were used to create:-
<br><span>&nbsp;&nbsp;&nbsp;&nbsp;</span>1. 1st Timeline is used to make the capsule shorter or revert back it to its original size for the player going into crouch or getting into walk or run state resp.
<br><span>&nbsp;&nbsp;&nbsp;&nbsp;</span>2. 2nd Timeline is used to create an infinite loop the the slide as the slide is physics based then it will only stop when the velocity goes down a certain point.
</p>
<br>You can also cancle the slide but only with jump as the player movement input is disabled when the player is in sliding state.
