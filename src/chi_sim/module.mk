SOURCES= Calendar.cpp \
    Parameters.cpp \
	CSVReader.cpp \
	PlaceId.cpp \
	gdb_debug.cpp \
	FileOutput.cpp \
	file_utils.cpp

local_src := $(addprefix $(chi_sim)/, $(SOURCES))
src += $(local_src)

#move_persons.cpp \