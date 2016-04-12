WARNING: THE VIDEO CONTAINS RAPIDLY FLASHING COLOURS AFTER 1:17


Unfortunately we were not able to get our path finder working in parallel.
The video uses a simple stepper that increments the camera towards 0,0,0.
This is done by multiplying the current camera position by 0.9993 which progressed too quickly.
0.99978 or greater (but less than 1) should produce a much smooth video that spends much less time rendering from inside the objects.

The video is alright until about 1:17 where the camera renders while mostly inside objects,
resulting in frames of near solid colouring.



