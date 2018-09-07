/* 
 * File:   RemainTime.h
 * Author: Rin
 *
 * Created on 2012/11/03, 12:36
 */

#ifndef REMAINTIME_H
#define	REMAINTIME_H

#include<iostream>
#include<ctime>

using namespace std;

struct rt{
	int d;
	int h;
	int m;
	int s;
};

class RemainTime
{
protected:
	double total;
	int d1, h1, m1, s1;
	time_t t1, t2;
	tm *ltm1, *ltm2;
public:
	RemainTime(void);
	RemainTime(const double &tot);
	virtual ~RemainTime(void);
	void set(const double &tot);
	void settot(const double &tot){ total = tot; }
	void set_dhms1(const rt* itm);
	tm* past_t();
	tm* remain(const double &prog);
};

#endif	/* REMAINTIME_H */

