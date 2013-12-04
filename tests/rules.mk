
# C file compilation

transformed.o: transformed.c $(ACC_PUBLIC_DEPS) $(ACC_PRIVATE_DEPS)
	$(CXX) $(CFLAGS) $(INCLUDES) -c transformed.c -o transformed.o

host-data.o: host-data.c $(ACC_INTERNAL_DEPS)
	$(CC) $(CFLAGS) $(INCLUDES) $(HOST_ENV_MACRO) -c host-data.c -o host-data.o

reference.o: reference.c
	$(CC) $(CFLAGS) $(INCLUDES) -c reference.c -o reference.o

# LibOpenACC dependency

$(OPENACC_LIB_FILE):
	$(MAKE) -C $(OPENACC_LIB_DIR) lib$(OPENACC_LIB_NAME).so

$(OPENACC_LIB_DIR)/utils/timer.o:
	$(MAKE) -C $(OPENACC_LIB_DIR) utils/timer.o

