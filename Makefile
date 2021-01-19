pciape: src/CFsProvider.cpp src/CGObject.cpp src/CGraphics.cpp src/CPciTree.cpp src/pciape.cpp
	g++ -o $@ $? `pkg-config --cflags gtk+-3.0 --libs gtk+-3.0`
