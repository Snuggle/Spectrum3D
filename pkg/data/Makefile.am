## Process this file with automake to produce Makefile.in

#icon file
svgicondir = $(datadir)/icons
dist_svgicon_DATA = spectrum3d.svg

icondir = $(datadir)/icons
dist_icon_DATA = spectrum3d-refresh.png \
		spectrum3d-play.png \
		spectrum3d-pause.png \
		spectrum3d-stop.png \
		spectrum3d-record.png \
		spectrum3d-microphone.png \
		spectrum3d-microphone_grey.png \
		spectrum3d-file.png \
		spectrum3d-file_grey.png \
		spectrum3d-qjackctl.png \
		spectrum3d-equalizer.png \
		spectrum3d-gain.png\
		spectrum3d.png

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(svgicondir)

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	@-if test -z "$(datadir)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated.  After (un)install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi

#desktop file
desktop_in_files= spectrum3d.desktop.in
desktop_files=$(desktop_in_files:.desktop.in=.desktop)

Applicationsdir = $(datadir)/applications
Applications_DATA = $(desktop_files)

DISTCLEANFILES = $(desktop_files)



