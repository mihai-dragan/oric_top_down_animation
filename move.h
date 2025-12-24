void move_right(struct sprite *spr) {
	if(spr->cur_frame < last_frame) { // set next frame
		spr->cur_frame++;
		spr->frame_pointer+=frame_offset;
		if(spr->cur_frame == last_frame) {
			need_cleanup = 1;
			spr->pos = spr->pos+1;
		}
	} else {
		spr->cur_frame = 0;
		spr->frame_pointer = 0;
		spr->steps = spr->steps + 1;
		if(spr->steps==3) {
			spr->origpos = spr->pos;
			spr->gridpos = spr->gridpos + 1;
			spr->steps = 0;
			spr->action = STAY;
		}
	}
}

void move_left(struct sprite *spr) {
	if(spr->cur_frame == 0) { // set next frame
		spr->cur_frame = last_frame;
		spr->frame_pointer = last_frame_offset;
	} else {
		if(spr->cur_frame == 1) {
			spr->cur_frame = 0;
			spr->frame_pointer = 0;
			spr->steps = spr->steps + 1;
			if(spr->steps==3) {
				spr->origpos = spr->pos;
				spr->gridpos = spr->gridpos - 1;
				spr->steps = 0;
				spr->action = STAY;
			}
		} else {
			spr->pos = spr->pos-1;
			need_cleanup = 1;
			spr->cur_frame--;
			spr->frame_pointer-=frame_offset;
		}
	}
}

void move_down(struct sprite *spr) {
	if(spr->cur_frame < last_frame) { // set next frame
		if(spr->cur_frame==0&&spr->steps==0) {
			spr->origpos = spr->pos;
		}
		spr->pos = spr->pos+80;
		spr->cur_frame++;
		spr->frame_pointer+=frame_offset;
	} else {
		spr->pos = spr->pos+80;
		spr->cur_frame = 0;
		spr->frame_pointer = 0;
		need_cleanup = 1;
		spr->steps = spr->steps + 1;
		if(spr->steps==3) {
			spr->gridpos = spr->gridpos + 13;
			spr->steps = 0;
			spr->action = STAY;
		}
	}
}

void move_up(struct sprite *spr) {
	if(spr->cur_frame < last_frame) { // set next frame
		if(spr->cur_frame==0&&spr->steps==0) {
			spr->origpos = spr->pos;
		}
		spr->pos = spr->pos-80;
		spr->cur_frame++;
		spr->frame_pointer+=frame_offset;
	} else {
		spr->pos = spr->pos-80;
		spr->cur_frame = 0;
		spr->frame_pointer = 0;
		need_cleanup = 1;
		spr->steps = spr->steps + 1;
		if(spr->steps==3) {
			spr->gridpos = spr->gridpos - 13;
			spr->steps = 0;
			spr->action = STAY;
		}
	}
}
