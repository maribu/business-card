APPLICATION := button-demo
BOARD := business-card
RIOTBASE ?= $(CURDIR)/../../RIOT

EXTERNAL_BOARD_DIRS := $(CURDIR)/../../boards
EXTERNAL_MODULE_DIRS := $(CURDIR)/../../modules

DEVELHELP ?= 1
QUIET ?= 1

USEMODULE += led_matrix
USEMODULE += button_matrix
USEMODULE += stdio_null

include $(RIOTBASE)/Makefile.include
