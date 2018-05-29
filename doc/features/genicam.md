# Installation / Build

 * Download the [GenAPI reference implementation](https://www.emva.org/wp-content/uploads/GenICam_V3_0_2_public_data.zip)
 * Extract all the archives corresponding to your platform in the same directory
 * Run CMake after unchecking no-GenAPI
 * Enter the GenAPI_ROOT_DIR as the directory where the archives were extracted
 * Under Windows, change your PATH to append the directory containing the GenAPI dlls
 * Build

# Camera device usage

 * Use `Factory("DeviceFactory").select("camera").select("genicam")`
 * Locate the transport layer (".cti") file of your provider. Use it in the next `select()` step.
 * Then, follow the `selectDescription()` or `selectValueList()` of each factory to go to the leaf factory.
 * Create the camera

# Example .yml conf file
Yaml configuration files can be used to expose camera parameters (see above, parameters without corresponding value) or to preset some parameters (the value is given, the parameter is not exposed). 
Here follows an example configuration. 

```yaml
ExposureTime:
Width:
Height:
TriggerMode:
TriggerMode: Off
PixelFormat: Mono8
TriggerActivation: RisingEdge
```