#include "math.h"
#include "swing.h"


void Cswing::reset(void)
{ 	status=idle;
	phase=0;

	in.left_mm=10;
	in.right_mm=10;
	view.position=0;
//	deltaT=0.001;//1ms
//	Vmax=0.10;
//	A=1.33;
}
//-----------------------------
void Cswing::sureInput(void)
{  in.valid=1;
}
//---------------------------------
void Cswing::run(void)
{	switch(phase)
	{
		case 0:// load data move to left
			ob1.reset();
			ob1.in.length=in.left_mm;//9000;
			ob1.sureInput();
			view.position=0;
			start_posi=view.position;
			phase=1;

		//break;
		case 1:// move to left
			ob1.run();
			view.position=start_posi - ob1.view.position;

			if(ob1.status==END)
			{	phase=2;
				ob1.reset();//load data	 move to zero
				ob1.in.length=in.left_mm;//9000;
				ob1.sureInput();
				start_posi=view.position;
			}
		break;
		case 2:// move to zero
			ob1.run();
			view.position=start_posi + ob1.view.position;

			if(ob1.status==END)
			{	phase=3;
				ob1.reset();//load input move to right
				ob1.in.length=in.right_mm;//9000;
				ob1.sureInput();
				start_posi=view.position;
			}
		break;
		case 3:// move to right
			ob1.run();
			view.position=start_posi + ob1.view.position;

			if(ob1.status==END)
			{	phase=4;
				ob1.reset();//load data move to zero
				ob1.in.length=in.right_mm;//9000;
				ob1.sureInput();
				start_posi=view.position;
			}
		break;
		case 4:// move to zero
			ob1.run();
			view.position=start_posi - ob1.view.position;

			if(ob1.status==END)
			{	phase=1;
				ob1.reset();//load data move to left
				ob1.in.length=in.left_mm;//9000;
				ob1.sureInput();
				start_posi=view.position;
			}
		break;
		default:phase=0;
	}
}


