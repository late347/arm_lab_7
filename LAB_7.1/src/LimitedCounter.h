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


	LimitedCounter(int curvalue=50, int minvalue=0, int maxvalue=1000) : count(curvalue), min(minvalue), max(maxvalue) {}

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
