#include <time.h>
#include "glunar.h"
#include "jlunar.h"

struct _GILunarDate
{
	GObject parent_instance;
	guint  year;	/*公历年月日*/
	guint  month;
	guint  day;
	guint  weekday;		/*星期几*/
	guint  lunar_year;	/*农历年月日*/
	guint  lunar_month;
	guint  lunar_day;
	guint  is_leapmonth;	/*农历是否为闰月*/
	/* 农历字符串信息 */
	gchar* ganzhi;		/*农历干支*/
	gchar* shengxiao;	/*农历生肖*/
	gchar* month_name;	/*农历月名称*/
	gchar* day_name;	/*农历日名称*/
	gchar* jieqi;		/*农历24节气*/
	gchar* jieri;		/*节日*/
};

G_DEFINE_TYPE (GILunarDate, gi_lunar_date, G_TYPE_OBJECT)

enum {
	PROP_0,
	PROP_YEAR,
	PROP_MONTH,
	PROP_DAY,
	PROP_WEEKDAY,
	PROP_LUNAR_YEAR,
	PROP_LUNAR_MONTH,
	PROP_LUNAR_DAY,
	PROP_IS_LEAPMONTH,
	PROP_GANZHI,
	PROP_SHENGXIAO,
	PROP_MONTH_NAME,
	PROP_DAY_NAME,
	PROP_WEEK_NAME,	/* 星期名称 */
	PROP_JIEQI,
	PROP_JIERI,
	LAST_PROP
};

//内部函数：是否字符串资源
static void gi_lunar_date_free_pointer(GILunarDate *self);

//内部函数：用 p_lunardate 的值是指对象的数值
static void gi_lunar_date_set_values(GILunarDate *self, const LUNARDATE* p_lunardate);

//属性参数信息
static GParamSpec *gParamSpecs [LAST_PROP];

/**
 * gi_lunar_date_new:
 *
 * Allocates a new #GILunarDate.
 *
 * Returns: (transfer full): a #GILunarDate.
 */
GILunarDate * gi_lunar_date_new(void)
{
	return g_object_new(GI_TYPE_LUNAR_DATE, NULL);
}

/**
 * 相当与析构函数
 */
static void gi_lunar_date_finalize (GObject *object)
{
	GILunarDate * self = (GILunarDate *)object;
	/* 释放字符串对象 */
	gi_lunar_date_free_pointer(self);
	/* 调用父类的函数 */
	G_OBJECT_CLASS (gi_lunar_date_parent_class)->finalize (object);
}

/**
 * 获取属性
 */
static void gi_lunar_date_get_property (GObject  *object,
										guint  prop_id,
										GValue  *value,
										GParamSpec  *pspec)
{
	GILunarDate *self = GI_LUNAR_DATE (object);

	switch (prop_id)
		{
		case PROP_YEAR:
			g_value_set_uint(value, self->year);
			break;
		case PROP_MONTH:
			g_value_set_uint(value, self->month);
			break;
		case PROP_DAY:
			g_value_set_uint(value, self->day);
			break;
		case PROP_WEEKDAY:
			g_value_set_uint(value, self->weekday);
			break;
		case PROP_LUNAR_YEAR:
			g_value_set_uint(value, self->lunar_year);
			break;
		case PROP_LUNAR_MONTH:
			g_value_set_uint(value, self->lunar_month);
			break;
		case PROP_LUNAR_DAY:
			g_value_set_uint(value, self->lunar_day);
			break;
		case PROP_IS_LEAPMONTH:
			g_value_set_uint(value, self->is_leapmonth);
			break;
		case PROP_GANZHI:
			g_value_set_string (value, self->ganzhi);
			break;
		case PROP_SHENGXIAO:
			g_value_set_string (value, self->shengxiao);
			break;
		case PROP_MONTH_NAME:
			g_value_set_string (value, self->month_name);
			break;
		case PROP_DAY_NAME:
			g_value_set_string (value, self->day_name);
			break;
		case PROP_WEEK_NAME:	//星期名称字符串
			g_value_set_string (value, cjxGetWeekName(self->weekday));
			break;
		case PROP_JIEQI:
			g_value_set_string (value, self->jieqi);
			break;
		case PROP_JIERI:
			g_value_set_string (value, self->jieri);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		}
}

/**
 * 设置属性
 */
static void gi_lunar_date_set_property (GObject	*object,
										guint	prop_id,
										const GValue *value,
										GParamSpec *pspec)
{
	GILunarDate *self = GI_LUNAR_DATE (object);

	switch (prop_id)
		{
		case PROP_YEAR:
			self->year = g_value_get_uint(value);
			break;
		case PROP_MONTH:
			self->month = g_value_get_uint(value);
			break;
		case PROP_DAY:
			self->day = g_value_get_uint(value);
			break;
		case PROP_LUNAR_YEAR:
			self->lunar_year = g_value_get_uint(value);
			break;
		case PROP_LUNAR_MONTH:
			self->lunar_month = g_value_get_uint(value);
			break;
		case PROP_LUNAR_DAY:
			self->lunar_day = g_value_get_uint(value);
			break;
		case PROP_IS_LEAPMONTH:
			self->is_leapmonth = g_value_get_uint(value);
			break;
		default:
			//其他属性为只读属性
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		}
}

/**
 * 类初始化函数
 */
static void gi_lunar_date_class_init (GILunarDateClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize	   = gi_lunar_date_finalize;
	object_class->get_property = gi_lunar_date_get_property;
	object_class->set_property = gi_lunar_date_set_property;

	//year
	gParamSpecs[PROP_YEAR] =
		g_param_spec_uint(  "year",
							"Year",
							"full year (1900-2100).",
							1900,	/* minimum value */
							2100,	/* maximum value */
							1900,	/* default value */
							G_PARAM_READWRITE);
	//month
	gParamSpecs[PROP_MONTH] =
		g_param_spec_uint(  "month",
							"Month",
							"month(1-12).",
							1,	/* minimum value */
							12,	/* maximum value */
							1,	/* default value */
							G_PARAM_READWRITE);
	//day
	gParamSpecs[PROP_DAY] =
		g_param_spec_uint(  "day",
							"Day",
							"day(1-31).",
							1,	/* minimum value */
							31,	/* maximum value */
							1,	/* default value */
							G_PARAM_READWRITE);
	//weekday
	gParamSpecs[PROP_WEEKDAY] =
		g_param_spec_uint(  "weekday",
							"Weekday",
							"weekday(0-6).",
							0,	/* minimum value */
							6,	/* maximum value */
							0,	/* default value */
							G_PARAM_READWRITE);
	
	//lunar_year
	gParamSpecs[PROP_LUNAR_YEAR] =
		g_param_spec_uint(  "lunar_year",
							"LunarYear",
							"full lunar year (1899-2100).",
							1899,	/* minimum value */
							2100,	/* maximum value */
							1900,	/* default value */
							G_PARAM_READWRITE);
	//lunar_month
	gParamSpecs[PROP_LUNAR_MONTH] =
		g_param_spec_uint(  "lunar_month",
							"LunarMonth",
							"lunar month(1-12).",
							1,	/* minimum value */
							12,	/* maximum value */
							1,	/* default value */
							G_PARAM_READWRITE);
	//lunar_day
	gParamSpecs[PROP_LUNAR_DAY] =
		g_param_spec_uint(  "lunar_day",
							"LunarDay",
							"day(1-30).",
							1,	/* minimum value */
							30,	/* maximum value */
							1,	/* default value */
							G_PARAM_READWRITE);
	//is_leapmonth
	gParamSpecs[PROP_IS_LEAPMONTH] =
		g_param_spec_uint(  "is_leapmonth",
							"Weekday",
							"weekday(0-1).",
							0,	/* minimum value */
							1,	/* maximum value */
							0,	/* default value */
							G_PARAM_READWRITE);
	/*   下面的是字符串属性   */
	//ganzhi
	gParamSpecs[PROP_GANZHI] =
		g_param_spec_string( "ganzhi",
							 "GanZhi",
							 "农历年的干支.",
							 NULL,
							 G_PARAM_READWRITE);
	
	//shengxiao
	gParamSpecs[PROP_SHENGXIAO] =
		g_param_spec_string( "shengxiao",
							 "ShengXiao",
							 "农历年的生肖.",
							 NULL,
							 G_PARAM_READWRITE);
	
	//month_name
	gParamSpecs[PROP_MONTH_NAME] =
		g_param_spec_string( "month_name",
							 "MonthName",
							 "农历月名称.",
							 NULL,
							 G_PARAM_READWRITE);

	//day_name
	gParamSpecs[PROP_DAY_NAME] =
		g_param_spec_string( "day_name",
							 "DayName",
							 "农历日名称.",
							 NULL,
							 G_PARAM_READWRITE);

	//week_name
	gParamSpecs[PROP_WEEK_NAME] =
		g_param_spec_string( "week_name",
							 "WeekName",
							 "星期名称.",
							 NULL,
							 G_PARAM_READWRITE);

	//jieqi
	gParamSpecs[PROP_JIEQI] =
		g_param_spec_string( "jieqi",
							 "JieQi",
							 "农历二十四名称.",
							 NULL,
							 G_PARAM_READWRITE);

	//jieri
	gParamSpecs[PROP_JIERI] =
		g_param_spec_string( "jieri",
							 "JieRi",
							 "节日名称.",
							 NULL,
							 G_PARAM_READWRITE);

	//设置所有的属性
	g_object_class_install_properties (object_class, LAST_PROP, gParamSpecs);
}

/**
 * 对象初始化
 */
static void gi_lunar_date_init (GILunarDate *self)
{
	// 获取当前时间
	time_t rawtime = time(NULL);
	struct tm * ptm = gmtime ( &rawtime );
	// 创建农历信息
	LUNARDATE lunardate;
	memset(&lunardate, 0, sizeof(lunardate));
	lunardate.wYear  = ptm->tm_year + 1900;
	lunardate.wMonth = ptm->tm_mon  + 1;
	lunardate.wDay   = ptm->tm_mday;
	cjxGetLunarDate(&lunardate);
	// 设置对象的值
	gi_lunar_date_set_values(self, &lunardate);
}

/******  其它类方法  ******/
/* 使用公历数据计算农历  */
void gi_lunar_date_calc_lunar_date(GILunarDate *self)
{
	// 创建农历信息
	LUNARDATE lunardate;
	memset(&lunardate, 0, sizeof(lunardate));
	lunardate.wYear  = self->year;
	lunardate.wMonth = self->month;
	lunardate.wDay   = self->day;
	cjxGetLunarDate(&lunardate);
	// 释放字符串对象
	gi_lunar_date_free_pointer(self);
	// 设置对象的值
	gi_lunar_date_set_values(self, &lunardate);
}

/* 使用农历数据计算公历  */
void  gi_lunar_date_calc_solar_date(GILunarDate *self)
{
	// 创建农历信息
	LUNARDATE lunardate;
	memset(&lunardate, 0, sizeof(lunardate));
	lunardate.wLunarYear  = self->lunar_year;
	lunardate.wLunarMonth = self->lunar_month;
	lunardate.wLunarDay   = self->lunar_day;
	lunardate.wIsLeapMonth = self->is_leapmonth;
	cjxGetSolarDate(&lunardate);
	// 释放字符串对象
	gi_lunar_date_free_pointer(self);
	// 设置对象的值
	gi_lunar_date_set_values(self, &lunardate);
}

/*  设置巩俐日期，自动计算农历数据 */
void  gi_lunar_date_set_solar_date(GILunarDate *self, guint year, guint month, guint day)
{
	// 创建农历信息
	LUNARDATE lunardate;
	memset(&lunardate, 0, sizeof(lunardate));
	lunardate.wYear  = year;
	lunardate.wMonth = month;
	lunardate.wDay   = day;
	cjxGetLunarDate(&lunardate);
	// 释放字符串对象
	gi_lunar_date_free_pointer(self);
	// 设置对象的值
	gi_lunar_date_set_values(self, &lunardate);
}

/* 设置农历日期，并自动计算公历 */
void  gi_lunar_date_set_lunar_date(GILunarDate *self, guint year, guint month, guint day, guint leap)
{
	// 创建农历信息
	LUNARDATE lunardate;
	memset(&lunardate, 0, sizeof(lunardate));
	lunardate.wLunarYear  = year;
	lunardate.wLunarMonth = month;
	lunardate.wLunarDay   = day;
	lunardate.wIsLeapMonth = leap;
	cjxGetSolarDate(&lunardate);
	// 释放字符串对象
	gi_lunar_date_free_pointer(self);
	// 设置对象的值
	gi_lunar_date_set_values(self, &lunardate);
}

/**********************************************
 * 下面的是内部函数
 */
//内部函数：用 p_lunardate 的值是指对象的数值
static void gi_lunar_date_set_values(GILunarDate *self, const LUNARDATE* p_lunardate)
{
	// 指数值给对象
	self->year = p_lunardate->wYear;
	self->month = p_lunardate->wMonth;
	self->day	= p_lunardate->wDay;
	self->weekday = p_lunardate->wWeekDay;
	self->lunar_year = p_lunardate->wLunarYear;
	self->lunar_month = p_lunardate->wLunarMonth;
	self->lunar_day	= p_lunardate->wLunarDay;
	self->is_leapmonth = p_lunardate->wIsLeapMonth;
	// 设置字符串
	self->ganzhi = g_strdup(p_lunardate->szYearGanZhi);
	self->shengxiao = g_strdup(p_lunardate->szYearShengXiao);
	self->month_name = g_strdup(p_lunardate->szLunarMonth);
	self->day_name = g_strdup(p_lunardate->szLunarDay);
	//——————————————————————————————————————————————————————
	//是指公农历节日和二十四节气
	//——————————————————————————————————————————————————————
	//获取农历二十四节气信息
	const char* jieqistr = cjxGetTermName(self->year, self->month, self->day);
	if(jieqistr)
	{
		self->jieqi = g_strdup(jieqistr);
	}
	//获取节日信息
	char jieri_buffer[200];
	memset(jieri_buffer, 0, sizeof(jieri_buffer));
	//先获取农历节日, 设置农历节日为清明节
	if((jieqistr != NULL) && (strcmp(jieqistr, "清明")==0))
	{
		strcpy(jieri_buffer, jieqistr);
	}
	else
	{
		const char* lunar_jieri = cjxGetLunarHolidayName(self->lunar_month, self->lunar_day);
		if(lunar_jieri)
		{
			strcpy(jieri_buffer, lunar_jieri);
		}
	}
	//获取公历节日
	const char* solar_jieri = cjxGetSolarHolidayName(self->month, self->day);
	if(solar_jieri)
	{
		strcat(jieri_buffer, "|");
		strcat(jieri_buffer, solar_jieri);
	}
	//设置节日字符串
	if(strlen(jieri_buffer) > 0)
	{
		self->jieri = g_strdup(jieri_buffer);
	}
}


//内部函数：是否字符串资源
static void gi_lunar_date_free_pointer(GILunarDate *self)
{
	/* 释放字符串对象 */
	g_clear_pointer (&self->ganzhi, g_free);
	g_clear_pointer (&self->shengxiao, g_free);
	g_clear_pointer (&self->month_name, g_free);
	g_clear_pointer (&self->day_name, g_free);
	g_clear_pointer (&self->jieqi, g_free);
	g_clear_pointer (&self->jieri, g_free);
}
