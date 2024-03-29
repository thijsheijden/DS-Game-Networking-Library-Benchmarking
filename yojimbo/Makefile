# Alternative GNU Make workspace makefile autogenerated by Premake

ifndef config
  config=debug_x64
endif

ifndef verbose
  SILENT = @
endif

ifeq ($(config),debug_x64)
  test_config = debug_x64
  yojimbo_config = debug_x64
  client_config = debug_x64
  server_config = debug_x64
  secure_client_config = debug_x64
  secure_server_config = debug_x64
  client_server_config = debug_x64
  loopback_config = debug_x64
  soak_config = debug_x64

else ifeq ($(config),release_x64)
  test_config = release_x64
  yojimbo_config = release_x64
  client_config = release_x64
  server_config = release_x64
  secure_client_config = release_x64
  secure_server_config = release_x64
  client_server_config = release_x64
  loopback_config = release_x64
  soak_config = release_x64

else
  $(error "invalid configuration $(config)")
endif

PROJECTS := test yojimbo client server secure_client secure_server client_server loopback soak

.PHONY: all clean help $(PROJECTS) 

all: $(PROJECTS)

test: yojimbo
ifneq (,$(test_config))
	@echo "==== Building test ($(test_config)) ===="
	@${MAKE} --no-print-directory -C . -f test.make config=$(test_config)
endif

yojimbo:
ifneq (,$(yojimbo_config))
	@echo "==== Building yojimbo ($(yojimbo_config)) ===="
	@${MAKE} --no-print-directory -C . -f yojimbo.make config=$(yojimbo_config)
endif

client: yojimbo
ifneq (,$(client_config))
	@echo "==== Building client ($(client_config)) ===="
	@${MAKE} --no-print-directory -C . -f client.make config=$(client_config)
endif

server: yojimbo
ifneq (,$(server_config))
	@echo "==== Building server ($(server_config)) ===="
	@${MAKE} --no-print-directory -C . -f server.make config=$(server_config)
endif

secure_client: yojimbo
ifneq (,$(secure_client_config))
	@echo "==== Building secure_client ($(secure_client_config)) ===="
	@${MAKE} --no-print-directory -C . -f secure_client.make config=$(secure_client_config)
endif

secure_server: yojimbo
ifneq (,$(secure_server_config))
	@echo "==== Building secure_server ($(secure_server_config)) ===="
	@${MAKE} --no-print-directory -C . -f secure_server.make config=$(secure_server_config)
endif

client_server: yojimbo
ifneq (,$(client_server_config))
	@echo "==== Building client_server ($(client_server_config)) ===="
	@${MAKE} --no-print-directory -C . -f client_server.make config=$(client_server_config)
endif

loopback: yojimbo
ifneq (,$(loopback_config))
	@echo "==== Building loopback ($(loopback_config)) ===="
	@${MAKE} --no-print-directory -C . -f loopback.make config=$(loopback_config)
endif

soak: yojimbo
ifneq (,$(soak_config))
	@echo "==== Building soak ($(soak_config)) ===="
	@${MAKE} --no-print-directory -C . -f soak.make config=$(soak_config)
endif

clean:
	@${MAKE} --no-print-directory -C . -f test.make clean
	@${MAKE} --no-print-directory -C . -f yojimbo.make clean
	@${MAKE} --no-print-directory -C . -f client.make clean
	@${MAKE} --no-print-directory -C . -f server.make clean
	@${MAKE} --no-print-directory -C . -f secure_client.make clean
	@${MAKE} --no-print-directory -C . -f secure_server.make clean
	@${MAKE} --no-print-directory -C . -f client_server.make clean
	@${MAKE} --no-print-directory -C . -f loopback.make clean
	@${MAKE} --no-print-directory -C . -f soak.make clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "  debug_x64"
	@echo "  release_x64"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   test"
	@echo "   yojimbo"
	@echo "   client"
	@echo "   server"
	@echo "   secure_client"
	@echo "   secure_server"
	@echo "   client_server"
	@echo "   loopback"
	@echo "   soak"
	@echo ""
	@echo "For more information, see https://github.com/premake/premake-core/wiki"