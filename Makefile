MEMPIN_DIR := mempin
MEMPIN_COMPILE_TARGET := $(MEMPIN_DIR)/mempin.o
MEMPIN_LINK_TARGET := $(MEMPIN_DIR)/mempin.so
MEMPIN_TARGET := $(MEMPIN_LINK_TARGET)
MEMPIN_SOURCE := $(MEMPIN_DIR)/mempin.cpp
MEMPIN_HEADER := $(MEMPIN_DIR)/mempin.h

MSI_DIR := MSI
MSI_TARGET := $(MSI_DIR)/main
MSI_SOURCE := \
  $(MSI_DIR)/AtomicBus.cpp \
  $(MSI_DIR)/Cache.cpp \
  $(MSI_DIR)/Processor.cpp \
  $(MSI_DIR)/Ram.cpp \
  $(MSI_DIR)/Simulator.cpp
MSI_HEADER := \
  $(MSI_DIR)/AtomicBus.h \
  $(MSI_DIR)/Cache.h \
  $(MSI_DIR)/Processor.h \
  $(MSI_DIR)/RAM.h \
  $(MSI_DIR)/Simulator.h


MEMPIN_CMD_GOAL := $(findstring mempin, $(MAKECMDGOALS))$(findstring $(MEMPIN_TARGET), $(MAKECMDGOALS))

ifneq ( ,$(MEMPIN_CMD_GOAL))
  # Modified from intel pin project source/tools/MyPinTool/makefile
  # used to import pin makefiles
  PIN_ROOT := third_party/pin
  CONFIG_ROOT := $(PIN_ROOT)/source/tools/Config
  TEST_TOOL_ROOTS := mempin
  include $(CONFIG_ROOT)/makefile.config
  include $(TOOLS_ROOT)/Config/makefile.default.rules
endif

mempin: $(MEMPIN_LINK_TARGET)

msi: $(MSI_TARGET)

pin: third_party/pin

clear:
	rm $(MEMPIN_COMPILE_TARGET) $(MEMPIN_LINK_TARGET)
#	rm $(MSI_TARGET)

.SUFFIXES:

$(MEMPIN_COMPILE_TARGET): $(MEMPIN_SOURCE) $(MEMPIN_HEADER)
	$(CXX) $(TOOL_CXXFLAGS) $(COMP_OBJ)$(MEMPIN_COMPILE_TARGET) $(MEMPIN_SOURCE)

$(MEMPIN_LINK_TARGET): $(MEMPIN_COMPILE_TARGET)
	$(LINKER) $(TOOL_LDFLAGS) $(LINK_EXE)$(MEMPIN_LINK_TARGET) $(MEMPIN_COMPILE_TARGET) $(TOOL_LPATHS) $(TOOL_LIBS)

$(MSI_TARGET): $(MSI_SOURCE) $(MSI_HEADER)
	$(CXX) $(MSI_CXX_FLAGS) $(MSI_SOURCE) -o $(MSI_TARGET)

third_party/pin:
	wget https://software.intel.com/sites/landingpage/pintool/downloads/pin-3.27-98718-gbeaa5d51e-gcc-linux.tar.gz -O third_party/pin-3.27-98718-gbeaa5d51e-gcc-linux.tar.gz
	tar -x -f third_party/pin-3.27-98718-gbeaa5d51e-gcc-linux.tar.gz -C third_party
	mv third_party/pin-3.27-98718-gbeaa5d51e-gcc-linux third_party/pin
	rm third_party/pin-3.27-98718-gbeaa5d51e-gcc-linux.tar.gz