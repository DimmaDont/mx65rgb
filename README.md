# RGB utility for Meraki MX65

### `mx65rgb_set`
* cli utility for setting rgbwa values

### `mx65rgb_rainbow`
* rainbow
* 250ms (1/8s) per color, 4.5s cycle

### `mx65rgb_pride`
* pride colors
* 1s per color, 6s cycle

### `mx65rgb_baja_blast`
* thin baja blast line
* 1s per color, 12s cycle

### `mx65rgb_sin`
* also rainbow but vaguely more customizable. computes color values at startup
* 80ms (0.08s) per color, ~12.5s cycle


## Usage

### OpenWrt

1. Download a binary from releases to `/sbin`.
2. Add `/sbin/mx65rgb_<tool> &` to `/etc/rc.local`.
3. Start it with the same command, or reboot.

## Building

```
sudo apt install gcc-arm-linux-gnueabi make

git clone https://github.com/DimmaDont/mx65rgb
cd mx65rgb
make
```
