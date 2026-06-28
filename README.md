# pranshoe's CHIP8 Emulator

# Phewtures
- Can pretty much run all CHIP8 ROMS
- Plays sound too
- Isn't a NASA Supercomputer

## Build steps
There are two builds web and desktop. \
The desktop build gives you a compiled binary. The uasge is given below
```sh
make desktop "<path to ROM file>"
```

The web build uses Emscripten so make sure to set it up before use.
```
# Install Emscripten
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh

# Verify installation
emcc --version
```
```
# Build web
make web
```

The web build throws a WASM compiled CHIP8 Emulator, which you can import and use it in 
your web app. Or if you want to give it a try it's hosted at https://pranshoe.me/chip8.

### Debug Build
Uncomment `CFLAGS += -DDEBUG` and the build should give you proper verbose output.
