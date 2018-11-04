/*
 * LimitedCounter.h
 *
 *  Created on: 7 Mar 2018
 *      Author: Lauri
 */

#ifndef LIMITEDCOUNTER_H_
#define LIMITEDCOUNTER_H_

class LimitedCounter {
public:

	 const int centerValueServo=1500;
	 const int minValueServo=1000;
	 const int maxValueServo=2000;
	LimitedCounter(int curvalue=1500) : count(curvalue), min(minValueServo), max(maxValueServo) {}

	virtual ~LimitedCounter();

	LimitedCounter & operator++();	//prefix increment
	LimitedCounter operator++(int); //postfix incremnet

	LimitedCounter operator--(int); //postfix decrement
	LimitedCounter & operator--();	//prefix decrement

	LimitedCounter & operator=(const LimitedCounter & rhs); //assignment operator

	operator int() {return count;} // conversion operator

	/*boolean operators*/
	bool operator==(const LimitedCounter & rhs);
	bool operator<(const LimitedCounter & rhs);
	bool operator>(const LimitedCounter & rhs);
	bool operator<=(const LimitedCounter & rhs);
	bool operator>=(const LimitedCounter & rhs);



private:
	int count;
	int min;
	int max;
};

#endif /* LIMITEDCOUNTER_H_ */
