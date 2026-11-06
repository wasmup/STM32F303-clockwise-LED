# STM32F303 Clockwise LEDs

A bare-metal LED chasing example for the STM32F3DISCOVERY board. It lights the eight onboard LEDs in sequence, advancing every 100 ms. The firmware uses direct register access and SysTick with the default 8 MHz clock; it does not require HAL or an RTOS.

## Requirements

- STM32F3DISCOVERY board (STM32F303VCT6) with its onboard ST-LINK debugger
- A data-capable USB Mini-B cable
- GNU Make, the `arm-none-eabi-gcc` toolchain, and OpenOCD
- An ARM-capable GDB for debugging (optional)


```sh
sudo dnf install make arm-none-eabi-gcc openocd gdb usbutils
# or
# sudo apt install make arm-none-eabi-gcc openocd gdb usbutils
```

Package names may differ on other systems. Verify that `arm-none-eabi-gcc` and `openocd` are available on your `PATH`.

## Build and run

Clone the repository and enter its directory:

```sh
git clone https://github.com/wasmup/STM32F303-clockwise-LED.git
cd STM32F303-clockwise-LED
```

Ensure JP3 and both CN4 jumpers are installed, then connect the board's **USB ST-LINK** connector to your computer. Stop any existing OpenOCD session before flashing.

```sh
make
```

This removes the previous build, compiles `blink.elf`, and uses OpenOCD to program, verify, and reset the board. The onboard LEDs should then cycle in sequence. Flashing replaces the firmware already on the board.

Other targets:

| Command | Effect |
| --- | --- |
| `make build` | Build `blink.elf` without flashing. |
| `make run` or `make flash` | Build if needed, then program and reset the board. |
| `make clean` | Remove `blink.elf`. |

## Debugging

Build the firmware, then start OpenOCD from the repository directory:

```sh
make build
openocd -f stm32f3.cfg
```

In another terminal, start an ARM-capable GDB, for example:

```sh
arm-none-eabi-gdb blink.elf
```

Connect and set a breakpoint:

```gdb
target extended-remote localhost:3333
monitor reset halt
break main
continue
```

Stop OpenOCD with Ctrl+C when finished. If the debugger is not detected, check the USB cable and jumper connections. On Linux, `lsusb` can help confirm that the ST-LINK is detected; USB access errors may require the appropriate udev rules.

## Project files

- `main.c` — LED control and SysTick delay.
- `startup.c` — minimal vector table and reset handler.
- `linker.ld` — memory layout for the STM32F303VCT6.
- `stm32f3.cfg` — OpenOCD configuration for the onboard ST-LINK.
- `Makefile` — build and flashing targets.

The linker script uses the 40 KB main SRAM region; the separate 8 KB core-coupled memory is not used. The minimal startup code does not initialize global data or provide a complete interrupt vector table, so it needs extending for more general firmware.

## License

The project is licensed under the [MIT License](LICENSE). The OpenOCD configuration file carries its own GPL-2.0-or-later SPDX notice.
