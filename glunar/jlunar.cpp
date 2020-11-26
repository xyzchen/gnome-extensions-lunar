//------------------------------------------------------
//
//    jlunar.cpp
//        农历转换的实现部分
//
//       作者: 逸少 
//    jmchxy@163.com
//       创建日期: 2002.5.21
//
//-------------------------------------------------------

#include <math.h>
#include <string.h>
#include "jlunar.h"

//
//内部使用的数据结构
//
//--------------------------------------
//  几月几日 是 什么节 的 节日数据结构
//      公历农历通用
//--------------------------------------
typedef struct _tagHOLIDAY
{
	WORD  wMonth;			//月 1 ~ 12
	WORD  wDay;				//日 1 ~ 31
	char szName[24];		//节日名, 7 个汉字
}HOLIDAY;

//----------------------------------------
//  数据结构: 几月的第几个星期几是什么节?
//           公历专用
//----------------------------------------
typedef struct _tagHOLIDAY2
{
	WORD	wMonth;			// 几月? 1 ~ 12
	WORD	wN;				// 第几个星期?
	WORD	wWeekday;		// 星期几
	char	szName[24];		//节日名, 7 个汉字
}HOLIDAY2;

//-------------------------------------------------------------
//         下面是实现部分的常数数据定义
//-------------------------------------------------------------
/* PI值 */
const double PI = 3.1415926535897932;	

/* 15度对应的弧度  */
const double QUOTIETY = PI * 15.0 / 180.0; 

/* 天干名称 */
const char* const cg_szTianGan[] = {	//0 - 甲
		_T("甲"),	_T("乙"),	_T("丙"),	_T("丁"),	_T("戊"),
		_T("己"),	_T("庚"),	_T("辛"),	_T("壬"),	_T("癸")
	};

/* 地支名称 */
const char* const cg_szDiZhi[] = {	//0 - 子
		_T("子"),	_T("丑"),	_T("寅"),	_T("卯"),
		_T("辰"),	_T("巳"),	_T("午"),	_T("未"),
		_T("申"),	_T("酉"),	_T("戌"),	_T("亥")
	};

/* 十二生肖属相名称 */
const char* const cg_szShuXiang[] ={	// 0 - 鼠 
									_T("鼠"),	_T("牛"),	_T("虎"),	_T("兔"),
									_T("龙"),	_T("蛇"),	_T("马"),	_T("羊"),
									_T("猴"),	_T("鸡"),	_T("狗"),	_T("猪")
								};

/* 农历日期名 */
const char* const cg_szDayName[] = {	_T("*"), //0
		_T("初一"),  _T("初二"),  _T("初三"),  _T("初四"),  _T("初五"),
		_T("初六"),  _T("初七"),  _T("初八"),  _T("初九"),  _T("初十"),
		_T("十一"),  _T("十二"),  _T("十三"),  _T("十四"),  _T("十五"),
		_T("十六"),  _T("十七"),  _T("十八"),  _T("十九"),  _T("二十"),
		_T("廿一"),  _T("廿二"),  _T("廿三"),  _T("廿四"),  _T("廿五"),
		_T("廿六"),  _T("廿七"),  _T("廿八"),  _T("廿九"),  _T("三十")
		};

/* 农历月份名 */
const char* const cg_szMonName[] = {_T("*"),  //0
		_T("正"),  _T("二"),  _T("三"),  _T("四"),  _T("五"),  _T("六"),
		_T("七"),  _T("八"),  _T("九"),  _T("十"),  _T("冬"),  _T("腊")
		};

/* 星期名称, 0 = 星期日 */
const char* const cg_szWeekName[] = {
		_T("日"),	_T("一"),	_T("二"),	_T("三"),
		_T("四"),	_T("五"),	_T("六"),	_T("")
	};

/* 农历二十四节气名称数据 */
const char* const cg_szLunarJieqi[] = { // 0 ~ 23
		//  以 0 小寒 起算
		_T("小寒"), _T("大寒"), // 一月的节气
		_T("立春"), _T("雨水"), // 二月的节气
		_T("惊蛰"), _T("春分"),
		_T("清明"), _T("谷雨"),
		_T("立夏"), _T("小满"),
		_T("芒种"), _T("夏至"),
		_T("小暑"), _T("大暑"),
		_T("立秋"), _T("处暑"),
		_T("白露"), _T("秋分"),
		_T("寒露"), _T("霜降"),
		_T("立冬"), _T("小雪"),
		_T("大雪"), _T("冬至")
	};

/* 十二星座的数据 */
const char* const cg_szXingZuo[] = { //
		_T("摩羯"),	//12.22 ~ 1.21
		_T("宝瓶"),	// 1.22 ~ 2.21
		_T("双鱼"),	// 2.22 ~ 3.20
		_T("白羊"),	// 3.21 ~ 4.20
		_T("金牛"),	// 4.21 ~ 5.21
		_T("双子"),	// 5.22 ~ 6.21
		_T("巨蟹"),	// 6.22 ~ 7.21
		_T("狮子"),	// 7.22 ~ 8.21
		_T("处女"),	// 8.22 ~ 9.21
		_T("天秤"),	// 9.22 ~10.21
		_T("天蝎"),	//10.22 ~11.21
		_T("射手")	//11.22 ~12.21
	};

/* 公历节日数据 1 (几月几日)*/
const HOLIDAY cg_stHoliday[] = {
		{ 0,  0,  _T("")			},  //0
		{ 1,  1,  _T("元旦")		},	//1
		{ 2, 14,  _T("情人节")		},	//
		{ 3,  8,  _T("国际妇女节")  },	//
		{ 3, 12,  _T("植树节")      },	//
		{ 4,  1,  _T("愚人节")		},	//5
		{ 5,  1,  _T("国际劳动节")  },	//
		{ 5,  4,  _T("五四青年节")  },	//
		{ 5, 12,  _T("国际护士节")  },	//
		{ 6,  1,  _T("国际儿童节")  },	//
		{ 7,  1,  _T("建党节")      },	//10
		{ 8,  1,  _T("建军节")      },	//
		{ 9, 10,  _T("教师节")		},	//
		{10,  1,  _T("国庆节")		},	//
		{12, 15,  _T("作者生日")    },	//
		{12, 25,  _T("圣诞节")		}	//15
	};

/* 公历节日数据 2 (几月的第几个星期几)*/
// 我在这里只提供母亲节和父亲节
const HOLIDAY2 cg_stHoliday2[]= {
	{0, 0, 0, _T("")			},	// 0, 无
	{5, 2, 0, _T("母亲节")		},	// 5月的第2个星期天, 母亲节
	{6, 3, 0, _T("父亲节")		},	// 6月的第3个星期天, 父亲节
};

/* 农历传统节日数据　*/
const HOLIDAY cg_stLunarHoliday[] = {
		{ 0,  0,  _T("")			},//0, 无节日
		{ 1,  1,  _T("春节")		},//1
		{ 1, 15,  _T("元霄节")		},
		{ 5,  5,  _T("端午节")		},
		{ 7,  7,  _T("七夕")		},//5
		{ 8, 15,  _T("中秋节")		},
		{ 9,  9,  _T("重阳节")		},
		{12,  8,  _T("腊八节")		}
	};

//------------------------------------------------
//		下面是农历计算部分帮助函数
//------------------------------------------------
/* 公历每月前面的天数 */
const int cg_wMonthAdd[] = { // index =1 为1月
								0,	// 空白
								0,  31,  59,  90,  120,  151,	// 1 ~ 6月
								181,212, 243, 273, 304,  334	// 7 ~ 12月
							};

/* 公历每月的天数  */
const int cg_wMonthDays[] = { // index =1 为1月
								0, 
								31, 28, 31, 30, 31, 30, // 1 ~ 6月
								31, 31, 30, 31, 30, 31	// 7 ~ 12月
							};//

/* 农历数据 */
/*******************************************************************

数据格式:
	  月信息 从 bit13 开始存放.
      bit0  ~ bit13: 月份大小信息, bit12 ... bit0: 1月, 2月, .... 13月
	  bit13 ~ bit15: 空白
	  bit16 ~ bit19: 闰月月份(1 ~ 12), 0 表示没有闰月
	  bit20 ~ bit23: 春节的公历月份 (1 ~ 2)
	  bit24 ~ bit31: 春节的公历日.  (1 ~ 31)
  图:
   
     ----------------------------------------------------------------------------------------+
bit   31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16||15|14|13| 12 | 11 | ....... |  1 |  0 |
     ------------------------+-----------+-----------++--------+-----------------------------+
             日(1~31)        |  月(1~12) | (0 ~ 12)  || 0  0  0|1月 | 2月| ....... |12月|13月|
     ------------------------+-----------+-----------++--------+-----------------------------+
                       春节日期          | 闰月信息  || 未用   |       农历月份大小信息      |
     ------------------------------------+-----------++--------+-----------------------------+
********************************************************************/
const unsigned int cg_dwLunarInfo[] = 
	{// 时间原点为 1900-01-00(1899-12-31)为起点, 
		0x1F18096D, //1900年, 公历1900年1月31日春节(农历1900年正月初一), 以此为起点
		0x1320095C, 0x082014AE, 0x1D150A4D, 0x10201A4C, 0x04201B2A, 0x19140D55, 0x0D200AD4, 0x0220135A, 0x1612095D, 0x0A20095C,   // 1901 ～ 1910
		0x1E16149B, 0x1220149A, 0x06201A4A, 0x1A151AA5, 0x0E2016A8, 0x03201AD4, 0x171212DA, 0x0B2012B6, 0x01270937, 0x1420092E,   // 1911 ～ 1920
		0x08201496, 0x1C15164B, 0x10200D4A, 0x05200DA8, 0x181415B5, 0x0D20056C, 0x022012AE, 0x1712092F, 0x0A20092E, 0x1E160C96,	  // 1921 ～ 1930
		0x11201A94, 0x06201D4A, 0x1A150DA9, 0x0E200B5A, 0x0420056C, 0x1813126E, 0x0B20125C, 0x1F17192D, 0x1320192A, 0x08201A94,   // 1931 ～ 1940
		0x1B161B4A, 0x0F2016AA, 0x05200AD4, 0x1914155B, 0x0D2004BA, 0x0220125A, 0x1612192B, 0x0A20152A, 0x1D171695, 0x11200D94,   // 1941 ～ 1950
		0x062016AA, 0x1B150AB5, 0x0E2009B4, 0x032014B6, 0x18130A57, 0x0C200A56, 0x1F18152A, 0x12201D2A, 0x08200D54, 0x1C1715AA,   // 1951 ～ 1960
		0x0F20156A, 0x0520096C, 0x191414AE, 0x0D2014AE, 0x02200A4C, 0x15131D26, 0x09201B2A, 0x1E170B55, 0x11200AD4, 0x062012DA,   // 1961 ～ 1970
		0x1B15095D, 0x0F20095A, 0x0320149A, 0x17141A4D, 0x0B201A4A, 0x1F181AA5, 0x122016A8, 0x072016D4, 0x1C1612DA, 0x102012B6,   // 1971 ～ 1980
		0x05200936, 0x19141497, 0x0D201496, 0x022A164B, 0x14200D4A, 0x09200DA8, 0x1D1615B4, 0x1120156C, 0x0620126E, 0x1B15092F,   // 1981 ～ 1990
		0x0F20092E, 0x04200C96, 0x17130D4A, 0x0A201D4A, 0x1F180D65, 0x13200B58, 0x0720156C, 0x1C15126D, 0x1020125C, 0x0520192C,   // 1991 ～ 2000
		0x18141A95, 0x0C201A94, 0x01201B4A, 0x16120B55, 0x09200AD4, 0x1D17155B, 0x122004BA, 0x0720125A, 0x1A15192B, 0x0E20152A,   // 2001 ～ 2010
		0x03201694, 0x171416AA, 0x0A2015AA, 0x1F190AB5, 0x13200974, 0x082014B6, 0x1C160A57, 0x10200A56, 0x05201526, 0x19140E95,   // 2011 ～ 2020
		0x0C200D54, 0x012015AA, 0x161209B5, 0x0A20096C, 0x1D1612AE, 0x1120149C, 0x06201A4C, 0x1A151D26, 0x0D201AA6, 0x03200B54,   // 2021 ～ 2030
		0x17130D6A, 0x0B2012DA, 0x1F1B095D, 0x1320095A, 0x0820149A, 0x1C161A4B, 0x0F201A4A, 0x04201AA4, 0x18151B54, 0x0C2016B4,   // 2031 ～ 2040
		0x01200ADA, 0x1612095B, 0x0A200936, 0x1E171497, 0x11201496, 0x0620154A, 0x1A1516A5, 0x0E200DA4, 0x022015B4, 0x17130AB6,   // 2041 ～ 2050
		0x0B20126E, 0x0128092F, 0x1320092E, 0x08200C96, 0x1C160D4A, 0x0F201D4A, 0x04200D64, 0x1814156C, 0x0C20155C, 0x0220125C,   // 2051 ～ 2060
		0x1513192E, 0x0920192C, 0x1D171A95, 0x11201A94, 0x05201B4A, 0x1A150B55, 0x0E200AD4, 0x032014DA, 0x17140A5D, 0x0B200A5A,   // 2061 ～ 2070
		0x1F18152B, 0x1320152A, 0x07201694, 0x1B1616AA, 0x0F2015AA, 0x05200AB4, 0x181414BA, 0x0C2014B6, 0x02200A56, 0x16131527,   // 2071 ～ 2080
		0x09200D26, 0x1D170E53, 0x11200D54, 0x062015AA, 0x1A1509B5, 0x0E20096C, 0x032014AE, 0x18140A4E, 0x0A201A4C, 0x1E181D26,   // 2081 ～ 2090
		0x12201AA4, 0x07201B54, 0x1B160D6A, 0x0F200ADA, 0x0520095C, 0x1914149D, 0x0C20149A, 0x01201A2A, 0x15121B25, 0x09201AA4,   // 2091 ～ 2100
	};

//--------------------------------------------------------------------
//		日期函数的实现
//--------------------------------------------------------------------

//--------------------------------------------
// 获取星期几的名字(0="日",1="一"...)
//     名字串不包含星期字样
//--------------------------------------------
LUNAR_API const char* cjxGetWeekName(WORD wWeekDay)
{
	return cg_szWeekName[wWeekDay];
}

//--------------------------------------------
// 返回 公历 y年某m 月的天数
//      
//     参数:  y  - 年, 1900 ~ 2100
//            m  - 月, 1    ~ 12
//--------------------------------------------
LUNAR_API int cjxGetSolarMonthDays(int y, int m) 
{
   if( m == 2 )  // 二月, 特殊处理
   {
	   //公历闰年
	   return(( ((y%4 == 0) && (y%100 != 0)) || (y%400 == 0))? 29 : 28);
   }
   return cg_wMonthDays[m];	//平年
}

//-------------------------------------------
// 计算公历某年的天数
//-------------------------------------------
LUNAR_API int cjxGetSolarYearDays(int y)
{
	if( ((y%4 == 0) && (y%100 != 0)) || (y%400 == 0) )
	{
		return 366;	// 公历闰年
	}
	return 365;	//公历平年
}

//------------------------------------------------------
//  计算 公历 year-month-day 到 1900-1-0的天数
//   
//     参数: 公历的年月日 1900-1-1 ~ 2100-12-31 之间
//
//     返回:
//          天数, 一个正数值 1900-1-1为 1
//          
//-----------------------------------------------------
LUNAR_API int  cjxGetOffsetSolarDays(int year, int month, int day)
{
	int days = 0;
	
	// 对1900年特别处理
	if(year == 1900)
	{
		// 本年不是闰年
		days += cg_wMonthAdd[month];
		days += day;
	}
	else
	{
		days += (year - 1900) * 365;	// 加之前 年数  * 365
		days += (year - 1901) / 4  ;	// 加闰年数
		// 加当年的天数
		days += cg_wMonthAdd[month];	// 加上当月之前的天数
		days += day;					// 加上当月天数

		// 如果当年是闰年, 并且2月之后, 天数加1
		if( (( (year%4 == 0) && (year%100 != 0) ) || (year%400 == 0) ) && (month > 2) )
		{
			days++;
		}
	}
	return days;
}

//-------------------------------------------------------
//  计算公历 y 年 m 月 d 日 是星期几 (0=星期天)
//-------------------------------------------------------
LUNAR_API int cjxGetWeekday(int y, int m, int d)
{
	//
	// 计算到初始时间 1900年1月1日的天数：1900-1-1(星期一)---*/
	return (cjxGetOffsetSolarDays(y, m, d) % 7);
}

//-------------------------------------------------------
//   返回农历 y 年 闰几月(1 ~ 12), 没有返回0
//-------------------------------------------------------
LUNAR_API int cjxLunarLeapMonth(int y)
{
	return ( (cg_dwLunarInfo[y - 1900] >> 16) & 0xf );
}

//-------------------------------------------------------
//   返回农历 y年m月 是大月还是小月
//
//   参数:
//         y = 1900 ~ 2100(指的是农历)
//         m = 1 ~ 13(指的是农历): 
//   返回:
//         1 : 月大
//         0 : 月小
//-------------------------------------------------------
LUNAR_API int cjxLunarMonthBig(int y, int m)
{
	int moninfo = cg_dwLunarInfo[y - 1900] & 0x1fff;

	return (moninfo >> (13 -m)) & 0x1;
}

//-------------------------------------------------------
//  计算农历某年某月的天数
//      月分 1~ 13, 必须有13个月时13才有意义
//-------------------------------------------------------
LUNAR_API int cjxGetLunarMonthDays(int lunarY, int lunarM)
{
	return (cjxLunarMonthBig(lunarY, lunarM) ? 30 : 29);
}


//-------------------------------------------------------
//  返回农历 y年 春节的 公历日期
//-------------------------------------------------------
//  返回农历春节的公历月份
LUNAR_API int cjxSpringMonth(int y)
{
	return ( (cg_dwLunarInfo[y - 1900] >> 20) & 0xf );
}

//  返回农历春节的公历日
LUNAR_API int cjxSpringDay(int y)
{
	return ( (cg_dwLunarInfo[y - 1900] >> 24) & 0xff );
}


//-------------------------------------------------------
// 计算农历某年的天数
//-------------------------------------------------------
LUNAR_API int cjxGetLunaYearDays(int lunarY)
{
	int days = 0;
	int leapm = cjxLunarLeapMonth(lunarY);

	// 全部按小月计算
	if(leapm != 0)
	{
		days = 13 * 29;
	}
	else
	{
		days = 12 * 29;
	}
	// 增加大月的数据
	int moninfo = cg_dwLunarInfo[lunarY - 1900] & 0x1fff;
	for(int m=1; m<=13; m++)
	{
		if( (( moninfo >> (13 -m)) & 0x1) == 0x1 )
		{
			days ++;
		}
	}
	return days;
}

//---------------------------------------------------------------
//  计算农历 y年m月d日到农历 1900年正月初一(公历1900-1-31)的天数
//    当天到当天为 1
//    这里不区分闰月, 农历月份为 1 ~ 13
//---------------------------------------------------------------
LUNAR_API int  cjxGetOffsetLunarDays(int year, int month, int day)
{
	int days = 0;
	// 计算 year 之前的整年数
	for(int y = 1900; y < year; y++)
	{
		days += cjxGetLunaYearDays(y);
	}
	// 计算之前的月的
	for(int m = 1; m < month; m++)
	{
		days += cjxGetLunarMonthDays(year, m);
	}
	// 加当月天数
	days += day;
	return days;
}

///===========================================================
//    下面是公历到农历的转换函数
//============================================================

//--------------------------------------------------------
//    从公历到农历计算, pLunarData 制定公历日期
//        y-m-d 指定当天所在的农历正月初一的公历日期
//        当天的农历年数应该和y相等
//        此为内部帮助函数       
//      此函数填充农历年, 月(及是否是闰月), 日, 星期几
//---------------------------------------------------------
static bool cjxGetLunarDateHelp(LUNARDATE*  pLunarData, int y, int m, int d)
{
	//ASSERT(pLunarData);
	if(pLunarData == NULL)
	{
		return false;
	}

	// 计算当天是 1900-1-1 后的第几天(1900-1-1为1)
	int offdays1 = cjxGetOffsetSolarDays(pLunarData->wYear, pLunarData->wMonth, pLunarData->wDay);
	// 计算当年春节是 1900-1-1 后的第几天(1900-1-1为1)
	int offdays2 = cjxGetOffsetSolarDays(y, m, d);

	// 计算星期几, 并填充结构
	pLunarData->wWeekDay	= offdays1 % 7;

	// 填充农历年数
	pLunarData->wLunarYear	= y;

	//ASSERT( offdays1 >= offdays2);
	//--------------------------------------
	// 当天到农历当年春节的天数
	//   当天到当天的天数为1!
	int offdays = offdays1 - offdays2 + 1;

	// 然后计算农历月日
	int lm  = 1;	//农历月计数

	//   计算月
	while( offdays > cjxGetLunarMonthDays(y, lm) )
	{
		// 减去当月天数
		offdays -= cjxGetLunarMonthDays(y, lm);
		lm++; // 农历月计数+1
	}
	// 剩下的不足一个月的天数为日期
	// 填充农历日
	pLunarData->wLunarDay = offdays;

	// 填充月份
	// 计算当年是否有闰月
	int leapm = cjxLunarLeapMonth(y);
	if( leapm == 0 )
	{	// 当年没有闰月
		pLunarData->wLunarMonth  = lm;
		pLunarData->wIsLeapMonth = 0;	//不是闰月
	}
	else	// 当年有闰月
	{
		if( lm <= leapm )
		{
			// 在闰月之前
			pLunarData->wLunarMonth  = lm;
			pLunarData->wIsLeapMonth = 0;	//不是闰月
		}
		else if( lm == leapm + 1) //闰月
		{
			// 闰月, 月份-1, 标记是闰月
			pLunarData->wLunarMonth  = lm - 1;
			pLunarData->wIsLeapMonth = 1;	//是闰月
		}
		else
		{
			// 在闰月之后, 月份-1
			pLunarData->wLunarMonth  = lm - 1;
			pLunarData->wIsLeapMonth = 0;	//不是闰月
		}
	}
	return true;
}


//---------------------------------------------------------
//  填充农历汉字内容
//---------------------------------------------------------
#if _MSC_VER > 1300
#pragma warning(disable: 4996)	//strcpy 不安全警告
#endif 
static void lunarFillLunarString(LUNARDATE*  pLunarData)
{
	// 填充字符串部分
	//     农历1864年为甲子
	//-----
	//	先清零字符串
	memset(pLunarData->szLunarDay, 0, sizeof(pLunarData->szLunarDay));
	memset(pLunarData->szLunarMonth, 0, sizeof(pLunarData->szLunarMonth));
	memset(pLunarData->szYearGanZhi, 0, sizeof(pLunarData->szYearGanZhi));
	memset(pLunarData->szYearShengXiao, 0, sizeof(pLunarData->szYearShengXiao));

	//  天干序号
	int tg = ((pLunarData->wLunarYear - 1864) % 60) % 10;
	//  地支序号, 属相相同
	int dz = ((pLunarData->wLunarYear - 1864) % 60) % 12;

	// 农历年的干支, 生肖
	strcpy( pLunarData->szYearGanZhi, cg_szTianGan[tg] );
	strcat( pLunarData->szYearGanZhi, cg_szDiZhi[dz] );
	strcpy( pLunarData->szYearShengXiao,cg_szShuXiang[dz]);

	// 农历月名字
	if( pLunarData->wIsLeapMonth )
	{
		// 闰月
		int mon = pLunarData->wLunarMonth;
		strcpy( pLunarData->szLunarMonth, _T("闰"));
		strcat( pLunarData->szLunarMonth, cg_szMonName[mon] );
	}
	else
	{
		int mon = pLunarData->wLunarMonth;
		strcpy( pLunarData->szLunarMonth, cg_szMonName[mon] );
	}

	// 农历日名字
	strcpy( pLunarData->szLunarDay, cg_szDayName[pLunarData->wLunarDay]);
}


#if _MSC_VER > 1300
#pragma warning(default: 4996)	//strcpy 不安全警告
#endif 

//---------------------------------------------------------
// 获取农历的数据结构(完成公历到农历的转换)
//   
//     参数:
//		   LUNARDATE*  pLunarData:  输入输出
//               必须填充公历的年月日数据, 函数填充农历数据
//
//-----------------------------------------------------------
LUNAR_API bool cjxGetLunarDate(LUNARDATE*  pLunarData)
{
	// ASSERT(pLunarData);
	if(pLunarData == NULL)
	{
		return false;
	}
	int year  = pLunarData->wYear;
	int month = pLunarData->wMonth;
	int day   = pLunarData->wDay;

	// 参数检查, 断言
//	ASSERT( (year >= 1900) & (year <= 2100));
//	ASSERT( (month >=1 ) & (month <= 12));
//	ASSERT( (day >=1 ) & (day <= 31) ); 
	// 检查时间合法性
	if( (year>2100) || (year<1900) )
	{
		return false;
	}

	// 快速计算方法:
	//    从公历计算农历, 利用春节信息计算:

	// 获取当年(公历)的农历初一的公历日期
	int spring_y = year;	// 假设在当年春节之后
	int spring_m = cjxSpringMonth(spring_y);
	int spring_d = cjxSpringDay(spring_y);

	// 计算公历日期是否在当年春节之前	
	if( (spring_m > month) ||
		( (spring_m == month) && ( spring_d > day))
		)
	{	// 在春节之前, 
		//1900春节之前的特殊处理
		if( year == 1900)
		{
			// 此时只有可能是 1900-1-1 ~ 1900-1-30
			//   对应农历 1899-12-1 ~ 1899-12-30
			pLunarData->wLunarYear  = 1899;		//年
			pLunarData->wLunarMonth = 12;		//月
			pLunarData->wLunarDay   = day;		//日
			pLunarData->wIsLeapMonth= 0;		//非闰月
			pLunarData->wWeekDay    = day % 7;	//星期
		}
		else
		{	// 不是 1900
			spring_y = year -1;	// 春节在前一年
			spring_m = cjxSpringMonth(spring_y);
			spring_d = cjxSpringDay(spring_y);
			// 填充结构
			cjxGetLunarDateHelp(pLunarData, spring_y, spring_m, spring_d);
		}
	}
	else
	{
		// 计算当天在公历当年的春节之后
		//  农历年和公历年相等
		cjxGetLunarDateHelp(pLunarData, spring_y, spring_m, spring_d);
	}
	// 填充汉字部分
	lunarFillLunarString( pLunarData );

	return true;
}


//-----------------------------------------------------------
//  完成从 农历 到 公历的转换
//     参数:
//
//		LUNARDATE*  pLunarData:  输入输出
//          必须填充 农历的年月日(和是否是闰月)数据, 函数填充农历数据
//-----------------------------------------------------------
LUNAR_API bool cjxGetSolarDate(LUNARDATE*  pLunarData)
{
	//ASSERT(pLunarData);
	if(pLunarData == NULL)
	{
		return false;
	}

	int ly = pLunarData->wLunarYear;
	int lm = pLunarData->wLunarMonth;
	int ld = pLunarData->wLunarDay;

	// 月份修正
	int leapm = cjxLunarLeapMonth(ly);
	if(leapm != 0)	//当年有闰月
	{
		if(lm > leapm)
		{
			lm++;
		}
		else if( (lm == leapm) && (pLunarData->wIsLeapMonth) )
		{
			lm++;
		}
		else
		{
			;//在闰月之前, 不用修正
		}
	}
	// 计算到 公历1900-1-1的天数
	int days = cjxGetOffsetLunarDays(ly, lm, ld) + 30;

	// 星期数
	pLunarData->wWeekDay = days % 7;

	// 计算公历日期
	int year  = 1900;
	int month = 1;
	int day   = 1;

	int subdays = cjxGetSolarYearDays(year);
	// 定年
	while( days > subdays )
	{
		year ++;
		days -= subdays;
		subdays = cjxGetSolarYearDays(year);
	}
	// 定月
	subdays = cjxGetSolarMonthDays(year, month);
	while( days > subdays)
	{
		month ++;
		days -= subdays;
		subdays = cjxGetSolarMonthDays(year, month);
	}
	day = days;

	// 填充公历年月日
	pLunarData->wYear  = year;
	pLunarData->wMonth = month;
	pLunarData->wDay   = day;

	return true;
}


//-----------------------------------------------------------
//     获取 距离 1900年1月1日 days 天的日期
//         1900年1月1日为1
//-----------------------------------------------------------
LUNAR_API bool cjxGetDate(int days, LUNARDATE*  pLunarData)
{
	//ASSERT(pLunarData);
	if(pLunarData == NULL)
	{
		return false;
	}

	// 星期数
	pLunarData->wWeekDay = days % 7;

	// 计算公历日期
	int year  = 1900;
	int month = 1;
	int day   = 1;

	int subdays = cjxGetSolarYearDays(year);
	// 定年
	while( days > subdays )
	{
		year ++;
		days -= subdays;
		subdays = cjxGetSolarYearDays(year);
	}
	// 定月
	subdays = cjxGetSolarMonthDays(year, month);
	while( days > subdays)
	{
		month ++;
		days -= subdays;
		subdays = cjxGetSolarMonthDays(year, month);
	}
	day = days;

	// 填充公历年月日
	pLunarData->wYear  = year;
	pLunarData->wMonth = month;
	pLunarData->wDay   = day;

	// 填充农历
	return cjxGetLunarDate( pLunarData );
}
//----------------------------------------
//   计算下一个日期
//		参数中的公农历日期必须是正确的
//
//----------------------------------------
LUNAR_API bool cjxLunarNextDay(LUNARDATE*  pLunarData)
{
	// 农历字符串清0
	memset(pLunarData->szLunarDay, 0, sizeof(pLunarData->szLunarDay));
	memset(pLunarData->szLunarMonth, 0, sizeof(pLunarData->szLunarMonth));
	memset(pLunarData->szYearGanZhi, 0, sizeof(pLunarData->szYearGanZhi));
	memset(pLunarData->szYearShengXiao, 0, sizeof(pLunarData->szYearShengXiao));
	// 首先调整公历数据:
	pLunarData->wWeekDay = (pLunarData->wWeekDay + 1) % 7;//星期数
	pLunarData->wDay     = pLunarData->wDay + 1;	//天数 +1

	if( pLunarData->wDay > cjxGetSolarMonthDays(pLunarData->wYear, pLunarData->wMonth) )
	{
		// 超出了当月范围, 下个月1号
		pLunarData->wDay   = 1;
		pLunarData->wMonth = pLunarData->wMonth + 1; //月+1

		// 如果月超出范围, 下一年1月
		if( pLunarData->wMonth > 12 )
		{
			pLunarData->wMonth = 1;	//
			pLunarData->wYear  = pLunarData->wYear + 1;
		}
	}

	// 调整农历数据
	int ld = pLunarData->wLunarDay;
	int lm = pLunarData->wLunarMonth;
	int ly = pLunarData->wLunarYear;

	ld = ld + 1;	//天数 +1

	// 如果是闰月, 特殊处理
	if( pLunarData->wIsLeapMonth )
	{
		// 当月最大天数
		int md = cjxLunarMonthBig(ly, lm+1) ? 30 : 29;

		// 如果超出, 下一个月
		if( ld > md)
		{
			ld = 1;							//1号
			lm = lm + 1;					// 下一个月
			pLunarData->wIsLeapMonth = 0;	// 当月是闰月, 下一月一定不是闰月

			// 检查月是否超出范围
			if( lm > 12)
			{
				lm = 1;		//1月
				ly += 1;	//年 +1
			}
		}
		// 填充农历年月日
		pLunarData->wLunarDay	= ld;
		pLunarData->wLunarMonth = lm;
		pLunarData->wLunarYear	= ly;

	}
	else
	{// 当月不是闰月
		
		// 当年有闰月否 ?
		int pl = cjxLunarLeapMonth(ly);
		
		if( pl == 0)	// 当年也没有闰月
		{
			// 当月最大天数
			int md = cjxLunarMonthBig(ly, lm) ? 30 : 29;

			// 如果超出, 下一个月
			if( ld > md)
			{
				ld = 1;							//1号
				lm = lm + 1;					// 下一个月

				// 检查月是否超出范围
				if( lm > 12)
				{
					lm = 1;		//1月
					ly += 1;	//年 +1
				}
			}
			// 填充农历年月日
			pLunarData->wLunarDay	= ld;
			pLunarData->wLunarMonth = lm;
			pLunarData->wLunarYear	= ly;
		}
		else // 当年有闰月, 情况比较复杂, 直接重新计算农历好了
		{
			cjxGetLunarDate(pLunarData);
		}
	}

	// 填充汉字部分
	lunarFillLunarString( pLunarData );

	return true;
}

//----------------------------------------
// 星座(constellation)
//    获取 几月几日 是 什么 星座
//    日期必须是公历
//    永远不会是空字符串
//-----------------------------------------
LUNAR_API const char* cjxGetXingzuoName(int month, int day)
{
	int zodd[] = { 1222, 122, 222, 321, 421, 522,
					622, 722, 822, 922, 1022, 1122, 1222 };

	int i;
	int t = 100 * month + day;
	// 12.22 ~ 1.21
	if((t >= zodd[0])||(t < zodd[1]))
	{
		i = 0;
	}
	else
	{
		for(i=1; i<12; i++)
		{
			if((t >= zodd[i]) && (t<zodd[i+1]))
			{
				break;
			}
		}
	}
	return cg_szXingZuo[i];
}
//----------------------------------------------------
// 获取传统农历节日的名称
//
//       参数为农历 y月m日
//
//       如果不是节日, 返回 NULL
//-----------------------------------------------------
LUNAR_API const char* cjxGetLunarHolidayName(WORD wLunarMonth, WORD wLunarDay)
{
	for(size_t id=0; id < sizeof(cg_stLunarHoliday)/sizeof(HOLIDAY); id++)
	{
		if( (wLunarMonth == cg_stLunarHoliday[id].wMonth)
			&& (wLunarDay == cg_stLunarHoliday[id].wDay ) 
			)
		{
			return cg_stLunarHoliday[id].szName;
		}
	}
	return NULL;
}

//----------------------------------------------
// 获取公历节日的名称字符串
//       参数为公历日期(几月几日)
//       如果不是节日, 返回 NULL
//-----------------------------------------------
LUNAR_API const char* cjxGetSolarHolidayName(WORD wMonth, WORD wDay)
{
	for(size_t id = 1; id < sizeof(cg_stHoliday)/sizeof(HOLIDAY); id++)
	{
		if( (wMonth == cg_stHoliday[id].wMonth)
			&& (wDay == cg_stHoliday[id].wDay ) 
			)
		{
			return cg_stHoliday[id].szName;
		}
	}
	return NULL;
}

/********  下面为农历二十四节气函数   ******/
//-----------------------------------------------------
//    计算 y 年的第n个节气几号(从0小寒起算)
//       该节气的月份   (int)(n/2+1)
//         每个节气之间相差15度
//-----------------------------------------------------
LUNAR_API  int cjxGetTermYN(int y, int n)
{
	// 一个回归年 365.242 天
	// y年第n个节气距离(1900-1-0)的天数
	int offday = (int)(365.242 * (y - 1900) + 15.22 * n  - 1.9 * sin(QUOTIETY * n) + 6.2);

	LUNARDATE lunar;
	memset(&lunar, 0, sizeof(LUNARDATE));
	cjxGetDate(offday, &lunar);

	return lunar.wDay;
}

//-----------------------------------------------------
//    获取 某年第n个节气的名称(从0小寒起算)
//       n = 0 ~ 23
//-----------------------------------------------------
LUNAR_API  const char*  cjxGetTermNameN(int n)
{
	return cg_szLunarJieqi[n];
}

//-----------------------------------------------------
//   功能: 获取 Year 年 Month 月 Day 日 的 节气字符串
//        如果不是节气日, 返回 NULL
//
//   参数: Year:  年号, 如 2004
//         Month: 月份, 1 ~ 12
//         Day:   日期, 1 ~ 31
//----------------------------------------------------
LUNAR_API  const char* cjxGetTermName(int Year, int Month, int Day)
{
	int n = 0;
	if(Day < 15)
	{
		n = (Month - 1) * 2;
	}
	else
	{
		n = Month * 2 - 1;
	}
	if(cjxGetTermYN(Year, n) == Day)
	{// 是一个节气
		return cjxGetTermNameN(n);
	}
	return NULL;
}
