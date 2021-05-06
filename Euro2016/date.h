#include <string>
#include <iostream>
std::string months[12] = {"January", "February", "March", "April", "May", 
			  "June", "July", "August", "September", "October",
			  "November", "December"};
class Date{
	private:
	int year;
	int month;
	int day;
	public:
	Date(int d,int m, int y){day = d;month =m;year=y;}
	bool operator < (const Date& d){
		return (year < d.year) || (year ==d.year && month < d.month)||
			(year==d.year && month==d.month && day < d.day);
	};
	int get_year()const{return year;}
	int get_month()const{return month;}
	int get_day()const{return day;}
	friend std::ostream& operator << (std::ostream& ostr, const Date& d);

};
std::ostream& operator << (std::ostream& ostr, const Date& d){
	ostr << d.year << '-' << d.month << '-' << d.day;
	return ostr;
};
