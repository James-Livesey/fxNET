# fxNET
A web browser/internet suite for Casio fx-9750GIII and compatible calculators.

## Building
To build fxNET, you'll first need to install [fxSDK]([fxSDK](https://gitea.planet-casio.com/Lephenixnoir/fxsdk)). Once installed, run the following command, where `$CALC_FS_PATH` is the path to the connected calculator's mounted filesystem:

```bash
./fxupload.sh $CALC_FS_PATH
```