#!/usr/bin/env bash

# Get this script's location as an absolute path
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Add robotframework to Path
export PATH="$PATH:$SCRIPT_DIR/../lib/robotframework/bin/"

# Add git branch to bash prompt
parse_git_branch() {
    git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/ (\1)/'
}
export PS1="${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[33m\]\$(parse_git_branch)\[\033[00m\]\$ "

# Ensure Pico-SDK can be found
export PICO_SDK_PATH="$SCRIPT_DIR/../lib/pico-sdk"

# Ensure Pico-SDK can find the picotool
export picotool_DIR="$SCRIPT_DIR/../lib/picotool/"

# Ensure FreeRTOS can be found
export FREERTOS_PATH="$SCRIPT_DIR/../lib/freertos/"

export OPENOCD_PATH="$SCRIPT_DIR/../lib/openocd/"

# Function for simplifying building/flashing labs
lab() {
    case "$1" in
        config)
            mkdir -p build
            if [ "$2" = "debug" ]; then
                cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
            else
                cmake -B build -S .
            fi
            ;;
        clean)
            rm -rf build
            ;;
        build)
            cmake --build build
            ;;
        flash)
            local debug_mode=0
            local file=""

            # Check if debug mode was requested
            if [ "$2" = "debug" ]; then
                debug_mode=1
                file="$3"
            else
                file="$2"
            fi

            # If no file was provided, look for a .elf file in build/src/
            if [ -z "$file" ]; then
                local elf_files=(build/src/*.elf)

                if [ ! -f "${elf_files[0]}" ]; then
                    echo "Error: No .elf file found in build/src/" >&2
                    return 1
                fi

                if [ "${#elf_files[@]}" -gt 1 ]; then
                    echo "Notice: Multiple .elf files found. Using: ${elf_files[0]}" >&2
                fi

                file="${elf_files[0]}"
            fi

            if [ ! -f "$file" ]; then
                echo "Error: File '$file' does not exist." >&2
                return 1
            fi

            if [ "$debug_mode" -eq 1 ]; then
                if ! command -v tmux >/dev/null 2>&1; then
                    echo "Error: tmux is required for debug mode but is not installed." >&2
                    return 1
                fi

                local session="rp2040_debug_$$"

                # Create a detached session running OpenOCD on the left
                tmux new-session -d -s "$session" "openocd -s \"$OPENOCD_PATH\" -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c 'adapter speed 5000'"

                # Split horizontally (create a right pane) and start GDB
                # The 1-second sleep gives OpenOCD time to bind port 3333 before GDB attaches
                tmux split-window -h -t "$session" "sleep 1 && gdb-multiarch \"$file\" \
                    -ex 'target extended-remote localhost:3333' \
                    -ex 'monitor reset init' \
                    -ex 'load'"

                # Focus the right pane (GDB) and attach to tmux
                tmux select-pane -t "$session:0.1"
                tmux attach-session -t "$session"
            else
                openocd -s "$OPENOCD_PATH" \
                        -f interface/cmsis-dap.cfg \
                        -f target/rp2040.cfg \
                        -c "adapter speed 5000; program \"$file\" verify reset exit"
            fi
            ;;
	term)
		sudo minicom -D /dev/ttyACM0 -b 115200
		;;
        *)
            echo "Usage: lab {config [debug]|clean|build|flash [debug] [<file>]|term}" >&2
            return 1
            ;;
    esac
}
