APPLICATION := button-demo-rollover
BOARD := business-card
RIOTBASE ?= $(CURDIR)/../../RIOT

EXTERNAL_BOARD_DIRS := $(CURDIR)/../../boards
EXTERNAL_MODULE_DIRS := $(CURDIR)/../../modules

DEVELHELP ?= 1
QUIET ?= 1

USEMODULE += button_matrix
USEMODULE += led_matrix
USEMODULE += led_matrix_games
USEMODULE += stdio_null

include $(RIOTBASE)/Makefile.include
