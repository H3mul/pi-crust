# build:
# 	mkdir -p out
# 	g++ -o out/kvm.out hid.c xreadkeys.cpp scancodes.cpp -DXK_MISCELLANY=1 -DXK_LATIN1=1 -L/usr/include/ -lX11

# .PHONY: clean

# https://dev.to/dmendoza99/how-to-write-a-good-makefile-for-c-32e3
# SDL include notes: https://forums.libsdl.org/viewtopic.php?t=5997

CXX				:= g++
CXX_FLAGS		:= -Wall -Wextra -std=c++17 -ggdb
LINKER_FLAGS	:= -DXK_MISCELLANY=1 -DXK_LATIN1=1
BIN				:= bin
SRC				:= src
INCLUDE			:= include
LIB				:= LIB
LIBRARIES		:= -lX11 -lXi
EXECUTABLE		:= kvm


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	@echo "Executing..."
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp #$(SRC)/*.c
	@echo "Building..."
	$(CXX) $^  $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) -o $@ $(LIBRARIES) $(LINKER_FLAGS)

install: scripts/*
	@echo "Installing binaries..."
	-cp scripts/kvm_hid_setup /usr/bin/kvm_hid_setup
	-chmod +x /usr/bin/kvm_hid_setup
	-cp bin/kvm /usr/bin/kvm
	-chmod +x /usr/bin/kvm
	@echo "Installing systemd services..."
	-cp scripts/kvm.service /etc/systemd/system/kvm.service
	-cp scripts/x.service /etc/systemd/system/x.service
	-cp scripts/barrierc.service /etc/systemd/system/barrierc.service
	-chmod 644 /etc/systemd/system/kvm.service
	-chmod 644 /etc/systemd/system/x.service
	-chmod 644 /etc/systemd/system/barrierc.service
	-systemctl daemon-reload

start: scripts/*
	@echo "Starting systemd services..."
	-systemctl restart kvm.service
	-systemctl restart x.service
	-systemctl restart barrierc.service

enable: scripts/*
	@echo "Enabling systemd services..."
	-systemctl enable kvm.service
	-systemctl enable x.service
	-systemctl enable barrierc.service

clean:
	@echo "Cleaning..."
	-rm $(BIN)/*
