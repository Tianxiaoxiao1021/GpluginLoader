# GpluginLoader

## about

GpluginLoader means git plugin loader,is a plugin loader for the Git distributed version controller that allows users to customize additional features of the Git distributed version controller by writing Git plugin source files in C/C++.

## config

1.Write your plugin code and place it in a folder.

### Tips
The plugin needs to include the init_plugin() function and the cleanup_plugin() function.

2.
Type in command line: loader -git="path/to/git.exe" -plugin=path/to/pluginFolder" -mode=load/unload/enable/disable
This will configure the Git controller's plugin path to be path/to/pluginFolder and load plugin in it.

## license

The project is licensed under the [MIT License](https://opensource.org/licenses/MIT), which allows you to freely use, modify, and distribute the software as long as you retain the copyright notice and license information. (If you are one of the contributors to the project, you can ignore this requirement and do not need to declare the copyright owner).
