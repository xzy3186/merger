/* 
 * File:   RemainTime.cpp
 * Author: Rin
 * 
 * Created on 2012/11/03, 12:36
 */

#include "RemainTime.h"

RemainTime::RemainTime(void)
{
}

RemainTime::RemainTime(const double &tot)
{
	set(tot);
}

RemainTime::~RemainTime(void)
{
}

void RemainTime::set(const double &tot)
{
	time(&t1);
	ltm1 = localtime(&t1);
	total = tot;
	d1 = ltm1->tm_yday;
	h1 = ltm1->tm_hour;
	m1 = ltm1->tm_min;
	s1 = ltm1->tm_sec;
	return;
}

void RemainTime::set_dhms1(const rt* itm)
{
	d1 = itm->d;
	h1 = itm->h;
	m1 = itm->m;
	s1 = itm->s;
	return;
}

tm* RemainTime::past_t()
{
	tm *rtm;
	time(&t2);
	ltm2 = localtime(&t2);
	rtm = localtime(&t2);
	int d2 = ltm2->tm_yday, d3;
	int h2 = ltm2->tm_hour, h3;
	int m2 = ltm2->tm_min, m3;
	int s2 = ltm2->tm_sec, s3;
	int c;
	s3 = s2 - s1;
	if(s3 < 0){
		c = 1;
		s3 = 60 + s3;
	}else
		c = 0;
	m3 = m2 - m1 - c;
	if(m3 < 0){
		c = 1;
		m3 = 60 + m3;
	}else
		c = 0;
	h3 = h2 - h1 - c;
	if(h3 < 0){
		c = 1;
		h3 = 24 + h3;
	}else
		c = 0;
	d3 = d2 - d1 - c;

	rtm->tm_yday = d3;
	rtm->tm_hour = h3;
	rtm->tm_min = m3;
	rtm->tm_sec = s3;
	return rtm;
}

tm* RemainTime::remain(const double &prog)
{
	tm* rtm;
	rtm = past_t();
	long c;
	c = rtm->tm_sec;
	c += rtm->tm_min*60;
	c += rtm->tm_hour*3600;
	c += rtm->tm_yday*3600*24;
	const double r = total/prog - 1.0;
	c = c*r;
	rtm->tm_yday = (int)c/(3600*24);
	rtm->tm_hour = (int)(c/3600) % 3600;
	rtm->tm_min = (int)(c/60) % 60;
	rtm->tm_sec = c % 60;
	return rtm;
}

