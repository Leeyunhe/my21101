-include ./makefile.cfg 
$(TARGET):$(VAR)
	$(COM) $^ -o $@ -lsqlite3
%.o:%.c
	$(COM) $^ $(CAN) $@
clean:
	rm -rf $(VAR) $(TARGET)
