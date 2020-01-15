# usb-reset

A simple USB Device reset application.

## License

BSD License - See COPYING for full details

## Usage

```
usage: usbreset [-ald]
  -a : Reset all non Hub devices
  -l : List all devices;
  -d vendorID:productID : Reset device with VID:PID. values must be hex.
```

Examples.

* ```# usbreset -a``` to reset all non Hub devices.
* ```# usbreset -d 13d3:5702``` to reset the device with VID:PID 13d3:5702.


Source code formatting:
```$ astyle -A14 -T -p -xg -H -xe -k1 -W3 -j -xC120 -z2 *.c ```

