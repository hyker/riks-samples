## Run the sample in Visual Studio Developer Command Prompt

Run the batch file *build.bat*.

## Run the sample in Visual Studio

1. Start a new .NET App project.
2. Delete the default main class (*Program.cs*)
2. Go to the NuGet package manager for the project and install the latest version of the package **Hyker.Riks.NET** and **MQTTnet** (used only for demonstration purposes in this sample).
3. Include the source files from *src* (*RiksMQTTClient.cs* and *RiksSamplesMQTTClientMain.cs*) in the project.
4. Include the configuration file (*default.config*) from the root of this repository.
5. Select the configuration file and change the value of *Copy to Output Directory* to *Copy if newer*.
6. Change the variable *configFile* in *RiksSamplesMQTTClientMain.cs* to point to the included configuration file.
7. Click *Start* to run the demo.

### Troubleshooting

If you get System.BadImageFormatException or something similar, you might need to change the platform target of your project from *Any CPU* to *x86* or *x64* depending on your system.