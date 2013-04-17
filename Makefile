CXX = g++
LD = g++
RM = /bin/rm -f
CXXFLAGS = `pkg-config --cflags gtkmm-2.4 libxml++-2.6` -g
LDFLAGS = `pkg-config --libs gtkmm-2.4 libxml++-2.6`
OBJS = adddialog.o settingsdialog.o brightnessdialog.o progresswindow.o config.o x10.o mainwindow.o main.o
X10G = x10g

all: $(X10G)

$(X10G): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(X10G)

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $<

clean:  
	$(RM) $(X10G) $(OBJS)

