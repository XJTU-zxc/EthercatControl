
#include "variabledef.h"
#include "accLineFloat.h"
#include "accSFloat.h"

#ifndef swing_H
#define swing_H

class Cswing
{
	public:

		enum STATUS status;

		struct IN
		{	long   			left_mm;
			long			right_mm;
			unsigned int 	valid;
		} in;


		struct DIS
		{
			//long			position;
			float 			position;
		//	float			v;
			unsigned int	valid;
		} view;

		//class Cacc   ob1;

	private:
		long 			start_posi;
	 	unsigned int 	phase;

	//	class CaccLineFloat   ob1;
		class CaccSFloat   ob1;



public:
	void reset(void);
	void sureInput(void);
	void run(void);
};

#endif


