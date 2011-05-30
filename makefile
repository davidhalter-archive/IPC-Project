CMP=		g++
#CMPFLAGS=	-g
EXENAME0=	HSDisplay.e
EXENAME1=	HSControl.e
EXENAME2=	HSMain.e
EXENAME3=	SensorDevices.e
EXENAME4=	HSDataTx.e

display:	display.o
		$(CMP) $(LIB) $(CMPFLAGS) display.o -o $(EXENAME0)

control:	control.o 
		$(CMP) $(LIB) $(CMPFLAGS) control.o -o $(EXENAME1)

main:	main.o 
		$(CMP) $(LIB) $(CMPFLAGS) main.o -o $(EXENAME2)

sensor:	sensor_devices.o
		$(CMP) $(LIB) $(CMPFLAGS) sensor_devices.o -o $(EXENAME3)

data:	data.o
		$(CMP) $(LIB) $(CMPFLAGS) data.o -o $(EXENAME4)

.cc.o:		
		$(CMP) -c $(CMPFLAGS) $<

all:
		@make --no-print-directory display
		@make --no-print-directory control
		@make --no-print-directory main 
		@make --no-print-directory sensor
		@make --no-print-directory data

clean:		
		@rm -f *.e *.o
