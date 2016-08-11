build: main.hex

main.elf: main.c lcd.c
	avr-gcc -mmcu=atmega16 -Os -Wall  -o main.elf main.c lcd.c

main.hex: main.elf
	avr-objcopy  -j .text -j .data -O ihex  main.elf main.hex
	avr-size main.elf

clean:
	rm -rf main.elf main.hex