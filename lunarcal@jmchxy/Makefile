UUID=lunarcal@jmchxy
INSTALL_PATH=~/.local/share/gnome-shell/extensions/$(UUID)
ZIP_PATH=$(UUID).zip
SRC_PATH=src
SCHEMAS_PATH=src/schemas

zip-file:
	glib-compile-schemas $(SCHEMAS_PATH) \
		--targetdir=$(SCHEMAS_PATH) \
		--strict && \
	cd $(SRC_PATH) && \
	zip -r -u ../$(ZIP_PATH) .

install: zip-file
	mkdir -p $(INSTALL_PATH) && \
	unzip -o $(ZIP_PATH) -d $(INSTALL_PATH)

uninstall:
	rm $(INSTALL_PATH) -rf

clean:
	rm -f $(UUID).zip $(SCHEMAS_PATH)/gschemas.compiled

.PHONY: zip-file
