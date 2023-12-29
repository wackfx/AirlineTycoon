# Installing SDL2 dependencies
SDL2 version 2.10 is the minimum required version


## Windows 10
The `dl-sdl2.ps1` powershell script will automatically download and install SDL2 in this directory - ready to be used by the AT project

If you want to download it yourself, you can use these commands:
```ps
wget "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.1/SDL2_ttf-devel-2.20.1-VC.zip" -outfile "sdl2-ttf.zip"
wget "https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.6.2/SDL2_mixer-devel-2.6.2-VC.zip" -outfile "sdl2-mixer.zip"
wget "https://github.com/libsdl-org/SDL_image/releases/download/release-2.6.2/SDL2_image-devel-2.6.2-VC.zip" -outfile "sdl2-image.zip"
wget "https://github.com/libsdl-org/SDL/releases/download/release-2.24.2/SDL2-devel-2.24.2-VC.zip" -outfile "sdl2.zip"

Expand-Archive -Path sdl2.zip -DestinationPath .\temp\
Move-Item -Path .\temp\SDL2* -Destination .\sdl2\
Expand-Archive -Path sdl2-ttf.zip -DestinationPath .\temp\
Move-Item -Path .\temp\SDL2* -Destination .\sdl2-ttf\
Expand-Archive -Path sdl2-mixer.zip -DestinationPath .\temp\
Move-Item -Path .\temp\SDL2* -Destination .\sdl2-mixer\
Expand-Archive -Path sdl2-image.zip -DestinationPath .\temp\
Move-Item -Path .\temp\SDL2* -Destination .\sdl2-image\
rm .\temp\
rm .\sdl2.zip
rm .\sdl2-mixer.zip
rm .\sdl2-ttf.zip
rm .\sdl2-image.zip
```

## Linux
SDL2 should be included in most packaging tools (apt, etc.) under the name `libsdl2*-dev` or `SDL2*-devel`. CMake will automatically detect those installations for you in the make step