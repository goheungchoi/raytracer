CXX=g++
ROOT_DIR := $(shell git rev-parse --show-toplevel)
CXXFLAGS = -std=c++17 -g

LIB_NAME = libscene.a

BUILD_DIR = $(ROOT_DIR)/build/libs/$(LIB_NAME)
LIBFILE = $(ROOT_DIR)/build/lib/$(LIB_NAME)
INCLUDES = -I.. -Isrc -I. -Iinclude -I. -I$(ROOT_DIR)/libs/stream/include\
-I$(ROOT_DIR)/libs/stream/include/util -Im
SOURCES = $(shell find src -name '*.cpp')
OBJFILES = $(addprefix $(BUILD_DIR)/, $(SOURCES:.cpp=.o))

all: $(LIBFILE)

# Applicaiton Targets:
$(LIBFILE): $(OBJFILES)
	mkdir -p $(dir $@)
	ar rcs $@ $(OBJFILES)

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
	rm -rf $(LIBFILE)