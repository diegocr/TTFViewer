
all:
	gcc -m68020 -O2 -noixemul -fbaserel -msmall-code -s ttfviewer.c -o TTFViewer
	gcc -m68060 -m68881 -O2 -DVERSION=\"0.1.060\" -noixemul -fbaserel -msmall-code -s ttfviewer.c -o TTFViewer.060
