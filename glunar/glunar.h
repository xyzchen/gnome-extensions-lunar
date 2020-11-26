#ifndef GI_LUNAR_H
#define GI_LUNAR_H

#include <glib-object.h>

G_BEGIN_DECLS

#define GI_TYPE_LUNAR_DATE  (gi_lunar_date_get_type())

G_DECLARE_FINAL_TYPE(GILunarDate, gi_lunar_date, GI, LUNAR_DATE, GObject)

/* 构造对象 */
GILunarDate* gi_lunar_date_new(void);

/* 使用公历数据计算农历  */
void  gi_lunar_date_calc_lunar_date(GILunarDate *self);

/* 使用农历数据计算公历  */
void  gi_lunar_date_calc_solar_date(GILunarDate *self);

/* 设置公历日期，自动计算农历日期 */
void  gi_lunar_date_set_solar_date(GILunarDate *self, guint year, guint month, guint day);

/* 设置农历日期，自动计算公历日期 */
void  gi_lunar_date_set_lunar_date(GILunarDate *self, guint year, guint month, guint day, guint leap);

G_END_DECLS

#endif /* GI_LUNAR_H */
