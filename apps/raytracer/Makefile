CXX=g++
ROOT_DIR := $(shell git rev-parse --show-toplevel)
CXXFLAGS = -std=c++17 -g

APP_NAME = raytracer

BUILD_DIR = $(ROOT_DIR)/build/apps/$(APP_NAME)
EXEFILE = $(ROOT_DIR)/build/bin/$(APP_NAME)
INCLUDES = -I.. -Isrc -I. -Iinclude -I. -I$(ROOT_DIR)/libs/scene/include -I$(ROOT_DIR)/libs/stream/include
LIBDIRS = -L$(ROOT_DIR)/build/lib
LIBS = -lscene -lstream -lz
SOURCES = $(shell find src -name '*.cpp')
OBJFILES = $(addprefix $(BUILD_DIR)/, $(SOURCES:.cpp=.o))

all: $(EXEFILE)

# Applicaiton Targets:
$(EXEFILE): $(OBJFILES)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(LIBDIRS) $(OBJFILES) $(LIBS) -o $@

# Object File Targets:
$(BUILD_DIR)/%.o: %.cpp 
	mkdir -p $(dir $@)
	$(call make-depend-cxx,$<,$@,$(subst .o,.d,$@))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Generate dependencies
make-depend-cxx=$(CXX) -MM -MF $3 -MP -MT $2 $(CXXFLAGS) $(INCLUDES) $1
-include $(OBJFILES:.o=.d)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(EXEFILE)
