
const Lang = imports.lang;
const Gio = imports.gi.Gio;
const Gtk = imports.gi.Gtk;
const Gdk = imports.gi.Gdk;
const ExtensionUtils = imports.misc.extensionUtils;
const Me = ExtensionUtils.getCurrentExtension();
const Convenience = Me.imports.convenience;

let settings, openWinsCounter;

function init() {
}

function buildPrefsWidget() {
	let ui = new Gtk.Builder(), win = new Gtk.Table();
	ui.add_from_file(Me.dir.get_path() + "/prefs.ui");

	ui.get_object('content-table').reparent(win);

	if (!settings) {
		settings = Convenience.getSettings();
		openWinsCounter = 0;
	} else {
		openWinsCounter = openWinsCounter + 1;
	}

	settings.bind('show-date', ui.get_object('show-date'), 'active', Gio.SettingsBindFlags.DEFAULT);
	settings.bind('show-time', ui.get_object('show-time'), 'active', Gio.SettingsBindFlags.DEFAULT);
	settings.bind('show-calendar', ui.get_object('show-calendar'), 'active', Gio.SettingsBindFlags.DEFAULT);
	settings.bind('jieri', ui.get_object('jieri'), 'active', Gio.SettingsBindFlags.DEFAULT);

	let colorFromSetting = function() {
		let color = new Gdk.RGBA;
		color.parse(settings.get_string('jieri-color'));
		ui.get_object('holiday-color').set_rgba(color);
	};

	settings.connect('changed::jieri-color', colorFromSetting);
	colorFromSetting();
	ui.get_object('holiday-color').connect('color-set', function(object) {
		let color = new Gdk.RGBA;
		let color2 = object.get_rgba(color);
		if (color2 instanceof Gdk.RGBA) { color = color2; }
		settings.set_string('jieri-color', color.to_string());
	});

	let zti_inhibitor = false;
	for (let i = 1; i <= 4; ++i) {
		let cl = i;
		ui.get_object('zti-' + i).connect('toggled', function(object) {
				if (zti_inhibitor) { return; }
				zti_inhibitor = true;
				for (let j = 1; j <= 4; ++j) {
					ui.get_object('zti-' + j).set_active(j <= cl);
				}
				settings.set_enum('zti-dx', cl);
				zti_inhibitor = false;
			});
	}

	let fontsizeFromSetting = function() {
		ui.get_object('zti-' + settings.get_enum('zti-dx')).toggled();
	};

	settings.connect('changed::zti-dx', fontsizeFromSetting);
	fontsizeFromSetting();

	win.connect('destroy', function () {
		if (!openWinsCounter) {
			settings.run_dispose();
			settings = null;
		}
		else {
			openWinsCounter = openWinsCounter - 1;
		}
	});

	win.show_all();
	return win;
}
