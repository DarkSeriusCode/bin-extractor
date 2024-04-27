# Bin extractor
Simple C program that extracts such files as images, audio from other binary files. Personally, I use this program only for extracting assets from visual novels ;-;

## Build
```
make release=1
sudo make install
bin-extractor -h
```

Usage example: `bin-extractor png ogg DATA` - it will extract all png and ogg files from DATA
