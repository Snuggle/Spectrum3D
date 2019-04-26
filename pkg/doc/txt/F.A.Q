SPECTRUM3D : FAQ

	1. Spectrum3d command window or main display window fails to start.

You should start Spectrum3d in a terminal. You'll have different messages and probably some error message that you should report to the mail address nadaeck@hotmail.com.

	2. Main display window starts but remains black when playing.

Check if this happens only when you have chosen the 'realtime' mode; if so, you should check if you have a proper '/etc/security/limits.d/audio.conf', and if you are a member of the 'audio'group. Cfr Realtime.txt document.

	3. Despite the installation of the uTouch-Geis library, I get this error message at Spectrum3d's start: 

"error while loading shared libraries: libutouch-geis.so.1: cannot open shared object file: No such file or directory"

This seems related to an 'environment variable' problem. You have probably installed libutouch-geis in the '/usr/local' directory which is the default directory, unless another directory is specified), and spectrum3d doesn't look there to find it. This suggests that '/usr/local/lib' is not in the ld.so configuration. This directory should be in /etc/ld.so.conf or a .conf file in /etc/ld.so.conf.d.
For example, you can create a custom file that you will place in /etc/ld.so.conf.d : 'sudo gedit /etc/ld.so.conf.d'; then write '/usr/local/lib' in this text file and save; then type in a terminal 'sudo ldconfig'. Another way is to put '/usr/local/lib' in the '/etc/ld.so.conf' file.

	4. When 'Touch' function is activated, everything seems 'captured' by the 'Touch' function : mouse pointer cannot be controlled by the touchpad anymore. 

See Multitouch.txt document : How is it implemented in Spectrum3d?

5. In command line, I get this error message when multitouch is enabled :

WARNING **: Failed to fetch view type at /org/ayatana/bamf/window73400334: Method "ViewType" with signature "" on interface "org.ayatana.bamf.view" doesn't exist

Well, I don't have a clue to explain what it is, but this does not seem to be a real issue since everything works despite that message. It could be related somehow with Unity.


