PNAME		= main-funkey.elf
CC			= /opt/funkey-sdk/usr/bin/arm-linux-gcc
CFLAGS		+= -O3 -fdata-sections -ffunction-sections -flto -DFUNKEY
LDFLAGS		+= -ldl -lasound -lpng -ljpeg -lmikmod -logg -lvorbisidec -lmpg123 -lfreetype -lbz2 -Wl,--as-needed -Wl,--gc-sections -flto -s
SPARROW_LIBS = /home/julius/Dokumente/sparrow3d/build/funkey/sparrow3d/libsparrow3d.a /home/julius/Dokumente/sparrow3d/build/funkey/sparrow3d/libsparrowSound.a