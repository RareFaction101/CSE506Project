# CSE 506 Project

> **Note**
> this project only runs on Linux.


## Download Intel Pin

The automatic way is
```sh
make pin
```

If you want to download it by yourself, do this:

1. Go to the download page of pin https://www.intel.com/content/www/us/en/developer/articles/tool/pin-a-binary-instrumentation-tool-downloads.html, choose a version to download.
2. Extract the downloaded `.tar.gz` file, for example
    ```sh
    tar -x -f pin-3.27-98718-gbeaa5d51e-gcc-linux.tar.gz
    ```
3. Move the extracted folder under [](third_party), and rename it to be `pin`, for example
    ```sh
    mv pin-3.27-98718-gbeaa5d51e-gcc-linux third_party/pin
    ```

> **Note**
> please do not include the `tar.gz` or intermediate extracted files into the Git history.

## Build `mempin`

Mempin can only be built on Linux.

First, download Intel Pin.

Then use the makefile
```
make mempin
```

The result of building are `mempin/mempin.so` and `mempin/mempin.o`.

The cache mechanism of `make` will not build the targets again unless the source files are modified or the target files are missing. To force build again, delete `mempin/mempin.so` and `mempin/mempin.o`.

## Run `mempin`

Download Intel Pin and build `mempin` first, then

```
third_party/pin/pin -t mempin/mempin.so [-o <mem_trace_file>] -- <command_to_run>
```

This will generated 3 files
+ `pin.log`: the log file produced by pin
+ `pintool.log`: the log file produced by `mempin`
+ `<mem_trace_file>`: the file containing the trace of memory access. If you do not specify the `-o` option, this file will be `mempin_trace.txt`.