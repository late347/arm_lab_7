/*
 * LimitedCounter.cpp
 *
 *  Created on: 7 Mar 2018
 *      Author: Lauri
 */

#include "LimitedCounter.h"

LimitedCounter::~LimitedCounter() {
	// TODO Auto-generated destructor stub
}


LimitedCounter & LimitedCounter ::operator++(){
	if(count<max) {
		++count;
	}

	return *this;
}

LimitedCounter LimitedCounter::operator++(int){
	if(count<max){

		LimitedCounter old= *this;
		++count;
		return old;

	}
	else{
		return *this;
	}
}

LimitedCounter LimitedCounter::operator--(int){
	if(count>min){
		LimitedCounter old=*this;
		--count;
		return old;
	}
	else{
		return *this;
	}
}

LimitedCounter & LimitedCounter::operator--(){
	if(count>min){
		--count;
	}
	return *this;
}

LimitedCounter & LimitedCounter::operator=(const LimitedCounter & rhsref){
	if(this== &rhsref){
		return *this;
	}
	else{
		count=rhsref.count;
		min=rhsref.min;
		max=rhsref.max;
		return *this;
	}
}


bool LimitedCounter::operator==(const LimitedCounter & rhsref){
	if (count==rhsref.count){
		return true;
	}
	else{
		return false;
	}
}
bool LimitedCounter::operator>=(const LimitedCounter & rhsref){
	if(count>=rhsref.count){
		return true;
	}
	else{
		return false;
	}
}
bool LimitedCounter::operator<=(const LimitedCounter & rhsref){
	if(count<=rhsref.count){
		return true;
	}
	else{
		return false;
	}
}
bool LimitedCounter::operator<(const LimitedCounter & rhsref){
	if(count<rhsref.count){
		return true;
	}
	else{
		return false;
	}
}
bool LimitedCounter::operator>(const LimitedCounter & rhsref){
	if(count>rhsref.count){
		return true;
	}
	else{
		return false;
	}
}
