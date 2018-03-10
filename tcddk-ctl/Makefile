#
#
#

CXX_FLAGS=-Wall -fPIC -g -O0
LDX_FLAGS=-lusb-1.0 -pthread

all: tcddk-ctl

tcddk-ctl: main.cpp tcddk-ctl.cpp tcddk-api.cpp
	g++ $(CXX_FLAGS) $(LDX_FLAGS) -o tcddk-ctl $^

clean:
	rm tcddk-ctl
