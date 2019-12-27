project(NorthstarLauncher LANGUAGES CSharp)

include(CSharpUtilities)

#FILE(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" DOS_STYLE_SOURCE_DIR)

#include_external_msproject(
#    NorthstarLauncher "${CMAKE_CURRENT_SOURCE_DIR}/NorthstarLauncher.csproj"
#    TYPE FAE04EC0-301F-11D3-BF4B-00C04F79EFBC)

set(NorthstarLauncher_SOURCES 
    App.config
    App.xaml
    App.xaml.cs
    MainWindow.xaml
    MainWindow.xaml.cs
    explodedview.jpg
    steamvrconfig.json
    packages.config
    Resources/icon.ico
    Properties/AssemblyInfo.cs
    Properties/Resources.Designer.cs
    Properties/Resources.resx
    Properties/Settings.Designer.cs
    Properties/Settings.settings
)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/explodedview.jpg ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/explodedview.jpg COPYONLY)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/matrix.jpeg ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/matrix.jpeg COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/steamvrconfig.json ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/steamvrconfig.json COPYONLY)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Resources ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Resources COPYONLY)
#file(COPY Resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
add_custom_target(copy-runtime-files ALL
    COMMAND cmake -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Resources ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Resources)


#CMAKE_RUNTIME_OUTPUT_DIRECTORY
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Then set the file property.
set_property(SOURCE explodedview.jpg PROPERTY VS_TOOL_OVERRIDE "Resource")
set_property(SOURCE Resources/icon.ico PROPERTY VS_TOOL_OVERRIDE "Resource")
#set_property(SOURCE steamvrconfig.json PROPERTY VS_TOOL_OVERRIDE "Resource")

# Define the executable, including any .cs files. 
# The .resx and other Properties files are optional here, but including them makes them visible in the VS solution for easy editing. 
add_executable(NorthstarLauncher ${NorthstarLauncher_SOURCES})

# Set the source file properties for Windows Forms use.
csharp_set_windows_forms_properties(${NorthstarLauncher_SOURCES})

csharp_set_xaml_cs_properties(
    App.xaml
    App.xaml.cs
    MainWindow.xaml
    MainWindow.xaml.cs
    Properties/AssemblyInfo.cs
    Properties/Resources.Designer.cs
    Properties/Resources.resx
    Properties/Settings.Designer.cs
    Properties/Settings.settings
    Resources/icon.ico
)

set_property(SOURCE App.xaml PROPERTY VS_XAML_TYPE "ApplicationDefinition")

# Find Nuget (install the latest CLI here: https://www.nuget.org/downloads).
find_program(NUGET nuget)
if(NOT NUGET)
    message(FATAL "CMake could not find the nuget command line tool. Please install it!")
else()
    # Copy the Nuget config file from source location to the CMake build directory.
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/packages.config ${CMAKE_BUILD_DIR}/packages.config COPYONLY)
    #file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/packages.config DESTINATION ${CMAKE_BUILD_DIR}/packages.config)


    # Run Nuget using the .config file to install any missing dependencies to the build directory.
    execute_process(COMMAND 
        ${NUGET} restore "${CMAKE_BUILD_DIR}\\packages.config" -SolutionDirectory ${CMAKE_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

# Set the .NET Framework version for the executable.
set_property(TARGET NorthstarLauncher PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2")
set_property(TARGET NorthstarLauncher PROPERTY WIN32_EXECUTABLE TRUE)

# Provide the path to the Nuget-installed references, in case this is a fresh project build.
set_property(TARGET NorthstarLauncher PROPERTY 
    VS_DOTNET_REFERENCE_Newtonsoft.Json
    ${CMAKE_BINARY_DIR}/packages/Newtonsoft.Json.12.0.2/lib/net45/Newtonsoft.Json.dll)

set_property(TARGET NorthstarLauncher PROPERTY 
    VS_DOTNET_REFERENCE_MahApps.Metro
    ${CMAKE_BINARY_DIR}/packages/MahApps.Metro.1.6.5/lib/net47/MahApps.Metro.dll)

set_property(TARGET NorthstarLauncher PROPERTY 
    VS_DOTNET_REFERENCE_ControlzEx
    ${CMAKE_BINARY_DIR}/packages/ControlzEx.3.0.2.4/lib/net462/ControlzEx.dll)


set_property(TARGET NorthstarLauncher PROPERTY VS_GLOBAL_ApplicationIcon "${CMAKE_CURRENT_SOURCE_DIR}/Resources/icon.ico")    

# Add in the .NET reference libraries.
set_property(TARGET NorthstarLauncher PROPERTY VS_DOTNET_REFERENCES
"Microsoft.CSharp"
"PresentationCore"
"PresentationFramework"
"System"
"System.Core"
"System.Data"
"System.Data.DataSetExtensions"
"System.IO.Compression.FileSystem"
"System.Net.Http"
"System.Xaml"
"System.Xml"
"System.Xml.Linq",
"System.Windows.Forms",
"WindowsBase"
)



get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    #message(STATUS "${_variableName}=${${_variableName}}")
endforeach()