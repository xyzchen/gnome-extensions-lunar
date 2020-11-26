const Lang = imports.lang;
const GLib = imports.gi.GLib;
const GObject = imports.gi.GObject;
const GnomeDesktop = imports.gi.GnomeDesktop;
const Gdk = imports.gi.Gdk;
const St = imports.gi.St;
//UI库
const Main = imports.ui.main;
const Calendar = imports.ui.calendar;
const MessageList = imports.ui.messageList;
//扩展库
const ExtensionUtils = imports.misc.extensionUtils;
const Me = ExtensionUtils.getCurrentExtension();
const Convenience = Me.imports.convenience;
//地支名称
const _g_diZhi = ["子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥"];

//农历组件
const LunarDate = imports.gi.GILunar.Date;

//为农历组件添加功能

//设置日期
LunarDate.prototype.setDate = function(date) {
	//设置日期并计算农历
	this.set_solar_date(date.getFullYear(), date.getMonth()+1, date.getDate());
};

//获取当前时间字符串
LunarDate.prototype.getTime = function(){
	let d = new Date();
	let minutes = d.getMinutes();
	if(minutes <10){
		minutes = "0" + minutes;
	}
	return d.getHours() + ":" + minutes;
}

//获取时名称
LunarDate.prototype.getShi = function(){
	let d = new Date();
	return _g_diZhi[~~(d.getHours()/2)];
}

//获取日期字符串
LunarDate.prototype.getDateSting = function(){
	if(this.jieri != null && this.jieri !=""){
		let jieri_arr = this.jieri.split("|");
		if(jieri_arr[0] != ""){
			return jieri_arr[0];
		} else {
			return jieri_arr[1];
		}
	} else if(this.jieqi != null && this.jieqi != ""){
		return this.jieqi;
	} else if(this.lunar_day == 1){
		return this.month_name + "月";
	} else {
		return this.day_name;
	}
}

//农历字符串
LunarDate.prototype.lunarString = function () {
	return this.ganzhi + "年" + "(" + this.shengxiao + ")" + this.month_name + "月" + this.day_name;
};

//公历字符串
LunarDate.prototype.solarString = function () {
	return this.year + "年" + this.month + "月" + this.day + "日";
};

//转换成字符串（公历和农历）
LunarDate.prototype.toString = function() {
	return this.solarString() + "(农历：" + this.lunarString() + ")" + " 星期" + this.week_name;
};

//全局变量
let replacementFunc = [], settingsChanged = [], localSettings = {};
let settings;
let ld = new LunarDate();

//====================================================================
//  以下为扩展代码
//====================================================================
function init() {
	settings = Convenience.getSettings();
}

function _make_new_with_args (my_class, args) {
	return new (Function.prototype.bind.apply(my_class, [null].concat(Array.prototype.slice.call(args))))();
}

//始终显示字符串
function _getLunarClockDisplay() {
	let ret_string = "\u2001";
	if(settings.get_boolean('show-date')){
		ret_string += ld.toString();
	}
	if(settings.get_boolean('show-time')){
		ret_string += " " + ld.getShi() + "时  ";
	}
	return ret_string;
};


var LunarCalendarMessage = class LunarCalendarMessage extends MessageList.Message {

	canClear() { return false; }

	canClose() { return false; }

	_sync() {
		super._sync();
		this._closeButton.visible = this.canClear();
	}
};


var LunarCalendarSection = class LunarCalendarSection extends MessageList.MessageListSection {

	constructor() {
		super('Lunar Calendar');

		this._title = new St.Button({ style_class: 'events-section-title',
										label: '',
										x_align: St.Align.START,
										can_focus: true 
									});
		this.actor.insert_child_below(this._title, null);
	}

	get allowed() {
		return Main.sessionMode.showCalendarEvents;
	}

	_reloadEvents() {
		this._reloading = true;

		this._list.destroy_all_children();

		//节日
		let jr = settings.get_boolean('jieri') ? ld.jieri : "";
		if(jr != null && jr != "")
		{
			let _jr_str = ld.jieri.replace("|", "\n");
			this.addMessage(new LunarCalendarMessage("节日", _jr_str, false));
		}
		let jq = settings.get_boolean('jieri') ? ld.jieqi : "";
		if(jq != null && jq != ""){
			this.addMessage(new LunarCalendarMessage("二十四节气", jq, false));
		}
		this._reloading = false;
		this._sync();
	}

	setDate(date) {
		super.setDate(date);
		ld.setDate(date)
		let cny = ld.ganzhi + "(" + ld.shengxiao + ")";
		this._title.label = ld.lunarString();
		this._reloadEvents();
	}

	_shouldShow() { return true; }

	_sync() {
		if (this._reloading){
			return;
		}
		super._sync();
	}
};

//启用扩展
function enable() {
	settings = Convenience.getSettings();
	settings.connect('changed', function() {
		for (x in settingsChanged) {
			settingsChanged[x]();
		}
	});

	let dm = Main.panel.statusArea.dateMenu;

	let cal = dm._calendar;
	let ml  = dm._messageList;
	let cny_now;
	//保存原来的函数
	replacementFunc.calendarUpdater = cal._update;
	replacementFunc.originalMonthHeader = cal._headerFormat;
	//gnome 3.12
	replacementFunc.calendarRebuilder = cal._rebuildCalendar;

	//Header
	let fixupHeader = _("%A %B %e, %Y").match(/%Y[^%]+%/);
	if(fixupHeader){
		cal._headerFormat = cal._headerFormat.replace(/%Y.%/, fixupHeader);
	}

	//颜色处理函数
	let _toHex = function (number) {
		let t = (number * 65535).toString(16);
		while (t.length < 4){
			t = '0' + t;
		}
		return t;
	};

	//更改了颜色
	settingsChanged.updateLocal = function () {
		let color = new Gdk.RGBA;
		color.parse('white');
		color.parse(settings.get_string('jieri-color'));
		localSettings.jieri_color = "#" + _toHex(color.red) + _toHex(color.green) + _toHex(color.blue);
	};
	settingsChanged.updateLocal();

	let updateDate = function () {
		ld.setDate(new Date());
		cny_now = ld.ganzhi;
		// gnome 3.16
		ml._lunarCalendarSection.cny_now = cny_now;
		let date_label = dm._date._dateLabel;
		date_label.text = date_label.text + (date_label.text.match(/[.,]/) ? ", " : "\u2001") + cny_now;
	};

	dm._clock.run_dispose();

	// gnome 3.14.1
	replacementFunc.openMenuId = dm.menu.connect('open-state-changed', function(menu, isOpen) {
		if (isOpen)
			updateDate();
	});

	//设置时钟文字
	dm._clock = new GnomeDesktop.WallClock();
	let _clockUpdater = function () {
		ld.setDate(new Date());
		dm._clockDisplay.text = dm._clock.clock + _getLunarClockDisplay();
	};
	dm._clock.connect('notify::clock', Lang.bind(dm, _clockUpdater));
	_clockUpdater();
	settingsChanged.refreshClock = _clockUpdater;

	//农历日历
	let lunarButton = function (orig_button, iter_date, oargs) {
		let fat_button = false;
		if(+oargs[0].label == +iter_date.getDate().toString()) {
			iter_date._lunar_iter_found = true;
			ld.setDate(iter_date);
			let yd = settings.get_boolean('show-calendar') ? ld.day_name : "";
			let jr = settings.get_boolean('jieri') ? ld.jieri : "";
			let dx = settings.get_string('zti-dx');
			fat_button = yd != "";
			let l = oargs[0].label;
			if (jr != null && jr != ""){
				l = "<span weight='bold' color='" + localSettings.jieri_color + "'>" + l + "</span>";
			}
			if(yd != ""){
				l += "\n<small>" + ld.getDateSting() + "</small>";
			}
			if(dx != "none"){
				l = "<span size='" + dx + "'>" + l + "</span>";
			}
			oargs[0].label = l;
		}
		let new_button = _make_new_with_args(orig_button, oargs);
		return new_button;
	};

	//选择年
	let updateYear = function(that) {
		ld.setDate(that._selectedDate);
		let cny = ld.ganzhi + "(" + ld.shengxiao + ")";
		if(cny != cny_now){
			that._monthLabel.text = that._monthLabel.text + " / " + cny;
		}
	};

	//gnome 3.12
	cal._rebuildCalendar = function() {
		let orig_button = St.Button;
		let orig_date = Date;
		let iter_date = new orig_date();

		Date = function () {
			let new_date = _make_new_with_args(orig_date, arguments);
			if (!iter_date._lunar_iter_found &&
			  arguments.length > 0 && arguments[0] instanceof orig_date) {
				iter_date = new_date;
			}
			return new_date;
		};
		St.Button = function () {
			return lunarButton(orig_button, iter_date, arguments);
		};
		replacementFunc.calendarRebuilder.apply(this, arguments);
		St.Button = orig_button;
		Date = orig_date;
		let cal_style_class = cal.actor.style_class.split(' ').filter(function(e){ return e.length && e != 'lunar-calendar'; });
		if(settings.get_boolean('show-calendar')){
			cal_style_class.push('lunar-calendar');
		}
		cal.actor.style_class = cal_style_class.join(' ');
	};

	cal._update = function () {
		replacementFunc.calendarUpdater.apply(this, arguments);
		updateYear(this);
	};

	settingsChanged.rebuildCal = function () {
		cal._rebuildCalendar();
	}

	settingsChanged.rebuildCal();

	// gnome 3.16
	ml._lunarCalendarSection = new LunarCalendarSection();
	ml._addSection(ml._lunarCalendarSection);
	ml._lunarCalendarSection._sync();
	ml._sync();
}

//禁用扩展
function disable() {
	let dm = Main.panel.statusArea.dateMenu;
	//恢复原来的样式
	let restore_style = dm._calendar.actor.style_class.split(' ')
		.filter(function(e){ return e.length && e != 'lunar-calendar'; })
		.join(' ');
	dm._calendar.actor.style_class = restore_style;
	//恢复原来的函数
	dm._calendar._update = replacementFunc.calendarUpdater;
	dm._calendar._headerFormat = replacementFunc.originalMonthHeader;
	dm._calendar._rebuildCalendar = replacementFunc.calendarRebuilder;

	// gnome 3.16
	dm._messageList._removeSection(dm._messageList._lunarCalendarSection);
	delete dm._messageList._lunarCalendarSection;

	dm._clock.run_dispose();
	dm._clock = new GnomeDesktop.WallClock();

	// gnome 3.14.1
	dm.menu.disconnect(replacementFunc.openMenuId);
	dm._clock.bind_property('clock', dm._clockDisplay, 'text', GObject.BindingFlags.SYNC_CREATE);
	settingsChanged.rebuildCal();

	settingsChanged = [];
	settings.run_dispose();
	settings = null;

	delete replacementFunc.calendarUpdater;
	delete replacementFunc.originalMonthHeader;
	delete replacementFunc.calendarRebuilder;
	delete replacementFunc.openMenuId;
}
