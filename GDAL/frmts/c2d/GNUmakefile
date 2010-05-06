
include ../../GDALmake.opt

OBJ	=	c2ddataset.o

CPPFLAGS	:=	$(GDAL_INCLUDE) 

default:	$(OBJ:.o=.$(OBJ_EXT))

clean:
	rm -f *.o

install-obj:	$(O_OBJ:.o=.$(OBJ_EXT))
