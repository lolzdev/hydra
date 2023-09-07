include config.mk

OBJ=${SRC:.c=.o}
OBJ:=${OBJ:.S=.o}
DIRS= ${shell find ./ -maxdepth 1 -type d -print | sed "s/.\///g;:a;N;$$!ba;s/\n/ /g;s/^[t ]*//g;s/[t ]*$$//g;"}

all: ${OBJ} ${DIRS}

.PHONY: ${DIRS}
ifneq (${DIRS},)
${DIRS}:
ifeq (,$(filter clean,$(MAKECMDGOALS)))
	make -C $@
else
	make -C $@ clean
endif
endif

%.o: %.c
	${CC} ${CFLAGS} -o $@ -c $<
%.o: %.S
	${CC} ${ASFLAGS} -o $@ -c $<

.PHONY: clean

clean: ${DIRS}
	@rm -rfv ${OBJ}
