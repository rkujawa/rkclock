file bin/rkclock.elf
target remote | openocd -f st_nucleo_l011k4.cfg -c "gdb_port pipe; log_output /dev/null"

define restart
monitor reset halt
end

#define reload
#monitor reset halt
#monitor program bin/rkclock.elf verify
#monitor reset halt
#end

