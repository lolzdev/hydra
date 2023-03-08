all: build

build: buildkernel

buildservices:
	@make -C services

buildkernel:
	@make -C core

.PHONY: clean
clean:
	@make -C core clean
