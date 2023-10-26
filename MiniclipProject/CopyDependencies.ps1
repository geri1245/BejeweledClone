param (
  [string]$ProjectDir,
  [string]$TargetDir,
  [string]$PlatformShortName
)

robocopy "$($ProjectDir)Assets" "$($TargetDir)Assets" /mir /log:$($TargetDir)robocopy_assets.log
robocopy "$($ProjectDir)../SDL/lib/$($PlatformShortName)" "$TargetDir" SDL2.dll /log:$($TargetDir)robocopy_sdl.log
robocopy "$($ProjectDir)../SDLImage/lib/$($PlatformShortName)" "$TargetDir" SDL2_image.dll /log:$($TargetDir)robocopy_sdlImage.log