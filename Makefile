BUILD_DIR ?= build

CXXFLAGS += -std=c++14

TESTS := path_parser

OBJS := $(TESTS:%=$(BUILD_DIR)/test/%.test.cpp.o)
DEPS := $(OBJS:.o=.d)

.PHONY: test
test: $(TESTS:%=$(BUILD_DIR)/test-%)

$(BUILD_DIR)/%.cpp.o: %.cpp Makefile
	@mkdir -p $(dir $@)
	$(CXX) -MMD -MP $(CXXFLAGS) -Iinclude -c $*.cpp -o $@

$(BUILD_DIR)/test-%: $(BUILD_DIR)/test/%.test.cpp.o Makefile
	$(CXX) $(LDFLAGS) $(BUILD_DIR)/test/$*.test.cpp.o -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
