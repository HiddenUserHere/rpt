cmd /c fxc.exe /T fx_2_0 /nologo /Fo"LitSolid.fxc" "LitSolid.fx"
cmd /c fxc.exe /T fx_2_0 /nologo /Fo"LitSolid1.fxc" "LitSolid.fx" /D SELFILLUMINATIONMAP
cmd /c fxc.exe /T fx_2_0 /nologo /Fo"LitSolid5.fxc" "LitSolid.fx" /D SELFILLUMINATIONMAP /D VERTEXCOLOR
cmd /c fxc.exe /T fx_2_0 /nologo /Fo"LitSolid4.fxc" "LitSolid.fx" /D VERTEXCOLOR
cmd /c fxc.exe /T fx_2_0 /nologo /Fo"LitSolid8.fxc" "LitSolid.fx" /D _PS_3_0
cmd /c fxc.exe /T fx_2_0 /nologo /Fo"LitSolid9.fxc" "LitSolid.fx" /D SELFILLUMINATIONMAP /D _PS_3_0
cmd /c fxc.exe /T fx_2_0 /nologo /Fo"LitSolid11.fxc" "LitSolid.fx" /D SELFILLUMINATIONMAP /D SKINNED /D _PS_3_0
cmd /c fxc.exe /T fx_2_0 /nologo /Fo"LitSolid10.fxc" "LitSolid.fx" /D SKINNED /D _PS_3_0
cmd /c fxc.exe /T fx_2_0 /nologo /Fo"LitSolid13.fxc" "LitSolid.fx" /D SELFILLUMINATIONMAP /D VERTEXCOLOR /D _PS_3_0
cmd /c fxc.exe /T fx_2_0 /nologo /Fo"LitSolid12.fxc" "LitSolid.fx" /D VERTEXCOLOR /D _PS_3_0
cmd /c fxc.exe /T fx_2_0 /nologo /Fo"Terrain.fxc" "Terrain.fx"
cmd /c fxc.exe /T fx_2_0 /nologo /Fo"Terrain8.fxc" "Terrain.fx" /D _PS_3_0

copy /Y *.fxc E:\Projects\rpt\client\game\scripts\shaders

pause