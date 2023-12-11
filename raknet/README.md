# Building RakNet/SLikeNet library
Download SLikeNet source from [here](https://www.slikesoft.com/?page_id=1236). Then follow these steps provided in the SLikeNet Github repository.

```
3.3.1 Building SLikeNet
To build SLikeNet on Linux or OSX, you need a supported version of CMake and a
supported compiler version. See chapter 2.2 for a list of what is supported.

1. Create a directory which you will use as the root-directory for SLikeNet (we
   refer to that directory as [SLikeNetRootDirectory])
2. Extract the SLikeNet package to [SLikeNetRootDirectory]/source
3. Adjust [SLikeNetRootDirectory]/source/Source/NativeFeatureIncludesOverrides.h
   and define any optional macros to enable (or disable) certain features
4. Create a new directory: [SLikeNetRootDirectory]/cmake
5. Change the directory to [SLikeNetRootDirectory]/cmake
6. Run cmake ../source
7. Run make
```

Then copy the static library file (.a file) to your C/C++ library path (e.g. /usr/local/lib).
