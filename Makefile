TARGET=out/openapi-client
SRC:=$(shell find src -type f -name "*.carp")
HEAD:=$(shell find lib -type f -name "*.h")

all: $(TARGET)

$(TARGET): main.carp $(SRC) $(HEAD)
	carp -b $< --optimize

clean:
	rm -rf out

.PHONY: all clean
