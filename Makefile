######################################
# target
######################################
TARGET = ndsblc

# Add extension for clean in Windows
UNAME = $(shell uname -s)

ifneq (,$(findstring MINGW,$(UNAME))$(findstring CYGWIN,$(UNAME)))
	TARGET_EXT = .exe
endif


######################################
# building variables
######################################
# optimization
OPT = -O3


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build


######################################
# source
######################################
# C sources
C_SOURCES =  $(wildcard src/*.c)


#######################################
# binaries
#######################################
CC = gcc


#######################################
# CFLAGS
#######################################
# C includes
C_INCLUDES =  \
-Iinc

# compile gcc flags
CFLAGS = $(C_INCLUDES) $(OPT) -Wall


#######################################
# LDFLAGS
#######################################
LDFLAGS =

# default action: build all
all: $(TARGET)


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	@$(CC) -c $(CFLAGS) $< -o $@

$(TARGET): $(OBJECTS) Makefile
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR):
	@mkdir $@


#######################################
# clean up
#######################################
clean:
	@rm -Rf $(BUILD_DIR) $(TARGET)$(TARGET_EXT)


# *** EOF ***