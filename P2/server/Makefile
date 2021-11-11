# OPTIMIZA
OPT=-g # Guardar toda la información para poder debugear. No optimiza
# OPT=-O3 # Optimiza al máximo, descarta toda la información de debug.
# PARAMETROS
# -Wunused = (Warn Unused) Da aviso de las variables que no se estan usando
# -Wall    = (Warn All) Da aviso de todos los posibles errores de compilación
# $(OPT)   = Nivel de optimización
CFLAGS=-Wunused -Wall $(OPT)

# LIBRERÍAS                                                                 
# Matemáticas (C Math library)
MTH=-lm
LIB=$(GTK) $(MTH)
FILES := $(wildcard src/*.c)

all: $(FILES)
	@gcc -o server $(FILES) $(CFLAGS) $(LIB) -I./src/

clean: 
	@rm -fv server && rm -rfv obj && echo "done cleaning"

