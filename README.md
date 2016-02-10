`libs.thalmic.myo` -- Craftr module to compile program with the Thalmic Myo SDK

__Requirements__:

- `MYO_SDK_PATH` environment variable
- `MYO_SDK_WINARCH` set to `32` or `64` for Windows builds

__Usage__:

Import the `libmyo` framework and pass it to the compiler.

```python
from craftr.ext.libs.thalmic.myo import libmyo
```
