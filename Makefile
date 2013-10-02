###############################################################################
# 
# Makefile
# 
# 	This makefile is used to create an archive or execuatble from cpp source
# 	files. It will also build and run unit tests associated with each cpp source
# 	file. Each cpp class is expected to have the following files: 
# 		1) tcMyClass.h
# 		2) tcMyClass.cpp
# 		3) tcMyClass_test.cpp
#
# 	All of these files are expected to be in the src directory.  This makefile
# 	will create a directory tree structure and store the build and test
# 	artifacts in appropriate directories:
# 		bin - executable files
#		lib - archive files
#		obj - object files
#		tst - test result files
#
###############################################################################

###############################################################################
#
# This section should be customized per component
#

# component (executable or achive) name
TARGET += gmock_samples

# objects to include in executable (production objects - no test objects)
PROD_OBJ += $(OBJ_DIR)/sample1.o $(OBJ_DIR)/sample2.o $(OBJ_DIR)/sample3.o $(OBJ_DIR)/sample4.o

# all rule: should be "[library] [executable] [test] [coverage] [dox]" 
all : executable #test library coverage dox

###############################################################################
#
# This section should the same for all components
#

# gtest directory
GTEST_DIR = /cygdrive/c/Users/Dad/Documents/gtest-1.6.0
GMOCK_DIR = /cygdrive/c/Users/Dad/Documents/gmock-1.6.0

# local directories
SRC_DIR      = src
SRC_TEST_DIR = src_test
OBJ_DIR      = obj
LIB_DIR      = lib
BIN_DIR      = bin
TST_DIR      = tst
DOX_DIR      = dox
LOG_DIR      = log

# Flags passed to the preprocessor.
CPPFLAGS += -I$(GTEST_DIR)/include -I$(GMOCK_DIR)/include
CPPFLAGS += -MF $(@:.o=.d) -MMD

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra #-fprofile-arcs -ftest-coverage

# Flags passed to the linker
LINKFLAGS += -lpthread

# List of test object for each PROD_OBJ
TEST_OBJ += $(PROD_OBJ:.o=_test.o)

# library name
LIBNAME = $(TARGET).a

# executable name
EXENAME = $(TARGET).exe 

# test exe name
TESTNAME = $(TARGET)_test.exe

# test out name
OUTPUTNAME = $(TESTNAME:.exe=.xml)

# this rule defines the location of the library/archive file
library : $(LIB_DIR)/$(LIBNAME)

# this rule defines the location of the executable file
executable : $(BIN_DIR)/$(EXENAME)

# this rule defines the location and name of the test output file for each
# production object (i.e. obj/tcMyClass.o => tst/tcMyClass_test.xml)
test : $(PROD_OBJ:$(OBJ_DIR)/%.o=$(TST_DIR)/%_test.xml)

coverage : $(TST_DIR)/coverage.xml

dox : $(DOX_DIR)/html/index.html

# this rule cleans the build area
clean :
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) $(TST_DIR) $(DOX_DIR) $(LOG_DIR)

# these include the dependency files for each production and test object, if it 
# exists. (i.e. obj/tcMyClass.o => obj/tcMyClass.d)
# these files provide all the include file dependencies for each object 
-include $(PROD_OBJ:.o=.d)
-include $(TEST_OBJ:.o=.d)


# This rule makes an object file for each test cpp file. The compile line
# produces the object file and the include dependency file.
$(OBJ_DIR)/%_test.o: $(SRC_TEST_DIR)/%_test.cpp
	@mkdir -p $(OBJ_DIR) $(LOG_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(@:$(OBJ_DIR)/%_test.o=$(SRC_TEST_DIR)/%_test.cpp) -o $@ | tee -a $(LOG_DIR)/build.log

# this rule makes an object file for each cpp file. the compile line produces
# the object file and the include dependency file.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR) $(LOG_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(@:$(OBJ_DIR)/%.o=$(SRC_DIR)/%.cpp) -o $@ | tee -a $(LOG_DIR)/build.log

#$(SRC_DIR)/%_mock.h: $(SRC_DIR)/%.h
#	INDENT=3 $(GMOCK_DIR)/scripts/generator/gmock_gen.py $(SRC_DIR)/%.h > $(SRC_DIR)/%_mock.h

# this rule makes an archive containing only the production objects
$(LIB_DIR)/$(LIBNAME) : $(PROD_OBJ)
	@mkdir -p $(LIB_DIR) $(LOG_DIR)
	$(AR) $(ARFLAGS) $@ $^ | tee -a $(LOG_DIR)/build.log

# this rule makes the production executable
#$(BIN_DIR)/$(EXENAME) : $(PROD_OBJ)
$(BIN_DIR)/$(EXENAME) : $(PROD_OBJ) $(GMOCK_DIR)/make/gmock_main.a
	@mkdir -p $(BIN_DIR) $(LOG_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LINKFLAGS) $^ -o $@ | tee -a $(LOG_DIR)/build.log

# this rule makes the test executable
#$(TST_DIR)/$(TESTNAME) : $(PROD_OBJ) $(TEST_OBJ) $(GTEST_DIR)/make/gtest_main.a
$(TST_DIR)/$(TESTNAME) : $(PROD_OBJ) $(TEST_OBJ) $(GMOCK_DIR)/make/gmock_main.a
	@mkdir -p $(TST_DIR) $(LOG_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LINKFLAGS) -lgcov $^ -o $@ | tee -a $(LOG_DIR)/build.log
	
# this rule executes the test for the objects that have been updated
$(TST_DIR)/%_test.xml : $(OBJ_DIR)/%.o $(OBJ_DIR)/%_test.o | $(TST_DIR)/$(TESTNAME)
	@mkdir -p $(TST_DIR) $(LOG_DIR)
	$(TST_DIR)/$(TESTNAME) --gtest_filter=$(@:$(TST_DIR)/%_test.xml=%)*.* --gtest_output="xml:$@" #&> $(@:$(TST_DIR)/%.xml=$(LOG_DIR)/%.log)

# this rule converts the test coverage files to xml
$(TST_DIR)/coverage.xml : $(PROD_OBJ:.o=.gcda) $(PROD_OBJ:.o=.gcno) $(TEST_OBJ:.o=.gcda) $(TEST_OBJ:.o=.gcno)
	@cd $(OBJ_DIR) $(LOG_DIR)
	gcovr -x &> $@

# this rule generates the doxygen output
$(DOX_DIR)/html/index.html : $(PROD_OBJ)
	@mkdir -p $(DOX_DIR) $(LOG_DIR)
	doxygen Doxyfile &> $(LOG_DIR)/doxygen.log
