
undefine LIBRARY
dependencies := AntonAvrLib Tank-Shared Kernel-Tank-MASTER

BASEDIR := $(get-basedir)
include Defaults.mk
include DefaultTankSymbols.mk
include Main.mk
