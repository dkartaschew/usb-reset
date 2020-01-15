AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = usbreset
usbreset_SOURCES = \
main.c 

usbreset_CFLAGS = ${LIBUSB_CFLAGS} 
usbreset_LDADD = ${LIBUSB_LIBS} 
usbreset_LDFLAGS = ${LIBUSB_LDFLAGS}

