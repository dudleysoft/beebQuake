@echo off
beebasm -i beebCode.asm
xxd -i beebCode.bin >beebCode.c
xxd -i extraCode.bin >extraCode.c
xxd -i piCode.bin >piCode.c

