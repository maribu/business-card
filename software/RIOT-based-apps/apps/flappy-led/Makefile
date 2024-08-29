APPLICATION := flappy-led
BOARD := business-card
RIOTBASE ?= $(CURDIR)/../../RIOT

EXTERNAL_BOARD_DIRS := $(CURDIR)/../../boards
EXTERNAL_MODULE_DIRS := $(CURDIR)/../../modules

DEVELHELP ?= 1
QUIET ?= 1

USEMODULE += bitmap_fonts
USEMODULE += led_matrix
USEMODULE += led_matrix_games
USEMODULE += stdio_null

include $(RIOTBASE)/Makefile.include
