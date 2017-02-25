
//{translation,rotation,claw,lift,lock}
int autonomousRight[][] = {
	{12,0,0,130,0},
	{38,0,0,0,0},
	{0,180,0,0,0}
};

int autonomousLeft[][] = {
	{0,127,0,0,150},//run to fence
	{0,127,127,127,800},//run up to fence, with claws opening and lift going up
	{0,127,0,127,950},//keep going but claws stop
	{0,-127,0,0,400},//go back
	{0,-127,0,-127,900},//go back and lower lift
	{0,0,0,-127,200},//lower lift
	{-127,0,0,0,500},//turn towards cube
	{0,127,0,0,800},//drive towards cube
	{0,0,127,0,1000},//grab cube
	{0,127,30,127,300},//drive forwards and lift up
	{127,0,30,127,600},//lift cube while turning to fence
	{0,0,30,127,600},//lift cube
	{0,127,30,127,1050},//lift cube to fence height and drive to fence
	{0,0,-127,0,800},//drop cube
	{0,-127,0,0,500},//drive backwards
	{0,-127,0,-127,600},//drive backwards and lift down
	{0,127,0,0,600},//drive fowards
	{0,127,0,127,650},//drive fowards and lift up
	{0,-127,0,0,700},//drive backwards
	{0,-127,0,-127,500},//drive backwards and lift down
	{0,0,0,0,0}//stop
};
