file bin/rkclock.elf
target remote | openocd -f st_nucleo_l011k4.cfg -c "gdb_port pipe; log_output openocd.log"

