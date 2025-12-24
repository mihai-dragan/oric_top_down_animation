A minimal top down animation test for one 18x18 sprite. Move sprite with W,A,S,D keys. No screen bounds detection.
Created as a test to reproduce a bug in the Oric Skweeks game.
Here is also the pseudocode:
```
while(notsleeping > 0 && restartlvl == 0) {
	if(skw_act == STAY or skw_act == SLEEP) {
		if(!skw_over_arrow) { // can't move if stopped over arrow
			if('w' or 'a' or 's' or 'd' press) {
				skw_act = MOVE;
				skw_dir = LEFT or RIGHT or UP or DOWN
			}
		}
		if('ret' press) {
			change current skweek
			player->action = SELECT;
		}
	}
	if(time to redraw) {
		if(skw_act==SELECT) {
			draw select rectangle, can take multiple frames
		}
		if(skw_act==MOVE) {
			compute next frame and next draw address
			if(skw_steps==3) { //takes four steps to move from one grid square to the other
				if(next square is blocked) {
					if(square under is same color sleep pod) skw_action = SLEEP;
					else skw_action = STAY;
				}
			}
			redraw current skweek frame at pos and clean if necessary
		}
		if(skw_act == STAY or skw_act == SLEEP) {
			draw stay or sleep frame at skw_pos
		}
	}
}
```
